#ifndef _CONTIG_H
#define _CONTIG_H

#include "config.h"
#include "seqtree.h"
#include "seqhash.h"
#include "shindex.h"
#include "opt.h"
#include "contigreads.h"
#include "readlist.h"

// value for unknown quality of a base pair, must be less than zero
#define UNKNOWN_QUAL (-1)

// value for unknown cover of a base pair, must be less than zero
#define UNKNOWN_COVER (-1)

typedef struct {
	int id;
	int seed_id;
	int contig_qual[CONTIG_SIZE];
	int contig_cover[CONTIG_SIZE];
	char contig[CONTIG_SIZE];
	char seed_read[SEQREAD_SIZE];
	Contigreads* reads;  // used if we're mapping contigs to reads, else null
} Contiginfo;

void process_finished_contig(Contiginfo* contig_info, Opt* opt);
Contiginfo* get_contig_seed(Seqhash* set, Shindex* set_index, Seqtree* bin,
	Readlist* list, Opt* opt);
void finalize_contigs(void);
void initialize_contigs(Seqhash* set, Shindex* set_index, Seqtree* bin,
	Opt* opt);
void reverse_contig(Contiginfo* contig_info);
void write_reads_amos(Readlist* readlist, Opt* opt);
#endif
