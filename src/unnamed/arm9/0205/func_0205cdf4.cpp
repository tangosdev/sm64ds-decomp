//cpp

namespace IRQ {
unsigned int Disable(void);
void Restore(unsigned int saved);
}
extern "C" {

extern void func_0205c788(char *self, int arg);
extern void func_0205d8d8(void *node, void *list);
extern void func_0205cfa4(char *self);
extern void OS_SleepThread(char *self);
extern int func_0205cf5c(char *self);

/* The owner a request is queued on (NitroSDK FSArchive). */
struct FsArchive {
    unsigned char pad_00[0x10];
    int mFlags;       /* 0x10 */
    char mPending[4]; /* 0x14 -- the request list func_0205d8d8 appends to */
    char *mCurrent;   /* 0x18 -- the request being run */
    unsigned char pad_1c[0x2c];
    void (*mCallback)(char *, int); /* 0x48 */
    int mCallbackMask;              /* 0x4c */
};

/* A queued request (NitroSDK FSFile). */
struct FsFile {
    unsigned char pad_00[8];
    FsArchive *mArchive; /* 0x08 */
    int mStat;           /* 0x0c */
    int mCommand;        /* 0x10 */
    int mResult;         /* 0x14 */
    char mQueue[4];      /* 0x18 -- the thread queue a waiter sleeps on */
};

/* The ROM materialises each of these flag tests as 0/1 before branching on it. */
static inline int HasBits(int word, int bits) {
    return (word & bits) != 0;
}

#pragma opt_loop_invariants off
int func_0205cdf4(char *self, int arg1)
{
    FsFile *file = (FsFile *)self;
    FsArchive *list = file->mArchive;
    unsigned int mask = 1 << arg1;
    unsigned int saved;

    file->mCommand = arg1;
    file->mResult = 2;
    file->mStat |= 1;
    saved = IRQ::Disable();
    if (list->mFlags & 0x80) {
        func_0205c788(self, 3);
        IRQ::Restore(saved);
        return 0;
    }
    if (mask & 0x1fc) {
        file->mStat |= 4;
    }
    func_0205d8d8(self, list->mPending);
    if (!HasBits(list->mFlags, 0x10)) {
        list->mFlags |= 0x10;
        IRQ::Restore(saved);
        if (list->mCallbackMask & 0x200) {
            list->mCallback(self, 9);
        }
        if (!HasBits(file->mStat, 4)) {
            func_0205cfa4(self);
            return 1;
        }
    } else {
        if (!HasBits(file->mStat, 4)) {
            IRQ::Restore(saved);
            return 1;
        }
        do {
            OS_SleepThread(file->mQueue);
        } while (self != list->mCurrent);
        IRQ::Restore(saved);
    }
    return func_0205cf5c(self);
}
}
