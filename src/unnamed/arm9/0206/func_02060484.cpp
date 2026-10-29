//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern char data_020a8180[];
extern int data_020a6134[];
void OS_SleepThread(void* p);
void func_02060228(void* fn);
int func_0206062c(void* g);

/* The request block a chunk is handed over in (NitroSDK CARDiCommandArg);
 * names as func_0206062c.cpp spells them. */
typedef struct ChunkRequest {
    int mResult; /* 0x00 -- 0 (CARD_RESULT_SUCCESS) when the request went through */
} ChunkRequest;

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches. mSrc / mDst / mRemaining and mRequest follow
 * func_0206062c.cpp, the write task started here. */
typedef struct CardCommon {
    ChunkRequest *mRequest; /* 0x00 */
    unsigned char pad_04[0x14];
    int mSrc;                /* 0x18 -- RAM source of the write */
    int mDst;                /* 0x1c -- backup address */
    unsigned int mRemaining; /* 0x20 -- bytes left */
    unsigned char pad_24[4];
    /* 0x28 / 0x2c -- the completion callback and its argument (func_0206081c
     * calls it when a request ends; func_02060890 = CARD_Init zeroes both) */
    int mCallback;
    int mCallbackArg;
    unsigned char pad_30[4];
    /* 0x34 -- CARD_STAT_* bits: 4 (busy) is held for one request at a time
     * (func_020603c8, func_02060558 take it the same way) */
    u32 mFlags;
    unsigned char pad_38[0x98];
    /* 0xd0 -- the thread the request runs on: the caller's for a synchronous
     * request, the card task thread (func_02060228) otherwise */
    int mCurThread;
    /* 0xd4 -- callers wait here while another request holds the busy bit */
    char mBusyQueue[4];
} CardCommon;

/* Write len bytes from RAM src to backup address dst: wait for the
 * card to be free, take it, store the request, and run the write task
 * (func_0206062c) here or, when is_async, on the card thread. Synchronous
 * calls return whether the request succeeded. */
int func_02060484(int dst, int src, int len, int callback, int arg, int is_async) {
    CardCommon *g = (CardCommon *)data_020a8180;
    unsigned int irq = IRQ::Disable();
    if (g->mFlags & 4) {
        do {
            OS_SleepThread(g->mBusyQueue);
        } while (g->mFlags & 4);
    }
    g->mFlags |= 4;
    IRQ::Restore(irq);
    g->mSrc = src;
    g->mDst = dst;
    g->mRemaining = len;
    g->mCallback = callback;
    g->mCallbackArg = arg;
    if (is_async != 0) {
        func_02060228((void *)func_0206062c);
        return 1;
    }
    /* through the symbol, not g: the ROM reloads its address (via g, 4 bytes shorter) */
    ((CardCommon *)data_020a8180)->mCurThread = data_020a6134[2];
    func_0206062c(g);
    return g->mRequest->mResult == 0;
}
}
