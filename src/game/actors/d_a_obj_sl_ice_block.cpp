//cpp
/**
 * Snowman's Land sliding ice block, shot and spawner variants.
 *
 * Actor ID 0x5d is the shot: it slides, sinks toward mMinPosY and
 * destroys itself. Any other ID is the spawner: it periodically
 * spawns shot blocks above itself. InitResources loads the shared
 * model and collision files for the shot, or arms the spawn timer
 * for the spawner.
 *
 * Notes:
 * - The data homes keep their linker names.
 * - func_020393d4 stores dBgW::UpdatePosWithVelocity as the collider's
 *   callback; dBgW.h has no setter.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSlIceBlock_c_AssetId { ASSET_SL_ICE_BROCK_BMD = 0x06b1, ASSET_SL_ICE_BROCK_KCL = 0x06b2 };

#include "daObjSlIceBlock_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgActor_c.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"

extern "C" {
void func_020393d4(int *p, int v);
}
extern char data_ov027_02113108[];

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and destroy
 * through SharedFilePtr_Destruct_Clsn. The manifest aliases those undefined
 * member spellings onto the real ROM functions; each handle is the base plus
 * the two words that family actually stores. */
struct SlIceBlockModelFilePtr : SharedFilePtr {
    SlIceBlockModelFilePtr(u32 fileID);
    ~SlIceBlockModelFilePtr();
};

struct SlIceBlockClsnFileHandle : SharedFilePtr {
    SlIceBlockClsnFileHandle(u32 fileID);
    ~SlIceBlockClsnFileHandle();
};

extern SlIceBlockModelFilePtr data_ov027_02113be8;
extern SlIceBlockClsnFileHandle data_ov027_02113be0;

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjSlIceBlock_cD1Ev
// @symbol _ZN17daObjSlIceBlock_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * dExtSimpleModel_c and dBgW_KcMbg, then dActor_c. */
daObjSlIceBlock_c::~daObjSlIceBlock_c() {}

// @symbol _ZN17daObjSlIceBlock_c15OnHitByMegaCharER6Player
/* Same idiom as daObjBk_Dossunbar_c/daObjBk_Lift_c/daObjIceBoard_c: the
 * trailing unqualified Kill() dispatches virtually to the inherited
 * base implementation (this class does not override slot 31). */
void daObjSlIceBlock_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN17daObjSlIceBlock_c16CleanupResourcesEv
int daObjSlIceBlock_c::CleanupResources() {
    unsigned char ok = (actorID == 0x5d);
    if (ok) {
        mMeshCollider.Disable();
    }
    data_ov027_02113be8.Release();
    data_ov027_02113be0.Release();
    return 1;
}

// @symbol _ZN17daObjSlIceBlock_c6RenderEv
int daObjSlIceBlock_c::Render() {
    int x = actorID == 0x5d;
    if (x) {
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        mModel.Render(0);
    }
    return 1;
}

// @symbol _ZN17daObjSlIceBlock_c8BehaviorEv
int daObjSlIceBlock_c::Behavior() {
    int isType = (actorID == 0x5d);
    if (isType) {
        if (DecIfAbove0_Short((u16 *)&mDelayTimer) == 0) {
            ApproachLinear(mHorzSpeed, 0, 0x3000);
            if (ApproachLinear(mPosY, mMinPosY, 0xa000) != 0) {
                MarkForDestruction();
            }
        }
        UpdatePos(0);
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x98, mCamSpacePos, 0);
    } else {
        if (DecIfAbove0_Short((u16 *)&mDelayTimer) == 0) {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            int spawnType = 1;
            if (DecIfAbove0_Byte((u8 *)&mNumToBigIce) == 0) {
                mNumToBigIce = 5;
                spawnType = 2;
            } else {
                pos.y -= 0x50000;
            }
            unsigned char cnt = mNumToBigIce;
            mDelayTimer = (cnt + 1) * 0x14;
            dActor_c::Spawn(0x5d, spawnType, pos, &mAngle, mAreaId, -1);
        }
    }
    return 1;
}

// @symbol _ZN17daObjSlIceBlock_c13InitResourcesEv
int daObjSlIceBlock_c::InitResources() {
    dExtSimpleModel_c::LoadFile(data_ov027_02113be8);
    dBgW_Kc::LoadFile(data_ov027_02113be0);

    int on = (actorID == 0x5d);
    if (on) {
        if (mModel.SetFile((BMD_File *)data_ov027_02113be8.filePtr, 1, -1) == 0)
            return 0;
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        mMeshCollider.SetFile((KCL_File *)data_ov027_02113be0.filePtr, mClsnMat, 0x1000, mAngleY,
                              *(CLPS_Block *)data_ov027_02113108);
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
        mMeshCollider.edgeContacts = false;
        mMeshCollider.Enable(this);
        mHorzSpeed = 0x2d000;
        mDelayTimer = 0x64;
        mPrevAngleY = -0x4000;
        mMinPosY = mPosY - 0xc8000;
    } else {
        mDelayTimer = (u8)mNumToBigIce * 0x14;
        mNumToBigIce = 5;
    }
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjSlIceBlock_c through
 * RTTI, allocation size, vtable identity, and the SL_ICEBLOCK_SHOT registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: SlidingIce_Spawn. */
// @symbol daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT
extern "C" daObjSlIceBlock_c *daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT() {
    return new daObjSlIceBlock_c();
}

/* Reconstructed source-style name: SM64DS proves daObjSlIceBlock_c through
 * RTTI, allocation size, vtable identity, and the SL_ICEBLOCK registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: SlidingIceSpawner_Spawn. */
// @symbol daObjSlIceBlock_c_classInit_SL_ICEBLOCK
extern "C" daObjSlIceBlock_c *daObjSlIceBlock_c_classInit_SL_ICEBLOCK() {
    return new daObjSlIceBlock_c();
}

/* The ice block's shared file handles: the model, then the collision map.
 * Their constructors and the destructor registrations make mwcc emit this
 * TU's __sinit. */
SlIceBlockModelFilePtr data_ov027_02113be8(ASSET_SL_ICE_BROCK_BMD);
SlIceBlockClsnFileHandle data_ov027_02113be0(ASSET_SL_ICE_BROCK_KCL);
