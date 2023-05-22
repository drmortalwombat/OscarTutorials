#include <stdio.h>
#include <string.h>
#include "adventure.h"

extern Location loc_building;
extern Location loc_staircase;
extern Location loc_corridor;

Item	item_plant = {
	NOUN_FLOWER,
	"FLOWER",
	"A NICE GREEN PLANT IN A POT",
	ITF_CARRY
};

Item	item_key = {
	NOUN_KEY,
	"KEY",
	"A SMALL GOLDEN KEY",
	ITF_CARRY
};

Item	item_door = {
	NOUN_DOOR,
	"DOOR",
	"AN ORNAMENTAL WOODEN DOOR",
	ITF_OPEN
};

Location	loc_street = {
	"ON A STREET",
	"YOU ARE ON AN OPEN STREET, THERE IS A\n"
	"BUILDING TO THE NORTH",
	{[DIR_NORTH] = {&loc_building}},
	&item_plant
};

Location	loc_building = {
	"IN A BUILDING",
	"YOU ARE IN AN OFFICE BUILDING, THERE IS\n"
	"A STAIRCASE TO THE WEST, AND AN EXIT TO\n"
	"THE SOUTH",
	{[DIR_WEST] = {&loc_staircase}, [DIR_SOUTH] = {&loc_street}},
	&item_key
};

Location	loc_staircase = {
	"ON A STAIRCASE",
	"YOU ARE ON A STAIRCASE, THERE IS A\n"
	"VESTIBULE TO THE EAST AND A CORRIDOR\n"
	"TO THE WEST",
	{[DIR_EAST] = {&loc_building}, [DIR_WEST] = {&loc_corridor}},
	nullptr
};

Location	loc_office = {
	"IN AN OFFICE",
	"YOU ARE IN A DUSTY OFFICE, SOMEONE\n"
	"SHOULD CLEAN UP HERE",
	{[DIR_EAST] = {&loc_corridor, &item_door.open}},
	nullptr
};

Location	loc_corridor = {
	"IN A CORRIDOR",
	"YOU ARE ON THE UPPER FLOOR\n"
	"STAIRCASE TO THE EAST, A DOOR TO THE WEST",
	{[DIR_EAST] = {&loc_staircase}, [DIR_WEST] = {&loc_office, &item_door.open}},
	&item_door
};


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
		if (nouns[0] == NOUN_INVENTORY)
		{
			printf("YOU HAVE:" YELLOW);
			if (inventory)
			{
				Item * i = inventory;
				while (i)
				{
					printf(" %s", i->name);
					i = i->next;
				}
				printf("\n");
			}
			else
				printf(" NOTHING.\n");
		}
		else if (nouns[0] == NOUN_NONE)
		{
			printf("%s\n", loc->description);
			if (loc->items)
			{
				Item * i = loc->items;
				printf("YOU CAN SEE:" YELLOW);
				while (i)
				{
					printf(" %s", i->name);
					i = i->next;
				}
				printf("\n");
			}
		}
		else
		{
			Item * item = item_find(nouns[0]);
			if (item)
				printf("%s\n", item->description);
			else
				printf("CAN'T SEE IT.\n");
		}
		break;
	case VERB_TAKE:
		item_pickup(item_find(nouns[0]));
		break;
	case VERB_DROP:
		item_drop(item_find(nouns[0]));
		break;
	case VERB_OPEN:
		item_open(item_find(nouns[0]));
		break;
	}			
}

int main(void)
{
	loc = &loc_street;

	for(;;)
	{		
		printf(WHITE "YOU ARE %s\n", loc->name);		
		printf(BLUE);
		read_tokens();
		printf(WHITE);
		if (parse_tokens())
			execute();
	}

	return 0;
}
