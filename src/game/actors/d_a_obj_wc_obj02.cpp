//cpp
/* Arrow Lift (ARROW_LIFT 96) -- ov029/daObjWc_Obj02_c.
 *
 * ov029 is WDW. RTTI ov029:0x02113cac names daObjWc_Obj02_c; overlay_actors
 * maps profile 96 to ARROW_LIFT. Not daObjWc_Mizu_c (WDW_WATER) and not
 * daObjWc_Obj03_c (WATER_DIAMOND).
 *
 * common.h FIRST: dBgActor_c.h / Model.h otherwise win with math/Matrix.h's
 * nested Matrix4x3, and InitResources' SetFile matrix argument scalarizes.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. Highest-address first.
 * The destructor is inline in include/daObjWc_Obj02_c.h (D1 then D0, no D2).
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources passes Fix12<int> by value; the
 *   header method form size-DIFFs.
 * - dBgActor_c::IsClsnInRangeOnScreen 6az: Behavior; header method form
 *   refused (include/dBgActor_c.h).
 * - func_020393d4 / func_020393c4: InitResources stores
 *   dBgW::UpdatePosWithTransform and &func_ov029_021116e4 on mMeshCollider;
 *   dBgW.h has no setter.
 * - func_02012694: Behavior plays 0x17b at mCamSpacePosX.
 * - InitResources keeps (unsigned long long)((int)this + 0x94) for
 *   mPrevAngleY += 0x4000: `&mPrevAngleY` size-DIFFs.
 * - data_ov029_02114250 / 02114248 / 0211302c BMD/KCL/CLPS handles; this TU
 *   claims .text only.
 * - func_ov029_021116c4 / 021116e4 stay placeholder labels (BeforeClsn
 *   veneer + player-id trigger). decl_common.h's `extern int` DATA view of
 *   021116e4 forces the namespaced definition.
 * - S14: g_profile_WC_OBJ02 stays outside the licensed .text.
 * - common.h first (InitResources SetFile matrix argument).
 */

#include "common.h"
#include "daObjWc_Obj02_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"

extern "C" {
void func_02012694(int a, void *p);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, Matrix4x3 *mat, int scale,
    short angY, CLPS_Block &clps);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
extern SharedFilePtr data_ov029_02114250;
extern SharedFilePtr data_ov029_02114248;
extern CLPS_Block data_ov029_0211302c;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
/* (RESOLVED): CONFLICT -- alternate declaration of
 * _ZN10dBgActor_c21UpdateModelPosAndRotYEv / 19UpdateClsnPosAndRotEv from
 * Behavior vs InitResources; both are now the real dBgActor_c methods. */
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj02_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: ArrowLift_Spawn. Attribution stays on
 * daObjWc_Obj02_c_classInit. */
extern "C" daObjWc_Obj02_c *daObjWc_Obj02_c_classInit()
{
    return new daObjWc_Obj02_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two;
// #pragma long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call.
// (create dropped the legacy pragma; restored and bracketed. The int-returning
// view of the callee is this veneer's own, namespaced -- the definition below is void.)
namespace tail_6c4 { extern "C" int func_ov029_021116c4(void *a, void *b); }
#pragma long_calls on
/* decl_common.h declares this name as `extern int` DATA (InitResources takes its
 * address through that view); the function definition lives in a namespace so both
 * spellings of the one C symbol can coexist. */
namespace fndef_e4 {
extern "C" int func_ov029_021116e4(void *a, void *b, void *c)
{
    return tail_6c4::func_ov029_021116c4(b, c);
}
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* BeforeClsn helper: if the other actor's actorID (dActor_c+0x0c) is PLAYER
 * (0xbf), arm mTriggered. The unsigned char temporary is load-bearing. */
extern "C" void func_ov029_021116c4(char *a, char *b)
{
    unsigned char ok = *(unsigned short *)(b + 0xc) == 0xbf;
    if (ok)
        ((daObjWc_Obj02_c *)a)->mTriggered = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c13InitResourcesEv
int daObjWc_Obj02_c::InitResources()
{
    {
        BMD_File *bmd = (BMD_File *)Model::LoadFile(data_ov029_02114250);
        mModel.SetFile(bmd, 1, -1);
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov029_02114248);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov029_0211302c);
    }
    {
        /* MATCH: `&mPrevAngleY` size-DIFFs; (long long)(int) is the cartridge form. */
        short *p = (short *)((unsigned long long)((int)this + 0x94));
        short val = *p;
        int *arg0_1 = (int *)&mMeshCollider;
        int arg1_1 = (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
        *p = (short)(val + 0x4000);
        func_020393d4(arg0_1, arg1_1);
    }
    {
        int *arg0_2 = (int *)&mMeshCollider;
        int arg1_2 = (int)&func_ov029_021116e4;
        func_020393c4(arg0_2, arg1_2);
    }
    mTravelDist = 0;
    mStateTimer = 0;
    mTriggered = 0;
    mState = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c8BehaviorEv
int daObjWc_Obj02_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mTriggered != 0) {
            func_02012694(0x17b, &mCamSpacePosX);
            {
                u8 *p = &mState;
                *p = *p + 1;
            }
        }
        mHorzSpeed = 0xa000;
        break;
    case 1:
    case 3:
        UpdatePos(0);
        {
            s32 *p = &mTravelDist;
            *p = *p + mHorzSpeed;
        }
        if (mTravelDist >= 0x177000) {
            mTravelDist = 0;
            {
                u8 *p = &mState;
                *p = *p + 1;
            }
            mStateTimer = 0;
        }
        break;
    case 2:
    case 4:
        if (mStateTimer >= 0x3c) {
            {
                s16 *p = &mPrevAngleY;
                *p = *p + 0x8000;
            }
            if (mState == 4) {
                mState = 0;
            } else {
                {
                    u8 *p = &mState;
                    *p = *p + 1;
                }
                func_02012694(0x17b, &mCamSpacePosX);
            }
            mStateTimer = 0;
        } else {
            u16 *p = &mStateTimer;
            *p = *p + 1;
        }
        break;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x100000, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    mTriggered = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c6RenderEv
int daObjWc_Obj02_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c16CleanupResourcesEv
int daObjWc_Obj02_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov029_02114250.Release();
    data_ov029_02114248.Release();
    return 1;
}

/* daObjWc_Obj02_c's inline class-body destructor is instantiated by the
 * definitions above. mwccarm emits D1 and D0 into this object. */
