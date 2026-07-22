#pragma once

#if !defined(__ASSEMBLER__)
#error "This file can only be used by assembly files."
#endif

#if defined(__ELF__)

#define PROC_START(name) \
	.global name; \
	.type name, @function; \
	.cfi_startproc; \
	name:

#define PROC_START_NOCFI(name) \
	.global name; \
	.type name, @function; \
	name:

#define PROC_END(name) \
	.cfi_endproc; \
	.size name, . - name

#define PROC_END_NOCFI(name) \
	.size name, . - name

#define PROC_ALIAS(name, alias) \
	.global alias; \
	.type alias, @function; \
	.set alias, name

#define PROC_HIDDEN_ALIAS(name, alias) \
	.hidden alias; \
	.type alias, @function; \
	.set alias, name

#define GNU_STACK_NOTE() \
	.section .note.GNU-stack,"",%progbits

#else // COFF (e.g. Windows) targets have no equivalent for any of these.

#define PROC_START(name) \
	.global name; \
	name:

#define PROC_START_NOCFI(name) \
	.global name; \
	name:

#define PROC_END(name)

#define PROC_END_NOCFI(name)

#define PROC_ALIAS(name, alias) \
	.global alias; \
	.set alias, name

#define PROC_HIDDEN_ALIAS(name, alias) \
	.set alias, name

#define GNU_STACK_NOTE()

#endif
