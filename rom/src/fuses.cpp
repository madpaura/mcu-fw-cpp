/*++

Licensed under the Apache-2.0 license.

File Name:

    fuses.cpp

Abstract:

    MCU Fuses implementation.

--*/

#include "rom/fuses.hpp"

namespace caliptra {
namespace rom {

McuFuseLayoutPolicy::McuFuseLayoutPolicy()
    : anti_rollback_disable(FuseLayout::Single(Bits(1)))
    , idevid_cert_idevid_attr(FuseLayout::Single(Bits(768 * 8)))
    , soc_specific_idevid_certificate(std::nullopt)
    , idevid_manuf_hsm_identifier(FuseLayout::Single(Bits(32)))
    , soc_stepping_id(FuseLayout::Single(Bits(32)))
    , fmc_key_manifest_svn(FuseLayout::OneHotLinearMajorityVote(Bits(32), Duplication(3)))
    , runtime_svn(FuseLayout::OneHotLinearMajorityVote(Bits(128), Duplication(3)))
    , soc_manifest_svn(FuseLayout::OneHotLinearMajorityVote(Bits(128), Duplication(3)))
    , soc_manifest_max_svn(FuseLayout::Single(Bits(128)))
    , owner_pqc_key_type(FuseLayout::Single(Bits(1)))
    , owner_pk_hash_valid(FuseLayout::Single(Bits(1)))
    , pqc_key_types(FuseLayout::Single(Bits(16)))
    , vendor_pk_hash_valid(FuseLayout::Single(Bits(16)))
    , owner_ecc_revocation(FuseLayout::LinearMajorityVote(Bits(1), Duplication(3)))
    , owner_lms_revocation(FuseLayout::LinearMajorityVote(Bits(1), Duplication(3)))
    , owner_mldsa_revocation(FuseLayout::LinearMajorityVote(Bits(1), Duplication(3)))
    , ecc_revocations(FuseLayout::WordMajorityVote(Bits(16), Duplication(3)))
    , lms_revocations(FuseLayout::WordMajorityVote(Bits(16), Duplication(3)))
    , mldsa_revocations(FuseLayout::WordMajorityVote(Bits(16), Duplication(3)))
{}

McuFuses::McuFuses(const RawFuses* raw_fuses, 
                   std::optional<McuFuseLayoutPolicy> policy_override)
    : raw_fuses_(raw_fuses)
    , policy_(policy_override.value_or(McuFuseLayoutPolicy())) {}

} // namespace rom
} // namespace caliptra
