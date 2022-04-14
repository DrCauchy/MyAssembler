/*
libnassar (personal collection of utility functions)

Copyright (c) 1998-2008 Nassib Nassar
Copyright (c) 1998-2007 Etymon Systems, Inc.
Copyright (c) 2007-2008 Renaissance Computing Institute

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

(Also known as the "MIT License")
*/

#include <stdlib.h>
#include <ctype.h>
#include "trim.h"

char* n_trim(char* str)
{
    char* q = str;
    char* p = str;
    char* mark = NULL;
    char qc;

    while (isspace(*q))//��鵽��һ���ǿհ��ַ�
        q++;

    while ((qc = *q)) {
        if (p != q)
            *p = qc;
        if (!isspace(qc))
            mark = p;
        p++;
        q++;
    }

    if (mark)
        *(mark + 1) = '\0';
    else
        *str = '\0';

    return str;
}


