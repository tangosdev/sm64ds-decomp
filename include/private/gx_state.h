#ifndef PRIVATE_GX_STATE_H
#define PRIVATE_GX_STATE_H

#include "types.h"

/* NitroSDK gxstate.h: the VRAM state block, which banks each user holds
 * (bank bits A 0x001 .. I 0x100). Every bank no user holds stays mapped to
 * the LCDC. GX_InitGXState clears it; the SetBankFor*,
 * Reset and Disable families in src/engine/gx/GX_VramCnt.cpp keep it. The
 * field order is the SDK's, and each offset is one GX_VramCnt.cpp reads or
 * writes at a halfword. */
typedef struct GXVRamCnt {
    u16 lcdc;           /* 0x00 */
    u16 bg;             /* 0x02 */
    u16 obj;            /* 0x04 */
    u16 arm7;           /* 0x06 */
    u16 tex;            /* 0x08 */
    u16 texPltt;        /* 0x0a */
    u16 clrImg;         /* 0x0c */
    u16 bgExtPltt;      /* 0x0e */
    u16 objExtPltt;     /* 0x10 */
    u16 sub_bg;         /* 0x12 */
    u16 sub_obj;        /* 0x14 */
    u16 sub_bgExtPltt;  /* 0x16 */
    u16 sub_objExtPltt; /* 0x18 */
} GXVRamCnt;

typedef char GXVRamCnt_size_must_be_0x1a[sizeof(GXVRamCnt) == 0x1a ? 1 : -1];

extern GXVRamCnt data_020a6088; /* gGXState */

#endif
