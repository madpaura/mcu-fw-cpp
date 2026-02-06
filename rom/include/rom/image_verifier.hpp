/*++

Licensed under the Apache-2.0 license.

File Name:

    image_verifier.hpp

Abstract:

    Image verification interface.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Image verification result
 */
struct ImageVerificationResult {
    bool signature_valid;
    bool manifest_valid;
    std::uint32_t svn;
    std::uint32_t entry_point;
    std::size_t image_size;
};

/**
 * Image verifier interface
 */
class ImageVerifier {
public:
    virtual ~ImageVerifier() = default;

    /**
     * Verify a firmware image
     * @param image_data Image data to verify
     * @return Verification result
     */
    virtual error::McuResult<ImageVerificationResult> 
        verify(std::span<const std::uint8_t> image_data) = 0;

    /**
     * Verify image manifest
     */
    virtual error::McuResult<void> 
        verify_manifest(std::span<const std::uint8_t> manifest_data) = 0;

    /**
     * Verify image signature
     */
    virtual error::McuResult<void> 
        verify_signature(std::span<const std::uint8_t> image_data,
                        std::span<const std::uint8_t> signature) = 0;
};

} // namespace rom
} // namespace caliptra
