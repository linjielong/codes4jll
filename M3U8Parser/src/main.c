/*
 * Copyright (c) 2014-2100.  jielong_lin(493164984@qq.com).  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include "linux_list.h"

#define  PR        printf("%s:%d\r\n", __FUNCTION__, __LINE__)

typedef    void           VOID;
typedef    char           CHAR;
typedef    unsigned int   UINT32;
typedef    int            INT32;
typedef    unsigned int   HANDLE_T;

typedef struct _LINE_ON_STREAM_T {
    struct list_head  t_list;
    VOID*             pv_data;
    UINT32            ui4_len;
} LINE_ON_STREAM_T;

LINE_ON_STREAM_T*  malloc_line_on_stream(VOID* pv_data, UINT32  ui4_len)
{
    LINE_ON_STREAM_T*  pt_obj;
    VOID*              pv_entry;

    if (!pv_data || !ui4_len) {
        return NULL;
    }

    pt_obj = malloc(sizeof(LINE_ON_STREAM_T));
    if (!pt_obj) {
        return NULL;
    }

    pv_entry = malloc(ui4_len + 1);
    if (!pv_entry) {
        free(pt_obj);
        return NULL;
    }
    memcpy(pv_entry, pv_data, ui4_len);
    *((CHAR*)pv_entry + ui4_len) = 0;

    pt_obj->pv_data = pv_entry;
    pt_obj->ui4_len = ui4_len;
    INIT_LIST_HEAD(&pt_obj->t_list);

    return pt_obj;
}

VOID  free_line_on_stream(LINE_ON_STREAM_T *pt_obj)
{
    if (!pt_obj) {
        return;
    }

    list_del(&pt_obj->t_list);

    if (pt_obj->pv_data) {
        free(pt_obj->pv_data);
    }

    free(pt_obj);
}

/**
 * utils_skip_on_stream - 字节流的滤过接口.
 * @pv_stream:[in] 字节流.
 * @pui4_stream_len:[in/out] 处理前后的字节流的字节大小.
 * @pi1_token_db:[in] 作为滤过的关键字符集,必须是字符串. 各个字符之间是或的关系.
 *
 * 根据滤过的关键字符集中存在的字符,对字节流前面字符进行滤过.
 * 整个过程中一遇到非关键字符集中的字符时,则退出滤过流程并返回.
 *
 * 成功返回滤过后的地址;
 * 失败返回NULL. 除到达流结束位置之外,参数不出错,一般不会出现NULL.
 * 
 */
VOID*  utils_skip_on_stream(VOID* pv_stream, UINT32* pui4_stream_len, CHAR* pi1_token_db)
{
    CHAR*   pi1_cur;
    CHAR*   pi1_eos;
    UINT32  ui4_token_len;
    UINT32  ui4_i;
    UINT32  ui4_hit;

    if (!pv_stream || !pui4_stream_len || !*pui4_stream_len || !pi1_token_db) {
        return NULL;
    }

    pi1_cur = (CHAR* )pv_stream;
    pi1_eos = pi1_cur + *pui4_stream_len;
    ui4_token_len = strlen(pi1_token_db);
    while (pi1_cur < pi1_eos) {
        ui4_hit = 0;
        for (ui4_i = 0; ui4_i < ui4_token_len; ui4_i++) {
            if (*pi1_cur == pi1_token_db[ui4_i]) { /* Matched the token_sets to continue. */
                ui4_hit = 1;
                break;
            }
        }
        if (!ui4_hit) {
            break;
        }
        pi1_cur++;
    }

    *pui4_stream_len = pi1_eos - pi1_cur;
    if (pi1_eos == pi1_cur) {
        return NULL;
    }

    return pi1_cur;
}

/**
 * utils_strdup - 字符串拷贝接口.
 * @pi1_str:[in] 需要被拷贝的字符串.
 *
 * 分配新空间并拷贝字符串到该空间.
 *
 * 成功返回拷贝后的字符串指针;
 * 失败返回NULL.
 */
