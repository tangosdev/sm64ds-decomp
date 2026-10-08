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
 * reconstructed names (RTTI daObjCtMecha10_c, the three registry profiles);
 * retail does not store them. Historical aliases: RotatingClockHand_Spawn
 * (CT_MECHA10) and func_ov035_0211168c (CT_MECHA12L); CT_MECHA12S has none
 * on record.
 *
 * Leftover: measured, and left in the form that matches.
 * Folding both actorID tests into their ifs: InitResources 0x17c -> 0x164
 * (5 reloc destinations wrong) and CleanupResources 0x88 -> 0x7c (1 reloc
 * destination wrong). The widened int idMatch / isMecha12L stay.
 * dBgW_KcMbg::SetFile with a Fix12<int> local: InitResources 0x17c -> 0x184
 * and a 4-byte local .data symbol. The mangled call with scalar 0x1000 stays.
 * IsClsnInRange(int, int) does not compile (no conversion to Fix12<int>).
 * IsClsnInRange with two Fix12<int> zeros: Behavior 0x1f4 -> 0x224. The
 * mangled (this, 0, 0) call stays.
 * Writing mMeshCollider.unk_48 and beforeClsnCallback directly:
 * InitResources 0x17c -> 0x170. func_020396c0 and func_020393d4 stay.
 * One local for both data_ov035_02111ef0 stores: InitResources 0x17c -> 0x170.
 * Both stores stay. unk_326 is written and has no reader in this TU.
 * DecIfAbove0_Short, RandomIntInternal, the data_ov035_* files, tables and
 * CLPS block, data_0209f2c0 and data_0209e650 keep linker names. The three
 * g_profile_* descriptors live outside this TU. func_ov035_0211168c is the
 * historical alias of classInit_CT_MECHA12L, not a function defined here.
 */

#include "daObjCtMecha10_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

bool ApproachLinear(short &value, short target, short step);

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
    u32 words[2];

    Mecha10ModelFilePtr(u32 fileID);
    ~Mecha10ModelFilePtr();
};

struct Mecha10ClsnFilePtr : SharedFilePtr {
    u32 words[2];

    Mecha10ClsnFilePtr(u32 fileID);
    ~Mecha10ClsnFilePtr();
};

extern "C" {
extern Mecha10ModelFilePtr data_ov035_02112c60;   /* model of the fall-through profile (CT_MECHA12S) */
extern Mecha10ClsnFilePtr data_ov035_02112c68;    /* collision KCL of the id 0x77 cog */
extern Mecha10ModelFilePtr data_ov035_02112c70;   /* model of id 0x79 (CT_MECHA12L) */
extern Mecha10ModelFilePtr data_ov035_02112c78;   /* model of id 0x77 (CT_MECHA10) */
extern CLPS_Block    data_ov035_021121d8;
extern s16 data_ov035_02111ef0[];           /* |angle step| by rotation state */
extern s16 data_ov035_02111ef4[][4];        /* dwell by state, by clock setting */
extern u8  data_0209f2c0;                  /* arm9 clock setting */
extern int data_0209e650;                  /* arm9 RNG state */

u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *state);
void func_020393d4(dBgW *collider, void *callback);
void func_020396c0(dBgW *collider, int v);

/* local extern: not declared on dBgActor_c. int arguments do not convert to
   Fix12<int>, and a Fix12<int> pair grows Behavior (see the file banner). */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
/* local extern: dBgW_KcMbg::SetFile takes Fix12<int> by value, and a method
   call homes the scale (see the file banner). The scalar 0x1000 stays. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA10
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA10()
{
    return new daObjCtMecha10_c();
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA12L
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA12L()
{
    return new daObjCtMecha10_c();
}

// @symbol daObjCtMecha10_c_classInit_CT_MECHA12S
extern "C" daObjCtMecha10_c *daObjCtMecha10_c_classInit_CT_MECHA12S()
{
    return new daObjCtMecha10_c();
}

// @symbol _ZN16daObjCtMecha10_c13InitResourcesEv
/* The cog that carries Mario (CT_MECHA10, actor 0x77) is the only one with a
   collision mesh; the other two profiles are scenery and load a model alone.
   Either way the dwell and the step magnitude come out of the same two tables,
   indexed by the state this function just chose. */
int daObjCtMecha10_c::InitResources()
{
    /* Both ID tests are widened into one int and branched on. Folding them
       into the ifs reorders the body (see the file banner). */
    int idMatch;
    idMatch = (actorID == ACTOR_CT_MECHA10);
    if (idMatch) {
        mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c78), 1, -1);
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider,
            (KCL_File *)dBgW_Kc::LoadFile(data_ov035_02112c68),
            &mClsnMat, 0x1000, mAngleY, &data_ov035_021121d8);
        func_020396c0(&mMeshCollider, 0);
        func_020393d4(&mMeshCollider,
            (void *)&dBgW::UpdatePosAndAngs);
        mRotationState = 0;
    } else {
        idMatch = (actorID == ACTOR_CT_MECHA12L);
        if (idMatch)
            mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c70), 1, -1);
        else
            mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c60), 1, -1);
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
int daObjCtMecha10_c::Behavior()
{
    if (data_0209f2c0 == CLOCK_SETTING_STOPPED) {
        UpdateModelPosAndRotY();
        if (mRotationState == 0 &&
            _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
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
    if (mRotationState == 0 &&
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjCtMecha10_c6RenderEv
int daObjCtMecha10_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha10_c16CleanupResourcesEv
/* Give back exactly what InitResources took: the carrying cog frees its
   collider and both of its files, each decorative gear its one model. */
int daObjCtMecha10_c::CleanupResources()
{
    if (mRotationState == 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        data_ov035_02112c78.Release();
        data_ov035_02112c68.Release();
    } else {
        int isMecha12L = (actorID == ACTOR_CT_MECHA12L);
        if (isMecha12L)
            data_ov035_02112c70.Release();
        else
            data_ov035_02112c60.Release();
    }
    return 1;
}

/* Construction order. The three model handles share one destructor literal. */
Mecha10ModelFilePtr data_ov035_02112c78(1486);
Mecha10ClsnFilePtr data_ov035_02112c68(1487);
Mecha10ModelFilePtr data_ov035_02112c60(1491);
Mecha10ModelFilePtr data_ov035_02112c70(1490);
