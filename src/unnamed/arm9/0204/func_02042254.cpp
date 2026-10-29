//cpp
#pragma opt_strength_reduction off

#include "private/arm9_a1fc0_file_share.h"
#include "IRQ.h"

extern "C" {

extern char data_02099e6c[];
extern int data_020a1fc0;
extern char data_020a2400[];

extern void *func_0205d23c(void *a, int b);
extern void OS_SleepThread(unsigned short *self);
extern int func_0205d5e8(char *self, int a1, int a2, int a3, int a4);
extern int FS_ReadFile(char *a, char *b, int c);
extern void func_02041d28(char *base, void *node, int d);
extern int FS_CloseFile(char *self);
extern void func_02041a94(FileShareParentArea *area, FileShareNode *node);

/* The parent's loader thread: while the share is up (mState), take each node
 * off the busy list; a loading one (state 1) has its region's file opened
 * from the ROM archive (func_0205d5e8 = FS_OpenFileDirect over
 * [mFileOffset, mFileOffset + mFileSize), by mFileId), its first three
 * 0x400-byte pages read into the cache, and is handed to func_02041d28; a
 * releasing one has its file closed and goes back to the free list
 * (func_02041a94). The arc local is the archive func_0205d23c finds (by name,
 * 3 characters). */
void func_02042254(void) {
    int i;
    FileShareNode *node;
    int arc = (int)func_0205d23c(data_02099e6c, 3);
    int b = 2;
    int c = 3;
    int d = 1;
    FileShareWork *work = (FileShareWork *)&data_020a1fc0;
    char *base = data_020a2400;

    while (1) {
        unsigned int irq = IRQ::Disable();
        FileShareParentArea *area = (FileShareParentArea *)base;
        node = area->mBusyList;
        while (node == 0) {
            if (work->mState == 0) {
                IRQ::Restore(irq);
                return;
            }
            OS_SleepThread(0);
            node = area->mBusyList;
        }
        IRQ::Restore(irq);

        if (node->mState == 1) {
            func_0205d5e8((char *)node->mFile, arc, node->mFileOffset,
                          node->mFileOffset + node->mFileSize, node->mFileId);
            i = 0;
            {
                int val = 0;
                char *ptr = (char *)node->mPages;
                for (; i < 3; i++) {
                    node->mPageOffset[i] = val;
                    FS_ReadFile((char *)node->mFile, ptr, 0x400);
                    val += 0x400;
                    ptr += 0x400;
                }
            }
            node->mPrevSeqNo = 0;
            node->mLastRefillPage = b;
            node->mLoadingPage = c;
            irq = IRQ::Disable();
            func_02041d28(base, node, d);
            IRQ::Restore(irq);
        } else {
            FS_CloseFile((char *)node->mFile);
            irq = IRQ::Disable();
            func_02041a94(area, node);
            IRQ::Restore(irq);
        }
    }
}
}
