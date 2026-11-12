#include "nitro/fs/file.h"

void FS_InitFile(FSFile *p_file) {
    p_file->link.prev = 0;
    p_file->link.next = p_file->link.prev;
    p_file->queue = 0;
    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat = 0;
}
