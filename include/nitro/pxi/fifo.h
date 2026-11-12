#ifndef NITRO_PXI_FIFO_H
#define NITRO_PXI_FIFO_H

/*
 * The inter-processor FIFO, under the Nitro SDK's own names
 * (nitro/pxi/common/fifo.h and regname.h). The bodies are pxi_fifo.c, in
 * src/engine/pxi/PXI_Fifo.c.
 *
 * A FIFO word packs a PXIFifoTag, an error/reply bit and 26 bits of data
 * (PXIFifoMessage). Each library registers one PXIFifoCallback for its tag
 * and polls PXI_IsCallbackReady until the ARM7 side has registered too.
 *
 * This SDK's card library still talks on PXI_FIFO_TAG_FS (11); the later
 * SDK gives it PXI_FIFO_TAG_CARD.
 *
 * The SDK header's inline readiness helpers are left out, the parameters
 * are unnamed and every typedef carries a tag: an inline body, a named
 * parameter or an anonymous type in a shared header costs every including
 * C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PXIProc { PXI_PROC_ARM9 = 0, PXI_PROC_ARM7 = 1 } PXIProc;

typedef enum PXIFifoTag {
    PXI_FIFO_TAG_EX = 0,
    PXI_FIFO_TAG_USER_0,
    PXI_FIFO_TAG_USER_1,
    PXI_FIFO_TAG_SYSTEM,
    PXI_FIFO_TAG_NVRAM,
    PXI_FIFO_TAG_RTC,
    PXI_FIFO_TAG_TOUCHPANEL,
    PXI_FIFO_TAG_SOUND,
    PXI_FIFO_TAG_PM,
    PXI_FIFO_TAG_MIC,
    PXI_FIFO_TAG_WM,
    PXI_FIFO_TAG_FS,
    PXI_FIFO_TAG_OS,
    PXI_FIFO_TAG_CTRDG,
    PXI_MAX_FIFO_TAG = 32
} PXIFifoTag;

typedef enum PXIFifoStatus {
    PXI_FIFO_SUCCESS = 0,
    PXI_FIFO_FAIL_SEND_ERR = -1,
    PXI_FIFO_FAIL_SEND_FULL = -2,
    PXI_FIFO_FAIL_RECV_ERR = -3,
    PXI_FIFO_FAIL_RECV_EMPTY = -4,
    PXI_FIFO_NO_CALLBACK_ENTRY = -5
} PXIFifoStatus;

#define PXI_FIFOMESSAGE_BITSZ_TAG 5
#define PXI_FIFOMESSAGE_BITSZ_ERR 1
#define PXI_FIFOMESSAGE_BITSZ_DATA 26

typedef union PXIFifoMessage {
    struct {
        u32 tag : PXI_FIFOMESSAGE_BITSZ_TAG;
        u32 err : PXI_FIFOMESSAGE_BITSZ_ERR;
        u32 data : PXI_FIFOMESSAGE_BITSZ_DATA;
    } e;
    u32 raw;
} PXIFifoMessage;

/* (tag, data, err): err is the word's error/reply bit. */
typedef void (*PXIFifoCallback)(PXIFifoTag, u32, int);

void PXI_InitFifo(void);
void PXI_SetFifoRecvCallback(int, PXIFifoCallback);
int PXI_IsCallbackReady(int, PXIProc);
int PXI_SendWordByFifo(int, u32, int);
void PXIi_HandlerRecvFifoNotEmpty(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_PXI_FIFO_H */
