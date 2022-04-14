
#ifndef _READ_H
#define _READ_H

#include "seqhash.h"
#include "seqtree.h"

void readbin(Seqtree* bin, const char* fn, const char* re_fn, int tossout);
void readset(Seqhash* set, const char* fn);
void deleteread(Seqtree* bin, Seqhash* set, const char* nts);
//void consumeread(Seqtree *bin, Seqhash *set, const char *nts);
#endif