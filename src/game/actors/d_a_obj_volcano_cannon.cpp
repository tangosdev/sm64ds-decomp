//cpp
/**
 * Lethal Lava Land's volcano flame (daObj_volcanoCannon_c).
 *
 * .text 0x021125e0..0x02112950, immediately after daObjFlMaruta_c.
 * func_ov022_02112654 burns Mario; 0x021126ac and 0x02112710 are the
 * state's behavior and init, bound by the member-pointer constants
 * data_ov022_02114424 and data_ov022_0211442c.
 *
 * _ZTS21daObj_volcanoCannon_c 0x02114440. _ZTI21daObj_volcanoCannon_c
 * 0x02114434. _ZTV21daObj_volcanoCannon_c 0x0211447c.
 * Key function is the out-of-line destructor: D1 at 0x021125e0, D0 at
 * 0x02112610, under defer_codegen off. D2 is not in the ROM.
 */

#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

#include "daObj_volcanoCannon_c.h"
#include "daObjFl_Ring_c.h"
#include "daPly_c.h"

struct Vector3_16f;

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
}
namespace Particle {
struct System {
    static unsigned int NewUnkCallback818(unsigned int a, unsigned int b, Fix12i c, Fix12i d,
                                          Fix12i e, const Vector3_16f *f);
};
}
extern "C" {
int RandomIntInternal(int *seed);
extern int data_0209e650;
void func_ov022_02112654(daObj_volcanoCannon_c *c);

/* The state table, ov022 .bss; the definition at the end of this file
 * fills it from the two .data handler constants. One State (init + behavior). */
extern daObj_volcanoCannon_c::State data_ov022_02114690;
extern daObj_volcanoCannon_c::StateFunc data_ov022_0211442c;
extern daObj_volcanoCannon_c::StateFunc data_ov022_02114424;
}

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

#pragma defer_codegen off

// @symbol _ZN21daObj_volcanoCannon_cD1Ev
// @symbol _ZN21daObj_volcanoCannon_cD0Ev
/* One out-of-line definition; D1 and D0 in cartridge order. The body is
 * empty: vptr store, dCcAc_c member teardown and base chain from the
 * class layout; D0's deallocation is the inline operator delete. */
daObj_volcanoCannon_c::~daObj_volcanoCannon_c() {}

// @symbol func_ov022_02112654
/* Burns Mario (actor type 0xbf) if the actor the flame's cylinder last hit
 * is Mario, then retires itself. */
extern "C" void func_ov022_02112654(daObj_volcanoCannon_c *c) {
    if (c->mCylinderClsn.otherOwner == 0)
        return;
    daPly_c *a = static_cast<daPly_c *>(dActor_c::FindWithID(c->mCylinderClsn.otherOwner));
    if (a == 0)
        return;
    unsigned int b = ((dActor_c *)a)->actorID == 0xbf;
    if (b == 0)
        return;
    a->Burn();
    c->MarkForDestruction();
}

// @symbol func_ov022_021126ac
/* The state's behavior handler, bound through data_ov022_02114424.
 * The flame dies once it falls below mKillPosY or mKillTimer runs out. */
extern "C" int func_ov022_021126ac(daObj_volcanoCannon_c *c) {
    int a = c->mPosY;
    int b = c->mKillPosY;
    if (a < b) {
        c->MarkForDestruction();
        return 1;
    }
    unsigned short h = c->mKillTimer;
    if (h == 0) {
        c->MarkForDestruction();
        return 1;
    }
    func_ov022_02112654(c);
    return 1;
}

// @symbol func_ov022_02112710
/* The state's init handler, bound through data_ov022_0211442c.
 * Launches the flame: a 0x3c kill timer, a random heading, and randomised
 * horizontal and vertical speed in 12-bit fixed point, under a -4.0 gravity. */
extern "C" int func_ov022_02112710(daObj_volcanoCannon_c *c) {
    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
    c->mKillTimer = 0x3c;
    c->mPrevAngleY = (short)((r & 0xf) << 0xc);
    c->mHorzSpeed = (r & 0x1f) + 0x14;
    c->mHorzSpeed <<= 0xc;
    c->mVertAccel = -0x4000;
    c->mVertSpeed = (r & 0x3f) + 0x28;
    c->mVertSpeed <<= 0xc;
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE
/* The state record contains two pointer-to-member callbacks; the
 * transition stores the record and runs its init. */
int daObj_volcanoCannon_c::ChangeState(State *state) {
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}

// @symbol _ZN21daObj_volcanoCannon_c16CleanupResourcesEv
/* Vtable slot 3. Hands its slot back to the daObjFl_Ring_c that spawned
 * it, by decrementing the ring's live-flame count.
 * mSpawner is null for a flame that was never registered. */
int daObj_volcanoCannon_c::CleanupResources() {
    if (mSpawner)
        ((daObjFl_Ring_c *)mSpawner)->mSpawnCount -= 1;
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c8BehaviorEv
int daObj_volcanoCannon_c::Behavior() {
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    UpdatePos(&mCylinderClsn);
    mCylinderClsn.Clear();
    mCylinderClsn.Update();
    if (!IsOffScreen(this)) {
        mParticleID =
            Particle::System::NewUnkCallback818(mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
    }
    return 1;
}

// @symbol _ZN21daObj_volcanoCannon_c13InitResourcesEv
int daObj_volcanoCannon_c::InitResources() {
    mTerminalVelocity = -0xc8000;
    mCylinderClsn.Init(this, 0x1e000, 0x1e000, 0x200002, 0);
    ChangeState((State *)&data_ov022_02114690);
    return 1;
}

// @symbol daObj_volcanoCannon_c_classInit
extern "C" daObj_volcanoCannon_c *daObj_volcanoCannon_c_classInit(void) {
    return new daObj_volcanoCannon_c;
}

/* Static-init global (was the handwritten cannon initializer). The state
 * table is plain-copied from the ROM constants in retail order. */
daObj_volcanoCannon_c::State data_ov022_02114690 = {data_ov022_0211442c, data_ov022_02114424};
