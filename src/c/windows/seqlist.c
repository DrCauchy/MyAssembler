#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqlist.h"
#include "nts.h"
#include "error.h"

static int min_table_size(int size)
{
	return size < MINIMUM_TABLE_SIZE ? MINIMUM_TABLE_SIZE : size;
}

static void resize_table(Seqlist* seqlist, int size)
{
	seqlist->table_size = min_table_size(size);
	seqlist->table = (Seqlist_keyval *)realloc(seqlist->table, seqlist->table_size
		* sizeof(Seqlist_keyval));
	err_assert_mem(seqlist->table);
}

static void grow_table(Seqlist* seqlist)
{
	resize_table(seqlist, seqlist->table_size * 2);
}

static void ensure_table_size(Seqlist* seqlist, int size)
{
	if (seqlist->table_size < size)
		grow_table(seqlist);
}



void seqlist_init(Seqlist* seqlist, int size)
{
	seqlist->table = NULL;
	resize_table(seqlist, size);
	seqlist->table_head = 0;
}

void seqlist_delete(Seqlist* seqlist)
{
	free(seqlist->table);
}

void seqlist_get(Seqlist* seqlist, int index, Seqlist_keyval** keyval)
{
	*keyval = seqlist->table + index;
}

void seqlist_add(Seqlist* seqlist, int id, const char* key, int value)
{
	err_assert_str(key, SEQLIST_KEYSIZE);

	seqlist->table_head++;
	ensure_table_size(seqlist, seqlist->table_head + 1);
	strcpy(seqlist->table[seqlist->table_head - 1].key, key);
	seqlist->table[seqlist->table_head - 1].value = value;
	seqlist->table[seqlist->table_head - 1].id = id;
}



int seqlist_length(Seqlist* seqlist)
{
	return seqlist->table_head;
}

void seqlist_clear(Seqlist* seqlist)
{
	seqlist->table_head = 0;
}