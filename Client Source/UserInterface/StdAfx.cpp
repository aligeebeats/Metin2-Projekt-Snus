#include "stdafx.h"

#ifndef IOB_FUNC
#define IOB_FUNC

#define stdin (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif

#ifdef _WIN32
int (WINAPIV* __vsnprintf)(char*, size_t, const char*, va_list) = _vsnprintf;
#endif