CHAR*   utils_strdup(CHAR*  pi1_str)
{
    CHAR*    pi1_new;
    UINT32   ui4_ret;

    if (pi1_str == NULL) {
        return NULL;
    }

    ui4_ret = strlen(pi1_str);
    if (ui4_ret == 0) {
        return NULL;
    }

    pi1_new = malloc(ui4_ret + 1);
    if (pi1_new == NULL) {
        return NULL;
    }

    memcpy(pi1_new, pi1_str, ui4_ret);
    *(pi1_new + ui4_ret) = 0;

    return pi1_new;
}

/**
 * utils_strstr - 字节流查询字符串接口
 * @pv_stream:[in] 字节流
 * @pui4_stream_len:[in/out] 字节流字节数
 * @pi1_token:[in] 匹配的关键字符串.
 *
 * 在字节流中查询字符串的位置. pui4_stream_len会返回除去
 * pi1_token为止字节数后的流的字节数.如果没有找到,则返回NULL,
 * 并且pui4_stream_len不变,查找到流结束时,返回NULL时,有可能
 * pui4_stream_len为0也可能不为0.
 *
 * 成功返回该字符串在字节流中的起始位置. 
 * 失败返回NULL.
 * 结束返回NULL;
 *
 * 没找到,返回NULL表示不需要再遍历查询下去了.
 * 参数不出错,一般不会因为出错而出现NULL.
 *     <1> 参数指针为NULL.
 *     <2> pi1_token为0字节的字符串.
 */
VOID*  utils_strstr(VOID* pv_stream, UINT32* pui4_stream_len, CHAR* pi1_token)
{
    CHAR*     pi1_s1;
    CHAR*     pi1_s2;
    UINT32    ui4_offset;
    UINT32    ui4_stream_base;

    if (!pv_stream || !pui4_stream_len || !*pui4_stream_len || !pi1_token || !*pi1_token) {
        /* Error */
        return NULL;
    }
    pi1_s1 = pv_stream;
    pi1_s2 = pi1_token;
    ui4_stream_base = 0;
    ui4_offset = strlen(pi1_s2) - 1; /* ui4_offset >= 0 */
    while (ui4_stream_base + ui4_offset < *pui4_stream_len) {
        for (ui4_offset = 0;
             *(pi1_s1 + ui4_stream_base + ui4_offset) == *(pi1_s2 + ui4_offset);
             ui4_offset++) {
            if (!*(pi1_s2 + ui4_offset + 1)) {
                *pui4_stream_len -= ui4_stream_base;
                return  (pi1_s1 + ui4_stream_base);
            }
        }
        ui4_stream_base++;
    }

    /* End Of Stream. */
    return NULL;
}

struct list_head*  utils_get_all_line_on_stream(VOID* pv_stream, UINT32* pui4_stream_len)
{
    struct list_head*  pt_tbl;
    LINE_ON_STREAM_T*  pt_obj;
    CHAR*              pi1_offset_lf;   /* '\n' */
    CHAR*              pi1_base;
    UINT32             ui4_len;
    UINT32             ui4_tmplen;
    
    if (!pv_stream || !pui4_stream_len || !*pui4_stream_len) {       
        return NULL;
    }

    if (!(pt_tbl = malloc(sizeof(struct list_head)))) {
        return NULL;
    }
    INIT_LIST_HEAD(pt_tbl);

    pi1_base = (CHAR* )pv_stream;
    ui4_len = *pui4_stream_len;
//printf("%s:%s:%d#start[%p$%d]\r\n", __FILE__, __FUNCTION__, __LINE__, pi1_base, ui4_len);

