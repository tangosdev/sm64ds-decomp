/* NitroSDK SPI_tp.c TP_WaitBusy: spin until none of mask's commands are still
 * outstanding. */
#include "nitro/spi/tp.h"

void TP_WaitBusy(int mask) {
    while (data_020a80cc.commandFlags & mask)
        ;
}
