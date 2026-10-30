/* NitroSDK SPI_tp.c TP_Init: once only, bring up PXI, clear the work block and
 * install TPi_TpCallback on tag 6. */
#include "types.h"
#include "nitro/spi/tp.h"

extern void PXI_Init(void);
extern int PXI_IsCallbackReady(int tag, int proc);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);
extern void TPi_TpCallback(void);

extern char data_020a80c8[]; /* the once-only init flag */

void TP_Init(void) {
    if (*(u16 *)data_020a80c8 != 0)
        return;
    *(u16 *)data_020a80c8 = 1;
    PXI_Init();
    data_020a80cc.state = 0;
    data_020a80cc.commandFlags = 0;
    data_020a80cc.index = 0;
    data_020a80cc.callback = 0;
    data_020a80cc.samplingBufs = 0;
    data_020a80cc.calibrated = 0;
    data_020a80cc.errFlags = 0;
    while (PXI_IsCallbackReady(6, 1) == 0)
        ;
    PXI_SetFifoRecvCallback(6, (void *)TPi_TpCallback);
}
