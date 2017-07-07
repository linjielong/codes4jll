



#define LOG_TAG "jll_dump"
#include <utils/Log.h>

/**
 * jll_dump - dump the content according to the specified format
 * @[in] pucContent: Byte Data First Address
 * @[in] u4SzContent: Size of Byte Data
 * @[in] u4Mode: 1 - left Hex format dump
 *               2 - right char format dump
 *              Any Digital - Default format dump contain left and right
 *
 * Dump the byte content according to the specified format as follows:
 *     
 */
#define JllPrint ALOGE
static void jll_dump(unsigned char *pucContent, unsigned int u4SzContent, unsigned int u4Mode)
{
    unsigned int   u4Idx;
    unsigned char *pucData; 
    unsigned int   u4I;
    unsigned int   u4UnusedIdx;
    unsigned char  arData[16]; 

    if (u4SzContent == 0) {
        return;
    }

    pucData = pucContent;
    u4Idx = 0;
    while ((u4Idx + 16) <= u4SzContent) {
        switch (u4Mode) {
        case 1:
            JllPrint("JLL.Dump: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X - " \
                     "0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X",
                     pucData[u4Idx], 
                     pucData[u4Idx+1], 
                     pucData[u4Idx+2], 
                     pucData[u4Idx+3], 
                     pucData[u4Idx+4], 
                     pucData[u4Idx+5], 
                     pucData[u4Idx+6], 
                     pucData[u4Idx+7], 
                     pucData[u4Idx+8], 
                     pucData[u4Idx+9], 
                     pucData[u4Idx+10], 
                     pucData[u4Idx+11], 
                     pucData[u4Idx+12], 
                     pucData[u4Idx+13], 
                     pucData[u4Idx+14], 
                     pucData[u4Idx+15]);
            break;
        case 2:
            JllPrint("JLL.Dump: %c%c%c%c%c%c%c%c - %c%c%c%c%c%c%c%c",
                     pucData[u4Idx], 
                     pucData[u4Idx+1], 
                     pucData[u4Idx+2], 
                     pucData[u4Idx+3], 
                     pucData[u4Idx+4], 
                     pucData[u4Idx+5], 
                     pucData[u4Idx+6], 
                     pucData[u4Idx+7], 
                     pucData[u4Idx+8], 
                     pucData[u4Idx+9], 
                     pucData[u4Idx+10], 
                     pucData[u4Idx+11], 
                     pucData[u4Idx+12], 
                     pucData[u4Idx+13], 
                     pucData[u4Idx+14], 
                     pucData[u4Idx+15]);
            break; 
        default: 
            JllPrint("JLL.Dump: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X - " \
                     "0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X  || " \
                     "%c%c%c%c%c%c%c%c - %c%c%c%c%c%c%c%c",
                     pucData[u4Idx], 
                     pucData[u4Idx+1], 
                     pucData[u4Idx+2], 
                     pucData[u4Idx+3], 
                     pucData[u4Idx+4], 
                     pucData[u4Idx+5], 
                     pucData[u4Idx+6], 
                     pucData[u4Idx+7], 
                     pucData[u4Idx+8], 
                     pucData[u4Idx+9], 
                     pucData[u4Idx+10], 
                     pucData[u4Idx+11], 
                     pucData[u4Idx+12], 
                     pucData[u4Idx+13], 
                     pucData[u4Idx+14], 
                     pucData[u4Idx+15], 
                     pucData[u4Idx], 
                     pucData[u4Idx+1], 
                     pucData[u4Idx+2], 
                     pucData[u4Idx+3], 
                     pucData[u4Idx+4], 
                     pucData[u4Idx+5], 
                     pucData[u4Idx+6], 
                     pucData[u4Idx+7], 
                     pucData[u4Idx+8], 
                     pucData[u4Idx+9], 
                     pucData[u4Idx+10], 
                     pucData[u4Idx+11], 
                     pucData[u4Idx+12], 
                     pucData[u4Idx+13], 
                     pucData[u4Idx+14], 
                     pucData[u4Idx+15]);
            break;
        }
        u4Idx += 16;
    }

    if (u4SzContent%16==0) {
        return;
    } 

    u4UnusedIdx = u4SzContent - u4Idx;
    for (u4I=0; u4I<16; u4I++) {
        if (u4I < u4UnusedIdx) {
            arData[u4I] = pucData[u4Idx++];
        } else {
            arData[u4I] = 0x00;
        }
    }
    jll_dump(arData, 16, u4Mode);
    return;
}


