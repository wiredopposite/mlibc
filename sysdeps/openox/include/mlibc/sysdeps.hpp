#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct OpenOX3SysdepTags :
	LibcPanic,
	LibcLog,
	Isatty,
	Write,
	TcbSet,
	AnonAllocate,
	AnonFree,
	Seek,
	Exit,
	Close,
	FutexWake,
	FutexWait,
	Read,
	Open,
	VmMap,
	VmUnmap,
	ClockGet
{};

template<typename Tag>
using Sysdeps = SysdepOf<OpenOX3SysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

} // namespace mlibc
