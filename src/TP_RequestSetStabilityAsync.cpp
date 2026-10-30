//cpp
/* NitroSDK SPI_tp.c TP_RequestSetStabilityAsync(retry, range): send
 * the set-stability command (3) to the ARM7 over PXI tag 6 and mark its
 * command bit (8); on a send failure set its error bit and report
 * (3, 4, 0) to the user callback. */
#include "types.h"
#include "IRQ.h"
#include "nitro/spi/tp.h"
extern "C" {
int PXI_SendWordByFifo(int a, int b, int c);
}
extern "C" void TP_RequestSetStabilityAsync(int a, int b) {
    unsigned int irq = IRQ::Disable();
    int ok;
    if (PXI_SendWordByFifo(6, a | 0x2000300, 0) < 0)
        ok = 0;
    else if (PXI_SendWordByFifo(6, b | 0x1010000, 0) < 0)
        ok = 0;
    else
        ok = 1;
    if (ok == 0) {
        IRQ::Restore(irq);
        data_020a80cc.errFlags |= 8;
        if (data_020a80cc.callback != 0)
            data_020a80cc.callback(3, 4, 0);
        return;
    }
    data_020a80cc.commandFlags |= 8;
    data_020a80cc.errFlags &= ~8;
    IRQ::Restore(irq);
}
