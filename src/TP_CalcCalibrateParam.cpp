//cpp
/* NitroSDK SPI_tp.c TP_CalcCalibrateParam: derive the calibration from
 * two raw/screen point pairs through the CP divider. 1 when a point is out
 * of range, when the two points share a raw or screen coordinate, or when a
 * result does not fit. */
#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/os/system.h"
#include "nitro/spi/tp.h"

extern "C" {

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                          u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2) {
    s32 rx_width, dx_width, ry_width, dy_width;
    s32 tmp32;
    OSIntrMode enabled;

    if (raw_x1 >= 0x1000 || raw_y1 >= 0x1000 || raw_x2 >= 0x1000 || raw_y2 >= 0x1000) {
        return 1;
    }

    if (dx1 >= 256 || dx2 >= 256 || dy1 >= 192 || dy2 >= 192) {
        return 1;
    }

    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2) {
        return 1;
    }

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = OS_DisableInterrupts();

    CP_SetDiv32_32(((u32)rx_width) << 8, (u32)dx_width);

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    tmp32 = CP_GetDivResult32();
    CP_SetDiv32_32(((u32)ry_width) << 8, (u32)dy_width);

    if (!IN_S16_RANGE(tmp32)) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->xDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_x1 + raw_x2) << 8) - ((s32)(dx1 + dx2) * calibrate->xDotSize)) >> 7);
    if (!IN_S16_RANGE(tmp32)) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->x0 = (s16)tmp32;

    tmp32 = CP_GetDivResult32();
    OS_RestoreInterrupts(enabled);

    if (!IN_S16_RANGE(tmp32)) {
        return 1;
    }
    calibrate->yDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_y1 + raw_y2) << 8) - ((s32)(dy1 + dy2) * calibrate->yDotSize)) >> 7);
    if (!IN_S16_RANGE(tmp32)) {
        return 1;
    }
    calibrate->y0 = (s16)tmp32;

    return 0;
}
}
