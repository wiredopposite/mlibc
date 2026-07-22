#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
	uint8_t v;
	asm volatile("inb %1, %0" : "=a"(v) : "Nd"(port));
	return v;
}

static void serial_init() {
	outb(0x3F9, 0x00);
	outb(0x3FB, 0x80);
	outb(0x3F8, 0x03);
	outb(0x3F9, 0x00);
	outb(0x3FB, 0x03);
	outb(0x3FA, 0xC7);
	outb(0x3FC, 0x0B);
}

alignas(16) static unsigned char heap[16 * 1024 * 1024];
static size_t heap_used = 0;

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	ssize_t unused;
	sysdep<Write>(2, msg, strlen(msg), &unused);
}

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	static bool initialized = false;
	if (!initialized) {
		serial_init();
		initialized = true;
	}

	auto p = static_cast<const char *>(buf);
	for (size_t i = 0; i < size; i++) {
		while (!(inb(0x3FD) & 0x20)) {}
		outb(0x3F8, p[i]);
	}
	*ret = size;
	return 0;
}

void Sysdeps<Exit>::operator()(int status) {
	(void)status;
	asm volatile("cli");
	for (;;)
		asm volatile("hlt");
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	size = (size + 0xFFF) & ~size_t(0xFFF);
	if (heap_used + size > sizeof(heap))
		return ENOMEM;
	*pointer = heap + heap_used;
	heap_used += size;
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *, size_t) {
	return 0; // no-op, can't reclaim yet
}

int Sysdeps<Isatty>::operator()(int fd) {
	(void)fd;
	return 0; // pretend everything is a tty for now
}

// --- Everything below needs kernel infrastructure that doesn't exist yet. ---
// Each STUB() traps at runtime (not a build error) the first time something
// actually calls it, so the library still builds and links fine.

int Sysdeps<TcbSet>::operator()(void *pointer) {
	(void)pointer;
	// Needs a GDT with a spare descriptor pointed at the TCB, loaded into
	// %gs (i386 thread-pointer convention). No GDT exists yet.
	STUB();
}

int Sysdeps<Close>::operator()(int) {
	STUB();
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	STUB();
}
int Sysdeps<FutexWait>::operator()(int *, int, timespec const *) {
	STUB();
}
int Sysdeps<Read>::operator()(int, void *, unsigned long, long *) {
	STUB();
}
int Sysdeps<Open>::operator()(const char *, int, unsigned int, int *) {
	STUB();
}
int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	STUB();
}
int Sysdeps<VmMap>::operator()(void *, size_t, int, int, int, off_t, void **) {
	STUB();
}
int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	STUB();
}
int Sysdeps<ClockGet>::operator()(int, time_t *, long *) {
	STUB();
}

} // namespace mlibc
