/*++

Licensed under the Apache-2.0 license.

File Name:

    romtime.cpp

Abstract:

    Main romtime implementation - common libraries for MCU ROM.

--*/

#include "romtime/romtime.hpp"
#include "romtime/platform.hpp"
#include <cstring>

namespace caliptra {
namespace romtime {

// Global pointers
Writer* g_writer = nullptr;
Exit* g_exiter = nullptr;

void set_printer(Writer* writer) {
    g_writer = writer;
}

void print(const char* str) {
    if (g_writer != nullptr && str != nullptr) {
        g_writer->write(str, std::strlen(str));
    }
}

void println(const char* str) {
    print(str);
    if (g_writer != nullptr) {
        g_writer->write_char('\n');
    }
}

void set_exiter(Exit* exiter) {
    g_exiter = exiter;
}

[[noreturn]] void test_exit(std::uint32_t code) {
    if (g_exiter != nullptr) {
        g_exiter->exit(code);
    }
    // If no exiter set, loop forever
    while (true) {}
}

void HexBytes::print_to(Writer* writer) const {
    if (writer == nullptr || data_ == nullptr) {
        return;
    }

    for (std::size_t i = 0; i < len_; ++i) {
        std::uint8_t x = data_[i];
        
        // High nibble
        std::uint8_t c = x >> 4;
        if (c < 10) {
            writer->write_char(static_cast<char>('0' + c));
        } else {
            writer->write_char(static_cast<char>('A' + c - 10));
        }

        // Low nibble
        c = x & 0x0F;
        if (c < 10) {
            writer->write_char(static_cast<char>('0' + c));
        } else {
            writer->write_char(static_cast<char>('A' + c - 10));
        }
    }
}

void HexWord::print_to(Writer* writer) const {
    std::uint8_t bytes[4];
    bytes[0] = static_cast<std::uint8_t>((value_ >> 24) & 0xFF);
    bytes[1] = static_cast<std::uint8_t>((value_ >> 16) & 0xFF);
    bytes[2] = static_cast<std::uint8_t>((value_ >> 8) & 0xFF);
    bytes[3] = static_cast<std::uint8_t>(value_ & 0xFF);
    
    HexBytes(bytes, 4).print_to(writer);
}

std::uint8_t crc8(std::uint8_t crc, std::uint8_t data) {
    return platform_crc8(crc, data);
}

std::uint64_t mcycle() {
    return platform_mcycle();
}

} // namespace romtime
} // namespace caliptra
