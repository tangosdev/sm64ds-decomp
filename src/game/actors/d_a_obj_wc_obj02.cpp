//cpp
/* Arrow Lift (ARROW_LIFT 96) -- ov029/daObjWc_Obj02_c.
 *
 * ov029 is WDW. RTTI ov029:0x02113cac names daObjWc_Obj02_c; overlay_actors
 * maps profile 96 to ARROW_LIFT. Not daObjWc_Mizu_c (WDW_WATER) and not
 * daObjWc_Obj03_c (WATER_DIAMOND).
 *
 * common.h FIRST: dBgActor_c.h / dExtSimpleModel_c.h otherwise win with math/Matrix.h's
 * nested Matrix4x3, and InitResources' SetFile matrix argument scalarizes.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. Highest-address first.
 * The destructor is inline in include/daObjWc_Obj02_c.h (D1 then D0, no D2).
 *
 * What stays:
 * - func_020393d4 / func_020393c4: InitResources stores
 *   dBgW::UpdatePosWithTransform and &StoodOnCallback on mMeshCollider;
 *   dBgW.h has no setter.
 * - func_02012694: Behavior plays 0x17b at mCamSpacePosX.
 * - data_ov029_02114250 / 02114248 are this TU's BMD/KCL file handles;
 *   data_ov029_0211302c the CLPS block stays unowned overlay data.
 * - S14: g_profile_WC_OBJ02 stays outside the licensed .text.
 * - common.h first (InitResources SetFile matrix argument).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj02_AssetId { ASSET_WC_OBJ02_BMD = 0x06ce, ASSET_WC_OBJ02_KCL = 0x06cf };

#include "common.h"
#include "daObjWc_Obj02_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj02ModelFilePtr : SharedFilePtr {
    WcObj02ModelFilePtr(u32 fileID);
    ~WcObj02ModelFilePtr();
};

struct WcObj02CollisionFilePtr : SharedFilePtr {
    WcObj02CollisionFilePtr(u32 fileID);
    ~WcObj02CollisionFilePtr();
};

/* local extern: no shared header declares these. */
extern "C" {
void func_02012694(int a, void *p);
void func_020393d4(dBgW *bgw, void *fn);
void func_020393c4(dBgW *bgw, void *fn);
}

extern WcObj02ModelFilePtr data_ov029_02114250;
extern WcObj02CollisionFilePtr data_ov029_02114248;
extern CLPS_Block data_ov029_0211302c;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj02_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: ArrowLift_Spawn. Attribution stays on
 * daObjWc_Obj02_c_classInit. */
extern "C" daObjWc_Obj02_c *daObjWc_Obj02_c_classInit() {
    return new daObjWc_Obj02_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* The mesh collider's stood-on callback gets three arguments; OnStoodOn
 * wants the last two. long_calls emits the pooled `ldr ip,[pc,#8]; bx ip`
 * absolute tail call. */
#pragma long_calls on
// @symbol _ZN15daObjWc_Obj02_c15StoodOnCallbackEPvPS_P8dActor_c
void daObjWc_Obj02_c::StoodOnCallback(void *collider, daObjWc_Obj02_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Arms mTriggered when the actor standing on the lift is the player (0xbf).
 * The unsigned char temporary is load-bearing. */
// @symbol _ZN15daObjWc_Obj02_c9OnStoodOnEP8dActor_c
void daObjWc_Obj02_c::OnStoodOn(dActor_c *other) {
    unsigned char ok = other->actorID == 0xbf;
    if (ok)
        mTriggered = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c13InitResourcesEv
int daObjWc_Obj02_c::InitResources() {
    {
        BMD_File *bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov029_02114250);
        mModel.SetFile(bmd, 1, -1);
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov029_02114248);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY, data_ov029_0211302c);
    }
    {
        mPrevAngleY += 0x4000;
        func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    }
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);
    mTravelDist = 0;
    mStateTimer = 0;
    mTriggered = 0;
    mState = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c8BehaviorEv
int daObjWc_Obj02_c::Behavior() {
    switch (mState) {
    case 0:
        if (mTriggered != 0) {
            func_02012694(0x17b, &mCamSpacePosX);
            mState++;
        }
        mHorzSpeed = 0xa000;
        break;
    case 1:
    case 3:
        UpdatePos(0);
        mTravelDist += mHorzSpeed;
        if (mTravelDist >= 0x177000) {
            mTravelDist = 0;
            mState++;
            mStateTimer = 0;
        }
        break;
    case 2:
    case 4:
        if (mStateTimer >= 0x3c) {
            mPrevAngleY += 0x8000;
            if (mState == 4) {
                mState = 0;
            } else {
                mState++;
                func_02012694(0x17b, &mCamSpacePosX);
            }
            mStateTimer = 0;
        } else {
            mStateTimer++;
        }
        break;
    }

    UpdateModelPosAndRotY();
    if (IsClsnInRangeOnScreen(0x100000, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    mTriggered = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c6RenderEv
int daObjWc_Obj02_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj02_c16CleanupResourcesEv
int daObjWc_Obj02_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov029_02114250.Release();
    data_ov029_02114248.Release();
    return 1;
}
// @symbol _ZN15daObjWc_Obj02_cD1Ev
// @symbol _ZN15daObjWc_Obj02_cD0Ev
/* daObjWc_Obj02_c's inline class-body destructor is instantiated by the
 * definitions above. mwccarm emits D1 and D0 into this object. */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj02.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj02ModelFilePtr data_ov029_02114250(ASSET_WC_OBJ02_BMD);
WcObj02CollisionFilePtr data_ov029_02114248(ASSET_WC_OBJ02_KCL);
