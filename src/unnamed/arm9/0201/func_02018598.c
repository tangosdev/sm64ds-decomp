#include "private/fs_views.h"
/* the ROM archive, as func_020189f0 last saw it resolve a path */
extern int data_0209d3b4;

/* Build the FSFileID for FAT file id arg in the ROM archive. */
void func_02018598(FSFileID *c, int arg) {
    FSFileID t;
    t.file_id = arg;
    t.arc = (FSArchive *)data_0209d3b4;
    *c = t;
}
