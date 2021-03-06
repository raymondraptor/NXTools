#pragma once
#include <string>
#include <fnd/types.h>
#include <fnd/MemoryBlob.h>
#include <fnd/ISerialiseableBinary.h>
#include <nx/aci.h>

namespace nx
{
	class AciHeader :
		public fnd::ISerialiseableBinary
	{
	public:
		enum AciType
		{
			TYPE_ACI0,	// for Access Control Info
			TYPE_ACID	// for Access Control Info Desc
		};

		struct sSection
		{
			size_t offset;
			size_t size;

			void operator=(const sSection& other)
			{
				offset = other.offset;
				size = other.size;
			}

			bool operator==(const sSection& other) const
			{
				return (offset == other.offset) \
					&& (size == other.size);
			}

			bool operator!=(const sSection& other) const
			{
				return !operator==(other);
			}
		};

		AciHeader();
		AciHeader(const AciHeader& other);
		AciHeader(const byte_t* bytes, size_t len);

		bool operator==(const AciHeader& other) const;
		bool operator!=(const AciHeader& other) const;
		void operator=(const AciHeader& other);

		// to be used after export
		const byte_t* getBytes() const;
		size_t getSize() const;

		// export/import binary
		virtual void exportBinary();
		virtual void importBinary(const byte_t* bytes, size_t len);

		// variables
		virtual void clear();
		size_t getAciSize() const;

		// ACI0 only
		uint64_t getProgramId() const;
		void setProgramId(uint64_t program_id);
		
		// ACID only
		size_t getAcidSize() const;
		//void setAcidSize(size_t size);
		uint64_t getProgramIdMin() const;
		void setProgramIdMin(uint64_t program_id);
		uint64_t getProgramIdMax() const;
		void setProgramIdMax(uint64_t program_id);

		// ACID & ACI0
		void setHeaderOffset(size_t offset);
		AciType getAciType() const;
		void setAciType(AciType type);
		bool isProduction() const;
		void setIsProduction(bool isProduction);
		bool isUnqualifiedApproval() const;
		void setIsUnqualifiedApproval(bool isUnqualifiedApproval);
		const sSection& getFacPos() const;
		void setFacSize(size_t size);
		const sSection& getSacPos() const;
		void setSacSize(size_t size);
		const sSection& getKcPos() const;
		void setKcSize(size_t size);

	private:
		const std::string kModuleName = "ACI_HEADER";

		// raw data
		fnd::MemoryBlob mBinaryBlob;

		// ACI variables
		uint64_t mProgramId;

		// ACID variables
		size_t mAcidSize;
		uint64_t mProgramIdMin;
		uint64_t mProgramIdMax;

		// ACI(D) variables
		size_t mHeaderOffset;
		AciType mType;
		bool mIsProduction;
		bool mIsUnqualifiedApproval;
		sSection mFac, mSac, mKc;

		void calculateSectionOffsets();
		bool isEqual(const AciHeader& other) const;
		void copyFrom(const AciHeader& other);
	};
}

