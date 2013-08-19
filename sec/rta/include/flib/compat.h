/* Copyright 2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_COMPAT_H__
#define __RTA_COMPAT_H__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef pr_debug
#ifdef RTA_DEBUG
#define pr_debug(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif
#endif

/**
 * ARRAY_SIZE(x) - Returns the number of elements in an array
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
#define ALIGN(x, a) (((x) + ((typeof(x))(a) - 1)) & ~((typeof(x))(a) - 1))
#endif

#endif /* __RTA_COMPAT_H__ */
