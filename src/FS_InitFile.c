#include "private/fs_views.h"

/* The callers pass the file as int *; the parameter keeps that spelling. */
void FS_InitFile(int *s)
{
    struct FSFile *p_file = (struct FSFile *)s;

    p_file->link.prev = 0;
    p_file->link.next = p_file->link.prev;
    p_file->queue = 0;
    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat = 0;
}
