#include "types.h"
extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void func_0205ba64(int a, void *fn);
extern void func_0205f300(void);

extern char data_020a80c8[];
extern char data_020a80cc[];

/* The touch-panel work at data_020a80cc (NitroSDK tp.c's state). Offsets
 * from the sibling TP functions: func_0205f300, the PXI receive callback,
 * calls mCallback, fills mBuf, advances mIndex through mSamplingBufs up to
 * mBufSize, sets mState and the per-command bits in mErrFlags /
 * mCommandFlags; func_0205eeac sets up auto sampling (mSamplingBufs,
 * mIndex, mFrequency, mBufSize); func_0205f0e0 fills mCalibration and sets
 * mCalibrated, or clears mCalibrated when given no parameters. */
typedef struct {
    void (*mCallback)(int, int, int); /* 0x00 */
    u16 mBuf[4];                      /* 0x04 -- the last TPData */
    u16 mIndex;                       /* 0x0c */
    u16 mFrequency;                   /* 0x0e */
    void *mSamplingBufs;              /* 0x10 */
    u16 mBufSize;                     /* 0x14 */
    u8 pad_16[2];
    int mCalibration[6];        /* 0x18 */
    u16 mCalibrated;            /* 0x30 */
    volatile u16 mState;        /* 0x32 */
    volatile u16 mErrFlags;     /* 0x34 */
    volatile u16 mCommandFlags; /* 0x36 */
} TpWork;

#define TP_WORK (*(TpWork *)data_020a80cc)

void func_0205f270(void)
{
    if (*(u16 *)data_020a80c8 != 0) return;
    *(u16 *)data_020a80c8 = 1;
    func_0205b858();
    TP_WORK.mState = 0;
    TP_WORK.mCommandFlags = 0;
    TP_WORK.mIndex = 0;
    TP_WORK.mCallback = 0;
    TP_WORK.mSamplingBufs = 0;
    TP_WORK.mCalibrated = 0;
    TP_WORK.mErrFlags = 0;
    while (func_0205ba3c(6, 1) == 0)
        ;
    func_0205ba64(6, (void *)func_0205f300);
}
