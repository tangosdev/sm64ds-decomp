//cpp
/*
 * daObjCtMecha10_c: the rotating cogs of Tick Tock Clock, one class behind
 * three profiles -- CT_MECHA10 (actor 0x77), CT_MECHA12L (0x79) and
 * CT_MECHA12S (0x7a), ids from symbols/actor_debug_names.tsv.
 *
 * InitResources reads the actor ID the registry spawned it under. 0x77 is the
 * cog that carries Mario: it loads a collision mesh as well as its model,
 * hands the collider dBgW's position/angle updater and takes mRotationState 0.
 * 0x79 and any other ID (CT_MECHA12S in practice) are decorative gears that
 * load a model only and take state 1.
 *
 * Behavior turns the cog toward mTargetAngleY at a fixed 0xc8 per frame. When
 * it arrives and mStepTimer runs out, the target advances by mAngleYStep and
 * the timer is re-seeded from data_ov035_02111ef4[state][clock setting]. Under
 * clock setting 2 the dwell is re-rolled instead, and every time mDirTimer
 * expires the step direction is re-picked at random (forward two times in
 * three, otherwise reversed). Setting 3 stops the
 * cog: the body only keeps the model and collider where the actor is and
 * returns.
 *
 * ROM: this TU owns text only. ov035 delinks no .data here, so the _ZTV /
 * _ZTI / _ZTS group the class names is compiler-only output, compared against
 * the cartridge's own copies at ov035 0x02112b00 / 0x02112a84 / 0x02112a90.
 * Source order is reverse ROM order: mwccarm 2004/b56 emits .text back to
 * front under this tree's flags, so the three classInit factories are
 * written first, then InitResources, and CleanupResources last. The
 * destructor pair comes off the in-class `~daObjCtMecha10_c() {}` in
 * include/daObjCtMecha10_c.h and lands ahead of everything, D1 then D0,
 * which is the order the cartridge has (0x021111a0, then 0x021111e4).
 *
 * daObjCtMecha10_c_classInit_CT_MECHA10/_CT_MECHA12L/_CT_MECHA12S are
 * reconstructed factory names (RTTI daObjCtMecha10_c, three registry
 * profiles); historical aliases: RotatingClockHand_Spawn (CT_MECHA10),
 * func_ov035_0211168c (CT_MECHA12L).
 */

// Notes (each measured): the widened int idMatch / isMecha12L stay (folding
// the actorID tests into the ifs reorders the body). func_020396c0 and
// func_020393d4 are the out-of-line stores into the collider's
// edgeToleranceShift and beforeClsnCallback; d_bg_w.cpp defines them and no
// header declares them. Both data_ov035_02111ef0 stores stay (one local
// shrinks InitResources). unk_326 is written with no reader in this TU.
// DecIfAbove0_Short, RandomIntInternal, the data_ov035_* files/tables/CLPS
// block, data_0209f2c0, data_0209e650 keep linker names. The three
// g_profile_* descriptors live outside this TU. func_ov035_0211168c is the
// historical alias of classInit_CT_MECHA12L, not a function defined here.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha10_c_AssetId {
    ASSET_CT_MECHA_OBJ10_BMD = 0x05ce,
    ASSET_CT_MECHA_OBJ10_KCL = 0x05cf,
    ASSET_CT_MECHA_OBJ12L_BMD = 0x05d2,
    ASSET_CT_MECHA_OBJ12S_BMD = 0x05d3
};

#include "daObjCtMecha10_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/Approach.h"

/* Actor ids are the registry profile ids. Clock settings are the byte at
 * data_0209f2c0: 2 re-rolls the dwell, 3 holds the cog still. */
enum {
    ACTOR_CT_MECHA10 = 0x77,
    ACTOR_CT_MECHA12L = 0x79,
    CLOCK_SETTING_RANDOM = 2,
    CLOCK_SETTING_STOPPED = 3
};

/* Resource handles constructed by this TU's static initializer. The
 * constructor and destructor bodies stay out of line; mwcc registers them. */
struct Mecha10ModelFilePtr : SharedFilePtr {
    Mecha10ModelFilePtr(u32 fileID);
    ~Mecha10ModelFilePtr();
};

struct Mecha10ClsnFilePtr : SharedFilePtr {
    Mecha10ClsnFilePtr(u32 fileID);
    ~Mecha10ClsnFilePtr();
};

extern Mecha10ModelFilePtr
    gCtMechaObj12sModelFile; /* model of the fall-through profile (CT_MECHA12S) */
extern Mecha10ClsnFilePtr gCtMechaObj10CollisionFile; /* collision KCL of the id 0x77 cog */
extern Mecha10ModelFilePtr gCtMechaObj12lModelFile;   /* model of id 0x79 (CT_MECHA12L) */
extern Mecha10ModelFilePtr gCtMechaObj10ModelFile;    /* model of id 0x77 (CT_MECHA10) */
extern CLPS_Block data_ov035_021121d8;
extern s16 data_ov035_02111ef0[];    /* |angle step| by rotation state */
extern s16 data_ov035_02111ef4[][4]; /* dwell by state, by clock setting */
extern u8 data_0209f2c0;             /* arm9 clock setting */
extern int data_0209e650;            /* arm9 RNG state */

