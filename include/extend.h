#ifndef _EXTEND_H
#define _EXTEND_H

#include "seqhash.h"
#include "seqtree.h"
#include "opt.h"
#include "runstats.h"
#include "contig.h"

void extend(Seqtree* bin, Seqhash* set, Contiginfo* ctginfo, const Opt* opt,
	Runstats* runstats);

#endif
