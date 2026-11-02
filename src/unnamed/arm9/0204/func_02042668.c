#include "private/arm9_a1fc0_file_share.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern char data_020a1fc0[];
extern void func_02041a00(FSFile *self);

/* The child's archive proc. func_020423dc installs it for every command with
 * FS_SetArchiveProc, and it returns NitroSDK FSResult codes.
 * A read (FS_COMMAND_READFILE, 0) fails with 5 (ERROR) until the share is
 * ready (mState 2), and succeeds at once (0) when empty. Otherwise
 * func_02041a00 asks the parent for the data and the read goes asynchronous
 * (6, PROC_ASYNC). A write (1) is unsupported (4), and any other command
 * falls through to the default handling (8, PROC_UNKNOWN). */
int func_02042668(FSFile *self, int sel) {
    switch (sel) {
    case 0:
        if (FILE_SHARE_WORK->mState != 2)
            return 5;
        if (self->arg.readfile.len == 0)
            return 0;
        func_02041a00(self);
        return 6;
    case 1:
        return 4;
    default:
        return 8;
    }
}
