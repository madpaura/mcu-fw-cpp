/*++

Licensed under the Apache-2.0 license.

File Name:

    error.hpp

Abstract:

    MCU Error definitions.

--*/

#pragma once

#include <cstdint>
#include <new>
#include <utility>

namespace caliptra {
namespace error {

/**
 * MCU Error codes
 */
enum class McuError : std::uint32_t {
    // Success
    SUCCESS = 0,

    // Generic errors
    GENERIC_EXCEPTION = 0x0001,
    INVALID_PARAMETER = 0x0002,
    TIMEOUT = 0x0003,
    NOT_SUPPORTED = 0x0004,

    // Lifecycle errors (0x0100 - 0x01FF)
    LC_INVALID_STATE = 0x0100,
    LC_TRANSITION_FAILED = 0x0101,

    // OTP errors (0x0200 - 0x02FF)
    OTP_READ_ERROR = 0x0200,
    OTP_WRITE_ERROR = 0x0201,
    OTP_LOCKED = 0x0202,

    // Flash errors (0x0300 - 0x03FF)
    FLASH_READ_ERROR = 0x0300,
    FLASH_WRITE_ERROR = 0x0301,
    FLASH_ERASE_ERROR = 0x0302,
    FLASH_INVALID_ADDRESS = 0x0303,

    // Image verification errors (0x0400 - 0x04FF)
    IMAGE_INVALID_SIGNATURE = 0x0400,
    IMAGE_INVALID_MANIFEST = 0x0401,
    IMAGE_VERIFICATION_FAILED = 0x0402,

    // Mailbox errors (0x0500 - 0x05FF)
    MBOX_LOCK_FAILED = 0x0500,
    MBOX_TIMEOUT = 0x0501,
    MBOX_CMD_FAILED = 0x0502,
    MBOX_INVALID_RESPONSE = 0x0503,

    // I3C errors (0x0600 - 0x06FF)
    I3C_INIT_FAILED = 0x0600,
    I3C_TRANSFER_FAILED = 0x0601,

    // Boot errors (0x0700 - 0x07FF)
    BOOT_FAILED = 0x0700,
    BOOT_INVALID_RESET_REASON = 0x0701,
};

/**
 * Result type for MCU operations
 * Supports move-only types
 */
template<typename T>
class McuResult {
public:
    McuResult() : has_value_(false), error_(McuError::SUCCESS) {}
    
    ~McuResult() {
        if (has_value_) {
            reinterpret_cast<T*>(&storage_)->~T();
        }
    }

    // Move constructor
    McuResult(McuResult&& other) noexcept 
        : has_value_(other.has_value_), error_(other.error_) {
        if (has_value_) {
            new (&storage_) T(std::move(*reinterpret_cast<T*>(&other.storage_)));
            other.has_value_ = false;
        }
    }

    // Move assignment
    McuResult& operator=(McuResult&& other) noexcept {
        if (this != &other) {
            if (has_value_) {
                reinterpret_cast<T*>(&storage_)->~T();
            }
            has_value_ = other.has_value_;
            error_ = other.error_;
            if (has_value_) {
                new (&storage_) T(std::move(*reinterpret_cast<T*>(&other.storage_)));
                other.has_value_ = false;
            }
        }
        return *this;
    }

    // Delete copy operations for safety
    McuResult(const McuResult&) = delete;
    McuResult& operator=(const McuResult&) = delete;

    static McuResult Ok(T&& value) {
        McuResult r;
        new (&r.storage_) T(std::move(value));
        r.has_value_ = true;
        r.error_ = McuError::SUCCESS;
        return r;
    }

    static McuResult Ok(const T& value) {
        McuResult r;
        new (&r.storage_) T(value);
        r.has_value_ = true;
        r.error_ = McuError::SUCCESS;
        return r;
    }

    static McuResult Err(McuError error) {
        McuResult r;
        r.error_ = error;
        return r;
    }

    bool is_ok() const { return error_ == McuError::SUCCESS && has_value_; }
    bool is_err() const { return error_ != McuError::SUCCESS; }

    T unwrap() && {
        if (is_err() || !has_value_) {
            // Panic in embedded context
            while(1) {}
        }
        has_value_ = false;
        return std::move(*reinterpret_cast<T*>(&storage_));
    }

    T unwrap_or(T default_value) && {
        if (is_ok() && has_value_) {
            has_value_ = false;
            return std::move(*reinterpret_cast<T*>(&storage_));
        }
        return std::move(default_value);
    }

    McuError error() const { return error_; }
    
    T& value() & { 
        return *reinterpret_cast<T*>(&storage_); 
    }
    
    const T& value() const & { 
        return *reinterpret_cast<const T*>(&storage_); 
    }
    
    T&& value() && {
        has_value_ = false;
        return std::move(*reinterpret_cast<T*>(&storage_));
    }

private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_ = false;
    McuError error_ = McuError::SUCCESS;
};

// Specialization for void
template<>
class McuResult<void> {
public:
    static McuResult Ok() {
        McuResult r;
        r.error_ = McuError::SUCCESS;
        return r;
    }

    static McuResult Err(McuError error) {
        McuResult r;
        r.error_ = error;
        return r;
    }

    bool is_ok() const { return error_ == McuError::SUCCESS; }
    bool is_err() const { return error_ != McuError::SUCCESS; }

    void unwrap() const {
        if (is_err()) {
            while(1) {}
        }
    }

    McuError error() const { return error_; }

private:
    McuError error_ = McuError::SUCCESS;
};

} // namespace error
} // namespace caliptra
