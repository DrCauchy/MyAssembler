#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqhash.h"
#include "error.h"
#include "lookup3.h"

static int hash(int table_size, const char* key)
{
	return hashlittle(key, strlen(key), 0) % table_size;
}

static Seqhash_keyval* find_key(Seqhash_keyval* pt, int index, const char* key)
{
	Seqhash_keyval* p;
	while (index) {
		p = pt + index;
		if (strcmp(p->key, key) == 0)
			return p;
		index = p->_next;
	}
	return NULL;
}

void seqhash_init(Seqhash* seqhash, int size)
{
	if ((size % 2) != 0)//sizeΪż��
		size++;
	seqhash->ht = (int*)calloc(size, sizeof(int));
	err_assert_mem(seqhash->ht);
	seqhash->ht_size = size;
	seqhash->pt_size = size + 1;
	seqhash->pt = (Seqhash_keyval*)malloc(
		seqhash->pt_size * sizeof(Seqhash_keyval));
	err_assert_mem(seqhash->pt);
	seqhash->pt_head = 1;  /* skip entry 0 (reserved to mean no entries) */
}

int seqhash_put(Seqhash* seqhash, const char* key)//����ֵ�����ϣ��
{
	Seqhash_keyval* new_p;
	int ht_x;
	Seqhash_keyval* f;

	err_assert_str(key, SEQHASH_KEYSIZE);

	ht_x = hash(seqhash->ht_size, key);
	f = find_key(seqhash->pt, seqhash->ht[ht_x], key);
	if (f) {  /* key is already in hash table */
		(f->value)++;
		return 0;
	}
	if (strlen(key) >= SEQHASH_KEYSIZE)
		return -1;
	if (seqhash->pt_head == seqhash->pt_size) {  /* ensure keyval table size */
		seqhash->pt_size *= 2;
		seqhash->pt = (Seqhash_keyval*)realloc(seqhash->pt, seqhash->pt_size * sizeof(Seqhash_keyval));
		err_assert_mem(seqhash->pt);
	}
	new_p = seqhash->pt + seqhash->pt_head;
	strcpy(new_p->key, key);
	new_p->value = 1;
	//new_p->consumed_cnt = 0;
	new_p->_next = seqhash->ht[ht_x];
	seqhash->ht[ht_x] = seqhash->pt_head;
	(seqhash->pt_head)++;
	return 0;
}

int seqhash_get(Seqhash* seqhash, const char* key)
{
	int ht_x;
	Seqhash_keyval* f;

	err_assert_str(key, SEQHASH_KEYSIZE);

	ht_x = hash(seqhash->ht_size, key);
	f = find_key(seqhash->pt, seqhash->ht[ht_x], key);
	return f ? f->value : 0;
}



void seqhash_remove(Seqhash* seqhash, const char* key)
{
	int ht_x;
	Seqhash_keyval* f;

	err_assert_str(key, SEQHASH_KEYSIZE);

	ht_x = hash(seqhash->ht_size, key);
	f = find_key(seqhash->pt, seqhash->ht[ht_x], key);
	if (f)
		f->value = 0;
}



void seqhash_delete(Seqhash* seqhash)
{
	free(seqhash->ht);
	free(seqhash->pt);
}