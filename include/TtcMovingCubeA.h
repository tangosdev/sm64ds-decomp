/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TtcMovingCubeA: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TTCMOVINGCUBEA_H
#define TTCMOVINGCUBEA_H
#include "types.h"
#include "Model.h"

struct TtcMovingCubeA {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x10];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    u8  unk_328;            /* 0x328 */
    u8  pad_329[0x7];
    s32 unk_330;            /* 0x330 */
    u8  mShadowModel;            /* 0x334 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
