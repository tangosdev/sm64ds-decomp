#ifndef DAOBJNUMBER_C_H
#define DAOBJNUMBER_C_H

#include "types.h"
#include "dActor_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmTexPat_c.h"

/* Floating score popup -- ov002/daObjNumber_c, profile NUMBER (330).
 * dActor_c::SpawnNumber puts one up as actor 0x14a. Direct base dActor_c.
 * Size 0x150 is the factory's operator new literal.
 *
 * The destructor is inline so D1/D0 emit in ROM order with no D2.
 * InitResources is then the key function; this class's TU owns
 * _ZTV/_ZTI/_ZTS. */

struct daObjNumber_c : dActor_c {
    dExtSimpleModel_c mModel;         /* 0x0d4 */
    dExtAnmTexPat_c mTextureSequence; /* 0x124 */
    /* Follow target. Behavior looks this uniqueID up with FindWithID;
       0 means the popup is not following anyone. SpawnNumber's owner. */
    s32 mOwnerUniqueID;              /* 0x138 */
    /* Copy of mPos taken in InitResources. mStartPos.y is the bounce floor. */
    Vector3 mStartPos;               /* 0x13c */
    s32 mFollowOffsetY;              /* 0x148 */
    /* SpawnNumber's delay: while nonzero Behavior returns immediately and
       Render decrements it, so the popup is not drawn yet. */
    u16 mDelay;                      /* 0x14c */
    /* Bounce. 0: rise, snap to mStartPos.y, give mSpeed.y 0xf000, go to 1.
       1: next fall below the floor emits particle 0xd2 and dies. */
    u8  mState;                      /* 0x14e */

    virtual ~daObjNumber_c() {}

    /* SpawnNumber hands the popup its owner: record the owner's uniqueID
       and the vertical offset to follow it at. Returns this. */
    daObjNumber_c *SetOwner(dActor_c *owner);

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

typedef char daObjNumber_c_size_must_be_0x150[
    sizeof(daObjNumber_c) == 0x150 ? 1 : -1];

#endif
