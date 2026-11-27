//cpp
/**
 * d_a_obj_c0_water.cpp
 * Object - Castle basement water
 *
 * ov012/daObjC0Water_c, 7 function(s), .text 0x02111450..0x02111768.
 * RTTI ov012:0x021123c4 names daObjC0Water_c; _ZTS at 0x021123d0;
 * _ZTV address point 0x02112408; g_profile_C0_WATER at 0x021123e4.
 * overlay_actors.md: ov012 BASEMENT_WATER(35). Ugly RTTI name is final.
 *
 * mwccarm emits ordinary text sections in reverse source order, so the
 * ROM-high factory is defined first. The inline destructor in
 * include/daObjC0Water_c.h emits the retail D1/D0 pair first and emits no
 * D2 body.
 *
 * Notes:
 * - dBgW_KcMbg::SetFile is a header member call; its scale is fx32, a
 *   plain s32 (notes/mwccarm-codegen.md 6az).
 * - Sound::PlayLong / Sound::PlaySecretSound come from Sound.h, which now
 *   declares both. The ROM mangle is RK7Vector3 (const Vector3 &); a pointer
 *   argument is byte-identical, so this caller alone does not prove the
 *   reference spelling -- the header's definition-side reading does.
 * - data_ov012_02111c24 / 02111c90 BTA / CLPS handles; this TU consumes
 *   them, overlay .data owns them. The 021124c8 / 021124d0 KCL / BMD
 *   handles are defined at the end of this file.
 * - gSaveBlock.file.flags2 & 0x80000 is the same group flag
 *   daObjC0_Switch_c::OnGroundPounded writes; this water drains when it is
 *   set. gWaterHeight is the arm9 water-height word this TU publishes.
 * - no daPly_c.h.
 * - homeless _ZN10dBgActor_cD2Ev from the inline dtor chain (deadstrip).
 */

#include "CLPS.h"
#include "daObjC0Water_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"
#include "SaveBlock.h"

struct BMD_File;
#include "private/BTA_File.h"

struct KCL_File;

/* 8-byte file handles. The model uses ModelFile::ModelFile / ModelFile::~ModelFile and the
 * collision file ClsnFile::ClsnFile / ClsnFile::~ClsnFile. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct C0WaterModelFilePtr : SharedFilePtr {
    C0WaterModelFilePtr(u32 fileID);
    ~C0WaterModelFilePtr();
};

struct C0WaterCollisionFilePtr : SharedFilePtr {
    C0WaterCollisionFilePtr(u32 fileID);
    ~C0WaterCollisionFilePtr();
};

/* Defined at the end of this file so the constructors do not enter .text. */
extern C0WaterModelFilePtr data_ov012_021124d0;
extern C0WaterCollisionFilePtr data_ov012_021124c8;
/* Stage data owned by d_s_map_ov012.c: the water texture animation and
 * collision properties. */
extern BTA_File data_ov012_02111c24;
extern CLPS_Block data_ov012_02111c90;

extern int gWaterHeight;

/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: BasementWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 832 = 0x340 is the class's own size, loaded into fBase_c's size_t
 * operator new; dBgActor_c's C2, this class's vptr store, and
 * dExtAnmTexSRT_c C1 on the member at 0x320 are the implicit
 * constructor. The null check is the one `new` itself emits. Declaring a
 * constructor of our own would emit a `bl` the factory does not have. */
daObjC0Water_c *daObjC0Water_c_classInit() {
    return new daObjC0Water_c();
}

int daObjC0Water_c::InitResources() {
    dExtSimpleModel_c::LoadFile(data_ov012_021124d0);
    dBgW_Kc::LoadFile(data_ov012_021124c8);
    mModel.SetFile((BMD_File *)data_ov012_021124d0.filePtr, 1, 0x14);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov012_021124d0.filePtr, data_ov012_02111c24);
    mTextureTransformer.SetFile(data_ov012_02111c24, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)data_ov012_021124c8.filePtr, mClsnMat, 0x1000, mAngle.y,
                          data_ov012_02111c90);
    mMeshCollider.Enable(this);
    mLoweredY = mPos.y - 0x4b0000;
    if (gSaveBlock.file.flags2 & 0x80000) {
        mPos.y = mLoweredY;
    }
    return 1;
}

int daObjC0Water_c::Behavior() {
    if (mWasJustDrained != 0)
        Sound::PlaySecretSound(this, &mSoundTimer);
    if (mPos.y <= mLoweredY) {
        mPos.y = mLoweredY;
    } else if (gSaveBlock.file.flags2 & 0x80000) {
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, mCamSpacePos, 0);
        mPos.y -= 0x5000;
        mWasJustDrained = 1;
        if (mPos.y <= mLoweredY) {
            mPos.y = mLoweredY;
            dMap_c::UpdateLevelSpecific();
        }
    }
    gWaterHeight = mPos.y;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    return 1;
}

int daObjC0Water_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

int daObjC0Water_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov012_021124d0.Release();
    data_ov012_021124c8.Release();
    return 1;
}

/* Source order is construction order: model file 1451, collision file 1452.
 * __sinit_d_a_obj_c0_water.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
C0WaterModelFilePtr data_ov012_021124d0(1451);
C0WaterCollisionFilePtr data_ov012_021124c8(1452);

extern "C" {
u32 data_ov012_021124f0[4];
}
