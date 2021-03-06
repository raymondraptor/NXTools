#pragma once
#include <vector>
#include <string>
#include <fnd/types.h>
#include <fnd/IFile.h>
#include <nx/npdm.h>
#include <nx/NsoHeader.h>

#include "nstool.h"
#include "RoMetadataProcess.h"

class NsoProcess
{
public:
	NsoProcess();
	~NsoProcess();

	void process();

	void setInputFile(fnd::IFile* file, bool ownIFile);
	void setCliOutputMode(CliOutputMode type);
	void setVerifyMode(bool verify);

	void setInstructionType(nx::npdm::InstructionType type);
	void setListApi(bool listApi);
	void setListSymbols(bool listSymbols);
private:
	const std::string kModuleName = "NsoProcess";

	fnd::IFile* mFile;
	bool mOwnIFile;

	CliOutputMode mCliOutputMode;
	bool mVerify;
	nx::npdm::InstructionType mInstructionType;
	bool mListApi;
	bool mListSymbols;

	nx::NsoHeader mHdr;
	fnd::MemoryBlob mTextBlob, mRoBlob, mDataBlob;
	RoMetadataProcess mRoMeta;

	void importHeader();
	void importCodeSegments();
	void displayNsoHeader();
	void processRoMeta();
};