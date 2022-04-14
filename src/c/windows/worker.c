#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "getopt.h"
#include "worker.h"
#include "opt.h"
#include "contig.h"
#include "seqhash.h"
#include "seqtree.h"
#include "shindex.h"
#include "error.h"
#include "extend.h"
#include "nts.h"
#include "logger.h"
#include "seqlist.h"
#include "contigreads.h"

/**
 * Do the assemble for a contig
 * return 1 if successful, else unsuccessful
 */
static int assemble_contig(Seqtree* bin, Seqhash* set, Shindex* set_index,
	Opt* opt, Runstats* runstats,
	Contiginfo* ctginfo)
{
	int i;
	int seedlen = strlen(ctginfo->seed_read);

	log_info("Start assembly of contig %i, seed length %d\n",
		ctginfo->id, seedlen);

	// copy seed into contig and init quality
	memcpy(ctginfo->contig, ctginfo->seed_read, seedlen + 1);
	for (i = 0; i < seedlen; i++) {
		ctginfo->contig_qual[i] = UNKNOWN_QUAL;
		ctginfo->contig_cover[i] = UNKNOWN_COVER;
	}
	if (ctginfo->reads != NULL)
		contigread_add(ctginfo->reads, ctginfo->seed_id, 0,
			opt->tracelength - 1, 0);
	//������չ
	extend(bin, set, ctginfo, opt, runstats);
	////������չ
	//reverse_contig(ctginfo);
	//extend(bin, set, ctginfo, opt, runstats);

	log_info("Finished assembly of contig %i, length %d\n", ctginfo->id,
		strlen(ctginfo->contig));

	return 1;
}

/**
 * Worker front end, does any pre and post processing
 * arg0 must be an instane of WorkerInfo
 */
void* run_worker(void* arg0)
{
	Workerinfo* workerinfo = (Workerinfo*)arg0;

	// start main loop, we'll get contig seeds from set iterator and continue until no seeds are available
	Contiginfo* ctginfo = NULL;
	while (1) {
		log_info("Getting next contig for worker %d\n",
			workerinfo->id);
		ctginfo = get_contig_seed(workerinfo->set,
			workerinfo->set_index,
			workerinfo->bin,
			workerinfo->list,
			workerinfo->opt);
		if (ctginfo == NULL)
			break;

		int res = assemble_contig(workerinfo->bin, workerinfo->set,
			workerinfo->set_index,
			workerinfo->opt,
			workerinfo->runstats,
			ctginfo);
		if (res == 1) {
			update_assembled_contigs(workerinfo->runstats,
				ctginfo);
			process_finished_contig(ctginfo,
				workerinfo->opt);
		}
	}

	return NULL;
}