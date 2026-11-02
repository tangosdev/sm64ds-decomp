//cpp
#include "types.h"
#include "CP15.h"
#include "IRQ.h"
#include "nitro/card/card_common.h"
extern "C" int PXI_IsCallbackReady(int tag, int proc);
extern "C" void OS_SpinWait(unsigned int cycles);
extern "C" int PXI_SendWordByFifo(unsigned int a, unsigned int c, unsigned int b);

/* NitroSDK CARDi_Request(p, req_type, retry_count): the first time, wait
 * for ARM7's FS PXI callback (tag 0xb) and send the INIT request (0)
 * recursively; then flush the request block and send req_type until the
 * result is not TIMEOUT (4) or the retries run out, sleeping until ARM7
 * clears CARD_STAT_REQ. INIT also sends the request block's address.
 * Returns whether the request succeeded. */
extern "C" int CARDi_Request(CARDiCommon *p, s32 req_type, s32 retry_count) {
    unsigned int saved;
    int r;

    if (!(p->flag & CARD_STAT_INIT_CMD)) {
        p->flag |= CARD_STAT_INIT_CMD;
        if (PXI_IsCallbackReady(0xb, 1) == 0) {
            do {
                OS_SpinWait(0x64);
            } while (PXI_IsCallbackReady(0xb, 1) == 0);
        }
        CARDi_Request(p, CARD_REQ_INIT, 1);
    }

    CP15::FlushAndInvalidateDataCache((unsigned int)p->cmd, sizeof(*p->cmd));
    CP15::DrainWriteBuffer();

    do {
        p->command = req_type;
        p->flag |= CARD_STAT_REQ;

        while (PXI_SendWordByFifo(0xb, req_type, 1) < 0) {
        }

        if (req_type == CARD_REQ_INIT) {
            CARDiCommandArg *q = p->cmd;
            while (PXI_SendWordByFifo(0xb, (unsigned int)q, 1) < 0) {
            }
        }

        saved = IRQ::Disable();
        while (p->flag & CARD_STAT_REQ) {
            OS_SleepThread(0);
        }
        IRQ::Restore(saved);

        r = p->cmd->result;
        if (r != CARD_RESULT_TIMEOUT) {
            break;
        }
        retry_count--;
    } while (retry_count > 0);

    return r == CARD_RESULT_SUCCESS;
}
