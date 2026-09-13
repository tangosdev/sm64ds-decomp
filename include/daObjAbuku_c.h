#ifndef DAOBJABUKU_C_H
#define DAOBJABUKU_C_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* ov002 water bubble. RTTI ov002:0x02108924 names daObjAbuku_c
 * (_ZTS12daObjAbuku_c); overlay_actors.md maps profile BUBBLE (291).
 * Not daObjLava_c / LAVA_BUBBLE (#2513). Direct base is dActor_c
 * (arm9:0x0208e390, offset 0).
 *
 * SIZE 0x114 is the factory's own literal; mParticle at 0x110 closes on it.
 * dActor_c ends at 0x0d0. pad_0d0 is unevidenced (not read in this TU);
 * dCcAc_c sits at 0x0d4. The old "Bubble" alias also names LavaBubble.
 *
 * Vtable vs _ZTV8dActor_c: only slot 0 (InitResources) and slot 6 (Behavior)
 * differ. InitResources is the key function.
 */
struct daObjAbuku_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by the factory C1 and the destructor D1 at +0xd4. */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    /* Same float as WingFeather: mSwayAngle += 0x400 a frame,
       (mSwayAngle >> 4) * 2 + 1 indexes data_02082214, times mDriftSpeed
       is mHorzSpeed. mDriftSpeed eases toward 0x6000. mLifeTimer starts
       at 0x12c and pops at 0; mParticle is the System::New handle. */
    s32 mDriftSpeed;            /* 0x108 */
    s16 mSwayAngle;            /* 0x10c */
    s16 mLifeTimer;            /* 0x10e */
    s32 mParticle;            /* 0x110 */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  Behavior();         /* slot  6 */

    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* Inline and last so instantiation emits retail D1 then D0 and no D2. */
    virtual ~daObjAbuku_c() {}            /* slots 16 (D1), 17 (D0) */
};

typedef char daObjAbuku_c_size_must_be_0x114[sizeof(daObjAbuku_c) == 0x114 ? 1 : -1];

#endif
