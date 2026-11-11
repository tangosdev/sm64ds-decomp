//cpp
/* NitroSDK SPI_tp.c TP_SetCalibrateParam, an older revision of pret's
 * SPI_tp.s (no zero dot-size branch): NULL clears the calibrated flag
 * (+0x30), else store x0/xDotSize and y0/yDotSize with their divider
 * inverses (0x10000000 / dot size) at +0x18..+0x2c and set it. */
#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/os/system.h"
#include "nitro/spi/tp.h"

extern "C" {


void TP_SetCalibrateParam(s16 *arg) {
    OSIntrMode state;

    if (arg == 0) {
        data_020a80cc.mCalibrated = 0;
        return;
    }

    state = OS_DisableInterrupts();

    CP_SetDiv32_32(0x10000000, (u32)arg[2]);
    data_020a80cc.mCalibration[0] = arg[0];
    data_020a80cc.mCalibration[1] = arg[2];
    data_020a80cc.mCalibration[2] = CP_GetDivResult32();
    CP_SetDiv32_32(0x10000000, (u32)arg[3]);
    data_020a80cc.mCalibration[3] = arg[1];
    data_020a80cc.mCalibration[4] = arg[3];
    data_020a80cc.mCalibration[5] = CP_GetDivResult32();

    OS_RestoreInterrupts(state);
    data_020a80cc.mCalibrated = 1;
}
}
