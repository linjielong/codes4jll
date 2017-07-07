/* Copyright(c) 2016-2100.  jielong.lin.  All rights reserved */
/*
 * Author: jielong.lin (jll)
 * Email:  493164984@qq.com
 * Date:   2016 September 11
 */



#include "vim.h"



#define IOBUFMAX  2048
static char ioBUF[IOBUFMAX];
static int  ioBUFLen=0;

void jll_print(char *pcFmt, ...)
{
    va_list pvArgs;

    if (pcFmt == NULL) {
        return;
    }
    va_start( pvArgs, pcFmt );
    ioBUFLen = vsnprintf( ioBUF, IOBUFMAX - 1, pcFmt, pvArgs );
    if (ioBUFLen < 0) {
        va_end( pvArgs );
        return;
    }
    ioBUF[ioBUFLen] = 0;
    va_end( pvArgs );

    MSG(ioBUF);
}

/*
 * JLLCONF_DEBUG_LineLength isn't less than 10, or can't ensure it work fine
 * It contains Prefix and Content
 */
#define JLLCONF_DEBUG_LineLength  100
 
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
                            char *pcFmt, ...)
{
    va_list pvArgs;
    int   i;
    int   i4Lines;
    int   i4PreLineLen;
    char  pcLineBuf[JLLCONF_DEBUG_LineLength+12];
    int   i4Len;
    int   i4ContentLen;

    i4Len = 0;

    /*
     * Prefix Tag format as 'pcPrefix|pcFunc:'
     */
    i4PreLineLen = strlen(pcPrefix);
    if (i4PreLineLen < 1) {
        /* Error*/
        jll_print("JLL|%s: Error due to Prefix Paramter is NULL\n",  __FUNCTION__);
        jll_print("JLL|%s: Please check Prefix Parameter\n", __FUNCTION__);
        return;
    }
    i4PreLineLen += 3;
    i4PreLineLen += strlen(pcFunc);
    if (i4PreLineLen >= JLLCONF_DEBUG_LineLength) {
        /* try to check if perfix format is valid in case pcFmt is NULL */
        i4PreLineLen--;
        if (pcFmt == NULL && i4PreLineLen == JLLCONF_DEBUG_LineLength) {
            /* in this case, only dump the prefix text information without : symbol */
            jll_print("%s|%s\n", pcPrefix, pcFunc);
            return;
        }

        /* Prefix length is too long to output content, so force to abbreviate and change it */
        i4PreLineLen = snprintf(pcLineBuf, 10, "JLL|***: ");
        i4Len = i4PreLineLen;
    }

    /*
     * do if pcFmt is NULL
     */
    if (pcFmt == NULL) {
        jll_print("%s|%s\n", pcPrefix, pcFunc);
        return;
    }

    /* Encapsule and Load Prefix data to Line buffer as 'pcPrefix|pcFunc:' */
    if (i4Len == 0) {
        i4PreLineLen = snprintf(pcLineBuf, i4PreLineLen+1, "%s|%s: ", pcPrefix, pcFunc); 
    }

    /*
     * load the format string data to raw buffer 'ioBUF'
     */
    va_start(pvArgs, pcFmt);
    ioBUFLen = vsnprintf(ioBUF, IOBUFMAX - 1, pcFmt, pvArgs);
    if (ioBUFLen < 0) {
        /* Error*/
        jll_print("%s|%s: io buffer is not enough\n", pcPrefix, __FUNCTION__);
        jll_print("%s|%s: Please not input more than %d bytes\n", pcPrefix, __FUNCTION__, IOBUFMAX);
        va_end(pvArgs);
        return;
    }
    va_end(pvArgs);
    ioBUF[ioBUFLen] = 0;

    /*
     * output only texture length less than JLLCONF_DEBUG_LineLength bytes for every line
     */
    i4ContentLen = JLLCONF_DEBUG_LineLength - i4PreLineLen;
    if (i4ContentLen < 1) {
        jll_print("%s|%s: Error because line buffer is too small\n", pcPrefix, __FUNCTION__);
        return;
    }
    i4Lines = ioBUFLen /i4ContentLen + 1;
    for (i = 0; i < i4Lines; i++ ) {
        if ((i+1)*i4ContentLen > ioBUFLen) { /* Current Line is the last line  */
            if (i4ContentLen > ioBUFLen) {
                i4Len = ioBUFLen;
            } else {
                i4Len = ioBUFLen - i*i4ContentLen;
            }
        } else {
            i4Len = i4ContentLen;
        }
        snprintf(pcLineBuf+i4PreLineLen, i4Len + 1, "%s", ioBUF+i*i4ContentLen);
        MSG(pcLineBuf);
        memset(pcLineBuf+i4PreLineLen, 0, i4Len + 1);
    }

    return;
}


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
                ioBUFLen = snprintf(ioBUF, IOBUFMAX, "%s: %s", acPrefix, ppcData[i4Idx]);
            } else {
                ioBUFLen = snprintf(ioBUF, IOBUFMAX, "%s", ppcData[i4Idx]);
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

