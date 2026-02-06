/*++

Licensed under the Apache-2.0 license.

File Name:

    soc_manager.cpp

Abstract:

    Caliptra SoC Manager implementation.

--*/

#include "romtime/soc_manager.hpp"

namespace caliptra {
namespace romtime {

// CaliptraMailboxResponse implementation

CaliptraMailboxResponse::CaliptraMailboxResponse(std::uintptr_t mbox_base, 
                                                   std::size_t dlen_bytes,
                                                   std::uint32_t expected_checksum)
    : mbox_base_(mbox_base)
    , dlen_bytes_(dlen_bytes)
    , expected_checksum_(expected_checksum) {}

CaliptraMailboxResponse::~CaliptraMailboxResponse() {
    if (!released_) {
        // Release the lock
        volatile std::uint32_t* execute = reinterpret_cast<volatile std::uint32_t*>(
            mbox_base_ + MBOX_EXECUTE);
        *execute = 0;  // execute(false)
    }
}

CaliptraMailboxResponse::CaliptraMailboxResponse(CaliptraMailboxResponse&& other) noexcept
    : mbox_base_(other.mbox_base_)
    , idx_(other.idx_)
    , dlen_bytes_(other.dlen_bytes_)
    , checksum_(other.checksum_)
    , expected_checksum_(other.expected_checksum_)
    , released_(other.released_) {
    other.released_ = true;  // Prevent other from releasing
}

CaliptraMailboxResponse& CaliptraMailboxResponse::operator=(CaliptraMailboxResponse&& other) noexcept {
    if (this != &other) {
        // Release current if needed
        if (!released_) {
            volatile std::uint32_t* execute = reinterpret_cast<volatile std::uint32_t*>(
                mbox_base_ + MBOX_EXECUTE);
            *execute = 0;
        }
        
        mbox_base_ = other.mbox_base_;
        idx_ = other.idx_;
        dlen_bytes_ = other.dlen_bytes_;
        checksum_ = other.checksum_;
        expected_checksum_ = other.expected_checksum_;
        released_ = other.released_;
        other.released_ = true;
    }
    return *this;
}

error::McuResult<void> CaliptraMailboxResponse::verify_checksum() const {
    std::uint32_t checksum = static_cast<std::uint32_t>(0) - checksum_;
    if (checksum == expected_checksum_) {
        return error::McuResult<void>::Ok();
    } else {
        return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
    }
}

std::optional<std::uint32_t> CaliptraMailboxResponse::next() {
    std::size_t word_count = (dlen_bytes_ + 3) / 4;
    if (idx_ >= word_count) {
        return std::nullopt;
    }

    if (idx_ == 0) {
        idx_++;
        return expected_checksum_;
    }

    volatile std::uint32_t* dataout = reinterpret_cast<volatile std::uint32_t*>(
        mbox_base_ + MBOX_DATAOUT);
    std::uint32_t data = *dataout;
    idx_++;

    // Calculate remaining bytes
    std::size_t remaining_bytes = dlen_bytes_ - (idx_ - 1) * 4;
    if (remaining_bytes > 4) {
        remaining_bytes = 4;
    }

    // Mask invalid bytes if this is the last chunk
    std::uint32_t valid_data = data;
    if (remaining_bytes < 4) {
        valid_data = data & ((1u << (remaining_bytes * 8)) - 1);
    }

    // Update checksum with valid bytes
    for (std::size_t i = 0; i < remaining_bytes; ++i) {
        checksum_ += (valid_data >> (i * 8)) & 0xFF;
    }

    return valid_data;
}

void CaliptraMailboxResponse::reset() {
    idx_ = 0;
    checksum_ = 0;
}

// CaliptraSoC implementation

CaliptraSoC::CaliptraSoC(std::optional<std::uint32_t> soc_ifc_addr,
                         std::optional<std::uint32_t> soc_ifc_trng_addr,
                         std::optional<std::uint32_t> soc_mbox_addr)
    : soc_ifc_addr_(soc_ifc_addr.value_or(DEFAULT_SOC_IFC_ADDR))
    , soc_ifc_trng_addr_(soc_ifc_trng_addr.value_or(DEFAULT_SOC_IFC_ADDR))
    , soc_mbox_addr_(soc_mbox_addr.value_or(DEFAULT_MBOX_ADDR)) {}

volatile std::uint32_t* CaliptraSoC::soc_ifc_reg(std::size_t offset) {
    return reinterpret_cast<volatile std::uint32_t*>(soc_ifc_addr_ + offset);
}

volatile std::uint32_t* CaliptraSoC::soc_mbox_reg(std::size_t offset) {
    return reinterpret_cast<volatile std::uint32_t*>(soc_mbox_addr_ + offset);
}

std::uint32_t CaliptraSoC::read_soc_ifc(std::size_t offset) {
    return *soc_ifc_reg(offset);
}

std::uint32_t CaliptraSoC::read_soc_mbox(std::size_t offset) {
    return *soc_mbox_reg(offset);
}

void CaliptraSoC::write_soc_ifc(std::size_t offset, std::uint32_t value) {
    *soc_ifc_reg(offset) = value;
}

void CaliptraSoC::write_soc_mbox(std::size_t offset, std::uint32_t value) {
    *soc_mbox_reg(offset) = value;
}

bool CaliptraSoC::is_mailbox_busy() {
    return (read_soc_mbox(MBOX_STATUS) & MBOX_STATUS_CMD_BUSY) != 0;
}

void CaliptraSoC::delay() {
    counter_ = counter_ + 1;
    // Prevent optimization
    volatile std::uint64_t tmp = counter_;
    (void)tmp;
}

error::McuResult<void> CaliptraSoC::lock_mailbox() {
    // Read a 0 to get the lock
    if ((read_soc_mbox(MBOX_LOCK) & 0x01) != 0) {
        return error::McuResult<void>::Err(error::McuError::MBOX_LOCK_FAILED);
    }
    return error::McuResult<void>::Ok();
}

error::McuResult<void> CaliptraSoC::set_command(std::uint32_t cmd, std::size_t payload_len_bytes) {
    // Mailbox lock should read 1 now
    if ((read_soc_mbox(MBOX_LOCK) & 0x01) == 0) {
        return error::McuResult<void>::Err(error::McuError::MBOX_LOCK_FAILED);
    }

    write_soc_mbox(MBOX_CMD, cmd);
    write_soc_mbox(MBOX_DLEN, static_cast<std::uint32_t>(payload_len_bytes));
    return error::McuResult<void>::Ok();
}

error::McuResult<void> CaliptraSoC::write_data(std::uint32_t data) {
    if ((read_soc_mbox(MBOX_LOCK) & 0x01) == 0) {
        return error::McuResult<void>::Err(error::McuError::MBOX_LOCK_FAILED);
    }
    write_soc_mbox(MBOX_DATAIN, data);
    return error::McuResult<void>::Ok();
}

error::McuResult<void> CaliptraSoC::execute_command() {
    if ((read_soc_mbox(MBOX_LOCK) & 0x01) == 0) {
        return error::McuResult<void>::Err(error::McuError::MBOX_LOCK_FAILED);
    }
    write_soc_mbox(MBOX_EXECUTE, 0x01);  // execute(true)
    return error::McuResult<void>::Ok();
}

error::McuResult<void> CaliptraSoC::start_mailbox_req(std::uint32_t cmd, std::size_t len_bytes,
                                                       const std::uint32_t* data, 
                                                       std::size_t data_count) {
    if (len_bytes > MAILBOX_SIZE) {
        return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
    }

    auto lock_result = lock_mailbox();
    if (lock_result.is_err()) {
        return lock_result;
    }

    auto cmd_result = set_command(cmd, len_bytes);
    if (cmd_result.is_err()) {
        return cmd_result;
    }

    for (std::size_t i = 0; i < data_count; ++i) {
        write_soc_mbox(MBOX_DATAIN, data[i]);
    }

    // Ask Caliptra to execute this command
    write_soc_mbox(MBOX_EXECUTE, 0x01);  // execute(true)
    return error::McuResult<void>::Ok();
}

error::McuResult<void> CaliptraSoC::initiate_request(std::uint32_t cmd, std::size_t len_bytes) {
    if (len_bytes > MAILBOX_SIZE) {
        return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
    }

    auto lock_result = lock_mailbox();
    if (lock_result.is_err()) {
        return lock_result;
    }

    return set_command(cmd, len_bytes);
}

error::McuResult<std::optional<CaliptraMailboxResponse>> 
CaliptraSoC::finish_mailbox_resp(std::size_t resp_min_size, std::size_t resp_size) {
    constexpr std::size_t header_size = sizeof(MailboxRespHeader);
    
    if (resp_size < header_size) {
        return error::McuResult<std::optional<CaliptraMailboxResponse>>::Err(
            error::McuError::MBOX_INVALID_RESPONSE);
    }
    if (resp_min_size < header_size) {
        return error::McuResult<std::optional<CaliptraMailboxResponse>>::Err(
            error::McuError::MBOX_INVALID_RESPONSE);
    }

    // Wait for completion
    std::uint32_t timeout_cycles = MAX_WAIT_CYCLES;
    while ((read_soc_mbox(MBOX_STATUS) & MBOX_STATUS_CMD_BUSY) != 0) {
        delay();
        if (--timeout_cycles == 0) {
            return error::McuResult<std::optional<CaliptraMailboxResponse>>::Err(
                error::McuError::MBOX_TIMEOUT);
        }
    }

    std::uint32_t status = read_soc_mbox(MBOX_STATUS);
    
    if ((status & MBOX_STATUS_CMD_FAILURE) != 0) {
        write_soc_mbox(MBOX_EXECUTE, 0);  // execute(false)
        std::uint32_t error_code = read_soc_ifc(SOC_IFC_CPTRA_FW_ERROR_FATAL);
        if (error_code == 0) {
            error_code = read_soc_ifc(SOC_IFC_CPTRA_FW_ERROR_NON_FATAL);
        }
        return error::McuResult<std::optional<CaliptraMailboxResponse>>::Err(
            error::McuError::MBOX_CMD_FAILED);
    }

    if ((status & MBOX_STATUS_CMD_COMPLETE) != 0) {
        write_soc_mbox(MBOX_EXECUTE, 0);  // execute(false)
        return error::McuResult<std::optional<CaliptraMailboxResponse>>::Ok(std::nullopt);
    }

    if ((status & MBOX_STATUS_DATA_READY) == 0) {
        return error::McuResult<std::optional<CaliptraMailboxResponse>>::Err(
            error::McuError::MBOX_INVALID_RESPONSE);
    }

    std::uint32_t dlen_bytes = read_soc_mbox(MBOX_DLEN);
    std::uint32_t expected_checksum = read_soc_mbox(MBOX_DATAOUT);

    CaliptraMailboxResponse response(soc_mbox_addr_, dlen_bytes, expected_checksum);
    return error::McuResult<std::optional<CaliptraMailboxResponse>>::Ok(std::move(response));
}

error::McuResult<void> CaliptraSoC::exec_mailbox_req_u32(std::uint32_t cmd,
                                                          std::uint32_t* req, 
                                                          std::size_t req_count,
                                                          std::uint32_t* resp, 
                                                          std::size_t resp_count) {
    constexpr std::size_t header_size = sizeof(MailboxReqHeader);
    
    if (req_count * 4 < header_size) {
        return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
    }

    // Calculate checksum for payload
    std::uint8_t* header_bytes = reinterpret_cast<std::uint8_t*>(req);
    std::uint8_t* payload_bytes = header_bytes + header_size;
    std::size_t payload_len = req_count * 4 - header_size;

    std::uint32_t checksum = calc_checksum(cmd, payload_bytes, payload_len);
    req[0] = checksum;  // Set header checksum

    auto start_result = start_mailbox_req(cmd, req_count * 4, req, req_count);
    if (start_result.is_err()) {
        return start_result;
    }

    std::size_t resp_len_bytes = resp_count * 4;
    auto finish_result = finish_mailbox_resp(resp_len_bytes, resp_len_bytes);
    if (finish_result.is_err()) {
        return error::McuResult<void>::Err(finish_result.error());
    }

    auto resp_opt = std::move(finish_result).value();
    if (!resp_opt.has_value()) {
        return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
    }

    auto& response = resp_opt.value();
    std::size_t i = 0;
    while (auto word = response.next()) {
        if (i < resp_count) {
            resp[i] = *word;
        }
        ++i;
    }

    return error::McuResult<void>::Ok();
}

std::uint32_t calc_checksum(std::uint32_t cmd, const std::uint8_t* payload, std::size_t len) {
    std::uint32_t checksum = 0;
    
    // Add command bytes
    checksum += (cmd >> 0) & 0xFF;
    checksum += (cmd >> 8) & 0xFF;
    checksum += (cmd >> 16) & 0xFF;
    checksum += (cmd >> 24) & 0xFF;
    
    // Add payload bytes
    for (std::size_t i = 0; i < len; ++i) {
        checksum += payload[i];
    }
    
    return ~checksum + 1;  // Two's complement negation
}

} // namespace romtime
} // namespace caliptra
