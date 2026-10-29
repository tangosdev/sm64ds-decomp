//cpp
#include "IRQ.h"

extern "C" {

extern void OS_WakeupThread(char* p);
extern void func_02058048(char* p);
typedef void (*CB)(void*);

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches; names as func_02060484.cpp spells them, plus
 * mThread. */
typedef struct CardCommon {
    unsigned char pad_00[0x28];
    CB mCallback;       /* 0x28 */
    void *mCallbackArg; /* 0x2c */
    unsigned char pad_30[4];
    /* 0x34 -- CARD_STAT_* bits: 4 busy, 8 a task for the card thread (set by
     * func_02060228, waited for by func_020602bc); with 0x10 set, ending a
     * task also wakes mThread */
    unsigned int mFlags;
    unsigned char pad_38[4];
    /* 0x3c -- the card task thread: func_02060228 points mCurThread (0xd0) at
     * it and wakes it with func_02058048 (OS_WakeupThreadDirect: mark it
     * ready and reschedule) */
    char mThread[0x94];
    unsigned char pad_d0[4];
    char mBusyQueue[4]; /* 0xd4 */
} CardCommon;

#ifndef SM64DS_PLATFORM_PC
typedef char
    CardCommon_busy_queue_must_be_at_0xd4[(int)&((CardCommon *)0)->mBusyQueue == 0xd4 ? 1 : -1];
#endif

/* NitroSDK CARDi_EndTask: take the callback, clear the busy and task bits,
 * wake every caller waiting on mBusyQueue and, if the card thread is waiting
 * for work, the card thread itself; then, interrupts restored, run the
 * callback. */
void func_0206081c(char* obj) {
    CardCommon *p = (CardCommon *)obj;
    CB cb = p->mCallback;
    void *arg = p->mCallbackArg;
    unsigned int saved = IRQ::Disable();
    p->mFlags &= ~0xc;
    OS_WakeupThread(p->mBusyQueue);
    if (p->mFlags & 0x10) {
        func_02058048(p->mThread);
    }
    IRQ::Restore(saved);
    if (cb != 0) {
        cb(arg);
    }
}
}
