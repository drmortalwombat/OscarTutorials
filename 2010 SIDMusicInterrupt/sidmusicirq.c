#include <c64/memmap.h>
#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <stdio.h>

// Prepare a secion for the SID file
#pragma section( music, 0)

// The SID file is exported for the 0xa000..0xc000 region
#pragma region( music, 0xa000, 0xc000, , , {music} )

// Set the data segment to be the newly created music section,
// so all initialized data gets placed there by the linker
#pragma data(music)

// Make sure that the music data is not dropped by the linker for
// not being referenced with the "__export" storage qualifier.
// We also drop the first 0x7e bytes of the SID file, they contain
// only file level meta data
__export const char music[] = {
	#embed 0x2000 0x7e "../Resources/gtu_a000_1x.sid" 
};

// Restore the data segment to use the default data section again
#pragma data(data)


// Initialize the music playback for the given sub tune
void music_init(char tune)
{
	// Call the function entry using inline assembler
	__asm
	{
		lda		tune
		jsr		$a000
	}
}

// Iterate the music playback routine, must be called onece
// per video frame
void music_play(void)
{
	// Call the function entry using inline assembler
	__asm
	{
		jsr		$a003
	}
}

__interrupt void music_irq(void)
{
	vic.color_border++;
	music_play();
	vic.color_border--;
}

RIRQCode	music_rirq;

int main(void)
{
	// Map in the RAM underneath the BASIC rom
	mmap_set(MMAP_NO_BASIC);

	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// Start playback of subtune 0
	music_init(0);

	// Init the music interrupt on raster line 250
	rirq_build(&music_rirq, 1);
	rirq_call(&music_rirq, 0, music_irq);
	rirq_set(0, 250, &music_rirq);

	// Prepare the raster IRQ order
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	// Do something to show that the music plays without
	// main thread pumping
	for(;;)
	{
		printf("HELLO WORLD ");
	}

	return 0;
}
