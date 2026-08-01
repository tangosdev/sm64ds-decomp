#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);

typedef struct {
    s16 x0;
    s16 y0;
    s16 xDotSize;
    s16 yDotSize;
} TPCalibrateParam;

#define reg_DIVCNT (*(volatile u16 *)0x4000280)
#define reg_DIV_NUMER_L (*(volatile u32 *)0x4000290)
#define reg_DIV_DENOM (*(volatile u64 *)0x4000298)
#define reg_DIV_RESULT_L (*(volatile u32 *)0x40002a0)

static inline void CP_SetDiv32_32(u32 numer, u32 denom)
{
    reg_DIVCNT = 0;
    reg_DIV_NUMER_L = numer;
    reg_DIV_DENOM = denom;
}

static inline void CP_WaitDiv(void)
{
    while (reg_DIVCNT & 0x8000) {
    }
}

static inline s32 CP_GetDivResult32(void)
{
    CP_WaitDiv();
    return (s32)reg_DIV_RESULT_L;
}

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

u32 func_0205eb58(TPCalibrateParam *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                  u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2)
{
    s32 rx_width, dx_width, ry_width, dy_width;
    s32 tmp32;
    u32 enabled;

    if (raw_x1 >= 0x1000 || raw_y1 >= 0x1000 || raw_x2 >= 0x1000 || raw_y2 >= 0x1000)
    {
        return 1;
    }

    if (dx1 >= 256 || dx2 >= 256 || dy1 >= 192 || dy2 >= 192)
    {
        return 1;
    }

    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2)
    {
        return 1;
    }

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = _ZN3IRQ7DisableEv();

    CP_SetDiv32_32(((u32)rx_width) << 8, (u32)dx_width);

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    tmp32 = CP_GetDivResult32();
    CP_SetDiv32_32(((u32)ry_width) << 8, (u32)dy_width);

    if (!IN_S16_RANGE(tmp32))
    {
        _ZN3IRQ7RestoreEj(enabled);
        return 1;
    }
    calibrate->xDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_x1 + raw_x2) << 8) - ((s32)(dx1 + dx2) * calibrate->xDotSize)) >> 7);
    if (!IN_S16_RANGE(tmp32))
    {
        _ZN3IRQ7RestoreEj(enabled);
        return 1;
    }
    calibrate->x0 = (s16)tmp32;

    tmp32 = CP_GetDivResult32();
    _ZN3IRQ7RestoreEj(enabled);

    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->yDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_y1 + raw_y2) << 8) - ((s32)(dy1 + dy2) * calibrate->yDotSize)) >> 7);
    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->y0 = (s16)tmp32;

    return 0;
}
