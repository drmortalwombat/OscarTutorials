#include <stdio.h>
#include <string.h>
#include "adventure.h"

// Up to four tokens with up to 20 chars	
char	tokens[4][21];
char	ntokens;
Word	verb, nouns[2], prep;

void read_tokens(void)
{
	ntokens = 0;
	while (ntokens == 0)
	{
		// Show a prompt
		printf("> ");

		// No tokens, no char
		int  i = 0;

		// Loop until end of line
		while ((char ch = getchar()) != '\n')
		{
			// Still tokenspace remaining
			if (ntokens < 4)
			{
				// White space, so end of word
				if (ch == ' ')
				{
					// Ignore space at start of word
					if (i != 0)
					{
						// Close string
						tokens[ntokens][i] = 0;
						// Next token
						ntokens++;
						i = 0;
					}
				}
				else if (i < 20)
				{
					// Still space in current token, so append it
					tokens[ntokens][i++] = ch;
				}
			}
		}
		// Check if pending token
		if (i > 0)
		{
			// Close string and increment number of tokens
			tokens[ntokens][i] = 0;
			ntokens++;
		}
		printf("\n");
	}
}


// Check if word is a verb
static inline bool isverb(Word w)
{
	return w < NOUN_FIRST;
}

// Check if verb is a noun
static inline bool isnoun(Word w)
{
	return w >= NOUN_FIRST && w < PREP_FIRST;
}

// Vocabulary entry
struct Vocab
{
	Word			word;	// The word itself
	const char *	text;	// Text of word
	Word			verb;	// Default verb if none provided
};

const Vocab	vocabulary[] = {
	// Main set of verbs
	{VERB_MOVE, "MOVE"},
	{VERB_TAKE, "TAKE"},
	{VERB_DROP, "DROP"},
	{VERB_USE,  "USE"},
	{VERB_LOOK, "LOOK"}, {VERB_LOOK, "L"},

	// Main nouns for things
	{NOUN_KEY,    "KEY",  VERB_USE},
	{NOUN_DOOR,   "DOOR", VERB_USE},
	{NOUN_FLOWER, "FLOWER", VERB_USE},
	{NOUN_INVENTORY, "INVENTORY", VERB_LOOK}, {NOUN_INVENTORY, "I", VERB_LOOK},

	// Directions with shortcuts
	{NOUN_WEST,  "WEST",  VERB_MOVE}, {NOUN_WEST, "W", VERB_MOVE},
	{NOUN_EAST,  "EAST",  VERB_MOVE}, {NOUN_EAST, "E", VERB_MOVE},
	{NOUN_NORTH, "NORTH", VERB_MOVE}, {NOUN_NORTH, "N", VERB_MOVE},
	{NOUN_SOUTH, "SOUTH", VERB_MOVE}, {NOUN_SOUTH, "S", VERB_MOVE},

	// Attributes and particles to ignore
	{PREP_ON,   "ON"},
	{PREP_WITH, "WITH"},
	{PREP_IN,   "IN"},
	{PREP_FROM, "FROM"},
	{PREP_INTO, "INTO"}
};

// Number of entries in our vocabulary
static const int VOCSIZE = sizeof(vocabulary) / sizeof(Vocab);

// Find the vocabulary entry, or -1 if no match
int vocab_find(const char * token)
{
	// Loop over words
	for(int i=0; i<VOCSIZE; i++)
		if (!strcmp(vocabulary[i].text, token))
			return i;

	// No match
	printf("UNKNOWN WORD <%s>\n", token);
	return -1;
}

bool parse_tokens(void)
{	
	// Clear current command
	verb = NOUN_NONE;
	prep = NOUN_NONE;
	nouns[0] = NOUN_NONE;
	nouns[1] = NOUN_NONE;

	// Loop over all tokens
	for(int i=0; i<ntokens; i++)
	{
		// Find the word in the vocabulary
		int k = vocab_find(tokens[i]);
		if (k < 0) return false;

		// Check for a verb
		if (isverb(vocabulary[k].word))
		{
			// Accept the verb, if it is the first
			if (verb != NOUN_NONE)
			{
				printf("TOO MANY VERBS\n");
				return false;
			}
			verb = vocabulary[k].word;;
		}
		else if (isnoun(vocabulary[k].word))
		{
			// Check if this is first or second noun
			if (nouns[1] != NOUN_NONE)
			{
				printf("TOO MANY NOUNS\n");
				return false;			
			}

			// Second noun
			if (nouns[0] != NOUN_NONE)
			{
				nouns[1] = vocabulary[k].word;
			}
			else
			{
				// First noun, might need default verb
				if (verb == NOUN_NONE)
					verb = vocabulary[k].verb;
				nouns[0] = vocabulary[k].word;
			}
		}
		else
		{
			// Some particle, just remember the last one
			prep = vocabulary[k].word;
		}
	}

	// We need at least a verb
	return verb != NOUN_NONE;
}

Location	*	loc;

void move(Direction dir)
{
	// Check if direction is available
	if (loc->directions[dir])
		loc = loc->directions[dir];
	else
		printf("YOU CAN'T MOVE THERE\n");
}

Item * item_find(Word w)
{
	// Look in location
	Item	*	i = loc->items;
	while (i)
	{
		// Check if matching noun
		if (i->noun == w)
			return i;

		// Traverse list
		i = i->next;
	}

	// Look in inventory
	i = inventory;
	while (i)
	{
		// Check if matching noun
		if (i->noun == w)
			return i;

		// Traverse list
		i = i->next;
	}

	// Not found
	return nullptr;
}

// Remove item from list
bool item_remove(Item ** items, Item * item)
{
	// Get head of list
	Item * i = *items;

	// Is the item the head?
	if (i == item)
	{
		// If so, just remove the head
		*items = i->next;
		return true;		
	}
	else
	{
		// Traverse list, by looking one item ahead
		while (i->next)
		{
			// Is the next item the one to remove?
			if (i->next == item)
			{
				// If so, remove by replacing the next pointer
				i->next = item->next;
				return true;
			}
			// Traverse list
			i = i->next;
		}
	}

	// Not found
	return false;
}

void item_pickup(Item * item)
{
	// Check if item is found and in location
	if (!item)
		printf("CAN'T FIND IT\n");		
	else if (!(item->flags & ITF_CARRY))
		printf("YOU CANNOT PICK THIS UP\n");
	else if (item_remove(&(loc->items), item))
	{
		// Place it in inventory
		item->next = inventory;
		inventory = item;
	}
	else
		printf("IT IS NOT HERE\n");
}

void item_drop(Item * item)
{
	// Check if item is in inventory
	if (!item)
		printf("CAN'T FIND IT\n");		
	else if (item_remove(&inventory, item))
	{
		// Put item into location
		item->next = loc->items;
		loc->items = item;
	}
	else
		printf("YOU DON'T HAVE THIS\n");
}
