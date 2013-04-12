#ifndef __STD_H__
#define __STD_H__

#include <stdint.h>

#define __offsetof(TYPE, MEMBER) (((uintptr_t) &((TYPE*)16)->MEMBER)-16)

#ifdef __GNUC__
#define __container_of(ptr, type, member) ({                    \
		const typeof( ((type *)0)->member ) *__xptr = (ptr);  \
		(type *)( (char *)__xptr - __offsetof(type,member) );})
#else
#define __container_of(ptr, type, member) \
		((type *)(((char *)(ptr)) - __offsetof(type,member)))
#endif

#endif //__STD_H__