    for (;;) {
        pi1_base = utils_skip_on_stream(pi1_base, &ui4_len, "\r\t\n ");
        if (!pi1_base) {
            break;
        }
//printf("%s:%s:%d#cur  [%p$%d]\r\n", __FILE__, __FUNCTION__, __LINE__, pi1_base, ui4_len);
        pi1_offset_lf = utils_strstr(pi1_base, &ui4_len, "\n");
        if (!pi1_offset_lf) {
            if (ui4_len) {
                if ((pt_obj = malloc_line_on_stream(pi1_base, ui4_len)) != NULL) {
                    list_add_tail(&pt_obj->t_list, pt_tbl);
                }
            }

//printf("%s:%s:%d#cur   [%p$%d]-->break\r\n", __FILE__, __FUNCTION__, __LINE__, pi1_offset_lf, ui4_len);
            break;
        }

//printf("%s:%s:%d#cur  [%p$%d]\r\n", __FILE__, __FUNCTION__, __LINE__, pi1_offset_lf, ui4_len);
        ui4_tmplen = pi1_offset_lf - pi1_base;
        if (ui4_tmplen) {
            if ((pt_obj = malloc_line_on_stream(pi1_base, ui4_tmplen)) != NULL) {
                list_add_tail(&pt_obj->t_list, pt_tbl);
            }
        }

        pi1_base = pi1_offset_lf;
    }

    if (list_empty(pt_tbl)) {
        free(pt_tbl);
        return NULL;
    }

    return pt_tbl;
}

VOID  utils_del_all_line_on_stream(struct list_head*   pt_tbl)
{
    LINE_ON_STREAM_T*   pt_pos;
    LINE_ON_STREAM_T*   pt_n;
    
    if (!pt_tbl) {
        return;
    }

    list_for_each_entry_safe(pt_pos, pt_n, pt_tbl, t_list) {
        free_line_on_stream(pt_pos);
    }

    free(pt_tbl);
}

typedef  struct  _TOKEN_UINT32_T {
    CHAR*   pi1_token;
    UINT32  ui4_value;
} TOKEN_UINT32_T;

INT32 utils_get_token_uint32(VOID* pv_data, UINT32 ui4_datalen, TOKEN_UINT32_T*  pt_token_uint32)
{
    CHAR*    pi1_data;
    UINT32   ui4_dlen;
    int      i4_digit;
    UINT32   ui4_value;
    UINT32   ui4_token_len;
    
    if (!pt_token_uint32 || !pt_token_uint32->pi1_token || !pv_data || !ui4_datalen) {
        return -1;
    }

    ui4_token_len = strlen(pt_token_uint32->pi1_token);
    if (!ui4_token_len) {
        return -1;
    }

    ui4_dlen = ui4_datalen;
    pi1_data = utils_strstr(pv_data, &ui4_dlen, pt_token_uint32->pi1_token);
    if (!pi1_data || !ui4_dlen) { /* Not found. */
        return -1;
    }
    pi1_data += ui4_token_len;
    pi1_data = utils_skip_on_stream(pi1_data, &ui4_dlen, " \r\n\t");
    if (!pi1_data || !ui4_dlen) { /* Invalid the value. */
        return -1;
    }

    ui4_value = 0;
    i4_digit = (int)*pi1_data;
    if (!isdigit(i4_digit)) {
        return -1;
    }
    while (ui4_dlen && isdigit(i4_digit)) {
        ui4_value = ui4_value * 10;
        ui4_value += (i4_digit - '0');
        ui4_dlen--;
        i4_digit = *++pi1_data;
    }
    pt_token_uint32->ui4_value = ui4_value;

    return 0;
}

