#ifndef XTYPES_H_INCLUDED
#define XTYPES_H_INCLUDED

#include <sys/types.h>

#ifdef _x86_64_
typedef u_int64_t xvm_offset_t;
#else
typedef u_int32_t xvm_offset_t;
#endif

#endif
