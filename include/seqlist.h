#ifndef _SEQLIST_H
#define _SEQLIST_H

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "seqhash.h"

#define SEQLIST_KEYSIZE SEQREAD_SIZE
#define MINIMUM_TABLE_SIZE (8)

typedef struct {
	char key[SEQLIST_KEYSIZE];
	int value;
	int id;
} Seqlist_keyval;

typedef struct {
	Seqlist_keyval* table;  /* keyval array */
	int table_size;  /* size of keyval array */
	int table_head;  /* index of next free entry in keyval table */
} Seqlist;

void seqlist_init(Seqlist* seqlist, int size);
void seqlist_delete(Seqlist* seqlist);

int seqlist_length(Seqlist* seqlist);
void seqlist_add(Seqlist* seqlist, int id, const char* key, int value);
void seqlist_get(Seqlist* seqlist, int index, Seqlist_keyval** keyval);
void seqlist_clear(Seqlist* seqlist);
void seqlist_debug(Seqlist* seqlist);

#endif
