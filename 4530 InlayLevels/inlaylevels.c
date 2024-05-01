#include <oscar.h>
#include <stdio.h>

// The constant arrays that hold the compressed inlay regions
const char Inlay0[];
const char Inlay1[];
const char Inlay2[];
const char Inlay3[];

// The inlay sections
#pragma section( icode0, 0 )
#pragma section( icode1, 0 )
#pragma section( icode2, 0 )
#pragma section( icode3, 0 )

// The regions to be compressed into the inlay constants
#pragma region( isec0, 0xc000, 0xd000, , Inlay0, { icode0 } )
#pragma region( isec1, 0xc000, 0xd000, , Inlay1, { icode1 } )
#pragma region( isec2, 0xc000, 0xd000, , Inlay2, { icode2 } )
#pragma region( isec3, 0xc000, 0xd000, , Inlay3, { icode3 } )

// Structure specifying a level.
struct Level
{
	const char * 	inlay;	// Inlay to expand for the level
	void (* entry)(void);	// Function to call in the inlay
};

// Switch to section for first inlay
#pragma code( icode0 )

// Entry function of first inlay
void Entry0(void)
{
	puts(p"entry inlay zero\n");
}

// Switch to section for second inlay
#pragma code( icode1 )

void Entry1(void)
{
	puts(p"entry inlay one\n");
}

#pragma code( icode2 )

void Entry2(void)
{
	puts(p"entry inlay two\n");
}

#pragma code( icode3 )

void Entry3(void)
{
	puts(p"entry inlay three\n");
}

// Now back to shared code
#pragma code( code )

// Constand structure with the level information
const Level	levels[] = {
	{Inlay0, Entry0},
	{Inlay1, Entry1},
	{Inlay2, Entry2},
	{Inlay3, Entry3}
};

int main(void)
{
	// Loop over all levels
	for(char i=0; i<4; i++)
	{
		// Expand the inlay of the level
		oscar_expand_lzo((char *)0xc000, levels[i].inlay);

		// Call the entry function of the level
		levels[i].entry();
	}
	return 0;
}
