/* Copyright(c) 2016-2100.   jielong.lin.   All rights reserved.
 * Author: jielong_lin (jll)
 * Email:  493164984@qq.com
 * Date:   2016 September 11
 */
#include "vim.h"

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define IOBUFMAX  2048
static char ioBUF[IOBUFMAX];
static int  ioBUFLen=0;

void jll_backtrace(int i4Depth32, char *pcFmt32, ...)
{
    int     i4Idx;
    int     i4Size;
    char  **ppcData;
    void   *pvData[32];
    char    acPrefix[32];
    va_list pvArgs;

    if (pcFmt32 == NULL) {
        return;
    }
    va_start( pvArgs, pcFmt32 );
    i4Size = vsnprintf( acPrefix, sizeof(acPrefix) - 1, pcFmt32, pvArgs );
    if (i4Size < 0) {
        va_end( pvArgs );
        return;
    }
    acPrefix[i4Size] = 0;
    va_end( pvArgs );

    i4Size = backtrace( pvData, sizeof(pvData) / sizeof(pvData[0]) );
    if (i4Size > 0) {
        if (i4Depth32 > 32) {
            i4Depth32 = 32;
        }
        i4Depth32 = (i4Depth32 > i4Size) ? i4Size : i4Depth32;
        ppcData = backtrace_symbols( pvData, i4Size );
        for (i4Idx = 0; i4Idx < i4Size; i4Idx++ ) {
            if (acPrefix[0] != 0) {
                //printf("%s | %s\n", acPrefix, ppcData[i4Idx]);
                ioBUFLen = snprintf(ioBUF, IOBUFMAX, "%s | %s\n", acPrefix, ppcData[i4Idx]);
            } else {
                ioBUFLen = snprintf(ioBUF, IOBUFMAX, "%s\n", ppcData[i4Idx]);
            }
            if (ioBUFLen < 1 || ioBUFLen+1 >= IOBUFMAX) {
                ioBUFLen=0;
                memset(ioBUF, 0, IOBUFMAX);
            } else {
                ioBUF[ioBUFLen]=0; 
                MSG(ioBUF);
            }
        }
        free(ppcData);
    }
}




