/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SnowmanBody: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNOWMANBODY_H
#define SNOWMANBODY_H
#include "types.h"
#include "Model.h"

struct SnowmanBody {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    /* 0x080..0x08c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x42];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mShadowModel;            /* 0x124 */
    u8  pad_125[0x27];
    u8  mMovingCylinderClsn;            /* 0x14c */
    u8  pad_14d[0x33];
    u8  mWithMeshClsn;            /* 0x180 */
    u8  pad_181[0x1bb];
    s32 unk_33c;            /* 0x33c */
    s32 unk_340;            /* 0x340 */
    s32 unk_344;            /* 0x344 */
    s16 unk_348;            /* 0x348 */
    s16 unk_34a;            /* 0x34a */
    s16 unk_34c;            /* 0x34c */
    u8  pad_34e[0x2];
    u8  unk_350;            /* 0x350 */
    u8  pad_351[0x3f];
    s32 unk_390;            /* 0x390 */
    u8  pad_394[0x10];
    u8  unk_3a4;            /* 0x3a4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