extern "C" {
u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *state);
void func_020393d4(dBgW *collider, void *callback);
void func_020396c0(dBgW *collider, int v);
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA10
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA10() {
    return new daObjCtMecha10_c();
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA12L
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA12L() {
    return new daObjCtMecha10_c();
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA12S
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA12S() {
    return new daObjCtMecha10_c();
}

// @symbol _ZN16daObjCtMecha10_c13InitResourcesEv
/* The cog that carries Mario (CT_MECHA10, actor 0x77) is the only one with a
   collision mesh; the other two profiles are scenery and load a model alone.
   Either way the dwell and the step magnitude come out of the same two tables,
   indexed by the state this function just chose. */
int daObjCtMecha10_c::InitResources() {
    /* Both ID tests are widened into one int and branched on. Folding them
       into the ifs reorders the body (see the file banner). */
    int idMatch;
    idMatch = (actorID == ACTOR_CT_MECHA10);
    if (idMatch) {
        mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gCtMechaObj10ModelFile), 1, -1);
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        ((dBgW_KcMbg *)(&mMeshCollider))
            ->dBgW_KcMbg::SetFile((KCL_File *)dBgW_Kc::LoadFile(gCtMechaObj10CollisionFile),
                                  mClsnMat, 0x1000, mAngleY, data_ov035_021121d8);
        func_020396c0(&mMeshCollider, 0);
        func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
        mRotationState = 0;
    } else {
        idMatch = (actorID == ACTOR_CT_MECHA12L);
        if (idMatch)
            mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gCtMechaObj12lModelFile), 1, -1);
        else
            mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gCtMechaObj12sModelFile), 1, -1);
        UpdateModelPosAndRotY();
        mRotationState = 1;
    }

    mStepTimer = data_ov035_02111ef4[mRotationState][data_0209f2c0];
    mAngleYStep = data_ov035_02111ef0[mRotationState];
    unk_326 = data_ov035_02111ef0[mRotationState];
    return 1;
}

// @symbol _ZN16daObjCtMecha10_c8BehaviorEv
/* Clock setting 3 stops the cog: hold position and leave. Otherwise turn
   toward the target, and once there and the dwell has run out, advance the
   target one step and re-seed the dwell. Setting 2 also re-rolls the dwell
   at random and, whenever mDirTimer expires, picks the step's direction at
   random: forward two times in three for 0x5a..0x10e frames, otherwise
   reversed for 0x1e frames. */
int daObjCtMecha10_c::Behavior() {
    if (data_0209f2c0 == CLOCK_SETTING_STOPPED) {
        UpdateModelPosAndRotY();
        if (mRotationState == 0 && IsClsnInRange(0, 0))
            UpdateClsnPosAndRot();
        return 1;
    }

    if (ApproachLinear(mAngleY, mTargetAngleY, 0xc8) &&
        DecIfAbove0_Short((u16 *)&mStepTimer) == 0) {
        mTargetAngleY += mAngleYStep;

        u8 setting = data_0209f2c0;
        mStepTimer = data_ov035_02111ef4[mRotationState][setting];
        if (setting == CLOCK_SETTING_RANDOM) {
            int rnd = RandomIntInternal(&data_0209e650);
            if (DecIfAbove0_Short((u16 *)&mDirTimer) == 0) {
                if ((unsigned int)rnd % 3 != 0) {
                    mAngleYStep = data_ov035_02111ef0[mRotationState];
                    mDirTimer = (rnd & 3) * 0x3c + 0x5a;
                } else {
                    mAngleYStep = -data_ov035_02111ef0[mRotationState];
                    mDirTimer = (((unsigned int)rnd % 3) + 1) * 0x1e;
                }
            }
            mStepTimer = ((unsigned int)rnd % 3) * 0x14 + 0xa;
        }
    }

    UpdateModelPosAndRotY();
    if (mRotationState == 0 && IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjCtMecha10_c6RenderEv
int daObjCtMecha10_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha10_c16CleanupResourcesEv
/* Give back exactly what InitResources took: the carrying cog frees its
   collider and both of its files, each decorative gear its one model. */
int daObjCtMecha10_c::CleanupResources() {
    if (mRotationState == 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        gCtMechaObj10ModelFile.Release();
        gCtMechaObj10CollisionFile.Release();
    } else {
        int isMecha12L = (actorID == ACTOR_CT_MECHA12L);
        if (isMecha12L)
            gCtMechaObj12lModelFile.Release();
        else
            gCtMechaObj12sModelFile.Release();
    }
    return 1;
}

/* Construction order. The three model handles share one destructor literal. */
Mecha10ModelFilePtr gCtMechaObj10ModelFile(ASSET_CT_MECHA_OBJ10_BMD);
Mecha10ClsnFilePtr gCtMechaObj10CollisionFile(ASSET_CT_MECHA_OBJ10_KCL);
Mecha10ModelFilePtr gCtMechaObj12sModelFile(ASSET_CT_MECHA_OBJ12S_BMD);
Mecha10ModelFilePtr gCtMechaObj12lModelFile(ASSET_CT_MECHA_OBJ12L_BMD);
