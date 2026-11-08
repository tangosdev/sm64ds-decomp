//cpp
#pragma long_calls on
#include "types.h"
#include "IRQ.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

extern int FSi_SendCommand(FSFile *self, int a);
extern void OS_SleepThread(unsigned short *self);
extern int FSi_ExecuteSyncCommand(FSFile *);
extern int func_0205d688(FSFile *p_file, void *dst, int len, int async);

/* NitroSDK FS_GetPathName: store buf and len in arg.getpath, restarting the
   measure unless the last command was already FS_COMMAND_GETPATH (5), and
   send it. */
int func_0205d4a0(FSFile *p_file, char *buf, u32 len) {
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

int func_0205d3f4(FSFile *t) {
    int woken = 0;
    u32 irq = IRQ::Disable();
    if (FS_IsBusy(t)) {
        woken = (t->arc->list.next != t) ? 1 : 0;
        if (woken) {
            t->stat |= 4;
        }
        do {
            OS_SleepThread(&t->queue);
        } while (FS_IsBusy(t));
    }
    IRQ::Restore(irq);
    if (woken) {
        return FSi_ExecuteSyncCommand(t);
    }
    return t->error == 0;
}

void func_0205d3e4(FSFile *p_file, void *dst, int len) {
    func_0205d688(p_file, dst, len, 1);
}
}
