/* Copyright(c) 2016-2100.  jielong.lin.  All rights reserved */
/*
 * Author: jielong.lin (jll)
 * Email:  493164984@qq.com
 * Date:   2016 September 11
 */
#ifndef __JLL_DEBUG_H__
#define __JLL_DEBUG_H__

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void jll_print(char *pcFmt, ...);

void jll_backtrace(int i4Depth32, char *pcFmt32, ...);

/**
 * _jll_debug_with_format
 *
 *
 * Format Output if pcFmt is NULL, as follows:
 *     pcPerfix|pcFunc: test@/home/jielong.lin/testproject/src/main.c:9999
 *
 * Format Output if pcFmt isn't NULL, as follows:
 *     pcPrefix|pcFunc: test@/home/jielong.lin/testproject/src/main.c:9999
 *     pcPrefix|pcFunc: hello world, I know the string texture is too long
 *     pcPrefix|pcFunc: more than a line
 */
void _jll_debug_with_format(const char *pcPrefix,
                            const char *pcFunc, 
                            char *pcFmt, ...);

#define jll_debug(pcFmt, ...)  _jll_debug_with_format("JLL", __FUNCTION__, pcFmt, ##__VA_ARGS__)

#endif /* End of __JLL_DEBUG_H__ */

