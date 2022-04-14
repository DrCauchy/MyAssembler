#ifndef _WORKER_H
#define _WORKER_H

#include "seqtree.h"
#include "seqhash.h"
#include "shindex.h"
#include "readlist.h"
#include "opt.h"
#include "runstats.h"

/*
 * Data structure to pass workers to give them access to global data structures
 */
typedef struct {
	int id;
	Seqtree* bin;
	Seqhash* set;
	Readlist* list;
	Shindex* set_index;
	Opt* opt;
	Runstats* runstats;
} Workerinfo;

void* run_worker(void*);

#endif
