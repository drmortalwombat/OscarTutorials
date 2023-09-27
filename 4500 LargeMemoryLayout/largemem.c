#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <c64/memmap.h>

// extending the main region to cover all memory from 0x0880 t0 0xc000
// so no BASIC ROM, but IO space and kernal stay in play.  The linker will make
// sure that the main code is placed at the beginning of the normal code
// section

#pragma region( main, 0x0880, 0xd000, , , {code, data, bss, heap, stack} )

int main(void)
{
	// Hide the basic ROM, must be first instruction

	mmap_set(MMAP_NO_BASIC);

	// Allocate a full slab of 40k

	char * largemem = malloc(40000);

	printf(p"Allocated 40k at 0x%04X to 0x%04X\n", largemem, largemem + 40000);

	// Fill the memory
	for(unsigned i=0; i<40000; i++)
		largemem[i] = i;

	// Done

	// Reenable basic before returning

	mmap_set(MMAP_ROM);

    return 0;
}
