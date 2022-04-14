#ifndef _ERR_H
#define _ERR_H

extern char* err_argv0;

#ifdef DEBUG
#define err_assert_mem(a) _err_assert_mem(a)
#define err_assert_str(a, b) _err_assert_str(a, b)
#else
#define err_assert_mem(a)
#define err_assert_str(a, b)
#endif

void _err_assert_mem(const void* ptr);
void _err_assert_str(const char* str, int size);

#endif
