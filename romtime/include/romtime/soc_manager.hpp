/*++

Licensed under the Apache-2.0 license.

File Name:

    soc_manager.hpp

Abstract:

    Caliptra SoC Manager - interface for SoC-to-Caliptra communication.

--*/

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include "common/error.hpp"

namespace caliptra {
namespace romtime {

// Mailbox size constant
constexpr std::size_t MAILBOX_SIZE = 256 * 1024;

// Maximum wait cycles
constexpr std::uint32_t MAX_WAIT_CYCLES = 400000;

/**
 * Caliptra API Error codes
 */
enum class CaliptraApiError {
    Success = 0,
    BufferTooLargeForMailbox,
    UnableToLockMailbox,
    MailboxTimeout,
    MailboxCmdFailed,
    MailboxRespTypeTooSmall,
    MailboxReqTypeTooSmall,
    MailboxNoResponseData,
    UnknownCommandStatus,
    MailboxRespInvalidChecksum,
};

/**
 * Mailbox request header
 */
struct MailboxReqHeader {
    std::uint32_t chksum;
};

/**
 * Mailbox response header
 */
struct MailboxRespHeader {
    std::uint32_t chksum;
    std::uint32_t fips_status;
};

/**
 * Caliptra mailbox response iterator
 */
class CaliptraMailboxResponse {
public:
    CaliptraMailboxResponse(std::uintptr_t mbox_base, std::size_t dlen_bytes, 
                            std::uint32_t expected_checksum);
    ~CaliptraMailboxResponse();

    // Non-copyable
    CaliptraMailboxResponse(const CaliptraMailboxResponse&) = delete;
    CaliptraMailboxResponse& operator=(const CaliptraMailboxResponse&) = delete;

    // Movable
    CaliptraMailboxResponse(CaliptraMailboxResponse&& other) noexcept;
    CaliptraMailboxResponse& operator=(CaliptraMailboxResponse&& other) noexcept;

    /**
     * Verify the checksum of the response
     */
    error::McuResult<void> verify_checksum() const;

    /**
     * Get response length in bytes
     */
    std::size_t len() const { return dlen_bytes_; }

    /**
     * Check if empty
     */
    bool is_empty() const { return dlen_bytes_ == 0; }

    /**
     * Read next word from response
     */
    std::optional<std::uint32_t> next();

    /**
     * Reset iteration
     */
    void reset();

private:
    std::uintptr_t mbox_base_;
    std::size_t idx_ = 0;
    std::size_t dlen_bytes_;
    std::uint32_t checksum_ = 0;
    std::uint32_t expected_checksum_;
    bool released_ = false;

    // Mailbox register offsets
    static constexpr std::size_t MBOX_DATAOUT = 0x00;
    static constexpr std::size_t MBOX_EXECUTE = 0x04;
};

/**
 * Caliptra SoC Manager
 * Provides interface for SoC-to-Caliptra communication via mailbox
 */
class CaliptraSoC {
public:
    /**
     * Construct with optional register base addresses
     */
    CaliptraSoC(std::optional<std::uint32_t> soc_ifc_addr = std::nullopt,
                std::optional<std::uint32_t> soc_ifc_trng_addr = std::nullopt,
                std::optional<std::uint32_t> soc_mbox_addr = std::nullopt);

    /**
     * Check if mailbox is busy
     */
    bool is_mailbox_busy();

    /**
     * Start a mailbox request (don't wait for response)
     */
    error::McuResult<void> start_mailbox_req(std::uint32_t cmd, std::size_t len_bytes,
                                              const std::uint32_t* data, std::size_t data_count);

    /**
     * Initiate a request (lock and set command, but don't write data)
     */
    error::McuResult<void> initiate_request(std::uint32_t cmd, std::size_t len_bytes);

    /**
     * Lock the mailbox
     */
    error::McuResult<void> lock_mailbox();

    /**
     * Set command and length
     */
    error::McuResult<void> set_command(std::uint32_t cmd, std::size_t payload_len_bytes);

    /**
     * Write data word to mailbox
     */
    error::McuResult<void> write_data(std::uint32_t data);

    /**
     * Execute the mailbox command
     */
    error::McuResult<void> execute_command();

    /**
     * Finish mailbox request and get response
     */
    error::McuResult<std::optional<CaliptraMailboxResponse>> 
        finish_mailbox_resp(std::size_t resp_min_size, std::size_t resp_size);

    /**
     * Execute mailbox request with u32 arrays
     */
    error::McuResult<void> exec_mailbox_req_u32(std::uint32_t cmd,
                                                 std::uint32_t* req, std::size_t req_count,
                                                 std::uint32_t* resp, std::size_t resp_count);

    /**
     * Delay function for polling
     */
    void delay();

private:
    std::uint64_t counter_ = 0;
    std::uintptr_t soc_ifc_addr_;
    std::uintptr_t soc_ifc_trng_addr_;
    std::uintptr_t soc_mbox_addr_;

    // Register access helpers
    volatile std::uint32_t* soc_ifc_reg(std::size_t offset);
    volatile std::uint32_t* soc_mbox_reg(std::size_t offset);
    std::uint32_t read_soc_ifc(std::size_t offset);
    std::uint32_t read_soc_mbox(std::size_t offset);
    void write_soc_ifc(std::size_t offset, std::uint32_t value);
    void write_soc_mbox(std::size_t offset, std::uint32_t value);

    // Mailbox register offsets
    static constexpr std::size_t MBOX_LOCK = 0x00;
    static constexpr std::size_t MBOX_CMD = 0x04;
    static constexpr std::size_t MBOX_DLEN = 0x08;
    static constexpr std::size_t MBOX_DATAIN = 0x0C;
    static constexpr std::size_t MBOX_DATAOUT = 0x10;
    static constexpr std::size_t MBOX_STATUS = 0x14;
    static constexpr std::size_t MBOX_EXECUTE = 0x18;

    // SoC IFC register offsets
    static constexpr std::size_t SOC_IFC_CPTRA_FW_ERROR_FATAL = 0x00;
    static constexpr std::size_t SOC_IFC_CPTRA_FW_ERROR_NON_FATAL = 0x04;

    // Status bits
    static constexpr std::uint32_t MBOX_STATUS_CMD_BUSY = 0x01;
    static constexpr std::uint32_t MBOX_STATUS_CMD_FAILURE = 0x02;
    static constexpr std::uint32_t MBOX_STATUS_CMD_COMPLETE = 0x04;
    static constexpr std::uint32_t MBOX_STATUS_DATA_READY = 0x08;

    // Default addresses
    static constexpr std::uint32_t DEFAULT_SOC_IFC_ADDR = 0x30030000;
    static constexpr std::uint32_t DEFAULT_MBOX_ADDR = 0x30020000;
};

/**
 * Calculate mailbox checksum
 */
std::uint32_t calc_checksum(std::uint32_t cmd, const std::uint8_t* payload, std::size_t len);

} // namespace romtime
} // namespace caliptra
