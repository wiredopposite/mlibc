#include "mlibc-asm/helpers.h"

#if defined(__ELF__)
.hidden __setjmp
#endif
#if defined(__ELF__)
.type __setjmp, "function"
#endif
__setjmp:
	mov 4(%esp), %eax       # Save argument (buffer) in eax
	mov %ebx, 0x00(%eax)
	mov %ebp, 0x04(%eax)
	mov %esi, 0x08(%eax)
	mov %edi, 0x0c(%eax)

	lea 4(%esp), %ecx       # esp before return eip is pushed
	mov %ecx, 0x10(%eax)
	mov (%esp), %ecx        # Return eip
	mov %ecx, 0x14(%eax)

	test %edx, %edx
	jnz 1f
	xor %eax, %eax
	ret

1:
	jmp __sigsetjmp

.global setjmp
#if defined(__ELF__)
.type setjmp, "function"
#endif
.global _setjmp
#if defined(__ELF__)
.type _setjmp, "function"
#endif
setjmp:
_setjmp:
	xor %edx, %edx
	jmp __setjmp

.global sigsetjmp
#if defined(__ELF__)
.type sigsetjmp, "function"
#endif
sigsetjmp:
	mov $1, %edx
	jmp __setjmp

.global longjmp
#if defined(__ELF__)
.type longjmp, "function"
#endif
.global _longjmp
#if defined(__ELF__)
.type _longjmp, "function"
#endif
longjmp:
_longjmp:
	mov 4(%esp), %ecx
	mov 0x00(%ecx), %ebx
	mov 0x04(%ecx), %ebp
	mov 0x08(%ecx), %esi
	mov 0x0c(%ecx), %edi

	mov 8(%esp), %eax
	cmp $1, %eax # sets CF if eax < 1
	adc $0, %eax # add CF to eax
	mov 0x10(%ecx), %esp
	jmp *0x14(%ecx)

GNU_STACK_NOTE()
