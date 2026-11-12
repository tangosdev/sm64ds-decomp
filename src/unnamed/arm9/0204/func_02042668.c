#include "private/arm9_a1fc0_file_share.h"
#include "nitro/fs/archive.h"

extern void func_02041a00(FSFile *self);

/* The child's archive proc. func_020423dc installs it for every command with
 * FS_SetArchiveProc. A read fails with FS_RESULT_ERROR until the share is
 * ready (mState 2), and succeeds at once when empty. Otherwise
 * func_02041a00 asks the parent for the data and the read goes asynchronous
 * (FS_RESULT_PROC_ASYNC). A write is unsupported, and any other command
 * goes back to the default handling (FS_RESULT_PROC_UNKNOWN). */
int func_02042668(FSFile *p_file, int command) {
    switch (command) {
    case FS_COMMAND_READFILE:
        if (data_020a1fc0.mState != 2)
            return FS_RESULT_ERROR;
        if (p_file->arg.readfile.len == 0)
            return FS_RESULT_SUCCESS;
        func_02041a00(p_file);
        return FS_RESULT_PROC_ASYNC;
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}
