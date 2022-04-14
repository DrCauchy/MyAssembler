#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "read.h"
#include "error.h"
#include "logger.h"
#include "nts.h"
#include "trim.h"

void readbin(Seqtree* bin, const char* fn, const char* re_fn, int tossout)
{
	FILE* fp;
	FILE* re_fp;
	char* line;
	char* revline;
	int linenum = 0;
	unsigned int id = 0;


	line = (char *)malloc(CONTIG_SIZE);
	revline = (char *)malloc(CONTIG_SIZE);
	err_assert_mem(line);
	err_assert_mem(revline);

	log_info("Loading bin\n");

	//��ȡ�����ļ�
	fp = fopen(fn, "r");
	if (!fp) {
		log_error("%s: %s: %s\n", err_argv0, fn, strerror(errno));
		abort();
	}

	while (fgets(line, CONTIG_SIZE, fp)) {
		linenum++;
		if (line[0] == '>')
			continue;
		id++;
		if (nts_encode(n_trim(line)))//����
			continue;
		if (strlen(line) < tossout)
			continue;
		if (tossout)
			line[tossout] = '\0';
		if (*line == '\0')
			continue;
		//nts_revcomp(line, revline);
		if (seqtree_put(bin, id, line)
			/*|| seqtree_put(bin, (0 - id), revline)*/) {
			if (strlen(line) >= SEQTREE_KEYSIZE) {
				log_error("%s: %s: %i: Sequence too long"
					" (maximum length = %i)\n",
					err_argv0, fn,
					linenum, SEQTREE_KEYSIZE);
				abort();
			}
		}
	}
	if (ferror(fp)) {
		log_error("%s: %s: %s\n", err_argv0, fn, strerror(errno));
		abort();
	}

	//��ȡ�����ļ�
	re_fp = fopen(re_fn, "r");
	if (!re_fp) {
		log_error("%s: %s: %s\n", err_argv0, re_fn, strerror(errno));
		abort();
	}

	linenum = 0;
	
	while (fgets(revline, CONTIG_SIZE, re_fp)) {
		linenum++;
		if (revline[0] == '>')
			continue;
		id++;
		if (nts_encode(n_trim(revline)))//����
			continue;
		if (strlen(revline) < tossout)
			continue;
		if (tossout)
			revline[tossout] = '\0';
		if (*revline == '\0')
			continue;
		//nts_revcomp(line, revline);
		if (seqtree_put(bin, id, revline)) {
			if (strlen(revline) >= SEQTREE_KEYSIZE) {
				log_error("%s: %s: %i: Sequence too long"
					" (maximum length = %i)\n",
					err_argv0, fn,
					linenum, SEQTREE_KEYSIZE);
				abort();
			}
		}
	}
	if (ferror(re_fp)) {
		log_error("%s: %s: %s\n", err_argv0, re_fn, strerror(errno));
		abort();
	}

	fclose(fp);
	fclose(re_fp);
	free(line);
	free(revline);
}

//set��ϣ��ֻ�洢���г���Ƶ��
void readset(Seqhash* set, const char* fn)
{
	FILE* fp;
	char* line;
	int linenum = 0;

	line = (char *)malloc(CONTIG_SIZE);
	err_assert_mem(line);

	log_info("Now reading SET\n");
	fp = fopen(fn, "r");
	if (!fp) {
		fprintf(stderr, "%s: %s: %s\n", err_argv0, fn,
			strerror(errno));
		abort();
	}

	while (fgets(line, CONTIG_SIZE, fp)) {
		linenum++;
		if (line[0] == '>')
			continue;
		if (nts_encode(n_trim(line)))//�����ֶԼ������
			continue;
		if (*line == '\0')
			continue;
		if (seqhash_put(set, line)) {
			if (strlen(line) >= SEQHASH_KEYSIZE) {
				fprintf(stderr, "%s: %s: %i: Sequence too long"
					" (maximum length = %i)\n",
					err_argv0, fn,
					linenum, SEQHASH_KEYSIZE);
				abort();
			}
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "%s: %s: %s\n", err_argv0, fn,
			strerror(errno));
		abort();
	}

	fclose(fp);
	free(line);
}


/**
 * Delete a read from seqtree and from seqhash
 */
void deleteread(Seqtree* bin, Seqhash* set, const char* nts)
{
	char rev_nts[SEQREAD_SIZE];

	err_assert_str(nts, sizeof rev_nts);

	/*nts_revcomp(nts, rev_nts);*/
	seqtree_remove(bin, nts);
	//seqtree_remove(bin, rev_nts);
	seqhash_remove(set, nts);
	//seqhash_remove(set, rev_nts);
}