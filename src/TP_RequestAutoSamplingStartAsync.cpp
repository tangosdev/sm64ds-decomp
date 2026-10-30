//cpp
/* NitroSDK SPI_tp.c TP_RequestAutoSamplingStartAsync(vcount, frequency,
 * samplingBufs, bufSize): record the ring, clear each sample's touch, then
 * send the auto-sampling command 1 (flag 2) over PXI tag 6. */
#include "types.h"
#include "IRQ.h"
#include "nitro/spi/tp.h"
extern "C" {
int PXI_SendWordByFifo(unsigned int a, unsigned int c, unsigned int b);
}

#pragma opt_strength_reduction off
extern "C" void TP_RequestAutoSamplingStartAsync(unsigned int vcount, unsigned int frequency,
                                                 int samplingBufs, unsigned int bufSize) {
    unsigned int i;
    unsigned int irq;
    int ok;

    data_020a80cc.samplingBufs = (TPData *)samplingBufs;
    data_020a80cc.index = 0;
    data_020a80cc.frequency = (u16)frequency;
    data_020a80cc.bufSize = (u16)bufSize;
    for (i = 0; i < bufSize; i++) {
        data_020a80cc.samplingBufs[i].touch = 0;
    }
    irq = IRQ::Disable();
    if (PXI_SendWordByFifo(6, (frequency & 0xff) | 0x2000100, 0) < 0)
        ok = 0;
    else if (PXI_SendWordByFifo(6, vcount | 0x1010000, 0) < 0)
        ok = 0;
    else
        ok = 1;
    if ((ok & 0xff) == 0) {
        IRQ::Restore(irq);
        data_020a80cc.errFlags |= 2;
        if (data_020a80cc.callback != 0)
            data_020a80cc.callback(1, 4, 0);
        return;
    }
    data_020a80cc.commandFlags |= 2;
    data_020a80cc.errFlags &= ~2;
    IRQ::Restore(irq);
}
