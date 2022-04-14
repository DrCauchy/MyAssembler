#ifndef _RUNSTATS_H
#define _RUNSTATS_H

#include <time.h>
#include "opt.h"
#include "contig.h"
#include <stdio.h>

#define MAX_NUM_EXTEND_COVERAGE (500)

typedef struct {
	int num_assembled_contigs;
	int max_contig_len;
	double mean_contig_len;
	int sum_contig_lens;

	int num_contigfail_findreads1;  // fail on contig extend
	int num_contigfail_findreads2;  // fail on contig extend, based on min_lib
	int num_contigfail_findreads3;  // fail on contig extend, based on min_werr
	int num_contigfail_zeroreads;
	int num_contigfail_zerocountedreads;
	int num_contigfail_minverif;
	int num_contigfail_noconsensus;
	int num_contigfail_extend;
	int num_contigfail_coverage;

	// stats on coverage at each contig extension, extend_cvg[x] gives the
	// number of times we saw a coverage of x when extending a contig
	// extend_cvg[MAX_NUM...] holds the number of times we saw a
	// coverage of MAX_NUM or greater
	long extend_cnt;
	long total_extend_cvg;
	long extend_cvg[MAX_NUM_EXTEND_COVERAGE + 1];

	int num_workers;

	time_t startt;
	time_t endt;
	double time_span;

	float meancoverage;
	float varcoverage;
}Runstats;

void get_desc_stats(int* data, float* stats, int startpos, int endpos);

void runstats_init(Runstats* rs, Opt* opt);
void runstats_startrun(Runstats* rs);
void runstats_endrun(Runstats* rs);
void runstats_delete(Runstats* rs);
void runstats_report(Runstats* rs, FILE* statfp);

void update_assembled_contigs(Runstats* rs, Contiginfo* ctginfo);
void update_contigfail_findreads1(Runstats* rs, int amt);
void update_contigfail_findreads2(Runstats* rs, int amt);
void update_contigfail_findreads3(Runstats* rs, int amt);
void update_contigfail_zeroreads(Runstats* rs, int amt);
void update_contigfail_zerocountedreads(Runstats* rs, int amt);
void update_contigfail_minverif(Runstats* rs, int amt);
void update_contigfail_noconsensus(Runstats* rs, int amt);
void update_contigfail_extend(Runstats* rs, int amt);
void update_contigfail_coverage(Runstats* rs, int amt);
//void update_failed_extend(Runstats* rs, int amt);
//void update_extend_stats(Runstats* rs, int seq_len, int num_reads);

#endif
