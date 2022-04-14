#ifndef _NTS_H
#define _NTS_H

int nts_encode(char* s);
int nts_decode(char* nts);

void nts_revcomp(const char* nts, char* revcomp);
void nts_revcomp_ip(char* nts);
void nts_rev_ip(char* nts);

void nts_debug(const char* nts);

char* nts_to_string(const char* nts);
#endif
