//cpp
/* Production translation unit for ov036/daObjRc_Tikuwa_c.
 * 10 function(s), .text 0x0211193c..0x02111d14. The RC_TIKUWA donut block:
 * once something has stood on it for fifteen frames it falls, poofs when it
 * lands or drops far enough below the player, and respawns at its home
 * position once the player has moved away.
 *
 * NAME: _ZTS16daObjRc_Tikuwa_c is "16daObjRc_Tikuwa_c" at ov036 0x02113cc0;
 * _ZTI at 0x02113cb4 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class DonutBlock
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x0211193c), D0
 * (0x02111988), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * OnStoodOn and StoodOnCallback are coined names (the ROM records none):
 * the collision callback body and the static entry InitResources installs
 * into dBgW's slot.
 *
 * What stays:
 * - func_020393d4 / func_020393c4 are 4-byte stores into dBgW's
 *   callback slots; naming belongs with dBgW in arm9.
 * - The named local in OnStoodOn is load-bearing -- folded
 *   into the `if`, mwcc emits 0x14 and the cartridge's is 0x20.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjRc_Tikuwa_c_AssetId { ASSET_RC_TIKUWA_BMD = 0x06af, ASSET_RC_TIKUWA_KCL = 0x06b0 };

#pragma defer_codegen off

/* decl_common.h declares these two handles as int[]. This TU defines them
 * as the 8-byte file objects the static initializer constructs. */
#define gRcTikuwaCollisionFile data_ov036_02114084_decl
#define gRcTikuwaModelFile data_ov036_0211408c_decl
#include "decl_common.h"
#undef gRcTikuwaCollisionFile
#undef gRcTikuwaModelFile
#include "daObjRc_Tikuwa_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TikuwaModelFilePtr : SharedFilePtr {
    TikuwaModelFilePtr(u32 fileID);
    ~TikuwaModelFilePtr();
};

struct TikuwaCollisionFilePtr : SharedFilePtr {
    TikuwaCollisionFilePtr(u32 fileID);
    ~TikuwaCollisionFilePtr();
};

typedef char TikuwaModelFilePtr_size_must_be_8[sizeof(TikuwaModelFilePtr) == 8 ? 1 : -1];
typedef char TikuwaCollisionFilePtr_size_must_be_8[sizeof(TikuwaCollisionFilePtr) == 8 ? 1 : -1];

extern TikuwaModelFilePtr gRcTikuwaModelFile;
extern TikuwaCollisionFilePtr gRcTikuwaCollisionFile;

/* local extern: no shared header declares these. */
extern "C" {
void func_020393d4(dBgW *bgw, void *fn);
void func_020393c4(dBgW *bgw, void *fn);
}

// @symbol _ZN16daObjRc_Tikuwa_cD1Ev
// @symbol _ZN16daObjRc_Tikuwa_cD0Ev
daObjRc_Tikuwa_c::~daObjRc_Tikuwa_c() {}

// @symbol _ZN16daObjRc_Tikuwa_c16CleanupResourcesEv
s32 daObjRc_Tikuwa_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    gRcTikuwaModelFile.Release();
    gRcTikuwaCollisionFile.Release();
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c16OnPendingDestroyEv
void daObjRc_Tikuwa_c::OnPendingDestroy() {}

// @symbol _ZN16daObjRc_Tikuwa_c6RenderEv
s32 daObjRc_Tikuwa_c::Render() {
    if (mState != 2)
        mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c8BehaviorEv
s32 daObjRc_Tikuwa_c::Behavior() {
    switch (mState) {
    case 0:
        if (mHadClsn == 0)
            mClsnTimer = 0;
        else {
            mClsnTimer += 1;
            mHadClsn = 0;
        }
        if (mClsnTimer >= 0xf)
            mState = 1;
        break;
    case 1:
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround() == 0) {
            if (DistToCPlayer() <= 0x9c4000)
                break;
        }
        TriplePoofDust();
        if (mMeshCollider.IsEnabled() != 0)
            mMeshCollider.Disable();
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        mState = 2;
        break;
    case 2: {
        s32 dist = DistToCPlayer();
        if (dist <= 0x3e8000)
            break;
        if (dist < 0x7d0000) {
            mVertSpeed = 0;
            mClsnTimer = 0;
            mHadClsn = 0;
            mState = 0;
        }
        break;
    }
    }
    UpdateModelPosAndRotY();
    if (mState != 2) {
        if (IsClsnInRange(0x5dc000, 0) != 0)
            UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c13InitResourcesEv
s32 daObjRc_Tikuwa_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gRcTikuwaModelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = dBgW_Kc::LoadFile(gRcTikuwaCollisionFile);
    mMeshCollider.SetFile((KCL_File *)kcl, mClsnMat, 0x1000, mAngleY,
                          *(CLPS_Block *)data_ov036_02112b48);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);
    mWithMeshClsn.Init(this, 0x32000, 0x64000, 0, 0);
    mTerminalVelocity = -0x1e000;
    mVertAccel = ~0x198;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* dBgW's collision callback body (ov036 0x02111ca4): an actor with actor ID
 * 0xbf (the Player) on the block sets mHadClsn. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Tikuwa_c9OnStoodOnEP8dActor_c
void daObjRc_Tikuwa_c::OnStoodOn(dActor_c *other) {
    /* The named local is load-bearing: folded into the `if`, mwcc drops the
     * store/reload pair and the function comes out 0x14. The cartridge's is
     * 0x20. */
    unsigned char isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        mHadClsn = 1;
}

/* -------------------------------------------------------------------------- */
/* The callback InitResources installs into dBgW's slot (ov036 0x02111cc4). It
 * drops the receiver dBgW passes first and hands the pair to the body above. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Tikuwa_c15StoodOnCallbackEPvPS_P8dActor_c
void daObjRc_Tikuwa_c::StoodOnCallback(void *collider, daObjRc_Tikuwa_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}

/* -------------------------------------------------------------------------- */
/* Factory: `return new` calls the inherited fBase_c::operator new -- the
 * 0x4ec allocation the cartridge's own factory makes. */
/* -------------------------------------------------------------------------- */
// @symbol daObjRc_Tikuwa_c_classInit
extern "C" daObjRc_Tikuwa_c *daObjRc_Tikuwa_c_classInit() {
    return new daObjRc_Tikuwa_c;
}

/* Source order is construction order: model file 0x6af, then the collision
 * file 0x6b0. The compiler registers each destructor beside the object. */
TikuwaModelFilePtr gRcTikuwaModelFile(ASSET_RC_TIKUWA_BMD);
TikuwaCollisionFilePtr gRcTikuwaCollisionFile(ASSET_RC_TIKUWA_KCL);
