#include <stdio.h>
#include <string.h>
#include "adventure.h"

struct Location
{
	const char 	* 	name;
	const char	*	description;
	Location	*	directions[4];
};

enum Direction
{
	DIR_NORTH,
	DIR_SOUTH,
	DIR_WEST,
	DIR_EAST
};

extern Location loc_building;
extern Location loc_staircase;

Location	loc_street = {
	"ON A STREET",
	"YOU ARE ON AN OPEN STREET, THERE IS A\n"
	"BUILDING TO THE NORTH",
	{[DIR_NORTH] = &loc_building}
};

Location	loc_building = {
	"IN A BUILDING",
	"YOU ARE IN AN OFFICE BUILDING, THERE IS\n"
	"A STAIRCASE TO THE WEST, AND AN EXIT TO\n"
	"THE SOUTH",
	{[DIR_WEST] = &loc_staircase, [DIR_SOUTH] = &loc_street}
};

Location	loc_staircase = {
	"ON A STAIRCASE",
	"YOU ARE ON A STAIRCASE, THERE IS A\n"
	"VESTIBULE TO THE EAST",
	{[DIR_EAST] = &loc_building}
};

Location	*	loc = &loc_street;

void move(Direction dir)
{
	if (loc->directions[dir])
		loc = loc->directions[dir];
	else
		printf("YOU CAN'T MOVE THERE\n");
}

void execute(void)
{
	switch (verb)
	{
	case VERB_MOVE:
		switch (nouns[0])
		{
		case NOUN_NORTH:
			move(DIR_NORTH);
			break;
		case NOUN_WEST:
			move(DIR_WEST);
			break;
		case NOUN_SOUTH:
			move(DIR_SOUTH);
			break;
		case NOUN_EAST:
			move(DIR_EAST);
			break;
		default:
			printf("YOU CAN'T MOVE THERE\n");
		}
		break;
	case VERB_LOOK:
		printf("%s\n", loc->description);
		break;
	case VERB_TAKE:
		switch (nouns[0])
		{
		case NOUN_KEY:
			printf("TAKING KEY\n");
			break;
		default:
			printf("YOU CAN'T TAKE THIS\n");
		}
		break;
	}			
}

int main(void)
{
	for(;;)
	{
		printf("YOU ARE %s\n", loc->name);		
		read_tokens();
		if (parse_tokens())
			execute();
	}

	return 0;
}
