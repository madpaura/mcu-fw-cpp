/*++

Licensed under the Apache-2.0 license.

File Name:

    types.hpp

Abstract:

    Common type definitions for the C++ port of Caliptra MCU SW.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <array>
#include <span>

namespace caliptra {
namespace common {

// Basic type aliases
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using usize = std::size_t;

// Pointer type for memory-mapped I/O
using volatile_u32_ptr = volatile std::uint32_t*;

// Result type template (simplified version of Rust Result)
template<typename T, typename E>
class Result {
public:
    static Result Ok(T value) {
        Result r;
        r.value_ = value;
        r.is_ok_ = true;
        return r;
    }

    static Result Err(E error) {
        Result r;
        r.error_ = error;
        r.is_ok_ = false;
        return r;
    }

    bool is_ok() const { return is_ok_; }
    bool is_err() const { return !is_ok_; }

    T unwrap() const {
        if (!is_ok_) {
            // In embedded, we might want to panic here
            while(1) {}
        }
        return value_;
    }

    T unwrap_or(T default_value) const {
        return is_ok_ ? value_ : default_value;
    }

    E error() const { return error_; }

private:
    T value_{};
    E error_{};
    bool is_ok_ = false;
};

// Unit type Result specialization
template<typename E>
class Result<void, E> {
public:
    static Result Ok() {
        Result r;
        r.is_ok_ = true;
        return r;
    }

    static Result Err(E error) {
        Result r;
        r.error_ = error;
        r.is_ok_ = false;
        return r;
    }

    bool is_ok() const { return is_ok_; }
    bool is_err() const { return !is_ok_; }

    void unwrap() const {
        if (!is_ok_) {
            while(1) {}
        }
    }

    E error() const { return error_; }

private:
    E error_{};
    bool is_ok_ = false;
};

// Never type placeholder (for functions that never return)
[[noreturn]] inline void panic() {
    while(1) {}
}

} // namespace common
} // namespace caliptra
