#ifndef _SEQTREE_H
#define _SEQTREE_H

#include <stdlib.h>
#include "config.h"

#define SEQTREE_KEYSIZE (50)
#define SEQTREE_HEIGHT (11)
#define SEQTREE_BRANCHES (4)

typedef struct {
	int value;
	unsigned int id;
	char key[SEQTREE_KEYSIZE - SEQTREE_HEIGHT];//50 - 11 = 39
	int _next;  /* index of next key in bucket */
} Seqtree_keyval;

typedef struct Seqtree_node_t {
	char utype;
	union {
		struct Seqtree_node_t* _next[SEQTREE_BRANCHES];
		int keyval;
	} u;
} Seqtree_node;

typedef struct {
	Seqtree_node* tree;  /* branching nodes */
	int tree_size;
	int tree_head;  /* index of next free entry in tree */
	Seqtree_keyval* pt;  /* keyval table */
	int pt_size;
	int pt_head;  /* index of next free entry in keyval table */
} Seqtree;

typedef int Seqtree_iterator;

void seqtree_init(Seqtree* seqtree, int size);
int seqtree_put(Seqtree* seqtree, unsigned int id, const char* key);
Seqtree_iterator seqtree_get(Seqtree* seqtree, Seqtree_iterator iterator,
	const char* key, Seqtree_keyval** keyval);
void seqtree_delete(Seqtree* seqtree);

int seqtree_remove(Seqtree* seqtree, const char* key);

void seqtree_debug(Seqtree* seqtree);

#endif
