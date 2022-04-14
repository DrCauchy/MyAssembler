#include "opt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 #ifdef _WIN32
 #include "getopt.h"
 #endif
#ifdef __linux__
#include <getopt.h>
#endif
static void init(Opt* opt) {
	opt->help = 0;
	opt->contig_fn = NULL;

 	opt->revread_fn = NULL;
	opt->read_fn = NULL;
	opt->seed_fn = NULL;

	/*opt->revread_fn = "Assembled_reads_Homo_sapiens_test2_R2.fasta";
	opt->read_fn = "Assembled_reads_Homo_sapiens_test2_R1.fasta";
	opt->seed_fn = "Assembled_reads_Homo_sapiens_test2_R1.fasta";*/
	opt->output_stats_fn = NULL;
	opt->output_qual_fn = NULL;
	opt->log_fn = NULL;
	opt->consensus_thresh = 0.6;
	opt->min_werr = 0;
	opt->tracelength = -1;
	opt->min_conserv = 18;
	opt->trip_to_lib = 5;
	opt->min_lib = 16;
	opt->min_contig_len = 0;
	opt->quit_count = 0;
	opt->min_verif = 4;
	opt->coverage = 0;
	opt->repcoverage = 0;
	opt->num_workers = 1;
	opt->term_low_reads = 0;
	//opt->amos = 0;
	/*opt->autoxy = 0;
	opt->autoxy_sdx = 2;
	opt->autoxy_sdy = 2;*/

	
}

static void eval_long(Opt* opt, char* name, const char* arg)
{
	if (!strcmp(name, "help")) {
		opt->help = 1;
		return;
	}
}

static void eval(Opt* opt, int argc, char* const argv[]) {

	static struct option longopts[] = {
			{ "help", 0, 0, 0 },
			{ 0, 0, 0, 0 }
	};
	int g;

	int output_stat_flag = 1;
	int output_qual_flag = 0;
	while (1) {
		int longindex = 0;
		g = getopt_long(argc, argv,
			"z:s:r:c:e:f:k:n:t:m:y:d::o:q:v:u::i:x:w:a::l:",
			longopts, &longindex);
		if (g == -1)
			break;

		switch (g) {
		case 0:
			eval_long(opt, (char*)longopts[longindex].name,
				optarg);
			break;
		case 'z':
			opt->contig_fn = optarg;
			break;
		case 's':
			opt->seed_fn = optarg;
			break;
		case 'r':
			opt->revread_fn = optarg;
			break;
		case 'f':
			opt->read_fn = optarg;
			break;
		case 'l':
			opt->log_fn = optarg;
			break;
		case 'u':
			output_qual_flag = 1;
			break;
		case 'd':
			output_stat_flag = 0;
			break;
		case 'c':  
			opt->consensus_thresh = strtof(optarg, (char**)NULL);
			break;
		case 'e':
			opt->min_werr = (int)strtol(optarg, (char**)NULL,
				10);
			break;
		case 'k':
			opt->tracelength = (int)strtol(optarg,
				(char**)NULL, 10);
			break;
		case 'n':
			opt->min_conserv = (int)strtol(optarg,
				(char**)NULL, 10);
			break;
		case 't':
			opt->trip_to_lib = (int)strtol(optarg,
				(char**)NULL, 10);
			break;
		case 'm':
			opt->min_lib = (int)strtol(optarg, (char**)NULL,
				10);
			break;
		case 'o':
			opt->min_contig_len = (int)strtol(optarg,
				(char**)NULL, 10);
			break;
		case 'q':
			opt->quit_count = (int)strtol(optarg,
				(char**)NULL, 10);
			break;
		case 'v':
			opt->min_verif = (int)strtol(optarg, (char**)NULL,
				10);
			break;
		case 'x':
			opt->coverage = (int)strtol(optarg, (char**)NULL,
				10);
			break;
		case 'y':
			opt->repcoverage = (int)strtol(
				optarg, (char**)NULL, 10);
			break;
		case 'w':
			opt->num_workers = (int)strtol(optarg, (char**)NULL,
				10);
			break;
		}
	}

	if (opt->read_fn != '\0' || opt->revread_fn != '\0') {
		// create filename and open relevant file pointers
		if (opt->contig_fn == NULL || strlen(opt->contig_fn) <= 0) {
			opt->contig_fn = opt->tmp_contig_fn;
			strcpy(opt->contig_fn, opt->read_fn);
			strncat(opt->contig_fn, ".contigs", 8);
		}
		if (output_stat_flag == 1) {
			opt->output_stats_fn = opt->tmp_output_stats_fn;
			strcpy(opt->output_stats_fn, opt->contig_fn);
			strncat(opt->output_stats_fn, ".stat", 5);
		}
		if (output_qual_flag == 1) {
			opt->output_qual_fn = opt->tmp_output_qual_fn;
			strcpy(opt->output_qual_fn, opt->contig_fn);
			strncat(opt->output_qual_fn, ".qual", 5);
		}
	}
}
void opt_init(Opt* opt, int argc, char* const argv[])
{
	init(opt);
	eval(opt, argc, argv);
	
}

