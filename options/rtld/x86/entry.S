#include "mlibc-asm/helpers.h"

PROC_START(_start)
#if defined(__ELF__)
.cfi_undefined eip
#endif
	call relocateSelf

	push %esp
	call interpreterMain

	jmp *%eax
PROC_END(_start)

GNU_STACK_NOTE()
