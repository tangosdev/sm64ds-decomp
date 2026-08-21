/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterDiamond: 5 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERDIAMOND_H
#define WATERDIAMOND_H
#include "types.h"
#include "Model.h"
#include "dCcAc_c.h"

struct WaterDiamond {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x54];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12WaterDiamondD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x124 */
    s32 mWaterID;            /* 0x158 */
    s8  mWaterParam;            /* 0x15c */
    u8  mActive;            /* 0x15d */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
