//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

/* NitroSDK FS_NotifyArchiveAsyncEnd(p_arc, ret): the archive's procedure
 * finished the command at the head of its list. An asynchronous one
 * (FS_ARCHIVE_FLAG_IS_ASYNC 0x100) is released with ret (func_0205c788 =
 * FSi_ReleaseCommand) and the next command started (func_0205d044 =
 * FSi_NextCommand, func_0205cfa4 = FSi_ExecuteAsyncCommand); a synchronous
 * one gets ret as its error, FS_ARCHIVE_FLAG_IS_SYNC (0x200) is cleared and
 * the waiting caller woken on sync_q. Field names follow the SDK's FSFile
 * and FSArchive, as func_0205d044.cpp spells them; only the fields this
 * function touches are spelled out. */
typedef struct FSFile {
    char pad_00[0x14];
    int error; /* 0x14 */
} FSFile;

typedef struct FSArchive {
    char pad_00[0xc];
    u16 sync_q; /* 0x0c -- the thread queue a synchronous caller sleeps on */
    u16 stat_q; /* 0x0e */
    u32 flag;   /* 0x10 */
    struct {
        FSFile *prev; /* 0x14 */
        FSFile *next; /* 0x18 -- the command list's head */
    } list;
} FSArchive;

extern void func_0205c788(void *p, int arg);
extern int func_0205d044(void *self);
extern void func_0205cfa4(int x);
extern void OS_WakeupThread(u16 *p);

void func_0205cd5c(FSArchive *self, int arg) {
    /* The FS_IsArchiveAsync test is materialized as a 0/1 int; testing the
       flag in the if directly moves bytes. */
    int b = (int)((self->flag & 0x100) != 0);

    if (b) {
        FSFile *t = self->list.next;
        int r;

        self->flag &= ~0x100;
        func_0205c788(t, arg);
        r = func_0205d044(self);
        if (r == 0) {
            return;
        }
        func_0205cfa4(r);
        return;
    }
    {
        FSFile *p = self->list.next;
        u32 irq = IRQ::Disable();

        p->error = arg;
        self->flag &= ~0x200;
        OS_WakeupThread(&self->sync_q);
        IRQ::Restore(irq);
    }
}
}
