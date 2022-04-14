#ifndef _CONTIGREADS_H
#define _CONTIGREADS_H

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "opt.h"

#define MINIMUM_CONTIGREADS_SIZE (10)
#define SET_CONTIGREAD_SIZE (100)

typedef struct {
	int readid;
	int readstart;
	int readend;
	int contigstart;
} Contigread_item;

typedef struct {
	Contigread_item* items;  /* item array */
	int items_size;          /* size of item array */
	int next_item;           /* index of next free entry in item array */
} Contigreads;

void contigread_init(Contigreads* contigreads, int size);
void contigread_delete(Contigreads* contigreads);
int  contigread_size(Contigreads* contigreads);
void contigread_clear(Contigreads* contigreads);
void contigread_get(Contigreads* contigreads, int index,
	Contigread_item** item);
void contigread_add(Contigreads* contigreads, int readid, int readstart,
	int readend, int contigstart);
void contigread_add_distinct(Contigreads* contigreads, int readid,
	int readstart, int readend, int contigstart);
void contigread_reverse(Contigreads* contigreads, int contiglen);
void contigread_extend(Contigreads* contigreads, const char* eseq,
	const char* seq, int value, int tracelength, int id);

#endif
