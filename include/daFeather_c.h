#ifndef DAFEATHER_C_H
#define DAFEATHER_C_H

/* Wing cap feather (FEATHER 345). Touching it gives the player wings.
 *
 * The class carries the ROM's own RTTI spelling (the tree's former coined
 * name WingFeather is gone). The factory and profile spellings
 * (daFeather_c_classInit, g_profile_FEATHER) are Tier-B reconstructions;
 * exact original spellings are not preserved.
 */
#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* 0x388 bytes per the factory's operator new(904). The four members are
 * constructed in declaration order and torn down in reverse:
 *
 *   dActor_c    0x000..0x0d0  (inherited, not redeclared)
 *   Model        0x0d4
 *   dCcAc_c      0x124
 *   dBgCh_Actr   0x158
 *   ShadowModel  0x314..0x33c
 *   (own fields) 0x33c..0x388
 *
 * The vtable at ov002 0x021088a8 overrides slots 0, 3, 6, 9 and the
 * destructor at 16/17; every other slot is inherited and not redeclared.
 */
struct daFeather_c : dActor_c {
    u8 pad_0d0[0x4];
    Model mModel;                 /* 0x0d4 */
    dCcAc_c mdCcAc_c;              /* 0x124 */
    dBgCh_Actr mWithMeshClsn;      /* 0x158 */
    ShadowModel mShadowModel;      /* 0x314 */
    /* The shadow matrix, rebuilt every frame by func_ov002_020b2c44 out of
       mAngleY and the position and handed to DropShadowRadHeight.
       Only this TU includes this header, and it includes common.h first, so
       the FLAT Matrix4x3 wins here: with math/Matrix.h's nested spelling
       the TU's copies would scalarize (Vector3 is non-POD) and the inline
       destructor below would gain a ~Vector3 call. */
    Matrix4x3 mShadowMtx;            /* 0x33c */
    u8  pad_36c[0xc];                /* 0x36c -- unread */
    /* The float. mSwayAngle advances 0x400 per airborne frame and eases back
       to 0 on the ground; (mSwayAngle >> 4) * 2 indexes the sin/cos table at
       data_02082214, which drives mAngleX/mAngleZ and, scaled by mDriftSpeed,
       mHorzSpeed. mDriftSpeed itself eases toward 0x10000 every frame. */
    s32 mDriftSpeed;            /* 0x378 */
    u16 mSwayAngle;            /* 0x37c */
    u8  pad_37e[0x2];
    /* Particle handle: Behavior passes the previous frame value back into
       Particle::System::New (effect 0x4a) as its first argument and stores the
       result. */
    u32 mParticle;            /* 0x380 */
    /* Seeded 0xb4 (180 frames) in InitResources and counted down only while
       the feather is ON THE GROUND; at 0 it emits particle 0xd2 and marks
       itself for destruction. Render skips drawing on odd values below 0x2d, so
       it blinks through its last 45 frames. */
    u8  mLifeTimer;            /* 0x384 */

    /* INLINE, AND DECLARED FIRST. Out of line, mwcc emits D0 ahead of D1
       and the cartridge has D1 first, plus a homeless D2; inline, this TU
       emits retail D1 then D0 and no D2. The empty body owns the
       ShadowModel, dBgCh_Actr, dCcAc_c and Model teardowns and the chain
       into dActor_c. With the destructor inline, InitResources becomes the
       first out-of-line virtual -- the key function -- so the vtable and
       the RTTI group land in the translation unit that defines it. */
    virtual ~daFeather_c() {}

    virtual s32 InitResources();        /* slot  0 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daFeather_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daFeather_c_size_must_be_0x388[sizeof(daFeather_c) == 0x388 ? 1 : -1];
#endif

#endif
