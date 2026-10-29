//cpp
/* DONKAKU / Grindel, the sliding crusher leaf of daDsnBase_c (sibling
 * daDsn_c / Thwomp). Overlay actor 162. daDgr_c in this overlay is DONGURU.
 *
 * ~daDkk_c() is inline in daDkk_c.h, which emits D1 0x021118c8 then D0
 * 0x02111928. InitResources is the key function. Definitions run from the
 * highest address back, which is the order this compiler emits.
 */
// Kept shapes (each alternative was measured against the ROM):
// - StateLandPause keeps `mTimer--`. `mTimer = mTimer - 1` and
//   `(u8)(mTimer - 1)` shrink 0x90 to 0x84.
// - StateFall keeps `mLandCount++` and `Vector3 v[2]`. Adding
//   one as `mLandCount = mLandCount + 1` shrinks 0xe0 to 0xd4. Two separate
//   Vector3 locals DIFF 8 words.
// - Earthquake and Particle::System::NewSimple take scalar Fix12i.
//   Fix12<int> by value grows StateFall (0xe0 to 0xe8, and 0xe0 to 0x104).
//   The ROM plays the impact through func_0201267c, not Sound::Play.
// - Behavior's IsClsnInRange stays the scalar (void *, Fix12i, Fix12i)
//   call with (0, 0). Fix12<int> by value grows Behavior 0xc0 to 0xdc.
// - InitResources writes the ray's far point before the near point, and
//   calls GetClsnPos twice. The first result is unread. One call, or
//   writing the near point first, shrinks the function.
// - States 0..4 and the shadow/egg tail are daDsnBase_c's members
//   (src/game/actors/d_a_dsn_base.cpp).
// - data_ov025_02113814 is the file-table handle. g_profile_DONKAKU stays
//   overlay data.

/* daDkk_c.h pulls common.h before dExtSimpleModel_c.h, so Matrix4x3 is the flat
 * s32 m[12]. Keep dBgCh_Lin.h below that include. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDkk_c_AssetId { ASSET_DONKAKU_BMD = 0x02dc, ASSET_DONKAKU_KCL = 0x02dd };

#include "daDkk_c.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/Approach.h"
#include "Particle__System.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. data_ov025_02113814 stays
 * the file table decl_common.h already declares. */
struct DkkModelFilePtr : SharedFilePtr {
    DkkModelFilePtr(unsigned int fileId);
    ~DkkModelFilePtr();
};
struct DkkClsnFileHandler : SharedFilePtr {
    DkkClsnFileHandler(unsigned int fileId);
    ~DkkClsnFileHandler();
};
typedef char DkkModelFilePtr_size_must_be_8[sizeof(DkkModelFilePtr) == 8 ? 1 : -1];
typedef char DkkClsnFileHandler_size_must_be_8[sizeof(DkkClsnFileHandler) == 8 ? 1 : -1];

/* decl_common.h declares data_ov025_02113814; Sound.h declares
 * func_0201267c. */

// @symbol daDkk_c_classInit
/* DONKAKU factory. The `new` is the whole body. */
extern "C" daDkk_c *daDkk_c_classInit() {
    return new daDkk_c();
}

// @symbol _ZN7daDkk_c13InitResourcesEv
/* Stores the file table, then either starts already airborne or raycasts
 * up for the ceiling and lowers mRiseY, the top of the rise, under it. */
int daDkk_c::InitResources() {
    mFileTable = (s32)data_ov025_02113814;
    int r = Init();
    if (param1 & 1) {
        mState = 6;
    } else {
        mState = 0;
        dBgCh_Lin ray;
        Vector3 va;
        Vector3 vb;
        /* Far point first, then the near point. */
        int x = mPosX;
        vb.x = x;
        int y = mPosY;
        vb.y = y;
        int z = mPosZ;
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        ray.SetObjAndLine(va, vb, this);
        if (ray.DetectClsn()) {
            /* The first result is unused. */
            Vector3 p1 = ray.GetClsnPos();
            Vector3 p2 = ray.GetClsnPos();
            mRiseY = p2.y - 0x190000;
        }
    }
    return r;
}

