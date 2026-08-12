/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PowerFlower: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef POWERFLOWER_H
#define POWERFLOWER_H
#include "types.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct PowerFlower {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    /* 0x08e..0x09c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_08e;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel1;            /* 0x0d4 */
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11PowerFlowerD0Ev.c] */
    Model mModel2;            /* 0x124 */
    u8  mShadowModel;            /* 0x174 */
    u8  pad_175[0x57];
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x1cc -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11PowerFlowerD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x1cc */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x200 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11PowerFlowerD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x200 */
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    u8  pad_3c4[0x6];
    u8  unk_3ca;            /* 0x3ca */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
