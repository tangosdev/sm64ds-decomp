#ifndef BULLETBILL_H
#define BULLETBILL_H

#include "types.h"

/* Five boundaries close on sizes other headers assert:
 *
 *     Enemy                     ends 0x110
 *     MovingCylinderClsnWithPos 0x110 + 0x040 = 0x150  -> WithMeshClsn
 *     WithMeshClsn              0x150 + 0x1bc = 0x30c  -> the first Model
 *     Model                     0x30c + 0x050 = 0x35c  -> the second
 *     Model                     0x35c + 0x050 = 0x3ac  -> the shadow
 *     (0x28 of shadow)          0x3ac + 0x028 = 0x3d4  -> mState
 *
 * The ShadowModel at 0x3ac is destroyed like the rest, and 0x3ac + 0x28 closes
 * exactly on mState.
 *
 * unk_130 and unk_134 are gone: both fall inside the MovingCylinderClsnWithPos
 * at 0x110 (+0x20 and +0x24).
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "Model.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsnWithPos.h"

struct BulletBill : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;  /* 0x110 */
    WithMeshClsn mWithMeshClsn;                            /* 0x150 */
    Model mModel1;                                         /* 0x30c */
    Model mModel2;                                         /* 0x35c */
    ShadowModel mShadowModel;                              /* 0x3ac */
    s32 mState;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */

    virtual ~BulletBill();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#else

/* The same object for a C translation unit, flat. */
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
    u8  mWithMeshClsn[0x1bc];            /* 0x150 */
    /* Model member, named by _ZN5ModelD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    u8  mModel1[0x50];            /* 0x30c */
    /* Model member, named by _ZN5ModelD1Ev at +0x35c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    u8  mModel2[0x50];            /* 0x35c */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x3ac -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10BulletBillD1Ev.c] */
    u8  mShadowModel[0x28];            /* 0x3ac */
    s32 mState;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
};

#endif /* __cplusplus */

#endif /* BULLETBILL_H */
