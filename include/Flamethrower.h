#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H
#include "types.h"

#ifdef __cplusplus

#include "dActor_c.h"
#include "dCcPos_c.h"
#include "math/Matrix.h"

/* The flame jet actor. The destructor supplies the otherwise-missing layout
 * evidence: the ROM destroys Vector3[12] at 0x3a4, then dCcPos_c[12] at 0xd4,
 * before entering dActor_c's base destructor. The factory's 0x46c allocation
 *
 * SM64DS proves this class as daObjFlamethrower_c through RTTI, allocation
 * size and vtable identity. The factory and profile spellings below are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols.
 *
 * daObjFlamethrower_c_classInit at 0x02136ed4 (historical alias
 * Flamethrower_Spawn) installs this class's cartridge vtable. It backs the
 * OBJ_FLAMETHROWER registry profile, whose descriptor at 0x021376cc is
 * reconstructed as g_profile_OBJ_FLAMETHROWER.
 * literal closes the object after the state fields below. */
struct Flamethrower : dActor_c {
    u8 pad_0d0[0x4];
    dCcPos_c mColliders[12];       /* 0x0d4 */
    Vector3 mFlamePositions[12];   /* 0x3a4 */
    Matrix4x3 mTransform;          /* 0x434 */
    u8 mState;                     /* 0x464 */
    u8 mActiveFlames;              /* 0x465 */
    u16 mTimer;                    /* 0x466 */
    u32 mSoundHandle;              /* 0x468 */

    virtual ~Flamethrower();
    virtual int InitResources();
    virtual int Behavior();
};

#else

struct Flamethrower {
    u8  pad_000[0x5c];
    /* 0x05c..0x08c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mPosX;                 /* 0x05c */
    s32 mPosY;                   /* 0x060 */
    s32 mPosZ;                   /* 0x064 */
    s32 mPrevPosX;                 /* 0x068 */
    s32 mPrevPosY;                 /* 0x06c */
    s32 mPrevPosZ;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    /* TODO: Behavior reads past this generated span:
       dCc_c mClsn[12] at 0x0d4 (stride 0x3c),
       Vector3 mPartPos[12] at 0x3a4 (stride 0xc),
       Matrix4x3 at 0x434, then u8 state 0x464 / u8 active 0x465 /
       u16 timer 0x466 / u32 sound handle 0x468. */
    /* trailing extent the ROM's `new Flamethrower` literal proves; see tools/opnew_sizes.py */
    u8 pad_094[0x3d8];
};

#endif /* __cplusplus */

typedef char Flamethrower_size_must_be_0x46c[sizeof(struct Flamethrower) == 0x46c ? 1 : -1];

#endif