INT32 utils_get_token_uint32_extern(VOID*            pv_data,
                                    UINT32           ui4_datalen,
                                    TOKEN_UINT32_T*  pt_token_uint32,
                                    UINT32           ui4_scaler)  /* scale=100, 1000, ... */
{
    CHAR*    pi1_data;
    UINT32   ui4_dlen;
    int      i4_digit;
    UINT32   ui4_value;
    UINT32   ui4_small_value;
    UINT32   ui4_token_len;
    UINT32   ui4_status;  /* UINT32:0  SmallUINT32:1 */
    
    if (!pt_token_uint32 || !pt_token_uint32->pi1_token || !pv_data || !ui4_datalen) {
        return -1;
    }

    ui4_token_len = strlen(pt_token_uint32->pi1_token);
    if (!ui4_token_len) {
        return -1;
    }

    ui4_dlen = ui4_datalen;
    pi1_data = utils_strstr(pv_data, &ui4_dlen, pt_token_uint32->pi1_token);
    if (!pi1_data || !ui4_dlen) { /* Not found. */
        return -1;
    }
    pi1_data += ui4_token_len;
    pi1_data = utils_skip_on_stream(pi1_data, &ui4_dlen, " \r\n\t");
    if (!pi1_data || !ui4_dlen) { /* Invalid the value. */
        return -1;
    }

    ui4_value = 0;
    ui4_small_value = 0;
    ui4_status = 0;
    i4_digit = (int)*pi1_data;
    if (!isdigit(i4_digit)) {
        return -1;
    }
    while (ui4_dlen) {
        if ((ui4_status == 0) && isdigit(i4_digit)) {
            ui4_value = ui4_value * 10;
            ui4_value += (i4_digit - '0');
            ui4_dlen--;
            i4_digit = *++pi1_data;
            continue;
        }

        if ((ui4_status == 0) && *pi1_data == '.') { /* goto the small digit status. */
            ui4_status = 1;
            ui4_dlen--;
            i4_digit = *++pi1_data;
            continue;
        }

        if ((ui4_status == 1) && isdigit(i4_digit)) {
            ui4_small_value = ui4_small_value * 10;
            ui4_small_value += (i4_digit - '0');
            ui4_dlen--;
            i4_digit = *++pi1_data;
            continue;
        }

        break;
    }

    pt_token_uint32->ui4_value = ui4_value * ui4_scaler + ui4_small_value;

    return 0;
}

CHAR*  utils_merger_strings(CHAR* pi1_data1, CHAR* pi1_data2)
{
    CHAR*    pi1_data;
    UINT32   ui4_dlen1;
    UINT32   ui4_dlen2;

    if (!pi1_data1 || !pi1_data2) {
PR;
        return NULL;
    }

    ui4_dlen1 = strlen(pi1_data1);
    ui4_dlen2 = strlen(pi1_data2);
    if (!ui4_dlen1 || !ui4_dlen2) {
PR;
        return NULL;
    }

    pi1_data = malloc(ui4_dlen1 + ui4_dlen2 + 1);
    if (!pi1_data) {
PR;
        return NULL;
    }
    *(pi1_data + ui4_dlen1 + ui4_dlen2) = 0;
    memcpy(pi1_data, pi1_data1, ui4_dlen1);
    memcpy(pi1_data + ui4_dlen1, pi1_data2, ui4_dlen2);

    return pi1_data;
}

CHAR*  utils_replace_file_and_merger_to_path(CHAR* pi1_pathfile, CHAR* pi1_filename)
{
    CHAR*  pi1_data;
    CHAR*  pi1_tmp;
    CHAR   i4_data;

    if (!pi1_pathfile || !pi1_filename) {
        return NULL;
    }

    pi1_tmp = strrchr(pi1_pathfile, '/');
    if (pi1_tmp == NULL) {
        return NULL;
    }
    i4_data = *(pi1_tmp + 1);
    *(pi1_tmp + 1) = 0; /* trip the filename in the pathfile. */
    pi1_data = utils_merger_strings(pi1_pathfile, pi1_filename);
    *(pi1_tmp + 1) = i4_data;

    return pi1_data;
}

CHAR*  get_file(CHAR* pi1_path, INT32*  pi4_file_size)
{
    int     fd;
    CHAR*   pi1_buff;
    INT32   i4_file_size;

    if (pi1_path == NULL || pi4_file_size == NULL) {
        return NULL;
    }

    *pi4_file_size = 0;
    if (!(fd = open(pi1_path, O_RDONLY))) {
        return NULL;
    }

    i4_file_size = lseek(fd, 0, SEEK_END);
    if (i4_file_size <= 0) {
        close(fd);
        return NULL;
    }
    lseek(fd, 0, SEEK_SET);
    pi1_buff = malloc(i4_file_size);
    if (pi1_buff == NULL) {
        close(fd);
        return NULL;
    }
    if (read(fd, pi1_buff, i4_file_size) <= 0) {
        free(pi1_buff);
        close(fd);
        return NULL;
    }
    close(fd);

    *pi4_file_size = i4_file_size;

    return pi1_buff;
}

