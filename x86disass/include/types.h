#ifndef _TYPES_H
#define _TYPES_H
#if defined(__linux__)
#include <linux/types.h>
#endif
#if defined(_WIN32)
typedef unsigned long long __u64;
typedef unsigned int __u32;
typedef unsigned short __u16;
typedef unsigned char __u8;

typedef long long __s64;
typedef int __s32;
typedef short __s16;
typedef char __s8;
#endif
#endif
