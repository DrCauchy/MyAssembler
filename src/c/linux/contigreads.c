#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "contigreads.h"
#include "error.h"
#include "opt.h"
#include "strrstr.h"

static int min_item(int size)
{
	return size < MINIMUM_CONTIGREADS_SIZE ?
		MINIMUM_CONTIGREADS_SIZE : size;
}

static void resize_items(Contigreads* contigreads, int size)
{
	contigreads->items_size = min_item(size);
	contigreads->items = (Contigread_item*)realloc
	(contigreads->items, contigreads->items_size * sizeof(Contigread_item));
	err_assert_mem(contigreads->items);
}

static void grow_items(Contigreads* contigreads)
{
	resize_items(contigreads, contigreads->items_size * 2);
}

static void ensure_next_item(Contigreads* contigreads)
{
	if (contigreads->next_item >= contigreads->items_size) {
		grow_items(contigreads);
	}
}



void contigread_init(Contigreads* contigreads, int size)
{
	contigreads->items = NULL;
	contigreads->items_size = size;
	contigreads->next_item = 1;
	resize_items(contigreads, size);
}





void contigread_extend(Contigreads* contigreads, const char* eseq,
	const char* seq, int value, int tracelength, int id)
{
	int sublen, estart, elen;
	char* substart;
	char subseq[SEQREAD_SIZE];

	elen = strlen(eseq);
	sublen = strlen(seq) - value;
	strncpy(subseq, seq, sublen);
	subseq[sublen] = '\0';
	substart = strrstr(eseq, subseq);
	estart = substart - eseq;
	sublen += 2;
	sublen = sublen > tracelength ? tracelength : sublen;
	sublen = estart + sublen > elen ? elen - estart : sublen;
	strncpy(subseq, seq, sublen);
	subseq[sublen] = '\0';
	substart = strrstr(eseq, subseq);

	if (!substart)
		sublen--;

	contigread_add_distinct(contigreads, id, 0, sublen, estart);
}

void contigread_add_distinct(Contigreads* contigreads, int readid,
	int readstart, int readend, int contigstart)
{
	int id, readstart1, readend1, readstart2, readend2;
	int contigstart1, contigstart2;
	int not_found = 1;

	for (id = 0; id < contigreads->next_item; id++) {
		if (abs(readid) == abs(contigreads->items[id].readid)) {
			not_found = 0;

			if (readstart > contigreads->items[id].readstart) {
				readstart1 = contigreads->items[id].readstart;
				readend1 = contigreads->items[id].readend;
				readstart2 = readstart;
				readend2 = readend;
				contigstart1 =
					contigreads->items[id].contigstart;
				contigstart2 = contigstart;
			}
			else {
				readstart1 = readstart;
				readend1 = readend;
				readstart2 = contigreads->items[id].readstart;
				readend2 = contigreads->items[id].readend;
				contigstart1 = contigstart;
				contigstart2 =
					contigreads->items[id].contigstart;
			}

			if (readstart2 == readend1 + 1) {
				contigreads->items[id].readstart = readstart1;
				contigreads->items[id].readend = readend2;
				contigreads->items[id].contigstart =
					contigstart1;
			}
			else {
				if ((readend1 - readstart1) >=
					(readend2 - readstart2)) {
					contigreads->items[id].readstart =
						readstart1;
					contigreads->items[id].readend =
						readend1;
					contigreads->items[id].contigstart =
						contigstart1;
				}
				else {
					contigreads->items[id].readstart =
						readstart2;
					contigreads->items[id].readend =
						readend2;
					contigreads->items[id].contigstart =
						contigstart2;
				}
			}
			break;
		}
	}

	if (not_found) {
		contigread_add(contigreads, readid, readstart, readend,
			contigstart);
	}
}

void contigread_add(Contigreads* contigreads, int readid, int readstart,
	int readend, int contigstart)
{
	ensure_next_item(contigreads);

	contigreads->items[contigreads->next_item].readid = readid;
	contigreads->items[contigreads->next_item].readstart = readstart;
	contigreads->items[contigreads->next_item].readend = readend;
	contigreads->items[contigreads->next_item].contigstart = contigstart;

	contigreads->next_item++;
}



void contigread_reverse(Contigreads* contigreads, int contiglen)
{
	int id, save, contigstart;

	for (id = 1; id < contigreads->next_item; id++) {
		contigstart = contigreads->items[id].contigstart;
		contigreads->items[id].contigstart =
			contiglen - contigreads->items[id].contigstart
			- (contigreads->items[id].readend
				- contigreads->items[id].readstart);
		save = contigreads->items[id].readstart;
		contigreads->items[id].readstart =
			contigreads->items[id].readend;
		contigreads->items[id].readend = save;
	}
}

void contigread_delete(Contigreads* contigreads)
{
	free(contigreads->items);
}

void contigread_clear(Contigreads* contigreads)
{
	contigreads->next_item = 1;
}

int contigread_size(Contigreads* contigreads)
{
	return contigreads->next_item;
}