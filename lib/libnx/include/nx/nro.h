#pragma once
#include <fnd/types.h>
#include <crypto/sha.h>
#include <nx/macro.h>

namespace nx
{
	namespace nro
	{
		static const uint32_t kNroSig = _MAKE_STRUCT_SIGNATURE("NRO0");


		static const uint32_t kDefaultFormatVersion = 0;
		static const size_t kRoCrtSize = 8;
		static const size_t kModuleIdSize = 32;
	}

#pragma pack(push,1)
	struct sNroSection
	{
		le_uint32_t memory_offset;
		le_uint32_t size;
	};

	struct sNroHeader
	{
		byte_t ro_crt[nro::kRoCrtSize];
		byte_t reserved_0[8];
		le_uint32_t signature;
		le_uint32_t format_version;
		le_uint32_t size;
		le_uint32_t flags;
		sNroSection text;
		sNroSection ro;
		sNroSection data;
		le_uint32_t bss_size;
		byte_t reserved_1[4];
		byte_t module_id[nro::kModuleIdSize];
		byte_t reserved_2[8];
		sNroSection embedded;
		sNroSection dyn_str;
		sNroSection dyn_sym;
	};
#pragma pack(pop)
}
