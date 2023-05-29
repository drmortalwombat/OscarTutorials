#ifndef ADVENTURE_H
#define ADVENTURE_H

#define WHITE "\x05"
#define BLUE "\x9A"
#define YELLOW "\x9E"

// Up to four tokens with up to 20 chars	
extern char	tokens[4][21];
extern char	ntokens;

// Words in our vocabulary
enum Word
{
	VERB_MOVE,
	VERB_TAKE,
	VERB_DROP,
	VERB_USE,
	VERB_LOOK,
	VERB_OPEN,

	NOUN_KEY,
	NOUN_DOOR,
	NOUN_FLOWER,
	NOUN_INVENTORY,

	NOUN_NORTH,
	NOUN_SOUTH,
	NOUN_WEST,
	NOUN_EAST,
	NOUN_NONE,

	PREP_ON,
	PREP_WITH,
	PREP_IN,
	PREP_INTO,
	PREP_FROM,
};

#define NOUN_FIRST	NOUN_KEY
#define PREP_FIRST	PREP_ON

// The resulting phrase
extern Word	verb, nouns[2], prep;

void read_tokens(void);

bool parse_tokens(void);

// Item flag for being able to pick up and carry
#define ITF_CARRY	0x01
#define ITF_OPEN	0x02

struct Item
{
	Word			noun;			// Noun of item
	const char 	* 	name;			// Name
	const char	*	description;	
	char			flags;	
	Item		*	key;			// key to open
	bool			open;
	Item		*	next;			// Linked list
};

struct Location;

struct Passage
{
	Location	*	location;
	bool		*	condition;
};

struct Location
{
	const char 	* 	name;
	const char	*	description;
	Passage			directions[4];
	Item		*	items;			// Items in this location
};

enum Direction
{
	DIR_NORTH,
	DIR_SOUTH,
	DIR_WEST,
	DIR_EAST
};

extern Location		*	loc;
extern Item			*	inventory;

// Move in direction
void move(Direction dir);

// Find an item by name
Item * item_find(Word w);

// First object
Item * item_object(void);

// Find an item referenced by with
Item * item_with(void);

// Pickup an item 
void item_pickup(Item * item);

// Drop an item from inventory
void item_drop(Item * item);

// Open an item
void item_open(Item * item, Item * with);

#pragma compile("adventure.c")

#endif

