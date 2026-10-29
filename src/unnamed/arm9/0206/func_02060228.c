extern char data_020a8180[];
extern int data_020a6134[];
extern int func_02057e7c(int *p);
extern int func_02057e84(void *node, int value);
extern void func_02058048(void *self);

struct CardCommon;
typedef void (*CardTask)(struct CardCommon *p);

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches. mFlags, mThread and mCurThread follow
 * func_0206081c.cpp and func_02060484.cpp. mTaskFunc (0x30, the SDK's
 * task_func) sits just before the flags and mPriority (0x38, the SDK's
 * priority) just after them; func_020602bc runs mTaskFunc on the card
 * thread. */
typedef struct CardCommon {
    unsigned char pad_00[0x30];
    CardTask mTaskFunc; /* 0x30 */
    /* 0x34 -- CARD_STAT_* bits: 8 a task for the card thread */
    unsigned int mFlags;
    /* 0x38 -- the card thread's priority, or 0x20 / 0x21 for one step
     * higher / lower than the caller's */
    int mPriority;
    char mThread[0x94]; /* 0x3c -- the card task thread */
    char *mCurThread;   /* 0xd0 -- the thread the request runs on */
} CardCommon;

#ifndef SM64DS_PLATFORM_PC
typedef char
    CardCommon_mCurThread_must_be_at_0xd0[(int)&((CardCommon *)0)->mCurThread == 0xd0 ? 1 : -1];
#endif

/* NitroSDK CARDi_SetTask: set the card thread to mPriority (relative to
 * the calling thread for 0x20 / 0x21; func_02057e7c / func_02057e84 get and
 * set a thread's priority), hand it the task, flag it and wake it
 * (func_02058048). */
void func_02060228(void *fn)
{
    CardCommon *p = (CardCommon *)data_020a8180;
    int v = p->mPriority;
    switch (v) {
    case 0x20:
        v = func_02057e7c((int *)data_020a6134[2]);
        if (v != 0) v -= 1;
        break;
    case 0x21:
        v = func_02057e7c((int *)data_020a6134[2]);
        if ((unsigned int)v < 0x1f) v += 1;
        break;
    }
    func_02057e84(p->mThread, v);
    p->mCurThread = p->mThread;
    p->mTaskFunc = (CardTask)fn;
    p->mFlags |= 8;
    func_02058048(p->mThread);
}
