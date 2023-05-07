#ifndef ADVENTURE_H
#define ADVENTURE_H

// Up to four tokens with up to 20 chars	
extern char	tokens[4][21];
extern char	ntokens;

void read_tokens(void);

#pragma compile("adventure.c")

#endif

