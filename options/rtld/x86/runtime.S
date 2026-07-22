.global pltRelocateStub
# save / restore all registers that can hold function parameters
pltRelocateStub:
	# we need to save / restore all registers than can hold function arguments
	# we do not need to save callee-saved registers as they will not be trashed by lazyRelocate
	# TODO: save floating point argument registers
	ud2

#if defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