typedef struct _M3U8_PLAYLIST_T {
    struct list_head t_list;
    UINT32           ui4_status; /* 0:stop  1:pause  2:play/seek   for pre-setting. */
    UINT32           ui4_start;  /* the media fragment file's msec-time used for start to playback */
    UINT32           ui4_end;    /* the media fragment file's msec-time used for end to playback */
    HANDLE_T         h_file;     /* the handle is created by open for the uri. */
    CHAR*            pi1_uri;    /* the media fragment file's uri for downlong from the httpserver */
    UINT32           ui4_skip_bytes; /* For seek and play. */
} M3U8_PLAYLIST_T;

typedef struct _M3U8_PLAYBACK_T {
    struct list_head t_list;
    CHAR*            pi1_m3u8_uri;
    UINT32           ui4_duration;
    UINT32           ui4_media_sequence;
    UINT32           ui4_total;
    HANDLE_T         h_file;          
    M3U8_PLAYLIST_T* pt_m3u8;        /* For pause */
    UINT32           ui4_status;     /* Status Machine For The Current Player */
    INT32  (*play_by_push)(M3U8_PLAYLIST_T *pt_m3u8);
    INT32  (*stop_by_push)(VOID);
    INT32  (*pause_by_push)(VOID);
} M3U8_PLAYBACK_T;

/* Only use for parsing the m3u8 file. */
typedef  enum {
    PARSE_M3U8_RECOGNIZE,
    PARSE_M3U8_VERSION,
    PARSE_M3U8_DURATION,
    PARSE_M3U8_SEQUENCE,
    PARSE_M3U8_EXTINF1,
    PARSE_M3U8_EXTINF2,
    PARSE_M3U8_END,
    PARSE_M3U8_EXIT,
} PARSE_M3U8_STATUS_T;

VOID  delete_m3u8(M3U8_PLAYBACK_T*  pt_playback)
{
    M3U8_PLAYLIST_T*    pt_pos_list;
    M3U8_PLAYLIST_T*    pt_n_list;

    if (pt_playback) {
        list_for_each_entry_safe(pt_pos_list, pt_n_list, &pt_playback->t_list, t_list) {
            list_del(&pt_pos_list->t_list);
            free(pt_pos_list->pi1_uri);
            free(pt_pos_list);
        }

        free(pt_playback);
    }
}

