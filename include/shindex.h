#ifndef _SHINDEX_H
#define _SHINDEX_H

#include "seqhash.h"

typedef struct {
	Seqhash* seqhash;
	Seqhash_keyval** index;
	int index_size;
} Shindex;

typedef size_t Shindex_iterator;

void shindex_init(Shindex* shindex, Seqhash* seqhash);
Shindex_iterator shindex_get_first(Shindex* shindex, Seqhash_keyval** keyval);
Shindex_iterator shindex_get_next(Shindex* shindex, Seqhash_keyval** keyval, Shindex_iterator iterator);
void shindex_delete(Shindex* shindex);

#endif