// @symbol _ZN7daDkk_c8BehaviorEv
/* Eight states, then the shared model and collision update. */
int daDkk_c::Behavior() {
    switch (mState) {
    case 0:
        StateRise();
        break;
    case 1:
        StateHover();
        break;
    case 2:
        StateSlam();
        break;
    case 3:
        StateRest();
        break;
    case 4:
        StateRecover();
        break;
    case 5:
        StateFall();
        break;
    case 6:
        StateLandPause();
        break;
    case 7:
        StateTurn();
        break;
    }
    UpdateModelPosAndRotY();
    UpdateDropShadow();
    if (IsClsnInRange(0, 0) != 0 || WakeNearYoshiEgg() != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN7daDkk_c9StateFallEv
/* mState 5, the fall. Integrates the drop, and on reaching the stored ground
 * height snaps to it, shakes the camera, spawns the impact particle and hands
 * over to state 6. */
void daDkk_c::StateFall() {
    Vector3 v[2];
    UpdatePos(0);
    if (mVertSpeed >= 0)
        mVertAccel = -0x4000;
    else
        mVertAccel = -0x8000;
    if (mPosY > mGroundY)
        return;
    mPosY = mGroundY;
    v[1].x = mPosX;
    v[1].y = mPosY;
    v[1].z = mPosZ;
    Earthquake(v[1], 0x7d0000);
    mTimer = 0x3c;
    mLandCount++;
    mState = 6;
    v[0].x = mPosX;
    v[0].y = mPosY;
    v[0].z = mPosZ;
    v[0].y = v[0].y + 0x3c000;
    Particle::System::NewSimple(0x2e, v[0].x, v[0].y, v[0].z);
    func_0201267c(0xc7, &mCamSpacePos);
}

// @symbol _ZN7daDkk_c14StateLandPauseEv
/* mState 6, the pause after landing. Counts mTimer down; at zero it either
 * starts the next fall (state 5) or, on the fourth pass, turns to face the
 * opposite way (state 7). */
void daDkk_c::StateLandPause() {
    mTimer--;
    if (mTimer != 0)
        return;
    if (mLandCount != 4) {
        mState = 5;
        mVertSpeed = 0x3c000;
        func_0201267c(0xf4, &mCamSpacePos);
        return;
    }
    mState = 7;
    mTurnTargetAngle = (s16)(mAngleY + 0x8000);
}

// @symbol _ZN7daDkk_c9StateTurnEv
/* mState 7, the turn. Steps mAngleY toward mTurnTargetAngle; once
 * ApproachLinear reports it has arrived, copies it into mPrevAngleY, clears the
 * landing counter and goes back to state 6. */
int daDkk_c::StateTurn() {
    int arrived = ApproachLinear(mAngleY, mTurnTargetAngle, 0x400);
    if (arrived == 0)
        return arrived;
    mPrevAngleY = mAngleY;
    mState = 6;
    mLandCount = 0;
    mTimer = 0x28;
    return 0x28;
}

// @symbol _ZN7daDkk_c16OnAimedAtWithEggEv
/* Egg-aim height, 206 units. The return is added to the egg's Y. */
int daDkk_c::OnAimedAtWithEgg() {
    return 0xce000;
}

/* D1 and D0 come from the inline destructor in daDkk_c.h. */
// @symbol _ZN7daDkk_cD1Ev
// @symbol _ZN7daDkk_cD0Ev

/* Retail construction order: model 0x2dc, then collision 0x2dd.
 * mwcc emits __sinit_d_a_dkk.cpp. */
DkkModelFilePtr gDonkakuModelFile(ASSET_DONKAKU_BMD);
DkkClsnFileHandler gDonkakuCollisionFile(ASSET_DONKAKU_KCL);
