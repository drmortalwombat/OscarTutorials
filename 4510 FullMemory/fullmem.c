#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <c64/memmap.h>

// extending the main region to cover all memory from 0x1000 t0 0xff80
// so no BASIC ROM, no IO space and no KERNAL.  An interrupt trampoline is
// installed to enable the kernal ROM during IRQ and NMI.

#pragma region( main, 0x1000, 0xff80, , , {code, data, bss, heap } )

// A stack region is set from 0x0880 to 0x1000 to ensure that the stack
// is available, even if the Kernal and IO are mapped in

#pragma stacksize(0x0780)
#pragma region( stack, 0x0880, 0x1000, , , { stack })

int main(void)
{
	// Hide the basic ROM, must be first instruction

	mmap_trampoline();
	mmap_set(MMAP_RAM);

	// Allocate a full slab of 40k

	char * largemem = malloc(50000);

	mmap_set(MMAP_NO_BASIC);
	printf(p"Allocated 50k at 0x%04X to 0x%04X\n", largemem, largemem + 50000);
	mmap_set(MMAP_RAM);

	// Fill the memory
	for(unsigned i=0; i<50000; i++)
		largemem[i] = i;

	// Done

	// Reenable ROM before returning

	mmap_set(MMAP_ROM);

    return 0;
}
