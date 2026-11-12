/* PXI_Init (the Nitro SDK's pxi_init.c): a tail call to PXI_InitFifo
 * (src/engine/pxi/PXI_Fifo.c). */
#include "nitro/pxi/fifo.h"
#include "nitro/pxi/init.h"

void PXI_Init(void) {
    PXI_InitFifo();
}
