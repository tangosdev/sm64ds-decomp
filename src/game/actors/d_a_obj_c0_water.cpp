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

#include "daObjC0Water_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"
#include "SaveBlock.h"

struct BMD_File;
#include "private/BTA_File.h"

struct CLPS_Block {
    u32 words[8];
};
struct KCL_File;

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
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
extern const u16 data_ov012_02111768[];
extern const fx32 data_ov012_02111770[];
extern const fx32 data_ov012_0211191c[];
extern const BTAMaterialRecord data_ov012_02111778[];
extern char data_ov012_02111bc4[];

BTA_File data_ov012_02111c24 = {
    91,
    0,
    (fx32 *)data_ov012_02111770,
    (u16 *)data_ov012_02111768,
    (fx32 *)data_ov012_0211191c,
    1,
    0,
    (BTAMaterialRecord *)data_ov012_02111778,
};

/* The texture-SRT keys and the single material record the file table above
 * drives. Retail keeps them in .rodata, interleaved with the keys of the
 * second BTA file at 0x02111c3c. The record is the one piece func_02046b64
 * patches in place (its material id), so the file table takes its address
 * through a non-const pointer. */

/* The single rotation key is the identity angle. */
const u16 data_ov012_02111768[2] = {0, 0};

const fx32 data_ov012_02111770[1] = {0x1000};

const BTAMaterialRecord data_ov012_02111778[1] = {
    {0xffff, 0, data_ov012_02111bc4, {1, 0}, {1, 0}, {1, 0}, {91, 0}, {1, 0}},
};

/* One texture translation per animation frame; the ramp climbs to 0x19a and
 * comes back, so the water scrolls one way and then the other. */
const fx32 data_ov012_0211191c[91] = {
    0x00000000, 0xffffffff, 0x00000000, 0x00000002, 0x00000005, 0x00000009, 0x0000000e, 0x00000014,
    0x0000001b, 0x00000023, 0x0000002b, 0x00000035, 0x0000003f, 0x0000004a, 0x00000055, 0x00000061,
    0x0000006d, 0x0000007a, 0x00000087, 0x00000095, 0x000000a2, 0x000000b0, 0x000000be, 0x000000cc,
    0x000000da, 0x000000e8, 0x000000f6, 0x00000103, 0x00000111, 0x0000011e, 0x0000012b, 0x00000137,
    0x00000143, 0x0000014f, 0x00000159, 0x00000164, 0x0000016d, 0x00000176, 0x0000017e, 0x00000185,
    0x0000018b, 0x00000190, 0x00000194, 0x00000197, 0x00000199, 0x0000019a, 0x00000199, 0x00000197,
    0x00000195, 0x00000191, 0x0000018c, 0x00000187, 0x00000180, 0x00000179, 0x00000171, 0x00000168,
    0x0000015f, 0x00000155, 0x0000014a, 0x0000013f, 0x00000134, 0x00000128, 0x0000011c, 0x0000010f,
    0x00000103, 0x000000f6, 0x000000e8, 0x000000db, 0x000000ce, 0x000000c1, 0x000000b3, 0x000000a6,
    0x00000099, 0x0000008c, 0x00000080, 0x00000073, 0x00000067, 0x0000005c, 0x00000051, 0x00000046,
    0x0000003c, 0x00000033, 0x0000002a, 0x00000022, 0x0000001a, 0x00000013, 0x0000000e, 0x00000009,
    0x00000005, 0x00000002, 0x00000000,
};

CLPS_Block data_ov012_02111c90 = {
    0x53504c43, 0x00010008, 0x00000fe0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};
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

/* The BMD material this file's record drives. */
char data_ov012_02111bc4[12] = "obj_water";

extern "C" {
u32 data_ov012_021124f0[4];
}
