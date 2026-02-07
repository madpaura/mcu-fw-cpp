/*++

Licensed under the Apache-2.0 license.

File Name:

    libc_stubs.cpp

Abstract:

    Minimal libc function implementations for bare-metal ROM.

--*/

#include <cstdint>
#include <cstddef>

extern "C" {

// Minimal memset implementation
void* memset(void* dest, int ch, std::size_t count) {
    unsigned char* d = static_cast<unsigned char*>(dest);
    unsigned char c = static_cast<unsigned char>(ch);
    while (count--) {
        *d++ = c;
    }
    return dest;
}

// Minimal memcpy implementation
void* memcpy(void* dest, const void* src, std::size_t count) {
    unsigned char* d = static_cast<unsigned char*>(dest);
    const unsigned char* s = static_cast<const unsigned char*>(src);
    while (count--) {
        *d++ = *s++;
    }
    return dest;
}

// Minimal memmove implementation
void* memmove(void* dest, const void* src, std::size_t count) {
    unsigned char* d = static_cast<unsigned char*>(dest);
    const unsigned char* s = static_cast<const unsigned char*>(src);
    
    if (d < s) {
        while (count--) {
            *d++ = *s++;
        }
    } else {
        d += count;
        s += count;
        while (count--) {
            *--d = *--s;
        }
    }
    return dest;
}

// Minimal memcmp implementation
int memcmp(const void* lhs, const void* rhs, std::size_t count) {
    const unsigned char* l = static_cast<const unsigned char*>(lhs);
    const unsigned char* r = static_cast<const unsigned char*>(rhs);
    while (count--) {
        if (*l != *r) {
            return *l < *r ? -1 : 1;
        }
        l++;
        r++;
    }
    return 0;
}

// Minimal strlen implementation
std::size_t strlen(const char* str) {
    std::size_t len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

// These are required by libgcc for exception handling (which we disable)
// Provide empty stubs
void __cxa_pure_virtual() {
    while (1) {}
}

} // extern "C"

// C++ operator new/delete stubs
// Since we don't use dynamic allocation in ROM, these should never be called
// But if they are, we panic.
void* operator new(std::size_t) {
    while (1) {}  // Panic - no heap in ROM
    __builtin_unreachable();
}

void* operator new[](std::size_t) {
    while (1) {}
    __builtin_unreachable();
}

void operator delete(void*) noexcept {
    // Should never be called - but don't panic in case of static destruction
}

void operator delete[](void*) noexcept {
    // Should never be called
}

void operator delete(void*, std::size_t) noexcept {
    // Should never be called
}

void operator delete[](void*, std::size_t) noexcept {
    // Should never be called
}
