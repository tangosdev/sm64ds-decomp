//cpp
/* NitroSDK SPI_tp.c TP_RequestSamplingAsync: send the one-shot sampling
 * command 0 (flag 1) over PXI tag 6; on a send failure set its error bit
 * and report (0, 4, 0) to the user callback. */
#include "IRQ.h"
#include "nitro/spi/tp.h"

extern "C" {

int PXI_SendWordByFifo(int cmd, int a, int b);

void TP_RequestSamplingAsync(void) {
    unsigned int saved = IRQ::Disable();
    int ok = (PXI_SendWordByFifo(6, 0x3000000, 0) >= 0);
    if (!ok) {
        void (*fp)(int, int, int);
        IRQ::Restore(saved);
        fp = data_020a80cc.callback;
        data_020a80cc.errFlags |= 1;
        if (fp == 0)
            return;
        fp(0, 4, 0);
        return;
    }
    data_020a80cc.commandFlags |= 1;
    data_020a80cc.errFlags &= ~1;
    IRQ::Restore(saved);
}
}
