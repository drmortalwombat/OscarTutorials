#include <c64/memmap.h>
#include <c64/vic.h>

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

int main(void)
{
	// Map in the RAM underneath the BASIC rom
	mmap_set(MMAP_NO_BASIC);

	// Start playback of subtune 0
	music_init(0);

	for(;;)
	{
		// Call the music playback routine
		music_play();

		// Wait for the next frame
		vic_waitFrame();
	}

	return 0;
}
