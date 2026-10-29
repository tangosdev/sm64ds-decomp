//cpp
#include "types.h"
extern "C" int func_0205ba3c(int bit, int word);
extern "C" void func_02059d8c(int x);
extern "C" int IPCSend(unsigned int a, unsigned int c, unsigned int b);
extern "C" void OS_SleepThread(u16 *x);

namespace CP15 {
    void FlushAndInvalidateDataCache(unsigned int a, unsigned int b);
    void DrainWriteBuffer();
}
namespace IRQ {
    unsigned int Disable();
    void Restore(unsigned int x);
}

/* The request block shared with ARM7 (NitroSDK CARDiCommandArg); the name
 * of its first field follows func_02060484.cpp's view. */
struct ChunkRequest {
    int mResult; /* 0x00 -- 0 (CARD_RESULT_SUCCESS), 4 (TIMEOUT) retried */
};

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches; names as func_02060484.cpp spells them. */
struct CardCommon {
    ChunkRequest *mRequest; /* 0x00 */
    /* 0x04 -- the request type sent to ARM7 (NitroSDK command): stored here
     * before each send; only this function writes it */
    int mCommand;
    char pad_08[0x2c];
    /* 0x34 -- CARD_STAT_* bits: 2 once the INIT request has been sent, 0x20
     * while a request is out (ARM7's reply clears it) */
    int mFlags;
};

/* NitroSDK CARDi_Request(p, req_type, retry_count): the first time, wait
 * for ARM7's FS PXI callback (tag 0xb) and send the INIT request (0)
 * recursively; then flush the request block and send req_type until the
 * result is not TIMEOUT (4) or the retries run out, sleeping until ARM7
 * clears the 0x20 flag. INIT also sends the request block's address.
 * Returns whether the request succeeded. */
#pragma opt_loop_invariants off
extern "C" int func_02060f60(CardCommon *thiz, int v, int count) {
    unsigned int saved;
    int r;

    if (!(*(volatile int *)&thiz->mFlags & 2)) {
        thiz->mFlags |= 2;
        if (func_0205ba3c(0xb, 1) == 0) {
            do {
                func_02059d8c(0x64);
            } while (func_0205ba3c(0xb, 1) == 0);
        }
        func_02060f60(thiz, 0, 1);
    }

    CP15::FlushAndInvalidateDataCache((unsigned int)thiz->mRequest, 0x20);
    CP15::DrainWriteBuffer();

    do {
        thiz->mCommand = v;
        thiz->mFlags |= 0x20;

        while (IPCSend(0xb, v, 1) < 0) {
        }

        if (v == 0) {
            ChunkRequest *q = thiz->mRequest;
            while (IPCSend(0xb, (unsigned int)q, 1) < 0) {
            }
        }

        saved = IRQ::Disable();
        while (thiz->mFlags & 0x20) {
            OS_SleepThread((u16 *)0);
        }
        IRQ::Restore(saved);

        r = thiz->mRequest->mResult;
        if (r != 4) {
            break;
        }
        count--;
    } while (count > 0);

    return r == 0;
}
