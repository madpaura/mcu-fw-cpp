/*++

Licensed under the Apache-2.0 license.

File Name:

    fuses.hpp

Abstract:

    MCU Fuses interface.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <optional>
#include <span>
#include "rom/fuse_layout.hpp"

namespace caliptra {
namespace rom {

/**
 * Owner public key hash structure (12 x 32-bit words = 384 bits)
 */
struct OwnerPkHash {
    std::array<std::uint32_t, 12> data;
};

/**
 * Vendor public key hash structure (12 x 32-bit words = 384 bits)
 */
struct VendorPkHash {
    std::array<std::uint32_t, 12> data;
};

/**
 * Abstract interface for raw fuse access
 */
class RawFuses {
public:
    virtual ~RawFuses() = default;

    virtual std::uint32_t anti_rollback_disable() const = 0;
    virtual std::span<const std::uint32_t> idevid_cert_idevid_attr() const = 0;
    virtual std::optional<std::span<const std::uint32_t>> soc_specific_idevid_certificate() const = 0;
    virtual std::span<const std::uint32_t> idevid_manuf_hsm_identifier() const = 0;
    virtual std::uint32_t soc_stepping_id() const = 0;
    virtual std::span<const std::uint8_t> fmc_key_manifest_svn() const = 0;
    virtual std::span<const std::uint8_t> runtime_svn() const = 0;
    virtual std::span<const std::uint8_t> soc_manifest_svn() const = 0;
    virtual std::span<const std::uint8_t> soc_manifest_max_svn() const = 0;
    virtual std::optional<OwnerPkHash> owner_pk_hash() const = 0;
    virtual std::span<const std::uint8_t> owner_pqc_key_type() const = 0;
    virtual std::uint32_t owner_pk_hash_valid() const = 0;
    virtual std::span<const VendorPkHash> vendor_pk_hashes() const = 0;
    virtual std::span<const std::uint32_t> pqc_key_types() const = 0;
    virtual std::span<const std::uint32_t> vendor_pk_hash_valid() const = 0;
    virtual std::span<const std::uint32_t> owner_ecc_revocation() const = 0;
    virtual std::span<const std::uint32_t> owner_lms_revocation() const = 0;
    virtual std::span<const std::uint32_t> owner_mldsa_revocation() const = 0;
    virtual std::span<const std::uint32_t> ecc_revocations() const = 0;
    virtual std::optional<std::span<const std::uint32_t>> lms_revocations() const = 0;
    virtual std::optional<std::span<const std::uint32_t>> mldsa_revocations() const = 0;
};

/**
 * MCU fuse layout policy configuration
 */
struct McuFuseLayoutPolicy {
    FuseLayout anti_rollback_disable;
    FuseLayout idevid_cert_idevid_attr;
    std::optional<FuseLayout> soc_specific_idevid_certificate;
    FuseLayout idevid_manuf_hsm_identifier;
    FuseLayout soc_stepping_id;
    FuseLayout fmc_key_manifest_svn;
    FuseLayout runtime_svn;
    FuseLayout soc_manifest_svn;
    FuseLayout soc_manifest_max_svn;
    FuseLayout owner_pqc_key_type;
    FuseLayout owner_pk_hash_valid;
    FuseLayout pqc_key_types;
    FuseLayout vendor_pk_hash_valid;
    FuseLayout owner_ecc_revocation;
    FuseLayout owner_lms_revocation;
    FuseLayout owner_mldsa_revocation;
    FuseLayout ecc_revocations;
    FuseLayout lms_revocations;
    FuseLayout mldsa_revocations;

    // Default constructor with default policy
    McuFuseLayoutPolicy();
};

/**
 * MCU fuses implemented on top of raw fuse view
 */
class McuFuses {
public:
    McuFuses(const RawFuses* raw_fuses, 
             std::optional<McuFuseLayoutPolicy> policy_override = std::nullopt);

    const RawFuses& raw() const { return *raw_fuses_; }
    const McuFuseLayoutPolicy& policy() const { return policy_; }

private:
    const RawFuses* raw_fuses_;
    McuFuseLayoutPolicy policy_;
};

} // namespace rom
} // namespace caliptra
