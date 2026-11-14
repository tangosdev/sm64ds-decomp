/* NitroSDK CARDi_OnFifoRecv (card_request.c): the PXI callback for the FS
 * tag, which CARDi_InitCommon registers. ARM7's reply ends the request
 * in flight, so clear CARD_STAT_REQ and wake the thread that sent it.
 */
#include "types.h"
#include "nitro/card/card_common.h"
#include "nitro/pxi/fifo.h"

typedef int BOOL;

void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err) {
    if ((tag == PXI_FIFO_TAG_FS) && err) {
        CARDiCommon *const p = &cardi_common;
        p->flag &= ~CARD_STAT_REQ;
        _Z21OS_WakeupThreadDirectP8OSThread(p->cur_th);
    }
}
