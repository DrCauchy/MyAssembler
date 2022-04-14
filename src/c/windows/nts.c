#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "nts.h"

//����Ϊascii 1-4��
int nts_encode(char* str)
{
	char* s = str;
	while (1) {
		switch (*s) {
		case '\0':
			return 0;
		case 'A':
		case 'a':
			*s = '\1';
			break;
		case 'C':
		case 'c':
			*s = '\2';
			break;
		case 'G':
		case 'g':
			*s = '\3';
			break;
		case 'T':
		case 't':
			*s = '\4';
			break;
		default:
			return s - str + 1;
		}
		s++;
	}
}

int nts_decode(char* nts)
{
	char* s = nts;
	while (1) {
		switch (*nts) {
		case '\0':
			return 0;
		case '\1':
			*nts = 'A';
			break;
		case '\2':
			*nts = 'C';
			break;
		case '\3':
			*nts = 'G';
			break;
		case '\4':
			*nts = 'T';
			break;
		default:
			return s - nts + 1;
		}
		nts++;
	}
}

static char complement(char c)
{
	return (char)(5 - (int)c);
}


void nts_revcomp_ip(char* nts)//��������
{
	char* lp = nts;
	char* rp = nts + strlen(nts) - 1;
	while (lp < rp) {
		char tmpl = complement(*lp);
		*lp = complement(*rp);
		*rp = tmpl;
		lp++;
		rp--;
	}
	if (lp == rp)
		*lp = complement(*lp);
}


void nts_revcomp(const char* nts, char* revcomp)//���ɷ�������
{
	const char* p = nts + strlen(nts);

	while (p > nts) {
		p--;
		*revcomp = complement(*p);
		revcomp++;
	}
	*revcomp = '\0';
}

void nts_debug(const char* nts)
{
	char* s = strdup(nts);

	nts_decode(s);
	printf("%s", s);
	free(s);
}

char* nts_to_string(const char* nts)
{
	char* s = strdup(nts);
	nts_decode(s);
	return s;
}




