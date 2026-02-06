/*++

Licensed under the Apache-2.0 license.

File Name:

    static_ref.hpp

Abstract:

    Wrapper type for safe pointers to static memory (memory mapped I/O registers).
    Ported from Tock OS StaticRef.

--*/

#pragma once

#include <cstdint>
#include <type_traits>

namespace caliptra {
namespace common {

/**
 * A pointer to statically allocated mutable data such as memory mapped I/O
 * registers.
 *
 * This is a simple wrapper around a raw pointer that encapsulates an unsafe
 * dereference in a safe manner. It serves the role of creating a reference
 * given a raw address.
 *
 * @tparam T The type of the pointed-to data
 */
template<typename T>
class StaticRef {
public:
    /**
     * Create a new StaticRef from a raw pointer.
     *
     * @param ptr Must be aligned, non-null, and dereferencable as T.
     *            The pointed-to memory must be valid for the program duration.
     */
    constexpr explicit StaticRef(const T* ptr) noexcept
        : ptr_(const_cast<T*>(ptr)) {}

    /**
     * Create a StaticRef from a memory address.
     *
     * @param addr The memory address to wrap
     */
    constexpr explicit StaticRef(std::uintptr_t addr) noexcept
        : ptr_(reinterpret_cast<T*>(addr)) {}

    // Default constructor (null pointer)
    constexpr StaticRef() noexcept : ptr_(nullptr) {}

    // Copy operations
    constexpr StaticRef(const StaticRef&) noexcept = default;
    constexpr StaticRef& operator=(const StaticRef&) noexcept = default;

    // Dereference operators
    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }

    // Get raw pointer
    T* get() const noexcept { return ptr_; }

    // Check if valid
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    // Comparison operators
    bool operator==(const StaticRef& other) const noexcept {
        return ptr_ == other.ptr_;
    }

    bool operator!=(const StaticRef& other) const noexcept {
        return ptr_ != other.ptr_;
    }

private:
    T* ptr_;
};

} // namespace common
} // namespace caliptra
