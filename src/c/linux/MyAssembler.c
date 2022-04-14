#include "MyAssembler.h"

static int check_help_needed(const Opt *opt, int argc)
{
	return opt->help || argc < 2;
}

 static int check_required_inputs(const Opt *opt, int argc, char *err_argv0)
 {
 	int e = 0;

 	if (opt->seed_fn == '\0') {
 		fprintf(stderr, "%s: missing required option -- s\n",
 			err_argv0);
 		e = 1;
 	}

 	if (opt->tracelength < 0) {
 		fprintf(stderr, "%s: missing required option -- k\n",
 			err_argv0);
 		e = 1;
 	}

 	if (opt->read_fn[0] == '\0') {
 		fprintf(stderr, "%s: missing required option -- f\n",
 			err_argv0);
 		e = 1;
 	}

	// if (opt->revread_fn[0] == '\0') {
	// 	fprintf(stderr, "%s: missing required option -- r\n",
	// 		err_argv0);
	// 	e = 1;
	// }

 	return e;
 }


int main(int argc, char* argv[])
{
	Opt opt;

	
	// 初始化
	char *err_argv0 = argv[0];
	opt_init(&opt, argc, argv);

	if (check_help_needed(&opt, argc)) {
		opt_print_help();
		exit(0);
	}
	 if (check_required_inputs(&opt, argc, err_argv0)) {
	 	opt_print_help();
	 	return 1;
	 }

	log_init(&opt);

	//拼接
	assemble(&opt);
	
	//结束
	log_info("Finished\n");
	opt_delete(&opt);
	//pthread_exit(NULL);
	return 0;
}


void assemble(Opt* opt) {

	int iw = 1;
	Readlist readlist;
	Seqhash set;
	Shindex set_index;
	Seqtree bin;


	Runstats runstats;

	runstats_init(&runstats, opt);

	log_info("Starting sequence assembly\n");

	
	runstats_startrun(&runstats);//计时

	log_info("Reading input files\n");

	//初始化数据结构
	readlist_init(&readlist, SET_ITEMS_SIZE);
	seqtree_init(&bin, SET_ITEMS_SIZE);
	seqhash_init(&set, SET_TABLE_SIZE);

	//生成哈希表
	readset(&set, opt->seed_fn);
	readbin(&bin, opt->read_fn, opt->revread_fn, opt->tracelength);

	
	log_info("Creating index into set and initializing contigs\n");
	shindex_init(&set_index, &set);//对种子出现次数排序
	initialize_contigs(&set, &set_index, &bin, opt);//拿取第一个种子，准备进行拼接


	/*多线程保留块*/
	// init compute nodes (threads) and pass them the global data structures
	
	Workerinfo* workerinfo = (Workerinfo*)malloc(sizeof(Workerinfo));
	workerinfo->id = iw;
	workerinfo->bin = &bin;
	workerinfo->set = &set;
	workerinfo->opt = opt;
	workerinfo->list = &readlist;
	workerinfo->runstats = &runstats;
	workerinfo->set_index = &set_index;

	run_worker((void*)workerinfo);

	//// now wait until the workers finish
	//int* status;
	//log_info("Waiting on worker threads\n");
	//for (iw = 0; iw < runstats->num_workers; iw++) {
	//	int res = pthread_join(workers[iw], (void*)&status);
	//	if (res != 0) {
	//		log_error("pthread_join call failed %d\n", res);
	//		exit(-1);
	//	}
	//}
	/*多线程保留块*/



	// clean up data
	log_info("Freeing resources\n");
	finalize_contigs();
	seqtree_delete(&bin);
	shindex_delete(&set_index);
	seqhash_delete(&set);

	runstats_endrun(&runstats);
	if (opt->output_stats_fn)
		output_stats(&runstats, opt);

	runstats_delete(&runstats);
}

static void output_stats(Runstats* runstats, Opt* opt)
{
	FILE* statfp;

	statfp = fopen(opt->output_stats_fn, "w");
	if (statfp == NULL) {
		log_error(
			"Could not open statistics file %s, no statistics were output\n",
			opt->output_stats_fn);
		return;
	}

	runstats_report(runstats, statfp);

	fflush(statfp);
	fclose(statfp);
}