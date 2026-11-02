#include "types.h"

/* NitroSDK FSFile, this SDK's layout (src/engine/fs/FS_Archive.cpp spells
   out the same record); only the fields cleared here are named. */
typedef struct FSFile {
    struct FSFile *prev; /* 0x00 -- link.prev */
    struct FSFile *next; /* 0x04 -- link.next */
    void *arc;           /* 0x08 */
    u32 stat;            /* 0x0c */
    int command;         /* 0x10 */
    int error;           /* 0x14 */
    u16 queue;           /* 0x18 -- OSThreadQueue */
} FSFile;

/* The callers pass the file as int *; the parameter keeps that spelling. */
void FS_InitFile(int *s)
{
    FSFile *p_file = (FSFile *)s;

    p_file->prev = 0;
    p_file->next = p_file->prev;
    p_file->queue = 0;
    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat = 0;
}
