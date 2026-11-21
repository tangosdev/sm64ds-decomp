//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_mizu_AssetId { ASSET_WC_MIZU_BMD = 0x06ca, ASSET_WC_MIZU_KCL = 0x06cb };

#include "Sound.h"
/* PROMOTED translation unit -- ov029/daObjWc_Mizu_c (9 function(s)).
 *
 * WDW water (profile WDW_WATER 101). RTTI ov029:0x02114098 names
 * daObjWc_Mizu_c; g_profile_WC_MIZU is at 0x021140b8. ov029 is WDW
 * (ROTATING_PLATFORM_WDW .. WDW_WATER) -- this is the water plane, not
 * daObjWc_Obj02_c / daObjWc_Obj03_c.
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x021121a4..0x02112630 in ov029 (config/tu_manifest.d/ov029/daObjWc_Mizu_c.json,
 * config/arm9/overlays/ov029/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks. The inline
 * destructor in include/daObjWc_Mizu_c.h emits the retail D1/D0 pair first
 * and emits no D2 body.
 *
 * NOTES:
 * - The daObjWc_Obj07_c_* names are a stale symbols.txt attribution: this TU
 *   is the only consumer and defines both handles (obj07's own handles live
 *   at 0x021142d4/0x021142dc).
 * - data_ov029_02112b2c is this TU's water-level table. The texture keys,
 *   the BTA and the CLPS block are defined here too.
 * - S14: g_profile_WC_MIZU stays outside the licensed .text.
 * - UpdateModelTransform (0x021122b4) and UpdateClsnTransform (0x02112250)
 *   are TU-local; Behavior calls both every frame.
 */

#include "common.h"
#include "math/MtxStack.h"
#include "daObjWc_Mizu_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "nitro/fx/fx_trig.h"
#include "private/BTA_File.h"

struct BMD_File;
struct KCL_File;

/* 'CLPS' collision-property block: tag, count/stride header, entries. */
struct CLPS_Block {
    u32 tag;
    u32 header;
    u32 entries[6];
};

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcMizuModelFilePtr : SharedFilePtr {
    WcMizuModelFilePtr(u32 fileID);
    ~WcMizuModelFilePtr();
};

struct WcMizuCollisionFilePtr : SharedFilePtr {
    WcMizuCollisionFilePtr(u32 fileID);
    ~WcMizuCollisionFilePtr();
};

u8 IsAreaShowing(int idx);

extern WcMizuModelFilePtr daObjWc_Obj07_c_ModelFile;
extern WcMizuCollisionFilePtr daObjWc_Obj07_c_ClsnFile;
extern const int data_ov029_02112b2c[3] = {
    0x0002d000,
    0x003e8000,
    0x00abe000,
};
extern u8 data_0209f2c0;
extern int gWaterHeight;

/* The texture animation's keys. The material record and the translation
   run are labels inside this table. */
extern u32 data_ov029_02112ec0;
extern const u32 data_ov029_021129a0[] = {
    0x00001000, 0x0000ffff, (u32)&data_ov029_02112ec0,
    0x00000001, 0x00000001, 0x00000001,
    0x0000005b, 0x0000005b, 0x00000000,
    0x0000002e, 0x0000005b, 0x00000089,
    0x000000b6, 0x000000e4, 0x00000111,
    0x0000013f, 0x0000016c, 0x0000019a,
    0x000001c7, 0x000001f5, 0x00000222,
    0x00000250, 0x0000027d, 0x000002ab,
    0x000002d8, 0x00000306, 0x00000333,
    0x00000361, 0x0000038e, 0x000003bc,
    0x000003e9, 0x00000417, 0x00000444,
    0x00000472, 0x0000049f, 0x000004cd,
    0x000004fa, 0x00000528, 0x00000555,
    0x00000583, 0x000005b0, 0x000005de,
    0x0000060b, 0x00000639, 0x00000666,
    0x00000694, 0x000006c1, 0x000006ef,
    0x0000071c, 0x0000074a, 0x00000777,
    0x000007a5, 0x000007d2, 0x00000800,
    0x0000082e, 0x0000085b, 0x00000889,
    0x000008b6, 0x000008e4, 0x00000911,
    0x0000093f, 0x0000096c, 0x0000099a,
    0x000009c7, 0x000009f5, 0x00000a22,
    0x00000a50, 0x00000a7d, 0x00000aab,
    0x00000ad8, 0x00000b06, 0x00000b33,
    0x00000b61, 0x00000b8e, 0x00000bbc,
    0x00000be9, 0x00000c17, 0x00000c44,
    0x00000c72, 0x00000c9f, 0x00000ccd,
    0x00000cfa, 0x00000d28, 0x00000d55,
    0x00000d83, 0x00000db0, 0x00000dde,
    0x00000e0b, 0x00000e39, 0x00000e66,
    0x00000e94, 0x00000ec1, 0x00000eef,
    0x00000f1c, 0x00000f4a, 0x00000f77,
    0x00000fa5, 0x00000fd2, 0x00001000,
};
extern u16 data_ov029_0211299c[];
extern fx32 data_ov029_021129c0[];
extern BTAMaterialRecord data_ov029_021129a4[];

