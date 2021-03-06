#include <fnd/SimpleTextOutput.h>
#include <fnd/MemoryBlob.h>
#include <compress/lz4.h>
#include "OffsetAdjustedIFile.h"
#include "NsoProcess.h"

NsoProcess::NsoProcess():
	mFile(nullptr),
	mOwnIFile(false),
	mCliOutputMode(_BIT(OUTPUT_BASIC)),
	mVerify(false)
{
}

NsoProcess::~NsoProcess()
{
	if (mOwnIFile)
	{
		delete mFile;
	}
}

void NsoProcess::process()
{
	if (mFile == nullptr)
	{
		throw fnd::Exception(kModuleName, "No file reader set.");
	}

	importHeader();
	importCodeSegments();
	if (_HAS_BIT(mCliOutputMode, OUTPUT_BASIC))
		displayNsoHeader();

	processRoMeta();
}

void NsoProcess::setInputFile(fnd::IFile* file, bool ownIFile)
{
	mFile = file;
	mOwnIFile = ownIFile;
}

void NsoProcess::setCliOutputMode(CliOutputMode type)
{
	mCliOutputMode = type;
}

void NsoProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void NsoProcess::setInstructionType(nx::npdm::InstructionType type)
{
	mRoMeta.setInstructionType(type);
}

void NsoProcess::setListApi(bool listApi)
{
	mRoMeta.setListApi(listApi);
}

void NsoProcess::setListSymbols(bool listSymbols)
{
	mRoMeta.setListSymbols(listSymbols);
}

void NsoProcess::importHeader()
{
	fnd::MemoryBlob scratch;
	if (mFile->size() < sizeof(nx::sNsoHeader))
	{
		throw fnd::Exception(kModuleName, "Corrupt NSO: file too small");
	}

	scratch.alloc(sizeof(nx::sNsoHeader));
	mFile->read(scratch.getBytes(), 0, scratch.getSize());

	mHdr.importBinary(scratch.getBytes(), scratch.getSize());
}

void NsoProcess::importCodeSegments()
{
	fnd::MemoryBlob scratch;
	uint32_t decompressed_len;
	crypto::sha::sSha256Hash calc_hash;

	// process text segment
	if (mHdr.getTextSegmentInfo().is_compressed)
	{
		scratch.alloc(mHdr.getTextSegmentInfo().file_layout.size);
		mFile->read(scratch.getBytes(), mHdr.getTextSegmentInfo().file_layout.offset, scratch.getSize());
		mTextBlob.alloc(mHdr.getTextSegmentInfo().memory_layout.size);
		compress::lz4::decompressData(scratch.getBytes(), (uint32_t)scratch.getSize(), mTextBlob.getBytes(), (uint32_t)mTextBlob.getSize(), decompressed_len);
		if (decompressed_len != mTextBlob.getSize())
		{
			throw fnd::Exception(kModuleName, "NSO text segment failed to decompress");
		}
	}
	else
	{
		mTextBlob.alloc(mHdr.getTextSegmentInfo().file_layout.size);
		mFile->read(mTextBlob.getBytes(), mHdr.getTextSegmentInfo().file_layout.offset, mTextBlob.getSize());
	}
	if (mHdr.getTextSegmentInfo().is_hashed)
	{
		crypto::sha::Sha256(mTextBlob.getBytes(), mTextBlob.getSize(), calc_hash.bytes);
		if (calc_hash != mHdr.getTextSegmentInfo().hash)
		{
			throw fnd::Exception(kModuleName, "NSO text segment failed SHA256 verification");
		}
	}

	// process ro segment
	if (mHdr.getRoSegmentInfo().is_compressed)
	{
		scratch.alloc(mHdr.getRoSegmentInfo().file_layout.size);
		mFile->read(scratch.getBytes(), mHdr.getRoSegmentInfo().file_layout.offset, scratch.getSize());
		mRoBlob.alloc(mHdr.getRoSegmentInfo().memory_layout.size);
		compress::lz4::decompressData(scratch.getBytes(), (uint32_t)scratch.getSize(), mRoBlob.getBytes(), (uint32_t)mRoBlob.getSize(), decompressed_len);
		if (decompressed_len != mRoBlob.getSize())
		{
			throw fnd::Exception(kModuleName, "NSO ro segment failed to decompress");
		}
	}
	else
	{
		mRoBlob.alloc(mHdr.getRoSegmentInfo().file_layout.size);
		mFile->read(mRoBlob.getBytes(), mHdr.getRoSegmentInfo().file_layout.offset, mRoBlob.getSize());
	}
	if (mHdr.getRoSegmentInfo().is_hashed)
	{
		crypto::sha::Sha256(mRoBlob.getBytes(), mRoBlob.getSize(), calc_hash.bytes);
		if (calc_hash != mHdr.getRoSegmentInfo().hash)
		{
			throw fnd::Exception(kModuleName, "NSO ro segment failed SHA256 verification");
		}
	}

	// process data segment
	if (mHdr.getDataSegmentInfo().is_compressed)
	{
		scratch.alloc(mHdr.getDataSegmentInfo().file_layout.size);
		mFile->read(scratch.getBytes(), mHdr.getDataSegmentInfo().file_layout.offset, scratch.getSize());
		mDataBlob.alloc(mHdr.getDataSegmentInfo().memory_layout.size);
		compress::lz4::decompressData(scratch.getBytes(), (uint32_t)scratch.getSize(), mDataBlob.getBytes(), (uint32_t)mDataBlob.getSize(), decompressed_len);
		if (decompressed_len != mDataBlob.getSize())
		{
			throw fnd::Exception(kModuleName, "NSO data segment failed to decompress");
		}
	}
	else
	{
		mDataBlob.alloc(mHdr.getDataSegmentInfo().file_layout.size);
		mFile->read(mDataBlob.getBytes(), mHdr.getDataSegmentInfo().file_layout.offset, mDataBlob.getSize());
	}
	if (mHdr.getDataSegmentInfo().is_hashed)
	{
		crypto::sha::Sha256(mDataBlob.getBytes(), mDataBlob.getSize(), calc_hash.bytes);
		if (calc_hash != mHdr.getDataSegmentInfo().hash)
		{
			throw fnd::Exception(kModuleName, "NSO data segment failed SHA256 verification");
		}
	}
}

