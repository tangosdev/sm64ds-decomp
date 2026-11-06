#ifndef NITRO_SPI_TP_H
#define NITRO_SPI_TP_H

/*
 * The touch panel, under the NitroSDK's own names. The bodies are SPI_tp.c,
 * still one function per file under src/, each named for its TP_ function
 * (TP_Init.c, TP_GetCalibratedPoint.c, ..., TPi_TpCallback.c).
 *
 * TPData and its members are the SDK's own spellings (they match pret's
 * pokediamond and pmd-sky tp.h). TPWork's members are coined from how the
 * sibling functions use them; the cartridge carries no names.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One panel sample. TP_GetCalibratedPoint transforms a raw pair into screen
   coordinates and copies the other two words through unchanged, so the first
   two are the axes and the last two the touch and validity flags the SDK
   carries alongside them. func_0203df40's input record carries one at +0x4. */
typedef struct TPData {
    u16 x;        /* 0x0 */
    u16 y;        /* 0x2 */
    u16 touch;    /* 0x4 */
    u16 validity; /* 0x6 */
} TPData;

/* tp.c's state, data_020a80cc.
 *
 * SIZE 0x38, fixed two ways: the first symbol row past the interior
 * data_020a80e4 (below) is data_020a8104, exactly 0x38 from the start, and
 * the last member is the volatile halfword pair the command helpers poll at
 * +0x34/+0x36.
 *
 * Offsets from the sibling functions: TPi_TpCallback, the PXI receive
 * callback, calls mCallback, fills mBuf, advances mIndex through
 * mSamplingBufs up to mBufSize, sets mState and the per-command bits in
 * mErrFlags / mCommandFlags; TP_RequestAutoSamplingStartAsync sets up auto
 * sampling (mSamplingBufs, mIndex, mFrequency, mBufSize);
 * TP_SetCalibrateParam fills mCalibration and sets mCalibrated, or clears
 * mCalibrated when given no parameters; TP_GetCalibratedPoint reads
 * mCalibration and mCalibrated to transform a sample.
 *
 * mCalibration keeps its own symbol row, data_020a80e4: relocs.txt names that
 * address from TP_GetCalibratedPoint's literal pool, and dsd needs a symbol at
 * every relocation target. The compiler reaches it as data_020a80cc + 0x18,
 * which resolves to the same word.
 *
 * mSamplingBufs is a TPData array: TP_RequestAutoSamplingStartAsync clears
 * each entry's third word with a byte stride of 8 (`(i << 3) + 4`), and
 * TPi_TpCallback indexes it by mIndex with that same stride. */
typedef struct TPWork {
    void (*mCallback)(int, int, int); /* 0x00 */
    TPData mBuf;                      /* 0x04 -- the last sample */
    u16 mIndex;                       /* 0x0c -- into mSamplingBufs */
    u16 mFrequency;                   /* 0x0e */
    TPData *mSamplingBufs;            /* 0x10 */
    u16 mBufSize;                     /* 0x14 -- entries in mSamplingBufs */
    u8 pad_16[2];                     /* 0x16 */
    int mCalibration[6];              /* 0x18 */
    u16 mCalibrated;                  /* 0x30 -- zero until calibrated */
    volatile u16 mState;              /* 0x32 */
    volatile u16 mErrFlags;           /* 0x34 */
    volatile u16 mCommandFlags;       /* 0x36 */
} TPWork;

/* The one work block, as include/nitro/card/card_common.h declares cardi_common.
   Every TP function reaches it by this name. */
extern TPWork data_020a80cc;

/* The SDK's TPCalibrateParam: the screen origin and dot size, in raw panel
   units, that TP_CalcCalibrateParam derives from two point pairs. */
typedef struct TPCalibrateParam {
    s16 x0;       /* 0x0 */
    s16 y0;       /* 0x2 */
    s16 xDotSize; /* 0x4 */
    s16 yDotSize; /* 0x6 */
} TPCalibrateParam;

u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                          u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2);
int TP_GetUserInfo(TPCalibrateParam *calibrate);

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char TPData_size_must_be_0x8[sizeof(struct TPData) == 0x8 ? 1 : -1];
typedef char TPWork_size_must_be_0x38[sizeof(struct TPWork) == 0x38 ? 1 : -1];
#endif

#ifdef __cplusplus
}
#endif

#endif /* NITRO_SPI_TP_H */
