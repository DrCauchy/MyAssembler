#include <string.h>
#include "get.h"
#include "nts.h"
#include "seqlist.h"

int geterrseqs(Seqtree* bin, const char* seq_to_find, Seqlist* found,
	Seqlist* this_frame, int index)
{
	int numfound = 0;
	Seqtree_iterator bin_iter = 0;
	Seqtree_keyval* bin_kv;
	char pass_key[SEQREAD_SIZE];
	char* p;
	const char* q;
	int num_replicants;
	int pass_value;
	int pass_id;
	int mismatch;

	strncpy(pass_key, seq_to_find, SEQTREE_HEIGHT);
	while (1) {
		bin_iter = seqtree_get(bin, bin_iter, seq_to_find, &bin_kv);
		if (!bin_iter)
			break;
		strcpy(pass_key + SEQTREE_HEIGHT, bin_kv->key);
		pass_value = bin_kv->value;
		pass_id = bin_kv->id;

		if (strncmp(pass_key + SEQTREE_HEIGHT,
			seq_to_find + SEQTREE_HEIGHT,
			strlen(seq_to_find) - SEQTREE_HEIGHT) != 0) {
			mismatch = 0;
			p = pass_key + SEQTREE_HEIGHT;
			q = seq_to_find + SEQTREE_HEIGHT;
			while (*q && *p) {
				if (*q != *p)
					mismatch++;
				q++;
				p++;
			}
			if (mismatch <= 1) {
				if (getframes(found, pass_key) > 1)
					return -1;
				for (num_replicants = 0; num_replicants
					< pass_value; num_replicants++) {
					if (this_frame)
						seqlist_add(this_frame, pass_id, pass_key, 0);
					seqlist_add(found, pass_id, pass_key, index);
					numfound++;
				}
			}
		}
	}
	return numfound;
}

/**
 * Given a list of sequences, return the number of
 * times that a sequence is found in the list
 */
int getframes(Seqlist* found, const char* pass_key)
{
	Seqlist_keyval* found_kv;
	int found_len = seqlist_length(found);
	int test_if_multiple_frames = 0;
	int x = 0;

	while (x < found_len) {
		seqlist_get(found, x, &found_kv);
		if (strcmp(pass_key, found_kv->key) == 0)
			test_if_multiple_frames++;
		x++;
	}
	return test_if_multiple_frames;
}

int getoverlapseqs(Seqtree* bin, const char* seq_to_find, Seqlist* found,
	Seqlist* this_frame, int index)
{
	int numfound = 0;
	Seqtree_iterator bin_iter = 0;
	Seqtree_keyval* bin_kv;
	char pass_key[SEQREAD_SIZE];
	int num_replicants;
	int pass_value;
	int pass_id;

	strncpy(pass_key, seq_to_find, SEQTREE_HEIGHT);
	while (1) {
		bin_iter = seqtree_get(bin, bin_iter, seq_to_find, &bin_kv);
		if (!bin_iter)
			break;
		strcpy(pass_key + SEQTREE_HEIGHT, bin_kv->key);
		pass_value = bin_kv->value;
		pass_id = bin_kv->id;

		if (strncmp(pass_key + SEQTREE_HEIGHT,
			seq_to_find + SEQTREE_HEIGHT,
			strlen(seq_to_find) - SEQTREE_HEIGHT) == 0) {
			if (getframes(found, pass_key) > 1)
				return -1;
			for (num_replicants = 0; num_replicants < pass_value;
				num_replicants++) {
				if (this_frame)
					seqlist_add(this_frame, pass_id, pass_key, 0);
				seqlist_add(found, pass_id, pass_key, index);
				numfound++;
			}
		}
	}
	return numfound;
}