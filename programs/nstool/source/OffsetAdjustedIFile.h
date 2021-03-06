#include <fnd/IFile.h>

class OffsetAdjustedIFile : public fnd::IFile
{
public:
	OffsetAdjustedIFile(fnd::IFile* file, bool ownIFile, size_t offset, size_t size);
	~OffsetAdjustedIFile();

	size_t size();
	void seek(size_t offset);
	void read(byte_t* out, size_t len);
	void read(byte_t* out, size_t offset, size_t len);
	void write(const byte_t* out, size_t len);
	void write(const byte_t* out, size_t offset, size_t len);
private:
	bool mOwnIFile;
	fnd::IFile* mFile;
	size_t mBaseOffset, mCurrentOffset;
	size_t mSize;
};