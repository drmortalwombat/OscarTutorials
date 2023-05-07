#ifndef ADVENTURE_H
#define ADVENTURE_H

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

	NOUN_KEY,
	NOUN_DOOR,

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

#pragma compile("adventure.c")

#endif

