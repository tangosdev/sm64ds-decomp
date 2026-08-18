/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Spiny: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SPINY_H
#define SPINY_H
#include "types.h"
#include "Model.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct Spiny {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    /* 0x08c..0x092 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_08c;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    /* 0x092..0x0b0 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_092;                 /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    /* 0x0b0..0x0d4 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 unk_0b0;                 /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x4];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SpinyD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    /* ModelAnim member, named by the class's own destructor calling
       ModelAnim's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SpinyD0Ev.c] */
    ModelAnim mModelAnim;            /* 0x124 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x188 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SpinyD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x188 */
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x1b0 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SpinyD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x1b0 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x1e4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5SpinyD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x1e4 */
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x37];
    s32 unk_3d8;            /* 0x3d8 */
    u8  pad_3dc[0xd];
    u8  unk_3e9;            /* 0x3e9 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
