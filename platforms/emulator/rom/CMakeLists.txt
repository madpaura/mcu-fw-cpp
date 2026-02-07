# Licensed under the Apache-2.0 license
# CMakeLists.txt for Emulator ROM ELF

cmake_minimum_required(VERSION 3.16)
project(mcu_rom_emulator LANGUAGES CXX C ASM)

# ROM ELF target name
set(ROM_ELF_NAME mcu-rom-emulator)

# Source files
set(ROM_SOURCES
    src/main.cpp
)

# For RISC-V builds, include the startup assembly and libc stubs
if(BUILD_RISCV32)
    list(APPEND ROM_SOURCES
        src/start.S
        src/libc_stubs.cpp
    )
endif()

# Create the executable
add_executable(${ROM_ELF_NAME} ${ROM_SOURCES})

# Include directories
target_include_directories(${ROM_ELF_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/rom/include
    ${CMAKE_SOURCE_DIR}/romtime/include
)

# Link against ROM library and romtime
target_link_libraries(${ROM_ELF_NAME} PRIVATE
    rom
    romtime
)

# Platform-specific settings
if(BUILD_RISCV32)
    # Use the linker script
    set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/rom.ld)
    
    target_compile_definitions(${ROM_ELF_NAME} PRIVATE
        TARGET_RISCV32
    )
    
    # Compile options for size optimization
    target_compile_options(${ROM_ELF_NAME} PRIVATE
        -Os                    # Optimize for size
        -ffunction-sections    # Place each function in its own section
        -fdata-sections        # Place each data item in its own section
        -fno-exceptions        # No C++ exceptions
        -fno-rtti              # No runtime type info
        -fno-threadsafe-statics # No thread-safe static initialization
    )
    
    # Linker flags for bare-metal RISC-V
    target_link_options(${ROM_ELF_NAME} PRIVATE
        -T${LINKER_SCRIPT}
        -nostartfiles
        -nostdlib
        -static
        -Wl,--gc-sections      # Remove unused sections
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ROM_ELF_NAME}.map
    )
    
    # Generate binary and disassembly
    add_custom_command(TARGET ${ROM_ELF_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ROM_ELF_NAME}> 
                ${CMAKE_CURRENT_BINARY_DIR}/${ROM_ELF_NAME}.bin
        COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:${ROM_ELF_NAME}> > 
                ${CMAKE_CURRENT_BINARY_DIR}/${ROM_ELF_NAME}.dis
        COMMENT "Generating ROM binary and disassembly"
    )
    
    # Print size info
    add_custom_command(TARGET ${ROM_ELF_NAME} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${ROM_ELF_NAME}>
        COMMENT "ROM size:"
    )
else()
    # Host build - just a regular executable
    target_link_options(${ROM_ELF_NAME} PRIVATE
        -static
    )
endif()

# Set output name without prefix
set_target_properties(${ROM_ELF_NAME} PROPERTIES
    OUTPUT_NAME ${ROM_ELF_NAME}
    SUFFIX ".elf"
)

# Installation
install(TARGETS ${ROM_ELF_NAME}
    RUNTIME DESTINATION bin
)

if(BUILD_RISCV32)
    install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/${ROM_ELF_NAME}.bin
        ${CMAKE_CURRENT_BINARY_DIR}/${ROM_ELF_NAME}.map
        DESTINATION bin
    )
endif()
