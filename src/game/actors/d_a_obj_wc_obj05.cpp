//cpp
/* Cage Lift (CAGE_LIFT 99) -- ov029/daObjWc_Obj05_c.
 *
 * ov029 is WDW. RTTI ov029:0x02113e30 names daObjWc_Obj05_c; overlay_actors
 * maps profile 99 to CAGE_LIFT. Not daObjWc_Obj02_c (ARROW_LIFT), not
 * daObjWc_Obj03_c (WATER_DIAMOND), not daObjWc_Obj04_c
 * (SWITCH_ACTIVATED_PLANK), and not daObjWc_Mizu_c (WDW_WATER).
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. Highest-address first.
 * The destructor is inline in include/daObjWc_Obj05_c.h (D1 then D0, no D2).
 *
 * What stays:
 * - dBgW::SetBeforeClsnCallback / dBgW::SetStoodOnCallback / dBgW::SetClipRadius: InitResources stores
 *   dBgW::UpdatePosWithTransform and &StoodOnCallback on mMeshCollider;
 *   Behavior clips at 0x250000 through the dBgW property member.
 * - gWcObj05ModelFile / 02114284 are this TU's BMD/KCL file handles;
 *   data_ov029_0211306c the CLPS block stays unowned overlay data.
 * - S14: g_profile_WC_OBJ05 stays outside the licensed .text.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj05_AssetId { ASSET_WC_OBJ05_BMD = 0x06d3, ASSET_WC_OBJ05_KCL = 0x06d4 };

#include "common.h"
#include "daObjWc_Obj05_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj05ModelFilePtr : SharedFilePtr {
    WcObj05ModelFilePtr(u32 fileID);
    ~WcObj05ModelFilePtr();
};

struct WcObj05CollisionFilePtr : SharedFilePtr {
    WcObj05CollisionFilePtr(u32 fileID);
    ~WcObj05CollisionFilePtr();
};

extern WcObj05ModelFilePtr gWcObj05ModelFile;
extern WcObj05CollisionFilePtr gWcObj05CollisionFile;
struct CLPS_Block {
    u32 words[8];
};

CLPS_Block data_ov029_0211306c = {
    0x53504c43, 0x00020008, 0x01000fc0, 0x000000ff, 0x01000fc5, 0x000000ff, 0xffffffff, 0xffffffff,
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: CageLift_Spawn. Attribution stays on
 * daObjWc_Obj05_c_classInit. */
daObjWc_Obj05_c *daObjWc_Obj05_c_classInit() {
    return new daObjWc_Obj05_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* The mesh collider's stood-on callback gets three arguments; OnStoodOn
 * wants the last two. long_calls emits the pooled `ldr ip,[pc,#8]; bx ip`
 * absolute tail call. */
#pragma long_calls on
void daObjWc_Obj05_c::StoodOnCallback(void *collider, daObjWc_Obj05_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Arms mPlayerOnTop when the actor standing on the cage is the player (0xbf).
 * The unsigned char temporary is load-bearing. */
void daObjWc_Obj05_c::OnStoodOn(dActor_c *other) {
    unsigned char ok = other->actorID == 0xbf;
    if (ok)
        mPlayerOnTop = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjWc_Obj05_c::InitResources() {
    BMD_File *f = dExtSimpleModel_c::LoadFile(gWcObj05ModelFile);
    mModel.SetFile(f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    KCL_File *mc = dBgW_Kc::LoadFile(gWcObj05CollisionFile);
    mMeshCollider.SetFile(mc, mClsnMat, 0x1000, mAngle.y, data_ov029_0211306c);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetStoodOnCallback((::StoodOnCallback)&StoodOnCallback);
    mPrevPlayerOnTop = 0;
    mPlayerOnTop = mPrevPlayerOnTop;
    mDwellTimer = 0;
    if (param1 & 1)
        mState = 3;
    else
        mState = 0;
    mRestPosY = mPos.y;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjWc_Obj05_c::Behavior() {
    mMeshCollider.SetClipRadius(0x250000);

    switch (mState) {
    case 0:
        if (mPlayerOnTop != 0) {
            if (mPrevPlayerOnTop == 0) {
                mState++;
            }
        }
        mPrevPlayerOnTop = mPlayerOnTop;
        break;
    case 1: {
        mPos.y -= 0x14000;
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8d, mCamSpacePos, 0);
        {
            int v = mRestPosY + (int)0xff5d8000;
            if (mPos.y <= v) {
                mPos.y = v;
                mState++;
                mDwellTimer = 0;
            }
        }
        break;
    }
    case 2: {
        if ((u16)mDwellTimer >= 110) {
            int snd = Sound::PlayLong(mSoundHandle, 3, 0x8d, mCamSpacePos, 0);
            mSoundHandle = snd;
            mPos.y += 0xa000;
            if (mPos.y >= mRestPosY) {
                mPos.y = mRestPosY;
                mState = 0;
                mDwellTimer = 0;
            }
        } else {
            u16 *p = (u16 *)&mDwellTimer;
            *p = *p + 1;
        }
        break;
    }
    case 3:
        break;
    }

    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0)) {
        UpdateClsnPosAndRot();
    }
    mMeshCollider.Transform(mClsnMat, mAngle.y);
    mPlayerOnTop = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjWc_Obj05_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjWc_Obj05_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    gWcObj05ModelFile.Release();
    gWcObj05CollisionFile.Release();
    return 1;
}
/* daObjWc_Obj05_c's inline class-body destructor is instantiated by the
 * definitions above. mwccarm emits D1 and D0 into this object. */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj05.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj05ModelFilePtr gWcObj05ModelFile(ASSET_WC_OBJ05_BMD);
WcObj05CollisionFilePtr gWcObj05CollisionFile(ASSET_WC_OBJ05_KCL);
