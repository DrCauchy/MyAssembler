#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "readlist.h"
#include <errno.h>
#include "error.h"
#include "trim.h"
#include "nts.h"
#include "logger.h"

static int min_item(int size)
{
	return size < MINIMUM_ITEMS_SIZE ? MINIMUM_ITEMS_SIZE : size;
}

static void resize_items(Readlist* readlist, int size)
{
	readlist->items_size = min_item(size);
	readlist->items = (Readlist_item *)realloc(readlist->items, readlist->items_size
		* sizeof(Readlist_item));
	err_assert_mem(readlist->items);
}

void readlist_init(Readlist* readlist, int size)
{
	readlist->items = NULL;
	readlist->items_size = size;
	readlist->next_item = 1;
	resize_items(readlist, size);
}

void readlist_delete(Readlist* readlist)
{
	free(readlist->items);
}

static void grow_items(Readlist* readlist)
{
	resize_items(readlist, readlist->items_size * 2);
}

static void ensure_next_item(Readlist* readlist)
{
	if (readlist->next_item >= readlist->items_size) {
		grow_items(readlist);
	}
}

int readlist_size(Readlist* readlist)
{
	return readlist->next_item - 1;
}

int readlist_add(Readlist* readlist, const char* read, const char* desc)
{
	err_assert_str(read, SEQREAD_SIZE);
	err_assert_str(desc, SEQDESC_SIZE);
	ensure_next_item(readlist);

	//	int x = 0;
	//	if (desc == '\0' || *desc == '\0' || readlist->next_item == 2000)
	//		x++;

	strcpy(readlist->items[readlist->next_item].read, read);
	strcpy(readlist->items[readlist->next_item].desc, desc);

	readlist->next_item++;

	return readlist->next_item;
}

void readlist_getitem(Readlist* readlist, int id, Readlist_item** item)
{
	*item = NULL;

	if (id < readlist->next_item)
		*item = &(readlist->items[id]);
}

int readlist_getid(Readlist* readlist, char* read)
{
	int x;

	for (x = 1; x < readlist->next_item; x++) {
		if (strcmp(read, readlist->items[x].read) == 0)
			break;
	}
	return x;
}

void readlist_clear(Readlist* readlist)
{
	readlist->next_item = 1;
}

void readlist_load(Readlist* readlist, const char* fn, int tossout)
{
	FILE* fp;
	int linenum = 0;
	char read[SEQREAD_SIZE] = { '\0' };
	char desc[SEQDESC_SIZE] = { '\0' };

	log_info("Loading readlist\n");
	fp = fopen(fn, "r");

	if (!fp) {
		log_error("%s: %s: %s\n", err_argv0, fn, strerror(errno));
		abort();
	}

	while (fgets(read, SEQREAD_SIZE, fp)) {
		linenum++;
		n_trim(read);
		if (read[0] == '>') {
			strncpy(desc, n_trim(read + 1), SEQDESC_SIZE);
		}
		else if (strlen(read) >= tossout) {
			read[tossout] = '\0';
			readlist_add(readlist, read, desc);
		}
		else {
			readlist->next_item++;
			log_warning("Read length is less than -k value (%i) at line #%i; read ignored.  Read: %s - %s\n", tossout, linenum, desc, read);
		}
	}

	if (ferror(fp)) {
		log_error("%s: %s: %s\n", err_argv0, fn, strerror(errno));
		abort();
	}
}


void fill_string(char* string, char fill, int count)
{
	int x;

	for (x = 0; x < count; x++)
		string[x] = fill;

	string[x] = '\0';
}

void readlist_write(Readlist* readlist, FILE* contig_fp, Opt* opt)
{
	int x;

	char qdata[SEQREAD_SIZE];
	fill_string(qdata, 'D', opt->tracelength);

	for (x = 1; x < readlist->next_item; x++) {
		if (readlist->items[x].read != '\0' && (strlen(readlist->items[x].read) > 0)) {
			fprintf(contig_fp, "{RED\nclr:%i,%i\neid:%s\niid:%i\nqlt:\n%s\n.\nseq:\n%s\n.\n}\n",
				1, opt->tracelength, readlist->items[x].desc, x, qdata, readlist->items[x].read);
		}
	}
}
