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

#include "SpawnInfo.h"
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

extern "C" {

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
    int y = mPos.y;
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
    mKillTimer = 60;
    mPrevAngle.y = (short)((r & 0xf) << 0xc);
    mHorzSpeed = (r & 0x1f) + 0x14;
    mHorzSpeed <<= 0xc;
    mVertAccel = -0x4000;
    mSpeed.y = (r & 0x3f) + 0x28;
    mSpeed.y <<= 0xc;
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
            Particle::System::NewCleanParticle(mParticleID, 0x129, mPos.x, mPos.y, mPos.z, 0);
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

extern "C" SpawnInfo<daObj_volcanoCannon_c> g_profile_OBJ_VOLCANO_CANNON = {
    daObj_volcanoCannon_c_classInit,
    243,
    101,
    0x00000002,
    0x00010000,
    0x00060000,
    0x04000000,
    0x00000000};

#pragma explicit_zero_data on
extern "C" u32 data_ov022_021144f8[2] = {0, 0};
#pragma explicit_zero_data off

/* Read-only table that begins where this class's code ends.
 * Interior data_ov022_* labels stay in symbols.txt. */
extern "C" u32 data_ov022_02113168[];
extern "C" const u32 data_ov022_02112950[] = {
    0x00000000, 0x00001000, 0x0000ffff, (u32)data_ov022_02113168,
    0x00000001, 0x00000001, 0x00000001, 0x00000001,
    0x000100c8, 0x00000000, 0x00000000, 0xffffffeb,
    0xffffffd7, 0xffffffc2, 0xffffffae, 0xffffff99,
    0xffffff85, 0xffffff70, 0xffffff5b, 0xffffff47,
    0xffffff32, 0xffffff1e, 0xffffff09, 0xfffffef4,
    0xfffffee0, 0xfffffecb, 0xfffffeb7, 0xfffffea2,
    0xfffffe8e, 0xfffffe79, 0xfffffe64, 0xfffffe50,
    0xfffffe3b, 0xfffffe27, 0xfffffe12, 0xfffffdfd,
    0xfffffde9, 0xfffffdd4, 0xfffffdc0, 0xfffffdab,
    0xfffffd97, 0xfffffd82, 0xfffffd6d, 0xfffffd59,
    0xfffffd44, 0xfffffd30, 0xfffffd1b, 0xfffffd06,
    0xfffffcf2, 0xfffffcdd, 0xfffffcc9, 0xfffffcb4,
    0xfffffca0, 0xfffffc8b, 0xfffffc76, 0xfffffc62,
    0xfffffc4d, 0xfffffc39, 0xfffffc24, 0xfffffc0f,
    0xfffffbfb, 0xfffffbe6, 0xfffffbd2, 0xfffffbbd,
    0xfffffba9, 0xfffffb94, 0xfffffb7f, 0xfffffb6b,
    0xfffffb56, 0xfffffb42, 0xfffffb2d, 0xfffffb18,
    0xfffffb04, 0xfffffaef, 0xfffffadb, 0xfffffac6,
    0xfffffab2, 0xfffffa9d, 0xfffffa88, 0xfffffa74,
    0xfffffa5f, 0xfffffa4b, 0xfffffa36, 0xfffffa21,
    0xfffffa0d, 0xfffff9f8, 0xfffff9e4, 0xfffff9cf,
    0xfffff9bb, 0xfffff9a6, 0xfffff991, 0xfffff97d,
    0xfffff968, 0xfffff954, 0xfffff93f, 0xfffff92a,
    0xfffff916, 0xfffff901, 0xfffff8ed, 0xfffff8d8,
    0xfffff8c4, 0xfffff8af, 0xfffff89a, 0xfffff886,
    0xfffff871, 0xfffff85d, 0xfffff848, 0xfffff833,
    0xfffff81f, 0xfffff80a, 0xfffff7f6, 0xfffff7e1,
    0xfffff7cd, 0xfffff7b8, 0xfffff7a3, 0xfffff78f,
    0xfffff77a, 0xfffff766, 0xfffff751, 0xfffff73c,
    0xfffff728, 0xfffff713, 0xfffff6ff, 0xfffff6ea,
    0xfffff6d6, 0xfffff6c1, 0xfffff6ac, 0xfffff698,
    0xfffff683, 0xfffff66f, 0xfffff65a, 0xfffff645,
    0xfffff631, 0xfffff61c, 0xfffff608, 0xfffff5f3,
    0xfffff5df, 0xfffff5ca, 0xfffff5b5, 0xfffff5a1,
    0xfffff58c, 0xfffff578, 0xfffff563, 0xfffff54e,
    0xfffff53a, 0xfffff525, 0xfffff511, 0xfffff4fc,
    0xfffff4e8, 0xfffff4d3, 0xfffff4be, 0xfffff4aa,
    0xfffff495, 0xfffff481, 0xfffff46c, 0xfffff457,
    0xfffff443, 0xfffff42e, 0xfffff41a, 0xfffff405,
    0xfffff3f1, 0xfffff3dc, 0xfffff3c7, 0xfffff3b3,
    0xfffff39e, 0xfffff38a, 0xfffff375, 0xfffff360,
    0xfffff34c, 0xfffff337, 0xfffff323, 0xfffff30e,
    0xfffff2fa, 0xfffff2e5, 0xfffff2d0, 0xfffff2bc,
    0xfffff2a7, 0xfffff293, 0xfffff27e, 0xfffff269,
    0xfffff255, 0xfffff240, 0xfffff22c, 0xfffff217,
    0xfffff203, 0xfffff1ee, 0xfffff1d9, 0xfffff1c5,
    0xfffff1b0, 0xfffff19c, 0xfffff187, 0xfffff172,
    0xfffff15e, 0xfffff149, 0xfffff135, 0xfffff120,
    0xfffff10c, 0xfffff0f7, 0xfffff0e2, 0xfffff0ce,
    0xfffff0b9, 0xfffff0a5, 0xfffff090, 0xfffff07b,
    0xfffff067, 0xfffff052, 0xfffff03e, 0xfffff029,
    0xfffff015, 0xfffff000,
};
