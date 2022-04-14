#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "logger.h"
#include "opt.h"

static char* log_fn;
static FILE* log_fp;

void log_init(Opt* opt)
{
	if (opt->log_fn != NULL && strlen(opt->log_fn) >= 1) {
		log_fn = (char*)malloc(strlen(opt->log_fn) + 1);
		strcpy(log_fn, opt->log_fn);

		log_fp = fopen(log_fn, "w");
		if (log_fp == NULL) {
			log_fn = NULL;
			fprintf(stderr,
				"ERROR - Could not open logging file %s, no logging will be output\n",
				log_fn);
			return;
		}
	}
	else {
		log_fn = NULL;
		log_fp = NULL;
	}
}

void log_finish(void)
{
	if (log_fn != NULL)
		free(log_fn);
}


void log_error(const char* msg, ...)
{
	va_list arg_list;

	va_start(arg_list, msg);
	if (log_fp != NULL) {
		fprintf(log_fp, "Error\t");
		vfprintf(log_fp, msg, arg_list);
	}
	fprintf(stderr, "Error\t");
	vfprintf(stderr, msg, arg_list);
	fprintf(stdout, "Error\t");
	vfprintf(stdout, msg, arg_list);
	va_end(arg_list);
}

void log_info(const char* msg, ...)
{
	va_list arg_list;

	va_start(arg_list, msg);
	if (log_fp != NULL) {
		fprintf(log_fp, "Info\t");
		vfprintf(log_fp, msg, arg_list);
	}
	fprintf(stdout, "Info\t");
	vfprintf(stdout, msg, arg_list);
	va_end(arg_list);
}

void log_warning(const char* msg, ...)
{
	va_list arg_list;

	va_start(arg_list, msg);
	if (log_fp != NULL) {
		fprintf(log_fp, "Warn\t");
		vfprintf(log_fp, msg, arg_list);
	}
	fprintf(stderr, "Warn\t");
	vfprintf(stderr, msg, arg_list);
	va_end(arg_list);
}

void log_msg(const char* type, const char* msg, ...)
{
	va_list arg_list;

	va_start(arg_list, msg);
	if (log_fp != NULL) {
		fprintf(log_fp, "%s\t", type);
		vfprintf(log_fp, msg, arg_list);
	}
	fprintf(stdout, "%s\t", type);
	vfprintf(stdout, msg, arg_list);
	va_end(arg_list);
}
