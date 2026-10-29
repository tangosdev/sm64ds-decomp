//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

/* NitroSDK FSi_NextCommand(p_arc): with interrupts off, release every queued
 * file marked for cancel (FS_ARCHIVE_FLAG_CANCELING 0x20, FS_FILE_STATUS_CANCEL
 * 2) as FS_RESULT_CANCELED (func_0205c788 = FSi_ReleaseCommand); unless the
 * archive is suspended (FS_ARCHIVE_FLAG_SUSPEND 8), mark it running
 * (FS_ARCHIVE_FLAG_RUNNING 0x10), call proc with FS_COMMAND_ACTIVATE (9) when
 * proc_flag asks for it (0x200), and return the head of the list -- or wake a
 * synchronous one (FS_FILE_STATUS_SYNC 4) and return NULL. Otherwise the
 * archive goes idle: proc gets FS_COMMAND_IDLE (0xa, proc_flag 0x400) on a
 * scratch FSFile, and a pending suspend (FS_ARCHIVE_FLAG_SUSPENDING 0x40)
 * becomes FS_ARCHIVE_FLAG_SUSPEND and wakes stat_q. Field names follow the
 * SDK's FSFile and FSArchive, as func_0205bc88.c and func_0205c9b8.c spell
 * them; only the fields this function touches are spelled out. */
typedef struct FSFile {
    struct FSFile *prev;   /* 0x00 -- link.prev */
    struct FSFile *next;   /* 0x04 -- link.next */
    struct FSArchive *arc; /* 0x08 */
    u32 stat;              /* 0x0c */
    int command;           /* 0x10 */
    int error;             /* 0x14 */
    u16 queue;             /* 0x18 -- the thread queue a synchronous caller sleeps on */
    char pad_1a[0x2a];     /* 0x1a -- prop and arg, unused here */
} FSFile;

typedef int (*FS_ARCHIVE_PROC_FUNC)(FSFile *p_file, int command);

typedef struct FSArchive {
    char pad_00[0xe];
    u16 stat_q; /* 0x0e -- woken once a suspend takes effect */
    u32 flag;   /* 0x10 */
    struct {
        FSFile *prev; /* 0x14 */
        FSFile *next; /* 0x18 -- the command list's head */
    } list;
    char pad_1c[0x2c];
    FS_ARCHIVE_PROC_FUNC proc; /* 0x48 */
    u32 proc_flag;             /* 0x4c -- which commands go to proc */
} FSArchive;

/* FS_InitFile fills a whole FSFile (0x44 bytes) on the stack. */
typedef char FSFile_size_must_be_0x44[sizeof(FSFile) == 0x44 ? 1 : -1];
typedef char FSArchive_proc_flag_must_be_at_0x4c[sizeof(FSArchive) == 0x50 ? 1 : -1];

extern void func_0205c788(char *self, int arg);
extern void OS_WakeupThread(u16 *self);
extern void FS_InitFile(int *s);

void *func_0205d044(void *pool) {
    FSArchive *p_arc = (FSArchive *)pool;
    u32 irqSave = IRQ::Disable();
    int f1 = (int)((*(volatile u32 *)&p_arc->flag & 0x20) != 0);

    if (f1 != 0) {
        p_arc->flag &= ~0x20;

        FSFile *head = p_arc->list.next;
        if (head != 0) {
            FSFile *node = head;
            do {
                FSFile *next = node->next;
                int flagset = (int)((*(volatile u32 *)&node->stat & 2) != 0);
                if (flagset != 0) {
                    if (p_arc->list.next == node)
                        p_arc->list.next = next;
                    func_0205c788((char *)node, 3);
                    if (next == 0)
                        next = p_arc->list.next;
                }
                node = next;
            } while (node != 0);
        }
    }

    {
        int f2 = (int)((*(volatile u32 *)&p_arc->flag & 8) != 0);
        if (f2 == 0) {
            FSFile *head2 = p_arc->list.next;
            if (head2 != 0) {
                int wasSet = (int)((*(volatile u32 *)&p_arc->flag & 0x10) != 0);
                int notSet = (int)(wasSet == 0);
                if (notSet != 0)
                    p_arc->flag |= 0x10;
                IRQ::Restore(irqSave);
                if (notSet != 0) {
                    if (p_arc->proc_flag & 0x200) {
                        FS_ARCHIVE_PROC_FUNC cb = p_arc->proc;
                        cb(head2, 9);
                    }
                }
                {
                    int nset = (int)((*(volatile u32 *)&head2->stat & 4) != 0);
                    if (nset != 0) {
                        OS_WakeupThread(&head2->queue);
                        return 0;
                    }
                }
                return head2;
            }
        }
    }

    {
        int f5 = (int)((*(volatile u32 *)&p_arc->flag & 0x10) != 0);
        if (f5 != 0) {
            p_arc->flag &= ~0x10;
            if (p_arc->proc_flag & 0x400) {
                FSFile tmp;
                FS_InitFile((int *)&tmp);
                tmp.arc = p_arc;
                {
                    FS_ARCHIVE_PROC_FUNC cb = p_arc->proc;
                    cb(&tmp, 0xa);
                }
            }
        }
    }

    {
        int f7 = (int)((*(volatile u32 *)&p_arc->flag & 0x40) != 0);
        if (f7 != 0) {
            p_arc->flag &= ~0x40;
            p_arc->flag |= 8;
            OS_WakeupThread(&p_arc->stat_q);
        }
    }

    IRQ::Restore(irqSave);
    return 0;
}
}