void opt_delete(Opt* opt) {
	free(opt);
}

void opt_print_help()
{
	Opt opt;

	opt_init(&opt, 0, NULL);

	fprintf(stderr, "-f <string>\tfile name for forward sequence fasta file holding "
		"unassembled reads\n");
	fprintf(stderr, "-r <string>\tfile name for reverse sequence fasta file holding "
		"unassembled reads\n");
	fprintf(stderr, "-z <string>\tfile name for output contigs (default "
		"is 'file.contigs')\n");
	fprintf(stderr, "-s <string>\tfile name for seed sequence fasta file \n");
	fprintf(stderr, "-l <string>\tfile name for logging (default is no "
		"logging)\n");
	fprintf(stderr, "-d\tif present the .stat output file will not be generated\n");
	fprintf(stderr, "-u\tif present the .quality output file will be generated\n");
	fprintf(stderr, "-a \t\tCreate ouput in AMOS format\n");
	fprintf(stderr, "-c <number>\tratio of most represented base required "
		"for extension [%G]\n", opt.consensus_thresh);
	fprintf(stderr, "-e <number>\tminimum overlap allowed for assembling "
		"k-mers with a single\n"
		"\t\terror (optional, 0 = no errors allowed, recomended: "
		"e = 22 or\n"
		"\t\tmore) [%i]\n", opt.min_werr);
	fprintf(stderr, "-k <number>\tlength of reads in -f file "
		"(required)\n");
	fprintf(stderr, "-n <number>\tstandard minimum overlap for getting "
		"coverage seqs [%i]\n", opt.min_conserv);
	fprintf(stderr, "-t <number>\tnumber of reads before tripping to the "
		"liberal overlap\n"
		"\t\trequirement [%i]\n", opt.trip_to_lib);
	fprintf(stderr, "-m <number>\tminimum overlap allowed for assembling "
		"k-mers if the liberal\n"
		"\t\toverlap trip is reached [%i]\n",
		opt.min_lib);
	fprintf(stderr, "-o <number>\tminimum contig size to print to .contig "
		"file - speeds up\n"
		"\t\tassembly (optional) [%i]\n",
		opt.min_contig_len);
	fprintf(stderr, "-q <number>\tquit after a set number of contigs "
		"(optional, 0 = never) [%i]\n", opt.quit_count);
	fprintf(stderr, "-v <number>\tmaximum number of disagreements with "
		"the majority base call\n"
		"\t\tallowed before termination (prevents extension through "
		"repeat\n"
		"\t\telements) [%i]\n", opt.min_verif);
	fprintf(stderr, "-i <yes>\tif 'yes' terminates extension when there "
		"is only 1 read voting\n"
		"\t\t(optional)\n");
	fprintf(stderr, "-x <number>\tTerminates over this level of coverage "
		"(avoids assembling\n"
		"\t\trepeated elements) (0 = infinite, recommend 10-20x "
		"expected\n"
		"\t\tcoverage) [%i]\n", opt.coverage);
	fprintf(stderr, "-w <number>\tnumber of worker threads to use for "
		"assembly, must be >= 1 [%i]\n", opt.num_workers);
	fprintf(stderr, "-y <number>\tOnly extends OVER this level of "
		"coverage; assembles ONLY\n"
		"\t\trepeated elements [%i]\n", opt.repcoverage);
	fprintf(stderr, "--help\t\tPrint this list of options\n");

	opt_delete(&opt);
}