/* The WDW water's texture animation: 0x5b frames, constant scale and
   translation, one material. */
BTA_File data_ov029_02112fb8 = {
    0x5b, 0, (fx32 *)data_ov029_021129a0, data_ov029_0211299c, data_ov029_021129c0,
    1,    0, data_ov029_021129a4,
};

/* The water surface's collision properties: one floor entry. */
CLPS_Block data_ov029_02112fec = {
    0x53504c43,
    0x00010008,
    {0x00000fe0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: WDW_Water_Spawn. */
daObjWc_Mizu_c *daObjWc_Mizu_c_classInit() {
    return new daObjWc_Mizu_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjWc_Mizu_c::InitResources() {
    void *m;
    void *k;

    m = dExtSimpleModel_c::LoadFile(daObjWc_Obj07_c_ModelFile);
    mModel.SetFile((BMD_File *)m, 1, -1);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)daObjWc_Obj07_c_ModelFile.filePtr, data_ov029_02112fb8);
    mTextureTransformer.SetFile(data_ov029_02112fb8, 0, 0x1000, 0);

    mUseSpawnPosY = (u8)(param1 & 1);
    if (mUseSpawnPosY == 0) {
        unsigned int idx = data_0209f2c0;
        if (idx >= 3)
            idx = 2;
        mPos.y = data_ov029_02112b2c[idx];
    }
    mTargetPosY = mPos.y;
    gWaterHeight = mPos.y;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    k = dBgW_Kc::LoadFile(daObjWc_Obj07_c_ClsnFile);
    mMeshCollider.SetFile((KCL_File *)k, mClsnMat, 0x1000, mAngle.y, data_ov029_02112fec);
    mMeshCollider.Enable(this);

    mTrueAreaID = mAreaId;
    mAreaId = -1;
    mPhaseAngle = 0;

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjWc_Mizu_c::Behavior() {
    int t;

    mAreaId = -1;

    if (IsAreaShowing(mTrueAreaID) == 0) {
        mAreaId = mTrueAreaID;
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
    } else {
        if (mMeshCollider.IsEnabled() == 0) {
            mMeshCollider.Enable(this);
        }
    }

    t = mTargetPosY;
    if (mPos.y != t) {
        if (mPos.y < t) {
            mPos.y += 0xa000;
            if (mPos.y > mTargetPosY)
                mPos.y = mTargetPosY;
        } else {
            mPos.y -= 0xa000;
            if (mPos.y < mTargetPosY)
                mPos.y = mTargetPosY;
        }
    }

    if (mPos.y != mPrevPos.y) {
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, mCamSpacePos, 0);
    }

    mPhaseAngle += 0x200;
    mWaterHeight = FX_SinIdx((u16)mPhaseAngle >> 4) * 15 + mPos.y;
    gWaterHeight = mWaterHeight;
    UpdateModelTransform();
    UpdateClsnTransform();
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjWc_Mizu_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjWc_Mizu_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    daObjWc_Obj07_c_ModelFile.Release();
    daObjWc_Obj07_c_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* The water model sits at the bobbing surface height, in model eighths. */
void daObjWc_Mizu_c::UpdateModelTransform() {
    Matrix4x3_FromTranslation(&mModel.mat4x3, mPos.x >> 3, mWaterHeight >> 3, mPos.z >> 3);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* The collision mesh takes the model's rotation with a full-scale translation. */
void daObjWc_Mizu_c::UpdateClsnTransform() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.t.x = mPos.x;
    mClsnMat.t.y = mWaterHeight;
    mClsnMat.t.z = mPos.z;
    mMeshCollider.Transform(mClsnMat, mAngle.y);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjWc_Mizu_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_mizu.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcMizuModelFilePtr daObjWc_Obj07_c_ModelFile(ASSET_WC_MIZU_BMD);
WcMizuCollisionFilePtr daObjWc_Obj07_c_ClsnFile(ASSET_WC_MIZU_KCL);
