#ifndef DAOBJNUMBER_C_H
#define DAOBJNUMBER_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"

/* Floating score popup -- ov002/daObjNumber_c, profile NUMBER (330).
 * dActor_c::SpawnNumber puts one up as actor 0x14a. Direct base dActor_c.
 * Size 0x150 is the factory's operator new literal.
 *
 * The destructor is inline so D1/D0 emit in ROM order with no D2.
 * InitResources is then the key function; this class's TU owns
 * _ZTV/_ZTI/_ZTS. */

struct daObjNumber_c : dActor_c {
    u8  pad_0d0[0x4];
    Model mModel;                    /* 0x0d4 */
    TextureSequence mTextureSequence; /* 0x124 */
    /* Follow target. Behavior looks this uniqueID up with FindWithID;
       0 means the popup is not following anyone. SpawnNumber's owner. */
    s32 mOwnerUniqueID;              /* 0x138 */
    /* Copy of mPos taken in InitResources. mStartPosY is the bounce floor. */
    s32 mStartPosX;                  /* 0x13c */
    s32 mStartPosY;                  /* 0x140 */
    s32 mStartPosZ;                  /* 0x144 */
    s32 mFollowOffsetY;              /* 0x148 */
    /* SpawnNumber's delay: while nonzero Behavior returns immediately and
       Render decrements it, so the popup is not drawn yet. */
    u16 mDelay;                      /* 0x14c */
    /* Bounce. 0: rise, snap to mStartPosY, give mVertSpeed 0xf000, go to 1.
       1: next fall below the floor emits particle 0xd2 and dies. */
    u8  mState;                      /* 0x14e */

    virtual ~daObjNumber_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjNumber_c_size_must_be_0x150[
    sizeof(daObjNumber_c) == 0x150 ? 1 : -1];
#endif

#endif
