#include <stdio.h>
#include <string.h>
#include "adventure.h"

// Words in our vocabulary
enum Word
{
	VERB_MOVE,
	VERB_TAKE,
	VERB_DROP,
	VERB_USE,

	NOUN_KEY,
	NOUN_DOOR,

	NOUN_WEST,
	NOUN_EAST,
	NOUN_NORTH,
	NOUN_SOUTH,
	NOUN_NONE,

	PREP_ON,
	PREP_WITH,
	PREP_IN,
	PREP_INTO,
	PREP_FROM,
};

#define NOUN_FIRST	NOUN_KEY
#define PREP_FIRST	PREP_ON

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

	// Main nouns for things
	{NOUN_KEY,  "KEY",  VERB_USE},
	{NOUN_DOOR, "DOOR", VERB_USE},

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

// The resulting phrase
Word	verb, nouns[2], prep;

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

void execute(void)
{
	switch (verb)
	{
	case VERB_MOVE:
		switch (nouns[0])
		{
		case NOUN_NORTH:
			printf("MOVING NORTH\n");
			break;
		case NOUN_WEST:
			printf("MOVING WEST\n");
			break;
		case NOUN_SOUTH:
			printf("MOVING SOUTH\n");
			break;
		case NOUN_EAST:
			printf("MOVING EAST\n");
			break;
		default:
			printf("I CAN'T MOVE THERE\n");
		}
		break;
	case VERB_TAKE:
		switch (nouns[0])
		{
		case NOUN_KEY:
			printf("TAKING KEY\n");
			break;
		default:
			printf("I CAN'T TAKE THIS\n");
		}
		break;
	}			
}

int main(void)
{
	for(;;)
	{
		read_tokens();
		if (parse_tokens())
			execute();
	}

	return 0;
}
