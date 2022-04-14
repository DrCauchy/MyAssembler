#ifndef _READLIST_H
#define _READLIST_H

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "opt.h"

#define SEQDESC_SIZE (100)
#define MINIMUM_ITEMS_SIZE (10)
#define SET_ITEMS_SIZE (2000)

typedef struct {
	char read[SEQREAD_SIZE];
	char desc[SEQDESC_SIZE];
} Readlist_item;

typedef struct {
	Readlist_item* items;  /* item array */
	int items_size;        /* size of item array */
	int next_item;         /* index of next free entry in item array */
} Readlist;

void fill_string(char* string, char fill, int count);
void readlist_init(Readlist* readlist, int size);
void readlist_delete(Readlist* readlist);
int  readlist_size(Readlist* readlist);
void readlist_clear(Readlist* readlist);
int  readlist_getid(Readlist* readlist, char* read);
void readlist_getitem(Readlist* readlist, int id, Readlist_item** item);
int  readlist_add(Readlist* readlist, const char* read, const char* desc);
void readlist_load(Readlist* readlist, const char* fn, int tossout);
void readlist_write(Readlist* readlist, FILE* contig_fp, Opt* opt);

#endif