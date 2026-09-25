//cpp
/**
 * Tick Tock Clock's rotating cog (registry profiles CT_MECHA10, CT_MECHA12L
 * and CT_MECHA12S).
 *
 * One class behind three profiles. InitResources reads the actor ID the
 * registry spawned it under: 0x77 is the cog that carries Mario, so it loads
 * the collision mesh as well as the model, hands the collider dBgW's
 * position/angle updater and takes mRotationState 0; 0x79 and anything else
 * are the two decorative gears, which load a model only and take state 1.
 *
 * Behavior turns the cog toward mTargetAngleY at a fixed 0xc8 per frame. When
 * it arrives and mStepTimer runs out, the target advances by mAngleYStep and
 * the timer is re-seeded from data_ov035_02111ef4[state][clock setting]. Under
 * clock setting 2 -- the random hand -- the dwell is re-rolled instead, and
 * every time mDirTimer expires the step flips sign for a randomly chosen span.
 * Setting 3 stops the clock: the body only keeps the model and collider where
 * the actor is and returns.
 *
 * This TU owns text only: ov035 delinks no .data here, so the _ZTV / _ZTI /
 * _ZTS group the class names is compiler-only output, compared against the
 * cartridge's own copies at ov035 0x02112b00 / 0x02112a84 / 0x02112a90.
 *
 * SOURCE ORDER IS REVERSE ROM ORDER. mwccarm 2004/b56 emits .text back to
 * front under this tree's flags, so InitResources is written first and
 * CleanupResources last; the destructor pair comes off the in-class
 * `~daObjCtMecha10_c() {}` in include/daObjCtMecha10_c.h and lands ahead of
 * everything, D1 then D0, which is the order the cartridge has (0x021111a0,
 * then 0x021111e4).
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay spelled as
 *   mangled extern-C free functions. Each takes Fix12<int> by value, and a
 *   real method call homes the argument and size-DIFFs the caller
 *   (notes/mwccarm-codegen.md 6az); include/dBgW_KcMbg.h records the same.
 * Leftover: _Z14ApproachLinearRsss, DecIfAbove0_Short and RandomIntInternal
 *   keep linker names -- no header home, the kaitendai precedent.
 * Leftover: func_020393d4 and func_020396c0 are small stores into dBgW (the
 *   collision callback and the range flag). This TU calls them; naming
 *   belongs with dBgW in arm9.
 * Leftover: data_ov035_02112c60 / c68 / c70 / c78 are this overlay's four
 *   shared-file handles, data_ov035_021121d8 its CLPS block, and
 *   data_ov035_02111ef0 / 02111ef4 the two rotation tables. None of them is
 *   owned by this TU.
 * Leftover: data_0209f2c0 is arm9's clock-setting byte and data_0209e650 the
 *   shared RNG state.
 * Leftover: g_profile_CT_MECHA10 / CT_MECHA12L / CT_MECHA12S and their three
 *   classInit factories live outside this TU, at 0x0211165c and above.
 */

#include "daObjCtMecha10_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
extern SharedFilePtr data_ov035_02112c60;   /* CT_MECHA10 model */
extern SharedFilePtr data_ov035_02112c68;   /* CT_MECHA12S collision KCL */
extern SharedFilePtr data_ov035_02112c70;   /* CT_MECHA12L model */
extern SharedFilePtr data_ov035_02112c78;   /* CT_MECHA12S model */
extern CLPS_Block    data_ov035_021121d8;
extern s16 data_ov035_02111ef0[];           /* |angle step| by rotation state */
extern s16 data_ov035_02111ef4[][4];        /* dwell by state, by clock setting */
extern u8  data_0209f2c0[];                 /* arm9 clock setting */
extern int data_0209e650[];                 /* arm9 RNG state */

int _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *state);
void func_020393d4(dBgW *collider, void *callback);
void func_020396c0(dBgW *collider, int v);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha10_c13InitResourcesEv
/* The cog that carries Mario (CT_MECHA12S, actor 0x77) is the only one with a
   collision mesh; the other two profiles are scenery and load a model alone.
   Either way the dwell and the step magnitude come out of the same two tables,
   indexed by the state this function just chose. */
int daObjCtMecha10_c::InitResources()
{
    /* MEASURED: both ID tests are widened into an int and branched on, rather
       than written `if (actorID == 0x77)`. mwccarm keeps the comparison result
       in a register across the arm and re-tests it; folding either test into
       its `if` re-orders the whole body and DIFFs. The reuse of one local for
       both tests is what the second branch's codegen wants. */
    int isCarrier;

    isCarrier = (actorID == 0x77);
    if (isCarrier) {
        mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c78), 1, -1);
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider,
            (KCL_File *)dBgW_Kc::LoadFile(data_ov035_02112c68),
            &mClsnMat, 0x1000, mAngleY, &data_ov035_021121d8);
        func_020396c0(&mMeshCollider, 0);
        func_020393d4(&mMeshCollider,
            (void *)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        mRotationState = 0;
    } else {
        isCarrier = (actorID == 0x79);
        if (isCarrier)
            mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c70), 1, -1);
        else
            mModel.SetFile((BMD_File *)Model::LoadFile(data_ov035_02112c60), 1, -1);
        UpdateModelPosAndRotY();
        mRotationState = 1;
    }

    mStepTimer = data_ov035_02111ef4[mRotationState][data_0209f2c0[0]];
    mAngleYStep = data_ov035_02111ef0[mRotationState];
    unk_326 = data_ov035_02111ef0[mRotationState];
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha10_c8BehaviorEv
/* Clock setting 3 is the stopped hand: hold position and leave. Otherwise turn
   toward the target, and once there and the dwell has run out, advance the
   target one step and re-seed the dwell. Setting 2 is the random hand, which
   also re-rolls the dwell every frame and flips the step's sign whenever
   mDirTimer expires. */
int daObjCtMecha10_c::Behavior()
{
    if (data_0209f2c0[0] == 3) {
        UpdateModelPosAndRotY();
        if (mRotationState == 0 &&
            _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
            UpdateClsnPosAndRot();
        return 1;
    }

    if (_Z14ApproachLinearRsss(&mAngleY, mTargetAngleY, 0xc8) != 0 &&
        DecIfAbove0_Short((u16 *)&mStepTimer) == 0) {
        mTargetAngleY += mAngleYStep;

        u8 setting = data_0209f2c0[0];
        mStepTimer = data_ov035_02111ef4[mRotationState][setting];
        if (setting == 2) {
            int rnd = RandomIntInternal(data_0209e650);
            if (DecIfAbove0_Short((u16 *)&mDirTimer) == 0) {
                if ((unsigned int)rnd % 3 != 0) {
                    mAngleYStep = data_ov035_02111ef0[mRotationState];
                    mDirTimer = (rnd & 3) * 0x3c + 0x5a;
                } else {
                    mAngleYStep = -data_ov035_02111ef0[mRotationState];
                    mDirTimer = ((unsigned int)rnd % 3 + 1) * 0x1e;
                }
            }
            mStepTimer = (unsigned int)rnd % 3 * 0x14 + 0xa;
        }
    }

    UpdateModelPosAndRotY();
    if (mRotationState == 0 &&
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha10_c6RenderEv
int daObjCtMecha10_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
        /* MEASURED: widened, like InitResources' pair above. */
        int isLarge = (actorID == 0x79);
        if (isLarge)
            data_ov035_02112c70.Release();
        else
            data_ov035_02112c60.Release();
    }
    return 1;
}
