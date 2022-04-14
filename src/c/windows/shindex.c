#include <stdlib.h>
#include "shindex.h"
#include "error.h"

static int compare_keyval_value_desc(const void* a, const void* b) {
	return (*((Seqhash_keyval**)b))->value - (*((Seqhash_keyval**)a))->value;
}

void shindex_init(Shindex* shindex, Seqhash* seqhash)
{
	int x;
	shindex->seqhash = seqhash;
	shindex->index_size = seqhash->pt_head - 1;
	shindex->index = (Seqhash_keyval**)malloc(shindex->index_size * sizeof(Seqhash_keyval*));
	err_assert_mem(shindex->index);
	for (x = 0; x < shindex->index_size; x++)
		shindex->index[x] = seqhash->pt + x + 1;
	qsort(shindex->index, shindex->index_size, sizeof(Seqhash_keyval*), compare_keyval_value_desc);
}

Shindex_iterator shindex_get_first(Shindex* shindex, Seqhash_keyval** keyval)
{
	return shindex_get_next(shindex, keyval, 1);
}

Shindex_iterator shindex_get_next(Shindex* shindex, Seqhash_keyval** keyval, Shindex_iterator iterator)
{
	if (!iterator || iterator > shindex->index_size) {
		*keyval = NULL;
		return 0;
	}
	else {
		*keyval = *(shindex->index + iterator - 1);
		return iterator + 1;
	}
}

void shindex_delete(Shindex* shindex)
{
	free(shindex->index);
}

