/* NitroSDK SPI_tp.c TP_GetCalibratedResult: 1 on a sampling error, else the
 * last sample calibrated in place and 0. */
#include "nitro/spi/tp.h"

extern void TP_GetCalibratedPoint(TPData *disp, TPData *raw);

int TP_GetCalibratedResult(TPData *result) {
    if (data_020a80cc.errFlags & 1)
        return 1;
    *result = data_020a80cc.buf;
    TP_GetCalibratedPoint(result, result);
    return 0;
}
