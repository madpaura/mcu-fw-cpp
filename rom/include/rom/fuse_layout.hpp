/*++

Licensed under the Apache-2.0 license.

File Name:

    fuse_layout.hpp

Abstract:

    Fuse layout definitions and policies.

--*/

#pragma once

#include <cstdint>
#include <cstddef>

namespace caliptra {
namespace rom {

/**
 * Number of bits wrapper
 */
struct Bits {
    std::uint32_t value;
    
    explicit constexpr Bits(std::uint32_t v) : value(v) {}
};

/**
 * Duplication count wrapper
 */
struct Duplication {
    std::uint32_t value;
    
    explicit constexpr Duplication(std::uint32_t v) : value(v) {}
};

/**
 * Fuse layout types
 */
enum class FuseLayoutType {
    /// Single value, no error correction
    Single,
    /// One-hot encoded with linear majority vote
    OneHotLinearMajorityVote,
    /// Linear majority vote
    LinearMajorityVote,
    /// Word-level majority vote
    WordMajorityVote,
};

/**
 * Fuse layout configuration
 */
class FuseLayout {
public:
    // Single value layout
    static FuseLayout Single(Bits bits) {
        FuseLayout layout;
        layout.type_ = FuseLayoutType::Single;
        layout.bits_ = bits.value;
        layout.duplication_ = 1;
        return layout;
    }

    // One-hot linear majority vote layout
    static FuseLayout OneHotLinearMajorityVote(Bits bits, Duplication dup) {
        FuseLayout layout;
        layout.type_ = FuseLayoutType::OneHotLinearMajorityVote;
        layout.bits_ = bits.value;
        layout.duplication_ = dup.value;
        return layout;
    }

    // Linear majority vote layout
    static FuseLayout LinearMajorityVote(Bits bits, Duplication dup) {
        FuseLayout layout;
        layout.type_ = FuseLayoutType::LinearMajorityVote;
        layout.bits_ = bits.value;
        layout.duplication_ = dup.value;
        return layout;
    }

    // Word majority vote layout
    static FuseLayout WordMajorityVote(Bits bits, Duplication dup) {
        FuseLayout layout;
        layout.type_ = FuseLayoutType::WordMajorityVote;
        layout.bits_ = bits.value;
        layout.duplication_ = dup.value;
        return layout;
    }

    FuseLayoutType type() const { return type_; }
    std::uint32_t bits() const { return bits_; }
    std::uint32_t duplication() const { return duplication_; }

private:
    FuseLayoutType type_ = FuseLayoutType::Single;
    std::uint32_t bits_ = 0;
    std::uint32_t duplication_ = 1;
};

} // namespace rom
} // namespace caliptra
