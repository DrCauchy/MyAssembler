// MyAssembler.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#include "logger.h"
#include "opt.h"
#include "runstats.h"
#include "readlist.h"
#include "read.h"
#include "worker.h"
#ifdef _WIN32
#include "getopt.h"
#endif
#ifdef __linux__
#include <getopt.h>
#endif

void assemble(Opt* opt);
static void output_stats(Runstats* runstats, Opt* opt);
