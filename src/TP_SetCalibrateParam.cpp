//cpp
/* NitroSDK SPI_tp.c TP_SetCalibrateParam, an older revision of pret's
 * SPI_tp.s (no zero dot-size branch): NULL clears the calibrated flag
 * (+0x30), else store x0/xDotSize and y0/yDotSize with their divider
 * inverses (0x10000000 / dot size) at +0x18..+0x2c and set it. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/spi/tp.h"

extern "C" {


void TP_SetCalibrateParam(s16 *arg) {
    OSIntrMode state;
    unsigned long long vv;

    if (arg == 0) {
        data_020a80cc.mCalibrated = 0;
        return;
    }

    state = OS_DisableInterrupts();

    vv = (u32)arg[2];
    *(volatile u16 *)0x4000280 = 0;
    *(volatile u32 *)0x4000290 = 0x10000000;
    *(volatile unsigned long long *)0x4000298 = vv;
    data_020a80cc.mCalibration[0] = arg[0];
    data_020a80cc.mCalibration[1] = arg[2];
    while (*(volatile u16 *)0x4000280 & 0x8000) {
    }

    data_020a80cc.mCalibration[2] = *(volatile u32 *)0x40002a0;
    vv = (u32)arg[3];
    *(volatile u16 *)0x4000280 = 0;
    *(volatile u32 *)0x4000290 = 0x10000000;
    *(volatile unsigned long long *)0x4000298 = vv;
    data_020a80cc.mCalibration[3] = arg[1];
    data_020a80cc.mCalibration[4] = arg[3];
    while (*(volatile u16 *)0x4000280 & 0x8000) {
    }
    data_020a80cc.mCalibration[5] = *(volatile u32 *)0x40002a0;

    OS_RestoreInterrupts(state);
    data_020a80cc.mCalibrated = 1;
}
}
