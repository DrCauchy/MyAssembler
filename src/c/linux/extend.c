#include <stdio.h>
#include <string.h>
#include "extend.h"
#include "config.h"
#include "seqlist.h"
#include "contigreads.h"
#include "nts.h"
#include "logger.h"
#include "get.h"
#include "read.h"
#include "strrstr.h"

static int perl_style_offset(int offset, int len)
{
	if (offset < 0)
		offset = len - offset;
	if (offset < 0)
		offset = 0;
	return offset;
}


static int vote(int* counter, char nt)
{
	switch (nt) {
	case '\1':
	case '\2':
	case '\3':
	case '\4':
		counter[(int)nt]++;
		return 0;
	default:
		return -1;
	}
}

/**
 * Count the votes for each base call for reads in matches.  This
 * assumes that the value for each read in matches holds the index
 * of the read within a contig
 *
 * Returns the number of reads that contributed to the counts
 */
static int count_votes(int* counter, Seqlist* matches, int span)
{
	int num_reads = seqlist_length(matches);
	Seqlist_keyval* kv;
	int x;

	// note that kv->value holds the index that the read matched the contig
	int num_counted_reads = num_reads;
	for (x = 0; x < num_reads; x++) {
		seqlist_get(matches, x, &kv);
		if (span - kv->value >= strlen(kv->key)) {
			num_counted_reads--;
		}
		else {
			if (vote(counter, kv->key[span - kv->value]) == -1)
				num_counted_reads--;
		}
	}
	return num_counted_reads;
}

static int get_max_index(const int* counter)
{
	int maxi = 1;
	int max = counter[maxi];
	int x, cx;
	for (x = 2; x <= 4; x++) {
		cx = counter[x];
		if (cx > max) {
			max = cx;
			maxi = x;
		}
	}
	return maxi;
}

static int get_runner_up(const int* counter, int maxindex)
{
	int x, cx;
	int rmax = -1;

	for (x = 1; x <= 4; x++) {
		if (x == maxindex)
			continue;
		cx = counter[x];
		if (cx > rmax)
			rmax = cx;
	}
	return rmax;
}

static int loopseqs(int errseqs, int trip_to_lib, Seqtree* bin,
	const char* subseq, int offset, int length,
	Seqlist* matches)
{
	const char* s = subseq + offset;
	int(*getseqs)(Seqtree*, const char*, Seqlist*, Seqlist*, int) =
		errseqs ? geterrseqs : getoverlapseqs;

	while (1) {
		if (offset > length)
			break;
		if (trip_to_lib >= 0 && seqlist_length(matches) >= trip_to_lib)
			break;
		if (getseqs(bin, s, matches, 0, offset) == -1)
			return -1;
		if (!*s)
			break;
		s++;
		offset++;
	}
	return seqlist_length(matches);
}

static void deleteseqs(Seqtree* bin, Seqhash* set, Seqlist* matches,
	Seqhash* used_reads, Contiginfo* ctginfo,
	const char* eseq, int tracelength)
{
	int matches_len = seqlist_length(matches);
	const char* lastseq = "";
	const char* seq;
	Seqlist_keyval* kv;
	int x, id, value;

	for (x = 0; x < matches_len; x++) {
		seqlist_get(matches, x, &kv);
		seq = kv->key;
		id = kv->id;
		value = kv->value;
		if (ctginfo && ctginfo->reads != NULL)
			contigread_extend(ctginfo->reads, eseq, seq,
				value, tracelength, id);

		if (strcmp(seq, lastseq) == 0)
			continue;
		if (used_reads) {
			seqhash_put(used_reads, seq);
			if (seqhash_get(used_reads, seq) > 2 * tracelength + 2)
				deleteread(bin, set, seq);
		}
		if (strstr(eseq, seq))
			deleteread(bin, set, seq);
		lastseq = seq;
	}
}

static int extend_by_one(int maxindex, char* seq, int seq_len)
{
	char* p = seq + seq_len;
	if (maxindex == 1)
		*p = '\1';
	else if (maxindex == 4)
		*p = '\4';
	else if (maxindex == 3)
		*p = '\3';
	else if (maxindex == 2)
		*p = '\2';
	else
		return 0;
	*(++p) = '\0';
	return 1;
}


