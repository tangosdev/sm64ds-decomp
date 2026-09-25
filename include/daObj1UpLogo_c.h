#ifndef DAOBJ1UPLOGO_C_H
#define DAOBJ1UPLOGO_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"

/* 1-Up logo popup -- ov002/daObj1UpLogo_c, profile OBJ_1UPLOGO (331 / 0x14b).
 * Player::IncMegaKillCount spawns one as actor 0x14b. Direct base dActor_c.
 * Size 0x150 is the factory's operator new. Same rise-and-pop as the score
 * popup (daObjNumber_c) next to it in this overlay.
 *
 * The destructor is out of line. One definition emits retail D1 then D0;
 * D2 is the base-object copy and is not in the ROM.
 *
 * Real inheritance: the ROM's own RTTI name; _ZTI14daObj1UpLogo_c at ov002
 * 0x0210b168 names _ZTI8dActor_c as its base. The vtable at 0x0210b1ac
 * overrides slots 0/3/6/9 (InitResources/CleanupResources/Behavior/Render)
 * plus the destructor pair at 16/17 (D1 0x020f0dd0, D0 0x020f0e08). */

struct daObj1UpLogo_c : dActor_c {
    u8 pad_0d0[0x4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 in the cartridge's own
       ~daObj1UpLogo_c (D1/D0), a relocation the ROM build checks. */
    Model mModel;                     /* 0x0d4 */
    /* TextureSequence member, named by _ZN15TextureSequenceD1Ev at +0x124 in
       the same destructor, called before the Model's (reverse order). */
    TextureSequence mTextureSequence; /* 0x124 */
    /* Follow target. Behavior looks this uniqueID up with FindWithID.
       0 means the logo is not following anyone. IncMegaKillCount passes
       a null owner, so a mega-kill logo stays where it spawned. */
    s32 mOwnerUniqueID;               /* 0x138 */
    /* Copy of mPos taken in InitResources. Y is the bounce floor. */
    Vector3 mStartPos;                /* 0x13c */
    s32 mFollowOffsetY;               /* 0x148 */
    /* While nonzero, Behavior returns immediately and Render decrements
       it, so the logo is not drawn yet. */
    u16 mDelay;                       /* 0x14c */
    /* 0: rise, snap to mStartPos.y, give mVertSpeed 0xf000, go to 1.
       1: next fall below the floor emits particle 0xd2 and dies. */
    u8 mState;                        /* 0x14e */
    u8 pad_14f;

    virtual ~daObj1UpLogo_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObj1UpLogo_c_size_must_be_0x150[
    sizeof(daObj1UpLogo_c) == 0x150 ? 1 : -1];
#endif

#endif
