/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BulletBill: 5 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BULLETBILL_H
#define BULLETBILL_H
#include "types.h"
#include "Model.h"
#include "WithMeshClsn.h"
#include "ShadowModel.h"

struct BulletBill {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* 0x074..0x08c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    /* The X of the camera-space triple, not a scalar: Behavior takes
       `(Vector3*)(c + 0x74)` twice -- once as the sound's position and once as
       the argument to func_02012694 -- so all twelve bytes are read at once,
       and 0x078/0x07c were already named as its y and z. */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x68];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x1f];
    s32 unk_130;            /* 0x130 */
    u32 unk_134;            /* 0x134 */
    u8  pad_138[0x18];
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10BulletBillD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x150 */
    /* Model member, named by _ZN5ModelD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel1;            /* 0x30c */
    /* Model member, named by _ZN5ModelD1Ev at +0x35c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x35c */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x3ac -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10BulletBillD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x3ac */
    s32 mState;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
