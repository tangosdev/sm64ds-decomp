//cpp
#include "CP15.h"

extern "C" {

#pragma opt_loop_invariants off
extern int func_02060f60(void *thiz, int a, int b);
extern void CpuCopy8(void *a, int b, unsigned int c);
extern void func_0206081c(void *thiz);

/* The request block a chunk is handed over in (NitroSDK CARDiCommandArg);
 * names as func_0206062c.cpp spells them. */
typedef struct ChunkRequest {
    unsigned char pad_00[0xc];
    int mSrc;          /* 0x0c */
    int mDst;          /* 0x10 */
    unsigned int mLen; /* 0x14 */
} ChunkRequest;

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches; names as func_0206062c.cpp and
 * func_02060484.cpp spell them. */
typedef struct CardCommon {
    ChunkRequest *mRequest; /* 0x00 */
    unsigned char pad_04[0x14];
    int mSrc;                /* 0x18 -- backup address */
    int mDst;                /* 0x1c -- RAM destination */
    unsigned int mRemaining; /* 0x20 */
    unsigned char pad_24[0xbc];
    char mBuffer[0x100]; /* 0xe0 -- one chunk, cache-invalidated before each request */
} CardCommon;

/* The card thread's backup-read task, the reverse of func_0206062c: request
 * up to 0x100 bytes from backup address mSrc into mBuffer (request 6), copy
 * them out to mDst, and advance until mRemaining is 0 or a request fails;
 * then end the task (func_0206081c). */
void func_0206071c(char *thiz) {
    CardCommon *p = (CardCommon *)thiz;
    unsigned int n;
    char *e0;

    e0 = p->mBuffer;

    do {
        n = p->mRemaining;
        if (n > 0x100)
            n = 0x100;
        CP15::InvalidateDataCache((unsigned int)p->mBuffer, 0x100);
        p->mRequest->mSrc = p->mSrc;
        p->mRequest->mDst = (int)p->mBuffer;
        p->mRequest->mLen = n;
        if (func_02060f60(thiz, 6, 1) == 0)
            break;
        CpuCopy8(e0, p->mDst, n);
        p->mSrc += n;
        p->mDst += n;
        p->mRemaining -= n;
    } while (p->mRemaining != 0);

    func_0206081c(thiz);
}
}
