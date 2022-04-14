#ifndef _SEQHASH_H
#define _SEQHASH_H

#include "config.h"

#define SEQHASH_KEYSIZE SEQREAD_SIZE

typedef struct {
	int value;          /* number of occurrences of this read */
//	int consumed_cnt;   /* number of times read has been marked as consumed  */
	char key[SEQHASH_KEYSIZE];
	int _next;  /* index of next key in bucket */
} Seqhash_keyval;

typedef struct {
	int* ht;  /* hash table with indexes into keyval table */
	int ht_size;  /* size of hash table */
	Seqhash_keyval* pt;  /* keyval table containing linked lists */
	int pt_size;  /* size of keyval table */
	int pt_head;  /* index of next free entry in keyval table */
} Seqhash;

void seqhash_init(Seqhash* seqhash, int size);
void seqhash_delete(Seqhash* seqhash);

int seqhash_put(Seqhash* seqhash, const char* key);
void seqhash_remove(Seqhash* seqhash, const char* key);
//void seqhash_consume(Seqhash *seqhash, const char *key);
int seqhash_get(Seqhash* seqhash, const char* key);
//int seqhash_get_consumed_cnt(Seqhash *seqhash, const char *key);
void seqhash_trim(Seqhash* seqhash);

void seqhash_dump(Seqhash* seqhash);

#endif
