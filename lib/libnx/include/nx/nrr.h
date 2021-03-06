#pragma once
#include <fnd/types.h>
#include <crypto/rsa.h>
#include <nx/macro.h>

namespace nx
{
	namespace nrr
	{
		static const uint32_t kNrrSig = _MAKE_STRUCT_SIGNATURE("NRR0");
	}

#pragma pack(push,1)
	struct sNrrCertificate
	{
		le_uint64_t application_id_mask;
		le_uint64_t application_id_pattern;
		byte_t nrr_body_modulus[crypto::rsa::kRsa2048Size];
		byte_t nrr_cert_signature[crypto::rsa::kRsa2048Size];
	};

	struct sNrrHeader
	{
		le_uint32_t signature;
		byte_t reserved_0[28];
		sNrrCertificate certificate;
		byte_t nrr_body_signature[crypto::rsa::kRsa2048Size];
		le_uint64_t application_id;
		le_uint32_t size;
		byte_t reserved_1[4];
		le_uint32_t hash_offset;
		le_uint32_t hash_count;
		byte_t reserved_2[8];
	};
#pragma pack(pop)
}
