#include <oscar.h>
#include <stdio.h>

const char Inlay0[];
const char Inlay1[];
const char Inlay2[];
const char Inlay3[];

#pragma section( icode0, 0 )
#pragma section( icode1, 0 )
#pragma section( icode2, 0 )
#pragma section( icode3, 0 )

#pragma region( isec0, 0xc000, 0xd000, , Inlay0, { icode0 } )
#pragma region( isec1, 0xc000, 0xd000, , Inlay1, { icode1 } )
#pragma region( isec2, 0xc000, 0xd000, , Inlay2, { icode2 } )
#pragma region( isec3, 0xc000, 0xd000, , Inlay3, { icode3 } )


struct Level
{
	const char * 	inlay;
	void (* entry)(void);
};

#pragma code( icode0 )

void Entry0(void)
{
	puts(p"entry inlay zero\n");
}

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

#pragma code( code )

const Level	levels[] = {
	{Inlay0, Entry0},
	{Inlay1, Entry1},
	{Inlay2, Entry2},
	{Inlay3, Entry3}
};

int main(void)
{
	for(char i=0; i<4; i++)
	{
		oscar_expand_lzo((char *)0xc000, levels[i].inlay);
		levels[i].entry();
	}
	return 0;
}
