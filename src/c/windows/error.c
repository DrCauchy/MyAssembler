#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* err_argv0 = NULL;

void _err_assert_mem(const void* ptr)
{
	if (!ptr) {
		fprintf(stderr, "%s: Out of memory\n", err_argv0);
		abort();
	}
}

void _err_assert_str(const char* str, int size)
{
	if (!str) {
		fprintf(stderr, "%s: String NULL pointer error\n", err_argv0);
		abort();
	}
	if (strlen(str) >= size) {
		fprintf(stderr, "%s: String buffer overflow\n", err_argv0);
		abort();
	}
}
