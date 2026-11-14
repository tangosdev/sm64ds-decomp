//cpp
/**
 * Lethal Lava Land's volcano flame (daObj_volcanoCannon_c).
 *
 * .text 0x021125e0..0x02112950, immediately after daObjFlMaruta_c.
 * CheckPlayerHit burns Mario; ExecuteFly and EnterFly are the one state's
 * behavior and init, bound by the member-pointer constants
 * data_ov022_02114424 and data_ov022_0211442c.
 *
 * _ZTS21daObj_volcanoCannon_c 0x02114440. _ZTI21daObj_volcanoCannon_c
 * 0x02114434. _ZTV21daObj_volcanoCannon_c 0x0211447c.
 * Key function is the out-of-line destructor: D1 at 0x021125e0, D0 at
 * 0x02112610, under defer_codegen off. D2 is not in the ROM.
 */

#include "decl_Actor.h"
#include "Particle__System.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
#include "math/CalcLib.h"

#include "daObj_volcanoCannon_c.h"
#include "daObjFl_Ring_c.h"
#include "daPly_c.h"
#include "LevelRng.h"

/* The state table, ov022 .bss; the definition at the end of this file
 * fills it from the two .data handler constants. One State (init + behavior). */
extern daObj_volcanoCannon_c::State data_ov022_02114690;
extern daObj_volcanoCannon_c::StateFunc data_ov022_0211442c;
extern daObj_volcanoCannon_c::StateFunc data_ov022_02114424;

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

#pragma defer_codegen off

/* One out-of-line definition; D1 and D0 in cartridge order. The body is
 * empty: vptr store, dCcAc_c member teardown and base chain from the
 * class layout; D0's deallocation is the inline operator delete. */
daObj_volcanoCannon_c::~daObj_volcanoCannon_c() {}

/* Burns Mario (actor type 0xbf) if the actor the flame's cylinder last hit
 * is Mario, then retires itself. */
void daObj_volcanoCannon_c::CheckPlayerHit() {
    if (mCylinderClsn.otherOwner == 0)
        return;
    daPly_c *player = static_cast<daPly_c *>(dActor_c::FindWithID(mCylinderClsn.otherOwner));
    if (player == 0)
        return;
    unsigned int isPlayer = ((dActor_c *)player)->actorID == 0xbf;
    if (isPlayer == 0)
        return;
    player->Burn();
    MarkForDestruction();
}

/* The state's behavior handler, bound through data_ov022_02114424.
 * The flame dies once it falls below mKillPosY or mKillTimer runs out. */
int daObj_volcanoCannon_c::ExecuteFly() {
    int y = mPosY;
    int killY = mKillPosY;
    if (y < killY) {
        MarkForDestruction();
        return 1;
    }
    unsigned short timer = mKillTimer;
    if (timer == 0) {
        MarkForDestruction();
        return 1;
    }
    CheckPlayerHit();
    return 1;
}

/* The state's init handler, bound through data_ov022_0211442c.
 * Launches the flame: a 0x3c kill timer, a random heading, and randomised
 * horizontal and vertical speed in 12-bit fixed point, under a -4.0 gravity. */
int daObj_volcanoCannon_c::EnterFly() {
    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650.value) >> 8;
    mKillTimer = 0x3c;
    mPrevAngleY = (short)((r & 0xf) << 0xc);
    mHorzSpeed = (r & 0x1f) + 0x14;
    mHorzSpeed <<= 0xc;
    mVertAccel = -0x4000;
    mVertSpeed = (r & 0x3f) + 0x28;
    mVertSpeed <<= 0xc;
    return 1;
}

/* The state record contains two pointer-to-member callbacks; the
 * transition stores the record and runs its init. */
int daObj_volcanoCannon_c::ChangeState(State *state) {
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}

/* Vtable slot 3. Hands its slot back to the daObjFl_Ring_c that spawned
 * it, by decrementing the ring's live-flame count.
 * mSpawner is null for a flame that was never registered. */
int daObj_volcanoCannon_c::CleanupResources() {
    if (mSpawner)
        ((daObjFl_Ring_c *)mSpawner)->mSpawnCount -= 1;
    return 1;
}

int daObj_volcanoCannon_c::Behavior() {
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    UpdatePos(&mCylinderClsn);
    mCylinderClsn.Clear();
    mCylinderClsn.Update();
    if (!IsOffScreen(this)) {
        mParticleID =
            Particle::System::NewCleanParticle(mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
    }
    return 1;
}

int daObj_volcanoCannon_c::InitResources() {
    mTerminalVelocity = -0xc8000;
    mCylinderClsn.Init(this, 0x1e000, 0x1e000, 0x200002, 0);
    ChangeState(&data_ov022_02114690);
    return 1;
}

daObj_volcanoCannon_c *daObj_volcanoCannon_c_classInit(void) {
    return new daObj_volcanoCannon_c;
}

/* Static-init global (was the handwritten cannon initializer). The state
 * table is plain-copied from the ROM constants in retail order. */
daObj_volcanoCannon_c::State data_ov022_02114690 = {data_ov022_0211442c, data_ov022_02114424};
