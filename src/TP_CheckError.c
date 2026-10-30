/* NitroSDK SPI_tp.c TP_CheckError: the error bits of mask in the TP state's
 * error flags. */
#include "nitro/spi/tp.h"

int TP_CheckError(int mask) {
    return data_020a80cc.errFlags & mask;
}
