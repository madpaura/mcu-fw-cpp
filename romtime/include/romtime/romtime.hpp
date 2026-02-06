/*++

Licensed under the Apache-2.0 license.

File Name:

    romtime.hpp

Abstract:

    Main header for romtime library - common libraries for MCU ROM.

--*/

#pragma once

#include "common/types.hpp"
#include "common/static_ref.hpp"
#include "common/error.hpp"
#include "romtime/mci.hpp"
#include "romtime/soc_manager.hpp"
#include "romtime/static_ref.hpp"
#include "romtime/platform.hpp"

namespace caliptra {
namespace romtime {

// Forward declarations
class Writer;
class Exit;

// Global writer pointer (for print/println functionality)
extern Writer* g_writer;
extern Exit* g_exiter;

/**
 * Abstract writer interface for output
 */
class Writer {
public:
    virtual ~Writer() = default;
    virtual void write(const char* str, std::size_t len) = 0;
    virtual void write_char(char c) = 0;
};

/**
 * Set the global writer for print operations
 * @param writer Pointer to writer instance (must remain valid)
 */
void set_printer(Writer* writer);

/**
 * Print formatted output (simplified version)
 */
void print(const char* str);

/**
 * Print formatted output with newline
 */
void println(const char* str);

/**
 * Abstract exit interface
 */
class Exit {
public:
    virtual ~Exit() = default;
    virtual void exit(std::uint32_t code) = 0;
};

/**
 * Set the global exiter
 */
void set_exiter(Exit* exiter);

/**
 * Exit with code (for testing)
 */
[[noreturn]] void test_exit(std::uint32_t code);

/**
 * Hex bytes display helper
 */
class HexBytes {
public:
    explicit HexBytes(const std::uint8_t* data, std::size_t len)
        : data_(data), len_(len) {}

    void print_to(Writer* writer) const;

private:
    const std::uint8_t* data_;
    std::size_t len_;
};

/**
 * Hex word display helper
 */
class HexWord {
public:
    explicit HexWord(std::uint32_t value) : value_(value) {}

    void print_to(Writer* writer) const;

private:
    std::uint32_t value_;
};

/**
 * CRC-8 calculation
 * CRC-8 with polynomial x^8 + x^2 + x^1 + 1
 */
std::uint8_t crc8(std::uint8_t crc, std::uint8_t data);

/**
 * Get current cycle count (platform-specific)
 */
std::uint64_t mcycle();

} // namespace romtime
} // namespace caliptra
