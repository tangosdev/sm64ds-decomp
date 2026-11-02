/* NitroSDK FSi_ReadFileCore(p_file, dst, len, async): clamp len to what is
 * left between prop.file.pos and prop.file.bottom (and to 0), store the
 * request in arg.readfile, and send FS_COMMAND_READFILE (0) through
 * FSi_SendCommand. A synchronous caller (async == 0; FS_ReadFile)
 * marks the file FS_FILE_STATUS_SYNC (4) first, waits in func_0205d3f4
 * (FS_WaitAsync) and returns the bytes the position moved, or -1 on failure;
 * func_0205d3e4 (FS_ReadFileAsync) returns the clamped length at once. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern int FSi_SendCommand(void *self, int a);
extern int func_0205d3f4(void *self);
int func_0205d688(char *self, int dst, int len, int async) {
    FSFile *p_file = (FSFile *)self;
    int n;
    int is_async;
    int oldpos;
    oldpos = p_file->prop.file.pos;
    /* The (long long) launders are load-bearing: plain copies change .text. */
    n = (int)(((long long)len));
    is_async = (int)(((long long)async));
    {
        int avail = p_file->prop.file.bottom - oldpos;
        if (n > avail) {
            n = avail;
        }
    }
    p_file->arg.readfile.dst = (void *)dst;
    if (n < 0) {
        n = 0;
    }
    p_file->arg.readfile.len_org = len;
    p_file->arg.readfile.len = n;
    if (is_async == 0) {
        p_file->stat |= 4;
    }
    FSi_SendCommand(self, 0);
    if (is_async == 0) {
        if (func_0205d3f4(self)) {
            n = p_file->prop.file.pos - oldpos;
        } else {
            n = -1;
        }
    }
    return n;
}
