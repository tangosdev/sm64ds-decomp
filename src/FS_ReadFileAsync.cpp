//cpp
#pragma long_calls on
#include "types.h"
#include "nitro/os/system.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

extern int FSi_SendCommand(FSFile *self, int a);
extern void OS_SleepThread(unsigned short *self);
extern int FSi_ExecuteSyncCommand(FSFile *);
extern int FSi_ReadFileCore(FSFile *p_file, void *dst, int len, int async);

/* NitroSDK FS_GetPathName: store buf and len in arg.getpath, restarting the
   measure unless the last command was already FS_COMMAND_GETPATH (5), and
   send it. */
int FS_GetPathName(FSFile *p_file, char *buf, u32 len) {
    if (p_file->command != 5) {
        p_file->arg.getpath.total_len = 0;
        p_file->arg.getpath.dir_id = 0;
    }
    p_file->arg.getpath.buf = (unsigned char *)buf;
    p_file->arg.getpath.buf_len = len;
    return FSi_SendCommand(p_file, 5);
}

/* NitroSDK FS_IsBusy: stat bit 0 (FS_FILE_STATUS_BUSY) as a BOOL. */
static inline int FS_IsBusy(FSFile *p_file) {
    return (p_file->stat & 1) ? 1 : 0;
}

int FS_WaitAsync(FSFile *p_file) {
    int is_owner = 0;
    OSIntrMode irq = OS_DisableInterrupts();
    if (FS_IsBusy(p_file)) {
        is_owner = (p_file->arc->list.next != p_file) ? 1 : 0;
        if (is_owner) {
            p_file->stat |= 4;
        }
        do {
            OS_SleepThread(&p_file->queue);
        } while (FS_IsBusy(p_file));
    }
    OS_RestoreInterrupts(irq);
    if (is_owner) {
        return FSi_ExecuteSyncCommand(p_file);
    }
    return p_file->error == 0;
}

int FS_ReadFileAsync(FSFile *p_file, void *dst, int len) {
    return FSi_ReadFileCore(p_file, dst, len, 1);
}
}