void NsoProcess::displayNsoHeader()
{
#define _HEXDUMP_L(var, len) do { for (size_t a__a__A = 0; a__a__A < len; a__a__A++) printf("%02x", var[a__a__A]); } while(0)

	printf("[NSO Header]\n");
	printf("  ModuleId:           ");
	_HEXDUMP_L(mHdr.getModuleId().data, nx::nso::kModuleIdSize);
	printf("\n");
	if (_HAS_BIT(mCliOutputMode, OUTPUT_LAYOUT))
	{
		printf("  Program Segments:\n");
		printf("     .module_name:\n");
		printf("      FileOffset:     0x%" PRIx32 "\n", mHdr.getModuleNameInfo().offset);
		printf("      FileSize:       0x%" PRIx32 "\n", mHdr.getModuleNameInfo().size);
		printf("    .text:\n");
		printf("      FileOffset:     0x%" PRIx32 "\n", mHdr.getTextSegmentInfo().file_layout.offset);
		printf("      FileSize:       0x%" PRIx32 "%s\n", mHdr.getTextSegmentInfo().file_layout.size, mHdr.getTextSegmentInfo().is_compressed? " (COMPRESSED)" : "");
		printf("    .ro:\n");
		printf("      FileOffset:     0x%" PRIx32 "\n", mHdr.getRoSegmentInfo().file_layout.offset);
		printf("      FileSize:       0x%" PRIx32 "%s\n", mHdr.getRoSegmentInfo().file_layout.size, mHdr.getRoSegmentInfo().is_compressed? " (COMPRESSED)" : "");
		printf("    .data:\n");
		printf("      FileOffset:     0x%" PRIx32 "\n", mHdr.getDataSegmentInfo().file_layout.offset);
		printf("      FileSize:       0x%" PRIx32 "%s\n", mHdr.getDataSegmentInfo().file_layout.size, mHdr.getDataSegmentInfo().is_compressed? " (COMPRESSED)" : "");
	}
	printf("  Program Sections:\n");
	printf("     .text:\n");
	printf("      MemoryOffset:   0x%" PRIx32 "\n", mHdr.getTextSegmentInfo().memory_layout.offset);
	printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getTextSegmentInfo().memory_layout.size);
	if (mHdr.getTextSegmentInfo().is_hashed && _HAS_BIT(mCliOutputMode, OUTPUT_EXTENDED))
	{
		printf("      Hash:           ");
		_HEXDUMP_L(mHdr.getTextSegmentInfo().hash.bytes, 32);
		printf("\n");
	}
	printf("    .ro:\n");
	printf("      MemoryOffset:   0x%" PRIx32 "\n", mHdr.getRoSegmentInfo().memory_layout.offset);
	printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getRoSegmentInfo().memory_layout.size);
	if (mHdr.getRoSegmentInfo().is_hashed && _HAS_BIT(mCliOutputMode, OUTPUT_EXTENDED))
	{
		printf("      Hash:           ");
		_HEXDUMP_L(mHdr.getRoSegmentInfo().hash.bytes, 32);
		printf("\n");
	}
	if (_HAS_BIT(mCliOutputMode, OUTPUT_EXTENDED))
	{
		printf("    .api_info:\n");
		printf("      MemoryOffset:   0x%" PRIx32 "\n",  mHdr.getRoEmbeddedInfo().offset);
		printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getRoEmbeddedInfo().size);
		printf("    .dynstr:\n");
		printf("      MemoryOffset:   0x%" PRIx32 "\n", mHdr.getRoDynStrInfo().offset);
		printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getRoDynStrInfo().size);
		printf("    .dynsym:\n");
		printf("      MemoryOffset:   0x%" PRIx32 "\n", mHdr.getRoDynSymInfo().offset);
		printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getRoDynSymInfo().size);
	}
	
	printf("    .data:\n");
	printf("      MemoryOffset:   0x%" PRIx32 "\n", mHdr.getDataSegmentInfo().memory_layout.offset);
	printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getDataSegmentInfo().memory_layout.size);
	if (mHdr.getDataSegmentInfo().is_hashed && _HAS_BIT(mCliOutputMode, OUTPUT_EXTENDED))
	{
		printf("      Hash:           ");
		_HEXDUMP_L(mHdr.getDataSegmentInfo().hash.bytes, 32);
		printf("\n");
	}
	printf("    .bss:\n");
	printf("      MemorySize:     0x%" PRIx32 "\n", mHdr.getBssSize());
#undef _HEXDUMP_L
}

void NsoProcess::processRoMeta()
{
	if (mRoBlob.getSize())
	{
		// setup ro metadata
		mRoMeta.setApiInfo(mHdr.getRoEmbeddedInfo().offset, mHdr.getRoEmbeddedInfo().size);
		mRoMeta.setDynSym(mHdr.getRoDynSymInfo().offset, mHdr.getRoDynSymInfo().size);
		mRoMeta.setDynStr(mHdr.getRoDynStrInfo().offset, mHdr.getRoDynStrInfo().size);
		mRoMeta.setRoBinary(mRoBlob);
		mRoMeta.setCliOutputMode(mCliOutputMode);
		mRoMeta.process();
	}
}