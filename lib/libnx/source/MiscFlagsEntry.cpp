#include <nx/MiscFlagsEntry.h>



nx::MiscFlagsEntry::MiscFlagsEntry() :
	mCap(kCapId),
	mFlags(0)
{}

nx::MiscFlagsEntry::MiscFlagsEntry(const KernelCapability & kernel_cap) :
	mCap(kCapId),
	mFlags(0)
{
	setKernelCapability(kernel_cap);
}

nx::MiscFlagsEntry::MiscFlagsEntry(uint32_t flags) :
	mCap(kCapId),
	mFlags(0)
{
	setFlags(flags);
}

const nx::KernelCapability & nx::MiscFlagsEntry::getKernelCapability() const
{
	return mCap;
}

void nx::MiscFlagsEntry::setKernelCapability(const KernelCapability & kernel_cap)
{
	if (kernel_cap.getType() != kCapId)
	{
		throw fnd::Exception(kModuleName, "KernelCapability is not type 'MiscFlags'");
	}

	mCap = kernel_cap;
	processCapField();
}

uint32_t nx::MiscFlagsEntry::getFlags() const
{
	return mFlags;
}

void nx::MiscFlagsEntry::setFlags(uint32_t flags)
{
	if ((flags & ~kMaxVal) != 0)
	{
		throw fnd::Exception(kModuleName, "Illegal MiscFlag bits set. (settable bits: 0-14 inclusive)");
	}

	mFlags = flags;
	updateCapField();
}
