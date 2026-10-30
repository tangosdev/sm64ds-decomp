/* NitroSDK SPI_tp.c TP_GetLatestIndexInAuto: the index auto sampling last
 * wrote in the ring. */
#include "nitro/spi/tp.h"

int TP_GetLatestIndexInAuto(void) {
    return data_020a80cc.index;
}
