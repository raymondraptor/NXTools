#include <es/SectionHeader_V2.h>



es::SectionHeader_V2::SectionHeader_V2()
{}

es::SectionHeader_V2::SectionHeader_V2(const SectionHeader_V2 & other)
{
	copyFrom(other);
}

es::SectionHeader_V2::SectionHeader_V2(const byte_t * bytes, size_t len)
{
	importBinary(bytes, len);
}

bool es::SectionHeader_V2::operator==(const SectionHeader_V2 & other) const
{
	return isEqual(other);
}

bool es::SectionHeader_V2::operator!=(const SectionHeader_V2 & other) const
{
	return !isEqual(other);
}

void es::SectionHeader_V2::operator=(const SectionHeader_V2 & other)
{
	copyFrom(other);
}

const byte_t * es::SectionHeader_V2::getBytes() const
{
	return mBinaryBlob.getBytes();
}

size_t es::SectionHeader_V2::getSize() const
{
	return mBinaryBlob.getSize();
}

void es::SectionHeader_V2::exportBinary()
{
	mBinaryBlob.alloc(sizeof(sSectionHeader_v2));
	sSectionHeader_v2* hdr = (sSectionHeader_v2*)mBinaryBlob.getBytes();

	hdr->section_offset = (mSectionOffset);
	hdr->record_size = (mRecordSize);
	hdr->section_size = (mSectionSize);
	hdr->record_num = (mRecordNum);
	hdr->section_type = (mSectionType);
}

void es::SectionHeader_V2::importBinary(const byte_t * bytes, size_t len)
{
	if (len < sizeof(sSectionHeader_v2))
	{
		throw fnd::Exception(kModuleName, "Binary too small");
	}

	clear();

	mBinaryBlob.alloc(sizeof(sSectionHeader_v2));
	memcpy(mBinaryBlob.getBytes(), bytes, mBinaryBlob.getSize());
	sSectionHeader_v2* hdr = (sSectionHeader_v2*)mBinaryBlob.getBytes();

	mSectionOffset = hdr->section_offset.get();
	mRecordSize = hdr->record_size.get();
	mSectionSize = hdr->section_size.get();
	mRecordNum = hdr->record_num.get();
	mSectionType = (ticket::SectionType)hdr->section_type.get();
}

bool es::SectionHeader_V2::isEqual(const SectionHeader_V2 & other) const
{
	return (mSectionOffset == other.mSectionOffset) \
		&& (mRecordSize == other.mRecordSize) \
		&& (mSectionSize == other.mSectionSize) \
		&& (mRecordNum == other.mRecordNum) \
		&& (mSectionType == other.mSectionType);
}

void es::SectionHeader_V2::copyFrom(const SectionHeader_V2 & other)
{
	if (other.getSize())
	{
		importBinary(other.getBytes(), other.getSize());
	}
	else
	{
		mBinaryBlob.clear();
		mSectionOffset = other.mSectionOffset;
		mRecordSize = other.mRecordSize;
		mSectionSize = other.mSectionSize;
		mRecordNum = other.mRecordNum;
		mSectionType = other.mSectionType;
	}
}

void es::SectionHeader_V2::clear()
{
	mBinaryBlob.clear();
	mSectionOffset = 0;
	mRecordSize = 0;
	mSectionSize = 0;
	mRecordNum = 0;
	mSectionType = ticket::SECTION_PERMANENT;
}

uint32_t es::SectionHeader_V2::getSectionOffset() const
{
	return mSectionOffset;
}

void es::SectionHeader_V2::setSectionOffset(uint32_t offset)
{
	mSectionOffset = offset;
}

uint32_t es::SectionHeader_V2::getRecordSize() const
{
	return mRecordSize;
}

void es::SectionHeader_V2::setRecordSize(uint32_t size)
{
	mRecordSize = size;
}

uint32_t es::SectionHeader_V2::getSectionSize() const
{
	return mSectionSize;
}

void es::SectionHeader_V2::getSectionSize(uint32_t size)
{
	mSectionSize = size;
}

uint16_t es::SectionHeader_V2::getRecordNum() const
{
	return mRecordNum;
}

void es::SectionHeader_V2::setRecordNum(uint16_t record_num)
{
	mRecordNum = record_num;
}

es::ticket::SectionType es::SectionHeader_V2::getSectionType() const
{
	return mSectionType;
}

void es::SectionHeader_V2::setSectionType(ticket::SectionType type)
{
	mSectionType = type;
}


