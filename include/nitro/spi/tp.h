#ifndef NITRO_SPI_TP_H
#define NITRO_SPI_TP_H

/*
 * The touch panel, under the NitroSDK's own names. The bodies are SPI_tp.c,
 * still one function per file: TP_Init, TP_RequestSamplingAsync,
 * TP_RequestAutoSamplingStartAsync, TP_RequestSetStabilityAsync,
 * TP_GetCalibratedPoint, TP_GetCalibratedResult, TP_GetLatestIndexInAuto,
 * TP_CheckError, TP_WaitBusy and TP_SetCalibrateParam.
 *
 * Field names are coined from how those functions use them; the cartridge
 * carries none. Three of them -- errFlags, commandFlags and index -- are the
 * names the per-file views already used.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One panel sample. TP_GetCalibratedPoint scales the first two words and
   copies the other two through unchanged, so the first pair are the axes and
   the second the touch and validity flags the SDK carries with them. */
typedef struct TPData {
    u16 x;        /* 0x0 */
    u16 y;        /* 0x2 */
    u16 touch;    /* 0x4 */
    u16 validity; /* 0x6 */
} TPData;

/* SPI_tp.c's state, data_020a80cc.
 *
 * SIZE 0x38, fixed two ways: the next symbol row, data_020a8104, is exactly
 * 0x38 past the start, and the last members are the volatile halfword pair
 * TP_CheckError and TP_WaitBusy poll at +0x34/+0x36.
 *
 * Offsets from the functions that use them. TP_Init clears callback,
 * samplingBufs, index, calibrated, state and errFlags. The PXI receive
 * callback fills buf, advances index through samplingBufs up to bufSize, sets
 * state, and sets the per-command bits in errFlags and commandFlags.
 * TP_RequestAutoSamplingStartAsync records samplingBufs, index, frequency and
 * bufSize. TP_SetCalibrateParam fills calibration and sets calibrated, or
 * clears calibrated when given no parameters. TP_GetCalibratedPoint reads
 * calibration and calibrated; TP_GetLatestIndexInAuto reads index;
 * TP_GetCalibratedResult reads buf and errFlags.
 *
 * samplingBufs is a TPData array: TP_RequestAutoSamplingStartAsync clears each
 * entry's third word with a byte stride of 8, and the receive callback indexes
 * it by index with that same stride. */
typedef struct TPWork {
    void (*callback)(int, int, int); /* 0x00 */
    TPData buf;                      /* 0x04 -- the last sample */
    u16 index;                       /* 0x0c -- into samplingBufs */
    u16 frequency;                   /* 0x0e */
    TPData *samplingBufs;            /* 0x10 */
    u16 bufSize;                     /* 0x14 -- entries in samplingBufs */
    u8 pad_16[2];                    /* 0x16 */
    int calibration[6];              /* 0x18 */
    u16 calibrated;                  /* 0x30 -- zero until calibrated */
    volatile u16 state;              /* 0x32 */
    volatile u16 errFlags;           /* 0x34 */
    volatile u16 commandFlags;       /* 0x36 */
} TPWork;

extern TPWork data_020a80cc;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char TPData_size_must_be_0x8[sizeof(struct TPData) == 0x8 ? 1 : -1];
typedef char TPWork_size_must_be_0x38[sizeof(struct TPWork) == 0x38 ? 1 : -1];
#endif

#ifdef __cplusplus
}
#endif

#endif /* NITRO_SPI_TP_H */
