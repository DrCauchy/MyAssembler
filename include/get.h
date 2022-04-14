#ifndef _GET_H
#define _GET_H

#include "seqhash.h"
#include "seqtree.h"
#include "seqlist.h"
#include "contigreads.h"
#include "opt.h"

int geterrseqs(Seqtree* bin, const char* seq_to_find, Seqlist* found,
	Seqlist* this_frame, int index);

int getframes(Seqlist* found, const char* pass_key);

int getoverlapseqs(Seqtree* bin, const char* seq_to_find, Seqlist* found,
	Seqlist* this_frame, int index);

#endif
