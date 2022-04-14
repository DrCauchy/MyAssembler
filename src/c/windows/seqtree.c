#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "seqtree.h"
#include "lookup3.h"
#include "nts.h"
#include "error.h"

static void ensure_pt_size(Seqtree* seqtree)
{
	if (seqtree->pt_head >= seqtree->pt_size) {
		seqtree->pt_size *= 2;
		seqtree->pt = (Seqtree_keyval*)realloc(
			seqtree->pt, seqtree->pt_size * sizeof(Seqtree_keyval)
		);
		err_assert_mem(seqtree->pt);
	}
}

static Seqtree_node* find_leaf(Seqtree* seqtree, const char* key, int create)
{
	const char* k = key;
	int c = 0;
	Seqtree_node* p = seqtree->tree;
	Seqtree_node* op = NULL;

	while (p && !p->utype) {
		op = p;
		p = p->u._next[(*k) - 1];
		k++;
		c++;
	}
	if (!p && create) {
		p = op;
		k--;
		c--;
		while (c < SEQTREE_HEIGHT) {
			p->u._next[(*k) - 1] =
				seqtree->tree + seqtree->tree_head;
			p = seqtree->tree + seqtree->tree_head;
			seqtree->tree_head++;
			k++;
			c++;
		}
		p->utype = 1;
		p->u.keyval = 0;
	}
	return p;
}

static int scan_keyval(Seqtree* seqtree, Seqtree_node* leaf,
	const char* key, int create)
{
	int k = leaf->u.keyval;
	const char* key_skip = key + SEQTREE_HEIGHT;

	if (k) {
		do {
			if (strcmp(key_skip, seqtree->pt[k].key) == 0)
				return k;
			k = seqtree->pt[k]._next;
		} while (k);
	}

	if (create) {
		k = seqtree->pt_head;
		seqtree->pt_head++;
		ensure_pt_size(seqtree);
		seqtree->pt[k]._next = leaf->u.keyval;
		leaf->u.keyval = k;
		seqtree->pt[k].value = 0;
		strcpy(seqtree->pt[k].key, key_skip);
		return k;
	}
	else {
		return 0;
	}
}

static int find_keyval(Seqtree* seqtree, const char* key, int create)
{
	Seqtree_node* leaf = find_leaf(seqtree, key, create);

	if (leaf)
		return scan_keyval(seqtree, leaf, key, create);
	else
		return 0;
}




long node_table_size()//���ع�ϣ����С
{
	/* calculate # of nodes as geometric series */
	return (powl(SEQTREE_BRANCHES, (long long)SEQTREE_HEIGHT + 1) - 1)
		/ ((long long)SEQTREE_BRANCHES - 1);
}

void seqtree_init(Seqtree* seqtree, int size)
{
	seqtree->tree_size = node_table_size();
	seqtree->tree = (Seqtree_node*)calloc(seqtree->tree_size, sizeof(Seqtree_node));
	err_assert_mem(seqtree->tree);
	seqtree->tree_head = 1;
	seqtree->pt_size = size;  /* skip element 0 */
	seqtree->pt = (Seqtree_keyval*)malloc(seqtree->pt_size * sizeof(Seqtree_keyval));
	err_assert_mem(seqtree->pt);
	seqtree->pt_head = 1;
}

/* assumes strlen(key) >= SEQTREE_BRANCHES  ������״��ϣ*/
int seqtree_put(Seqtree* seqtree, unsigned int id, const char* key)
{
	int kv;

	err_assert_str(key, SEQTREE_KEYSIZE);

	kv = find_keyval(seqtree, key, 1);
	seqtree->pt[kv].value++;
	seqtree->pt[kv].id = id;
	return 0;
}

int seqtree_remove(Seqtree* seqtree, const char* key)
{
	int keyval = find_keyval(seqtree, key, 0);

	if (keyval)
		seqtree->pt[keyval].value = 0;
	return 0;
}






Seqtree_iterator seqtree_get(Seqtree* seqtree, Seqtree_iterator iterator,
	const char* key, Seqtree_keyval** keyval)
{
	int kv;
	Seqtree_node* leaf;

	if (iterator) {
		kv = seqtree->pt[iterator]._next;
	}
	else {
		leaf = find_leaf(seqtree, key, 0);
		kv = leaf ? leaf->u.keyval : 0;
	}

	while (1) {
		if (!kv) {
			*keyval = NULL;
			break;
		}
		if (seqtree->pt[kv].value != 0) {
			*keyval = seqtree->pt + kv;
			break;
		}
		kv = seqtree->pt[kv]._next;
	}

	return kv;
}




void seqtree_delete(Seqtree* seqtree)
{
	free(seqtree->tree);
	free(seqtree->pt);
}