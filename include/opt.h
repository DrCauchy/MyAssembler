#ifndef _OPT_H
#define _OPT_H

#include "config.h"

typedef struct {
	int help;  /* 1 means print help/usage */
	char* read_fn;  /* required, name of input sequences */
	char* contig_fn;  /* required, name of output contig file*/
	char* revread_fn; /* required, name of the reverse input sequence file */
	char* seed_fn;/* required, name of seed sequence file */
	char* output_stats_fn;  /* filename for stats, NULL if don't output stats */
	char* output_qual_fn;  /* filename for quality scores, NULL if don't output scores */
	char* log_fn;  /* filename for logging */
	float consensus_thresh;
	int min_werr;
	int tracelength;  /* required */
	int min_conserv;
	int trip_to_lib;
	int min_lib;
	int min_contig_len;
	int quit_count;  /* 0 means never quit */
	int min_verif;
	int coverage;
	int repcoverage;  /* extends over this level of coverage */
	int num_workers;  /* number of worker threads to use, default is 1 */
	int term_low_reads;  /* terminate extension when there is only this number of fewer reads voting (-1 cancels) */
	//int amos;  /* 1 = output to amos file format */
	//int autoxy;  /* 1 enables automatic -x/-y parameter setting */
	//int autoxy_sdx;  /* the number of standard deviations to the right of mean,
	//			when automatically setting x */
	//int autoxy_sdy;  /* the number of standard deviations to the left of (2 * mean),
				//when automatically setting y */
				/* The following are not user options but are set internally: */
	char tmp_contig_fn[PATHSIZE];
	char tmp_output_stats_fn[PATHSIZE];
	char tmp_output_qual_fn[PATHSIZE];
} Opt;

void opt_init(Opt* opt, int argc, char* const argv[]);
void opt_delete(Opt* opt);

void opt_print_help();

#endif