void extend(Seqtree* bin, Seqhash* set, Contiginfo* ctginfo, const Opt* opt,
	Runstats* runstats)
{
	int tracelength = opt->tracelength;
	int min_conserv = opt->min_conserv;
	int trip_to_lib = opt->trip_to_lib;
	int min_lib = opt->min_lib;
	int min_werr = opt->min_werr;
	int min_verif = opt->min_verif;
	int min_voting_reads = opt->term_low_reads;
	int coverage = opt->coverage;
	int repcoverage = opt->repcoverage;
	float consensus_thresh = opt->consensus_thresh;
	Seqhash used_reads;
	int span;
	char* subseq;
	Seqlist matches;
	int num_reads;
	int counter[5];

	// get current length
	char* seq = ctginfo->contig;
	int* seq_qual = ctginfo->contig_qual;
	int* seq_cov = ctginfo->contig_cover;
	int seq_len = strlen(seq);

	// allocate local mem
	seqhash_init(&used_reads, USED_READS_HT_SIZE);
	seqlist_init(&matches, MATCHES_LIST_SIZE);

	/* delete up to the relevant overlap region */
	loopseqs(0, -1, bin, seq, 0, seq_len - tracelength, &matches);
	deleteseqs(bin, set, &matches, NULL, NULL, seq, 0);
	seqlist_clear(&matches);

	/* extend the contig */
	while (1) {

		// init lookups
		span = tracelength >= seq_len ? seq_len : tracelength + 1;
		subseq = seq + seq_len - span;

		// get matching reads, if failed then quit this contig
		num_reads = loopseqs(0, -1, bin, subseq, 0, span - min_conserv,
			&matches);
		if (num_reads < 0) {
			update_contigfail_findreads1(runstats, 1);
			goto done;
		}

		// if too few matching reads, then reduce the match
		// region and try to get more, if fails, then quit this contig
		if (num_reads < trip_to_lib) {
			int tmps = perl_style_offset(span - min_conserv + 1,
				span);
			num_reads = loopseqs(0, trip_to_lib, bin, subseq, tmps,
				span - min_lib, &matches);
			if (num_reads < 0) {
				update_contigfail_findreads2(runstats, 1);
				goto done;
			}
		}

		// if user asked to match with mismatches, then reduce
		// the match region and try to get more matching
		// reads.  If fails, then quit this contig
		if (min_werr && (num_reads < trip_to_lib)) {
			num_reads = loopseqs(1, trip_to_lib, bin, subseq, 0,
				span - min_werr, &matches);
			if (num_reads < 0) {
				update_contigfail_findreads3(runstats, 1);
				goto done;
			}
		}

		// see how many matching reads we found, if none then
		// quit this contig
		num_reads = seqlist_length(&matches);
		if (num_reads <= 0) {
			update_contigfail_zeroreads(runstats, 1);
			goto done;
		}

		// at this point we can vote on what the next base
		// call should be after some checks on the vote results
		counter[1] = 0;
		counter[2] = 0;
		counter[3] = 0;
		counter[4] = 0;
		int num_counted_reads = count_votes(counter, &matches, span);
		if (num_counted_reads <= min_voting_reads) {
			update_contigfail_zerocountedreads(runstats, 1);
			goto done;
		}
		int maxi = get_max_index(counter);
		float consensus = (float)counter[maxi] / num_counted_reads;
		if (consensus <= consensus_thresh) {
			update_contigfail_noconsensus(runstats, 1);
			goto done;
		}
		if (min_verif && get_runner_up(counter, maxi) >= min_verif) {
			update_contigfail_minverif(runstats, 1);
			goto done;
		}
		if (coverage != 0 && num_counted_reads >= coverage) {
			update_contigfail_coverage(runstats, 1);
			goto done;
		}
		if (num_counted_reads <= repcoverage) {
			update_contigfail_coverage(runstats, 1);
			goto done;
		}
		/* Ensure that the contig buffer is not full, before
		 * extending. */
		if ((seq_len + 1) >= CONTIG_SIZE) {
			fprintf(stderr, "*** Maximum contig length exceeded"
				" (increase CONTIG_SIZE in config.h)"
				" - aborting\n");
			exit(-1);
		}
		if (!extend_by_one(maxi, seq, seq_len)) {
			update_contigfail_extend(runstats, 1);
			goto done;
		}

		// set the quality score and coverage for the extended marker
		*(seq_cov + seq_len) = num_counted_reads;
		if (num_counted_reads > 2 && (consensus > 0.8)) {
			*(seq_qual + seq_len) = 40;
		}
		else if (num_counted_reads > 2) {
			*(seq_qual + seq_len) = 30;
		}
		else if (num_counted_reads == 2) {
			*(seq_qual + seq_len) = 20;
		}
		else if (num_counted_reads == 1) {
			*(seq_qual + seq_len) = 10;
		}
		else {
			*(seq_qual + seq_len) = 0;
		}
		seq_len++;

		// and delete all used up reads
		deleteseqs(bin, set, &matches, &used_reads, ctginfo, seq,
			tracelength);
		seqlist_clear(&matches);
	}

done:
	seqlist_delete(&matches);
	seqhash_delete(&used_reads);
}

