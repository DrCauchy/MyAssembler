#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <pthread.h>
#include "contig.h"
#include "opt.h"
#include "shindex.h"
#include "seqhash.h"
#include "seqlist.h"
#include "readlist.h"
#include "nts.h"
#include "error.h"
#include "logger.h"
#include "runstats.h"
#include "contigreads.h"
#include "readlist.h"
#include "read.h"
#include "contig.h"

/* Current value for the contig id, this needs to be thread safe */
static int contig_id = -999;

/* Flag to indicate to keep contig to read mapping */
static int map_reads_to_contigs = 0;

/* controls access to getting the next contig seed */
//static pthread_mutex_t contig_seed_lock;

/* controls access to output results */
//static pthread_mutex_t contig_results_lock;

/* our iterator into the read index */
static Shindex_iterator seq_iter;

/* file pointer for contig quality score */
static FILE* contig_qual_fp;

/* file pointer for contig output */
static FILE* contig_fp;


/**
 * Initialize contig creation, should only be called once
 */
void initialize_contigs(Seqhash* set, Shindex* set_index, Seqtree* bin,
	Opt* opt)
{
	//int res;
	Seqhash_keyval* kv;

	contig_id = 1;

	// create mutexes
	/*res = pthread_mutex_init(&contig_seed_lock, NULL);
	if (res != 0)
		log_error(
			"Error return from contig seed pthread_mutex_init, res %d\n",
			res);
	res = pthread_mutex_init(&contig_results_lock, NULL);
	if (res != 0)
		log_error(
			"Error return from contig results pthread_mutex_init, res %d\n",
			res);*/

	//map_reads_to_contigs = opt->amos;

	// init the sequence iterator
	seq_iter = shindex_get_first(set_index, &kv);

	// open the contig output file
	contig_fp = fopen(opt->contig_fn, "w");
	if (contig_fp == NULL) {
		log_error("Could not open contig output file %s\n", opt->contig_fn);
		// perror("Could not open contig output file %s\n", opt->contig_fn);
		exit(-1);
	}

	// open the quality control file
	if (opt->output_qual_fn != NULL) {
		contig_qual_fp = fopen(opt->output_qual_fn, "w");
		if (contig_qual_fp == NULL) {
			log_error(
				"Could not open contig quality output file %s\n",
				opt->output_qual_fn);
		}
	}
}

/**
 * Allocate a new vanilla contig info
*/

static Contiginfo* contiginfo_init()
{
	Contiginfo* ctginfo = (Contiginfo*)calloc(1, sizeof(Contiginfo));
	err_assert_mem(ctginfo);
	ctginfo->id = 0;
	ctginfo->seed_id = 0;
	ctginfo->contig[0] = '\0';
	ctginfo->seed_read[0] = '\0';
	if (map_reads_to_contigs) {
		ctginfo->reads = (Contigreads*)calloc(1,
			sizeof(Contigreads));
		contigread_init(ctginfo->reads, SET_CONTIGREAD_SIZE);
	}
	return ctginfo;
}


Contiginfo* get_contig_seed(Seqhash* set, Shindex* set_index, Seqtree* bin,
	Readlist* list, Opt* opt)
{

	//pthread_mutex_lock(&contig_seed_lock); /* lock updating the selected contig */

	// check user options
	if (opt->quit_count && contig_id > opt->quit_count) {
		log_info("Contig count reached max contig count %d - quitting\n", opt->quit_count);
		//pthread_mutex_unlock(&contig_seed_lock);			// now we can unlock
		return NULL;
	}

	Contiginfo* ctginfo = contiginfo_init();

	// continue to step through set until we find a non-consumed read
	//  which will be our contig seed
	Seqhash_keyval* kv;
	while (1) {
		seq_iter = shindex_get_next(set_index, &kv, seq_iter);
		if (seq_iter == 0) {
			//pthread_mutex_unlock(&contig_seed_lock);
			return NULL;
		}
		else if (kv->value > 0) //&& kv->consumed_cnt==0)
			break;
	}

	// new init the contig info
	ctginfo->id = contig_id++;
	strcpy(ctginfo->seed_read, kv->key);

	char readseq[SEQREAD_SIZE];
	strcpy(readseq, kv->key);
	nts_decode(readseq);
	if (map_reads_to_contigs)
		ctginfo->seed_id = readlist_getid(list, readseq);

	deleteread(bin, set, kv->key);
	//pthread_mutex_unlock(&contig_seed_lock);  // now we can unlock

	return ctginfo;
}


/**
 * Reverse the direction of a contig (and complement)
 */
//void reverse_contig(Contiginfo* ctginfo)
//{
//	int len = strlen(ctginfo->contig);
//
//	nts_revcomp_ip(ctginfo->contig);
//	reverse_int(ctginfo->contig_qual, len);
//	reverse_int(ctginfo->contig_cover, len);
//	if (ctginfo->reads != NULL)
//		contigread_reverse(ctginfo->reads, len);
//}

