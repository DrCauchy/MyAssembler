#include "runstats.h"
#ifdef _WIN32
#include <windows.h> 
#endif
int compare_int(const void* a, const void* b)
{
	const int* ia = (const int*)a;  // casting pointer types
	const int* ib = (const int*)b;
	return *ia - *ib;
}

HANDLE g_hMutex = NULL;

/**
 * Create the runstats.  Should only be called once on a stat object
 */
void runstats_init(Runstats* rs, Opt* opt)
{
	//g_hMutex = CreateMutex(NULL, FALSE, NULL);//����������
	/*int res = pthread_mutex_init(&results_lock, NULL);
	if (res != 0)
		log_error(
			"Error return from result lock pthread_mutex_init, res %d\n",
			res);*/

	
	//��ʼ����ֵ
	rs->num_assembled_contigs = 0;
	rs->max_contig_len = 0;
	rs->mean_contig_len = 0;
	rs->sum_contig_lens = 0;

	rs->num_contigfail_findreads1 = 0;
	rs->num_contigfail_findreads2 = 0;
	rs->num_contigfail_findreads3 = 0;
	rs->num_contigfail_zeroreads = 0;
	rs->num_contigfail_zerocountedreads = 0;
	rs->num_contigfail_minverif = 0;
	rs->num_contigfail_noconsensus = 0;
	rs->num_contigfail_extend = 0;
	rs->num_contigfail_coverage = 0;
	rs->num_workers = opt->num_workers;
	rs->startt = 0;
	rs->endt = 0;
	rs->time_span = 0;

	for (int i = 0; i <= MAX_NUM_EXTEND_COVERAGE; i++)
		rs->extend_cvg[i] = 0;
}

void runstats_delete(Runstats* rs) {

}

void runstats_startrun(Runstats* rs)
{
	//WaitForSingleObject(g_hMutex, INFINITE);
	time(&(rs->startt));
	//ReleaseMutex(g_hMutex);
}

void get_desc_stats(int* data, float* stats, int startpos, int endpos)
{
	int i, si;

	for (i = 0; i < 5; i++)
		stats[i] = -1;

	int datalen = endpos - startpos + 1;
	int *sorteddata = (int *)malloc(datalen);
	int sortedlen = 0;
	for (si = startpos; si <= endpos; si++) {
		if (data[si] >= 0) {
			sorteddata[sortedlen] = data[si];
			sortedlen++;
		}
	}
	if (sortedlen == 0)
		return;

	qsort(sorteddata, sortedlen, sizeof(int), compare_int);
	float sum = sorteddata[0];
	float max = sorteddata[0];
	float min = sorteddata[0];
	for (i = 1; i < sortedlen; i++) {
		sum += sorteddata[i];
		if (sorteddata[i] > max)
			max = sorteddata[i];
		if (sorteddata[i] < min)
			min = sorteddata[i];
	}
	float mean = sum / sortedlen;

	float var = 0;
	for (i = 0; i < sortedlen; i++)
		var += (sorteddata[i] - mean) * (sorteddata[i] - mean);
	var /= sortedlen;

	int halfi = sortedlen / 2;
	float median = (sortedlen % 2 == 0) ?
		0.5 * (sorteddata[halfi] + sorteddata[halfi + 1])
		: sorteddata[halfi];

	stats[0] = mean;
	stats[1] = var;
	stats[2] = median;
	stats[3] = min;
	stats[4] = max;
}


void update_assembled_contigs(Runstats* rs, Contiginfo* ctginfo)
{
	int i;

	//pthread_mutex_lock(&results_lock);
	rs->num_assembled_contigs++;

	// update contig len stats
	int contig_len = strlen(ctginfo->contig);
	rs->sum_contig_lens += contig_len;
	if (contig_len > rs->max_contig_len)
		rs->max_contig_len = contig_len;

	// update coverage stats
	for (i = 0; i < contig_len; i++) {
		int val = ctginfo->contig_cover[i];
		if (val != UNKNOWN_COVER) {
			if (val >= MAX_NUM_EXTEND_COVERAGE)
				val = MAX_NUM_EXTEND_COVERAGE;
			rs->total_extend_cvg += val;
			rs->extend_cvg[val]++;
			rs->extend_cnt++;
		}
	}

	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_findreads1(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_findreads1++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_findreads2(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_findreads2++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_findreads3(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_findreads3++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_zeroreads(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_zeroreads++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_zerocountedreads(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_zerocountedreads++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_minverif(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_minverif++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_noconsensus(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_noconsensus++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_extend(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_extend++;
	//pthread_mutex_unlock(&results_lock);
}

void update_contigfail_coverage(Runstats* rs, int amt)
{
	//pthread_mutex_lock(&results_lock);
	rs->num_contigfail_coverage++;
	//pthread_mutex_unlock(&results_lock);
}

void runstats_endrun(Runstats* rs)
{
	int i, j;
	float meancoverage;
	float varcoverage;

	//pthread_mutex_lock(&results_lock);
	time(&(rs->endt));
	rs->time_span = difftime(rs->endt, rs->startt);
	//pthread_mutex_unlock(&results_lock);

	/* Compute derived results */
	rs->mean_contig_len = rs->sum_contig_lens / rs->num_assembled_contigs;
	meancoverage = (float)rs->total_extend_cvg / rs->extend_cnt;
	varcoverage = 0;
	for (i = 1; i <= MAX_NUM_EXTEND_COVERAGE; i++) {
		for (j = 0; j < rs->extend_cvg[i]; j++)
			varcoverage += (i - meancoverage) * (i - meancoverage);
	}
	varcoverage /= rs->extend_cnt;
	rs->meancoverage = meancoverage;
	rs->varcoverage = varcoverage;
}

void runstats_report(Runstats *rs, FILE *statfp)
{
	fprintf(statfp, "\nSequence Assembly Statistics\n");
	fprintf(statfp, "num_assembled_contigs: %d\n",
		rs->num_assembled_contigs);
	fprintf(statfp, "max_contig_len: %d\n", rs->max_contig_len);
	fprintf(statfp, "mean_contig_len: %f\n", rs->mean_contig_len);

	fprintf(statfp, "num_contigfail_findreads1: %d\n",
		rs->num_contigfail_findreads1);
	fprintf(statfp, "num_contigfail_findreads2: %d\n",
		rs->num_contigfail_findreads2);
	fprintf(statfp, "num_contigfail_findreads3: %d\n",
		rs->num_contigfail_findreads3);
	fprintf(statfp, "num_contigfail_zeroreads: %d\n",
		rs->num_contigfail_zeroreads);
	fprintf(statfp, "num_contigfail_zerocountedreads: %d\n",
		rs->num_contigfail_zerocountedreads);
	fprintf(statfp, "num_contigfail_minverif: %d\n",
		rs->num_contigfail_minverif);
	fprintf(statfp, "num_contigfail_noconsensus: %d\n",
		rs->num_contigfail_noconsensus);
	fprintf(statfp, "num_contigfail_extend: %d\n",
		rs->num_contigfail_extend);
	fprintf(statfp, "num_contigfail_coverage: %d\n",
		rs->num_contigfail_coverage);

	fprintf(statfp, "num workers: %d\n", rs->num_workers);
	fprintf(statfp, "run time %f\n", rs->time_span);

	fprintf(statfp, "mean coverage: %f\n", rs->meancoverage);
	fprintf(statfp, "variance coverage: %f\n", rs->varcoverage);
	fprintf(statfp, "\n\n");
}
