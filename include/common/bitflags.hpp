/*++

Licensed under the Apache-2.0 license.

File Name:

    bitflags.hpp

Abstract:

    Bitflag utility macros and templates similar to Rust's bitflags crate.

--*/

#pragma once

#include <cstdint>
#include <type_traits>

namespace caliptra {
namespace common {

/**
 * Template for bitflag types
 */
template<typename T>
class Bitflags {
    static_assert(std::is_integral_v<T>, "Bitflags requires integral type");

public:
    using value_type = T;

    constexpr Bitflags() noexcept : value_(0) {}
    constexpr explicit Bitflags(T value) noexcept : value_(value) {}

    constexpr T bits() const noexcept { return value_; }

    constexpr bool is_empty() const noexcept { return value_ == 0; }
    constexpr bool is_all() const noexcept { return value_ == static_cast<T>(-1); }

    constexpr bool contains(Bitflags other) const noexcept {
        return (value_ & other.value_) == other.value_;
    }

    constexpr Bitflags operator|(Bitflags other) const noexcept {
        return Bitflags(value_ | other.value_);
    }

    constexpr Bitflags operator&(Bitflags other) const noexcept {
        return Bitflags(value_ & other.value_);
    }

    constexpr Bitflags operator^(Bitflags other) const noexcept {
        return Bitflags(value_ ^ other.value_);
    }

    constexpr Bitflags operator~() const noexcept {
        return Bitflags(~value_);
    }

    Bitflags& operator|=(Bitflags other) noexcept {
        value_ |= other.value_;
        return *this;
    }

    Bitflags& operator&=(Bitflags other) noexcept {
        value_ &= other.value_;
        return *this;
    }

    Bitflags& operator^=(Bitflags other) noexcept {
        value_ ^= other.value_;
        return *this;
    }

    void set(Bitflags other) noexcept { value_ |= other.value_; }
    void unset(Bitflags other) noexcept { value_ &= ~other.value_; }
    void toggle(Bitflags other) noexcept { value_ ^= other.value_; }

    constexpr operator T() const noexcept { return value_; }

private:
    T value_;
};

// Common bitflag sizes
using Bitflags8 = Bitflags<std::uint8_t>;
using Bitflags16 = Bitflags<std::uint16_t>;
using Bitflags32 = Bitflags<std::uint32_t>;
using Bitflags64 = Bitflags<std::uint64_t>;

} // namespace common
} // namespace caliptra

// Macro to define bitflag types
#define DEFINE_BITFLAGS(name, underlying_type, ...) \
    class name : public caliptra::common::Bitflags<underlying_type> { \
    public: \
        using Bitflags::Bitflags; \
        __VA_ARGS__ \
    };
