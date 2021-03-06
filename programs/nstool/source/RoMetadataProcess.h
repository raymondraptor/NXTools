#pragma once
#include <vector>
#include <string>
#include <fnd/types.h>
#include <fnd/MemoryBlob.h>

#include <nx/npdm.h>

#include "nstool.h"
#include "SdkApiString.h"
#include "ElfSymbolParser.h"

class RoMetadataProcess
{
public:
	RoMetadataProcess();

	void process();

	void setRoBinary(const fnd::MemoryBlob& bin);
	void setApiInfo(size_t offset, size_t size);
	void setDynSym(size_t offset, size_t size);
	void setDynStr(size_t offset, size_t size);

	void setCliOutputMode(CliOutputMode type);

	void setInstructionType(nx::npdm::InstructionType type);
	void setListApi(bool listApi);
	void setListSymbols(bool listSymbols);
private:
	const std::string kModuleName = "RoMetadataProcess";

	CliOutputMode mCliOutputMode;
	nx::npdm::InstructionType mInstructionType;
	bool mListApi;
	bool mListSymbols;	

	struct sLayout 
	{ 
		sLayout() : offset(0), size(0) {}
		size_t offset;
		size_t size;
	};

	sLayout mApiInfo;
	sLayout mDynSym;
	sLayout mDynStr;
	fnd::MemoryBlob mRoBlob;
	std::vector<SdkApiString> mSdkVerApiList;
	std::vector<SdkApiString> mPublicApiList;
	std::vector<SdkApiString> mDebugApiList;
	std::vector<SdkApiString> mPrivateApiList;
	
	ElfSymbolParser mSymbolList;

	void importApiList();
	void displayRoMetaData();

	const char* getSectionIndexStr(nx::elf::SpecialSectionIndex shn_index) const;
	const char* getSymbolTypeStr(nx::elf::SymbolType symbol_type) const;
	const char* getSymbolBindingStr(nx::elf::SymbolBinding symbol_binding) const;
};