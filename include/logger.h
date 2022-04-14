#ifndef LOGGER_H_
#define LOGGER_H_

#include "opt.h"

void log_init(Opt* opt);
void log_info(const char* msg, ...);
void log_warning(const char* msg, ...);
void log_error(const char* msg, ...);
void log_msg(const char* type, const char* msg, ...);
void log_finish(void);

#endif