void reverse_int(int* nts, int len)
{
	int* lp = nts;
	int* rp = nts + len - 1;
	while (lp < rp) {
		int tmpl = *lp;
		*lp = *rp;
		*rp = tmpl;
		lp++;
		rp--;
	}
}

void finalize_contigs(void)
{
	/*int res = pthread_mutex_destroy(&contig_seed_lock);
	if (res != 0)
		log_error(
			"Error return from contig seed pthread_mutex_destroy, res %d\n",
			res);
	res = pthread_mutex_destroy(&contig_results_lock);
	if (res != 0)
		log_error(
			"Error return from contig results pthread_mutex_destroy, res %d\n",
			res);*/

	if (map_reads_to_contigs) {
		fprintf(contig_fp, "{FEA\n}\n");
	}

	fclose(contig_fp);
	if (contig_qual_fp != NULL) {
		fclose(contig_qual_fp);
	}
}

static void write_contig(Contiginfo* ctginfo, Opt* opt)
{
	int contiglen = strlen(ctginfo->contig);

	/*if (opt->amos) {
		int x, readid, readstart, readend, contigstart, offset;
		char* qdata = (char *)malloc(contiglen + 1);
		memset(qdata, 'D', contiglen);
		qdata[contiglen] = '\0';

		fprintf(contig_fp,
			"{CTG\neid:CONTIG_%i\niid:%i\nqlt:\n%s\n.\nseq:\n%s\n.\nsts:S\n\n",
			ctginfo->id, ctginfo->id, qdata,
			ctginfo->contig);

		for (x = 1; x < ctginfo->reads->next_item; x++) {
			if (abs(ctginfo->reads->items[x].readend - ctginfo->reads->items[x].readstart) == opt->tracelength) {
				readid = ctginfo->reads->items[x].readid;
				contigstart = ctginfo->reads->items[x].contigstart;

				if (readid < 0) {
					readid = abs(readid);
					offset = opt->tracelength
						- abs(ctginfo->reads->items[x].readstart
							- ctginfo->reads->items[x].readend);
					readstart = ctginfo->reads->items[x].readend + offset;
					readend = ctginfo->reads->items[x].readstart + offset;
				}
				else {
					readstart = ctginfo->reads->items[x].readstart;
					readend = ctginfo->reads->items[x].readend;
				}

				fprintf(contig_fp, "{TLE\nclr:%i,%i\noff:%i\nsrc:%i\n}\n",
					readstart, readend, contigstart, readid);
			}
		}
		fprintf(contig_fp, "}\n");
		free(qdata);
	}
	else {*/
	fprintf(contig_fp, ">Contig_%i length:%i\n%s\n",
		ctginfo->id, contiglen, ctginfo->contig);
	//}
}

static void contiginfo_delete(Contiginfo* ctginfo)
{
	if (map_reads_to_contigs) {
		contigread_delete(ctginfo->reads);
		free(ctginfo->reads);
	}
	free(ctginfo);
}


/**
 * Process a contig that's considered finished, this will deallocate the
 * associated memory and write the contig information to file
 */
void process_finished_contig(Contiginfo* ctginfo, Opt* opt)
{
	int i;

	int ext_len = strlen(ctginfo->contig);
	//pthread_mutex_lock(&contig_seed_lock);
	if (ext_len >= opt->min_contig_len) {

		// print out the contig
		nts_decode(ctginfo->contig);
		write_contig(ctginfo, opt);

		// print out the quality and coverage scores
		if (contig_qual_fp != NULL) {
			float stats[5];
			get_desc_stats(ctginfo->contig_qual, stats, 0, ext_len - 1);
			fprintf(contig_qual_fp,
				">ContigQuality_%i length:%i mean:%f var:%f median:%f min:%f max:%f\n",
				ctginfo->id, ext_len, stats[0], stats[1],
				stats[2], stats[3], stats[4]);
			for (i = 0; i < ext_len; i++)
				fprintf(contig_qual_fp, "%d|",
					ctginfo->contig_qual[i]);
			fprintf(contig_qual_fp, "\n");

			get_desc_stats(ctginfo->contig_cover, stats, 0, ext_len - 1);
			fprintf(contig_qual_fp,
				">ContigCoverage_%i length:%i mean:%f var:%f median:%f min:%f max:%f\n",
				ctginfo->id, ext_len, stats[0], stats[1],
				stats[2], stats[3], stats[4]);
			for (i = 0; i < ext_len; i++)
				fprintf(contig_qual_fp, "%d|",
					ctginfo->contig_cover[i]);
			fprintf(contig_qual_fp, "\n");
		}

	}

	contiginfo_delete(ctginfo);
	//pthread_mutex_unlock(&contig_seed_lock);
}



