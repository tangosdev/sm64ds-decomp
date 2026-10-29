//cpp
/* NitroSDK FSi_TranslateCommand(p_file, command): mark the archive busy for a
 * synchronous (FS_ARCHIVE_FLAG_IS_SYNC 0x200) or asynchronous
 * (FS_ARCHIVE_FLAG_IS_ASYNC 0x100) caller by the file's FS_FILE_STATUS_SYNC
 * (4); offer the command to the archive's proc when proc_flag has its bit,
 * keeping SUCCESS / FAILURE / UNSUPPORTED (0, 1, 4) as the file's error and
 * dropping the bit on PROC_UNKNOWN (8); fall back to the default command
 * table (data_02086758) on PROC_DEFAULT (7). An ASYNC (6) result from a
 * synchronous caller sleeps on sync_q until IS_SYNC clears; anything else
 * clears the busy bit and either releases the file (func_0205c788 =
 * FSi_ReleaseCommand) or stores the result for the waiter. Field names follow
 * the SDK's FSFile and FSArchive, as func_0205d044.cpp spells them; only the
 * fields this function touches are spelled out. */
#include "IRQ.h"

extern "C" {

typedef struct FSArchive FSArchive;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc;    /* 0x08 */
    unsigned int stat; /* 0x0c */
    int command;       /* 0x10 */
    int error;         /* 0x14 */
} FSFile;

typedef int (*FS_ARCHIVE_PROC_FUNC)(FSFile *p_file, int command);

struct FSArchive {
    char pad_00[0xc];
    unsigned short sync_q; /* 0x0c -- a synchronous caller waits here */
    unsigned short stat_q; /* 0x0e */
    unsigned int flag;     /* 0x10 */
    char pad_14[0x34];
    FS_ARCHIVE_PROC_FUNC proc; /* 0x48 */
    unsigned int proc_flag;    /* 0x4c -- which commands go to proc */
};

extern void OS_SleepThread(unsigned short*);
extern void func_0205c788(char*, int);
extern unsigned int (*data_02086758[])(char*);

int func_0205c5e4(char *self, int idx) {
    FSFile *p_file = (FSFile *)self;
    FSArchive *r;
    unsigned int flags;
    int ret;
    int mask;
    unsigned int irq;
    int enabled;
    int busy;

    /* enabled/t/t2/busy stay materialized 0/1 locals: testing the bit directly,
       != 0, or an inline predicate each shrinks the function (0x188-0x19c) */

    flags = p_file->stat;
    enabled = 1;
    r = p_file->arc;
    mask = 1 << idx;
    if ((flags & 4) == 0)
        enabled = 0;

    if (enabled) {
        r->flag |= 0x200;
    } else {
        r->flag |= 0x100;
    }

    if (r->proc_flag & mask) {
        ret = r->proc(p_file, idx);
        switch (ret) {
        case 0:
        case 1:
        case 4:
            p_file->error = ret;
            break;
        case 8:
            r->proc_flag &= ~mask;
            ret = 7;
            break;
        }
    } else {
        ret = 7;
    }

    if (ret == 7)
        ret = data_02086758[idx](self);

    if (ret == 6) {
        int t = (p_file->stat & 4) ? 1 : 0;
        if (t != 0) {
            unsigned short *p;
            irq = IRQ::Disable();
            p = &r->sync_q;
            while ((busy = (r->flag & 0x200) ? 1 : 0) != 0)
                OS_SleepThread(p);
            ret = p_file->error;
            IRQ::Restore(irq);
        }
    } else {
        int t2 = (p_file->stat & 4) ? 1 : 0;
        if (t2 == 0) {
            r->flag &= ~0x100;
            func_0205c788(self, ret);
        } else {
            r->flag &= ~0x200;
            p_file->error = ret;
        }
    }

    return ret;
}
}