/* base on the status machine. */
M3U8_PLAYBACK_T*  parse_m3u8(struct list_head*  pt_tbl, CHAR*  pi1_uri)
{
    M3U8_PLAYBACK_T*    pt_playback;
    M3U8_PLAYLIST_T*    pt_pos_list;
    LINE_ON_STREAM_T*   pt_pos;
    LINE_ON_STREAM_T*   pt_n;
    TOKEN_UINT32_T      t_token_uint32;
    UINT32              ui4_index;
    UINT32              ui4_len;
    CHAR*               pi1_data;
    UINT32              ui4_timebase;
    UINT32              ui4_prev_duration;

    if (!pt_tbl || list_empty(pt_tbl) || !pi1_uri) {
        return NULL;
    }

    if (!(pt_playback = malloc(sizeof(M3U8_PLAYBACK_T)))) {
        return NULL;
    }
    memset(pt_playback, 0, sizeof(M3U8_PLAYBACK_T));
    INIT_LIST_HEAD(&pt_playback->t_list);

    ui4_index = PARSE_M3U8_RECOGNIZE;
    pt_pos = NULL;
    list_for_each_entry_safe(pt_pos, pt_n, pt_tbl, t_list) {

PARSE_M3U8_RETRY_SWITCH:
        switch (ui4_index) {
        case   PARSE_M3U8_RECOGNIZE:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            pi1_data = utils_strstr(pi1_data, &ui4_len, "#EXTM3U");
            if (!pi1_data || !ui4_len) { /* Not found. */
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            ui4_index = PARSE_M3U8_VERSION;
            break;

        case   PARSE_M3U8_VERSION:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            t_token_uint32.pi1_token = "#EXT-X-VERSION:";
            if (utils_get_token_uint32(pi1_data, ui4_len, &t_token_uint32)) {
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            ui4_index = PARSE_M3U8_DURATION;            
            break;

        case   PARSE_M3U8_DURATION:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            t_token_uint32.pi1_token = "#EXT-X-TARGETDURATION:";
            if (utils_get_token_uint32(pi1_data, ui4_len, &t_token_uint32)) {
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            pt_playback->ui4_duration = t_token_uint32.ui4_value;
            ui4_index = PARSE_M3U8_SEQUENCE;
            break;

        case   PARSE_M3U8_SEQUENCE:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            t_token_uint32.pi1_token = "#EXT-X-MEDIA-SEQUENCE:";
            if (utils_get_token_uint32(pi1_data, ui4_len, &t_token_uint32)) {
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            pt_playback->ui4_media_sequence = t_token_uint32.ui4_value;
            ui4_timebase = 0;
            ui4_prev_duration = 0;
            ui4_index = PARSE_M3U8_EXTINF1;
            break;

        case   PARSE_M3U8_EXTINF1:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            t_token_uint32.pi1_token = "#EXTINF:";
            if (utils_get_token_uint32_extern(pi1_data, ui4_len, &t_token_uint32, 1000)) {
                ui4_index = PARSE_M3U8_END;
                goto PARSE_M3U8_RETRY_SWITCH;
            }

            if (!t_token_uint32.ui4_value) {
                goto PARSE_M3U8_FAIL_HANDLE;
            }
            ui4_timebase += ui4_prev_duration;
            ui4_prev_duration = t_token_uint32.ui4_value;
            ui4_index = PARSE_M3U8_EXTINF2;
            break;

        case   PARSE_M3U8_EXTINF2:   /* get uri. */
            pi1_data = utils_replace_file_and_merger_to_path(pi1_uri,
                                                             pt_pos->pv_data);
            if (!pi1_data) {
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            if (!(pt_pos_list = malloc(sizeof(M3U8_PLAYLIST_T)))) {
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            memset(pt_pos_list, 0, sizeof(M3U8_PLAYLIST_T));
            pt_pos_list->pi1_uri = pi1_data;
            pt_pos_list->ui4_start = ui4_timebase;
            pt_pos_list->ui4_end = ui4_timebase + ui4_prev_duration - 1;
            pt_playback->ui4_total = pt_pos_list->ui4_end + 1;
            list_add_tail(&pt_pos_list->t_list, &pt_playback->t_list);
            ui4_index = PARSE_M3U8_EXTINF1;         
            break;

        case   PARSE_M3U8_END:
            ui4_len = pt_pos->ui4_len;
            pi1_data = pt_pos->pv_data;
            pi1_data = utils_strstr(pi1_data, &ui4_len, "#EXT-X-ENDLIST");
            if (!pi1_data || !ui4_len) { /* Not found. */
                goto  PARSE_M3U8_FAIL_HANDLE;
            }
            return pt_playback;

        default:
            goto  PARSE_M3U8_FAIL_HANDLE;
        }
    }

PARSE_M3U8_FAIL_HANDLE:

    printf("It is a invalid extm3u file\r\n");
    delete_m3u8(pt_playback);

    return NULL;
}

M3U8_PLAYBACK_T*  new_m3u8_playback(CHAR* pi1_uri)
{
    M3U8_PLAYBACK_T*     pt_playback;
    UINT32               ui4_stream_len;
    UINT32               ui4_stream_tmplen;
    CHAR*                pi1_base;
    CHAR*                pi1_stream;
    struct list_head*    pt_tbl;
    TOKEN_UINT32_T       t_token_uint32;

    pi1_stream = get_file("./db.txt", (INT32 *)&ui4_stream_len);
    pi1_base = pi1_stream;
    if (pi1_stream == NULL) {
        printf("Failed to get_file()\r\n");
        return NULL;
    }
    pi1_base = utils_skip_on_stream(pi1_base, &ui4_stream_len, "\r\t\n ");
    if (!pi1_base) {
        free(pi1_stream);
        return NULL;
    }

    t_token_uint32.pi1_token = "#EXTINF:";

    ui4_stream_tmplen = ui4_stream_len;

    pt_tbl = utils_get_all_line_on_stream(pi1_base, &ui4_stream_tmplen);

    pt_playback = parse_m3u8(pt_tbl, pi1_uri);

    return pt_playback;

    free(pi1_stream);
    utils_del_all_line_on_stream(pt_tbl);

    delete_m3u8(pt_playback);

    return 0;
}



#include <ctype.h>

#define NR_KBD_USER_DUMP   (1024 * 1024 * 10)

/**
 * kbd_user_dump - 用户数据的dump接口
 * @start: 用户数据的起始地址
 * @size:  用户数据的字节大小, 不允许超出10MB.
 *
 * 在终端上显示任何数据,其显示的格式如下实例:
 * 00000000: 68 65 6c 6c 6f 20 77 6f 72 6c 64 2c 20 78 78 78 | hello world, xxx
 * 00000016: 78 20 79 79 79 79 20 7a 7a 7a 7a 20 61 61 61 61 | x yyyy zzzz aaaa
 * 00000032: 20 62 62 62 62 62 62 62 62 62 62 20 2b 2d 2a 2f |  bbbbbbbbbb +-* /
 * 00000048: 20 30 31 32 33 34 35 36 37 38 39 00 00 00 00 00 |  0123456789.....
 * 00000064: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
 * 00000080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
 * 00000096: 00 00 00 00                                     | ....
 */
void  kbd_user_dump(unsigned char *start, size_t size)
{
    int   row;
    int   col;
    unsigned char ch;

    if (start == NULL || size == 0 || size > NR_KBD_USER_DUMP) {
        return ;
    }

    printf("\n\r");

    for (row = 0; row < (int)(size / 16); row++) {
        /* 偏移地址 */
        printf("%08u: ", row * 16);

        /* 十六进制表 */
        for (col = 0; col < 16; col++) {
            printf("%02x ", start[row * 16 + col]);
        }
        printf("| ");

        /* 可显字符表 */
        for (col = 0; col < 16; col++) {
            ch = start[row * 16 + col];
            isprint(ch) ? printf("%c", ch) : printf(".");
        }
        printf("\n\r");
    }

    if (size % 16) {
        /* 偏移地址 */
        printf("%08u: ", row * 16);

        /* 十六进制表 */
        for (col = 0; col < 16; col++) {
            if (col < (int)(size % 16)) {
                printf("%02x ", start[row * 16 + col]);
            } else {
                printf("   "); /* 输出三个空白格 */
            }
        }
        printf("| ");

        /* 可显字符表 */
        for (col = 0; col < 16; col++) {
            if (col < (int)(size % 16)) {
                ch = start[row * 16 + col];
                isprint(ch) ? printf("%c", ch) : printf(".");
            }
        }
        printf("\n\r");
    }
    printf("\n\r");

    return ;
}



int main(int argc, char **argv)
{
    M3U8_PLAYBACK_T*   pt_playback;
    M3U8_PLAYLIST_T*   pt_pos_list;
    M3U8_PLAYLIST_T*   pt_n_list;

    pt_playback = new_m3u8_playback("http://192.168.100.100/m3u8.txt");
    if (!pt_playback) {
        printf("Failed to parse_m3u8\r\n");
    } else {
        printf("EXTM3U\r\n");
        printf("EXT-X-TARGETDURATION:%d\r\n", pt_playback->ui4_duration);
        printf("EXT-X-MEDIA-SEQUENCE:%d\r\n", pt_playback->ui4_media_sequence);
        list_for_each_entry_safe(pt_pos_list, pt_n_list, &pt_playback->t_list, t_list) {
            printf("-----\r\nstart:%d\r\nend:%d\r\nuri:%s\r\n",
                   pt_pos_list->ui4_start,
                   pt_pos_list->ui4_end,
                   pt_pos_list->pi1_uri
                  );
        }
    }

    delete_m3u8(pt_playback);

    return 0;
}

