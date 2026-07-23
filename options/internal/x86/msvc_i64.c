// clang emits calls to these MSVC CRT helper symbols for 64-bit
// multiplication/division/remainder on i386, since the target has no
// native 64-bit divide instruction. They are not part of LLVM's
// compiler-rt (which uses different symbol names like __divdi3), so
// they have to be provided here for any i386 windows-msvc target.
//
// The compiler-generated call sites reference plain, undecorated names
// (e.g. "__alldiv") but expect callee-cleans-stack behavior, which on
// this ABI only __stdcall gives you -- at the cost of __stdcall's own
// "@N" name decoration ("___alldiv@16"). Real MSVC's CRT sidesteps this
// by hand-writing these in naked assembly under the plain name. Instead,
// each is implemented as a normal __stdcall function (getting the right
// calling convention for free from the compiler) under a renamed "_impl"
// symbol, then aliased back to the plain name the caller actually wants
// via a linker directive.

static unsigned long long udivmod64(unsigned long long num, unsigned long long den, unsigned long long *rem_out) {
	unsigned long long quot = 0;
	unsigned long long rem = 0;

	if (den == 0) {
		if (rem_out)
			*rem_out = 0;
		return 0;
	}

	for (int i = 63; i >= 0; i--) {
		rem = (rem << 1) | ((num >> i) & 1ULL);
		if (rem >= den) {
			rem -= den;
			quot |= (1ULL << i);
		}
	}

	if (rem_out)
		*rem_out = rem;
	return quot;
}

unsigned long long __stdcall __aulldiv_impl(unsigned long lo1, unsigned long hi1, unsigned long lo2, unsigned long hi2) {
	unsigned long long a = ((unsigned long long)hi1 << 32) | lo1;
	unsigned long long b = ((unsigned long long)hi2 << 32) | lo2;
	return udivmod64(a, b, 0);
}

unsigned long long __stdcall __aullrem_impl(unsigned long lo1, unsigned long hi1, unsigned long lo2, unsigned long hi2) {
	unsigned long long a = ((unsigned long long)hi1 << 32) | lo1;
	unsigned long long b = ((unsigned long long)hi2 << 32) | lo2;
	unsigned long long rem;
	udivmod64(a, b, &rem);
	return rem;
}

long long __stdcall __alldiv_impl(long lo1, long hi1, long lo2, long hi2) {
	unsigned long long a = ((unsigned long long)(unsigned long)hi1 << 32) | (unsigned long)lo1;
	unsigned long long b = ((unsigned long long)(unsigned long)hi2 << 32) | (unsigned long)lo2;

	int neg = 0;
	if (hi1 < 0) {
		a = (unsigned long long)-(long long)a;
		neg ^= 1;
	}
	if (hi2 < 0) {
		b = (unsigned long long)-(long long)b;
		neg ^= 1;
	}

	unsigned long long q = udivmod64(a, b, 0);
	return neg ? -(long long)q : (long long)q;
}

long long __stdcall __allrem_impl(long lo1, long hi1, long lo2, long hi2) {
	unsigned long long a = ((unsigned long long)(unsigned long)hi1 << 32) | (unsigned long)lo1;
	unsigned long long b = ((unsigned long long)(unsigned long)hi2 << 32) | (unsigned long)lo2;

	int neg = (hi1 < 0);
	if (hi1 < 0)
		a = (unsigned long long)-(long long)a;
	if (hi2 < 0)
		b = (unsigned long long)-(long long)b;

	unsigned long long rem;
	udivmod64(a, b, &rem);
	return neg ? -(long long)rem : (long long)rem;
}

// /alternatename only takes effect for object files the linker actually
// pulls out of the archive -- and nothing references the _impl@16 names
// directly, so that directive alone never gets a chance to run. Instead,
// make the plain name the linker actually looks for a real symbol: a
// one-instruction naked tail-jump to the __stdcall implementation. A
// tail jmp leaves the caller's stack frame untouched, so the callee's
// own "ret 16" returns straight to the original caller.
__attribute__((naked)) void _aulldiv(void) { __asm__("jmp ___aulldiv_impl@16"); }
__attribute__((naked)) void _aullrem(void) { __asm__("jmp ___aullrem_impl@16"); }
__attribute__((naked)) void _alldiv(void) { __asm__("jmp ___alldiv_impl@16"); }
__attribute__((naked)) void _allrem(void) { __asm__("jmp ___allrem_impl@16"); }
