//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_ks_water_AssetId { ASSET_KS_MIZU_BMD = 0x0689, ASSET_KS_MIZU_KCL = 0x068a };

#include "Sound.h"
/* Production translation unit for ov017/daObjKsWater_c, hand-curated.
 * 7 function(s), .text 0x021111a0..0x021114b8.
 *
 * The draining water inside the sunken ship (profile KS_MIZU, actor 62).
 * RTTI ov017:0x02111bb4 names daObjKsWater_c; _ZTS at 0x02111bc0;
 * _ZTV address point 0x02111bf8; g_profile_KS_MIZU at 0x02111bd4.
 * overlay_actors.md: ov017 SHIP_WATER(62). Ugly RTTI name is final.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daObjKsWater_c.h emits the retail D1/D0 pair
 * first and emits no D2 body.
 *
 * What stays:
 * - data_ov017_02111c80 / 02111c88 / 02111a60 / 02111a94 KCL / BMD / BTA / CLPS
 *   handles; this TU consumes them, overlay .bss/.data owns them.
 * - no daPly_c.h.
 * - homeless _ZN10dBgActor_cD2Ev from the inline dtor chain (deadstrip).
 */

#include "daObjKsWater_c.h"
#include "SharedFilePtr.h"
#include "daObjTbox_c.h"
#include "private/BTA_File.h"

struct BMD_File;
struct KCL_File;

struct CLPS_Block {
    u32 words[8];
};

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The manifest aliases those
 * undefined members onto the ROM symbols. */
struct KsWaterModelFilePtr : SharedFilePtr {
    KsWaterModelFilePtr(u32 fileID);
    ~KsWaterModelFilePtr();
};

struct KsWaterClsnFileHandle : SharedFilePtr {
    KsWaterClsnFileHandle(u32 fileID);
    ~KsWaterClsnFileHandle();
};

extern KsWaterClsnFileHandle data_ov017_02111c80;
extern KsWaterModelFilePtr data_ov017_02111c88;
extern const u32 data_ov017_021114b8[];
extern char data_ov017_02111a18[];

BTA_File data_ov017_02111a60 = {
    300,
    0,
    (fx32 *)&data_ov017_021114b8[1],
    (u16 *)data_ov017_021114b8,
    (fx32 *)&data_ov017_021114b8[9],
    1,
    0,
    (BTAMaterialRecord *)&data_ov017_021114b8[2],
};

/* Rotation key, scale key, one material record, then one translation per
 * frame. func_02046b64 patches the record's material id in place, so the
 * file table takes its address through a non-const pointer. */
const u32 data_ov017_021114b8[309] = {
    0,          0x1000,     0x0000ffff, (u32)&data_ov017_02111a18,
    1,          1,          1,          300,
    300,        0x00000000, 0x0000000e, 0x0000001b,
    0x00000029, 0x00000037, 0x00000044, 0x00000052,
    0x00000060, 0x0000006e, 0x0000007b, 0x00000089,
    0x00000097, 0x000000a4, 0x000000b2, 0x000000c0,
    0x000000cd, 0x000000db, 0x000000e9, 0x000000f7,
    0x00000104, 0x00000112, 0x00000120, 0x0000012d,
    0x0000013b, 0x00000149, 0x00000156, 0x00000164,
    0x00000172, 0x00000180, 0x0000018d, 0x0000019b,
    0x000001a9, 0x000001b6, 0x000001c4, 0x000001d2,
    0x000001df, 0x000001ed, 0x000001fb, 0x00000209,
    0x00000216, 0x00000224, 0x00000232, 0x0000023f,
    0x0000024d, 0x0000025b, 0x00000268, 0x00000276,
    0x00000284, 0x00000292, 0x0000029f, 0x000002ad,
    0x000002bb, 0x000002c8, 0x000002d6, 0x000002e4,
    0x000002f1, 0x000002ff, 0x0000030d, 0x0000031b,
    0x00000328, 0x00000336, 0x00000344, 0x00000351,
    0x0000035f, 0x0000036d, 0x0000037a, 0x00000388,
    0x00000396, 0x000003a4, 0x000003b1, 0x000003bf,
    0x000003cd, 0x000003da, 0x000003e8, 0x000003f6,
    0x00000403, 0x00000411, 0x0000041f, 0x0000042d,
    0x0000043a, 0x00000448, 0x00000456, 0x00000463,
    0x00000471, 0x0000047f, 0x0000048c, 0x0000049a,
    0x000004a8, 0x000004b6, 0x000004c3, 0x000004d1,
    0x000004df, 0x000004ec, 0x000004fa, 0x00000508,
    0x00000515, 0x00000523, 0x00000531, 0x0000053f,
    0x0000054c, 0x0000055a, 0x00000568, 0x00000575,
    0x00000583, 0x00000591, 0x0000059e, 0x000005ac,
    0x000005ba, 0x000005c7, 0x000005d5, 0x000005e3,
    0x000005f1, 0x000005fe, 0x0000060c, 0x0000061a,
    0x00000627, 0x00000635, 0x00000643, 0x00000650,
    0x0000065e, 0x0000066c, 0x0000067a, 0x00000687,
    0x00000695, 0x000006a3, 0x000006b0, 0x000006be,
    0x000006cc, 0x000006d9, 0x000006e7, 0x000006f5,
    0x00000703, 0x00000710, 0x0000071e, 0x0000072c,
    0x00000739, 0x00000747, 0x00000755, 0x00000762,
    0x00000770, 0x0000077e, 0x0000078c, 0x00000799,
    0x000007a7, 0x000007b5, 0x000007c2, 0x000007d0,
    0x000007de, 0x000007eb, 0x000007f9, 0x00000807,
    0x00000815, 0x00000822, 0x00000830, 0x0000083e,
    0x0000084b, 0x00000859, 0x00000867, 0x00000874,
    0x00000882, 0x00000890, 0x0000089e, 0x000008ab,
    0x000008b9, 0x000008c7, 0x000008d4, 0x000008e2,
    0x000008f0, 0x000008fd, 0x0000090b, 0x00000919,
    0x00000927, 0x00000934, 0x00000942, 0x00000950,
    0x0000095d, 0x0000096b, 0x00000979, 0x00000986,
    0x00000994, 0x000009a2, 0x000009b0, 0x000009bd,
    0x000009cb, 0x000009d9, 0x000009e6, 0x000009f4,
    0x00000a02, 0x00000a0f, 0x00000a1d, 0x00000a2b,
    0x00000a39, 0x00000a46, 0x00000a54, 0x00000a62,
    0x00000a6f, 0x00000a7d, 0x00000a8b, 0x00000a98,
    0x00000aa6, 0x00000ab4, 0x00000ac1, 0x00000acf,
    0x00000add, 0x00000aeb, 0x00000af8, 0x00000b06,
    0x00000b14, 0x00000b21, 0x00000b2f, 0x00000b3d,
    0x00000b4a, 0x00000b58, 0x00000b66, 0x00000b74,
    0x00000b81, 0x00000b8f, 0x00000b9d, 0x00000baa,
    0x00000bb8, 0x00000bc6, 0x00000bd3, 0x00000be1,
    0x00000bef, 0x00000bfd, 0x00000c0a, 0x00000c18,
    0x00000c26, 0x00000c33, 0x00000c41, 0x00000c4f,
    0x00000c5c, 0x00000c6a, 0x00000c78, 0x00000c86,
    0x00000c93, 0x00000ca1, 0x00000caf, 0x00000cbc,
    0x00000cca, 0x00000cd8, 0x00000ce5, 0x00000cf3,
    0x00000d01, 0x00000d0f, 0x00000d1c, 0x00000d2a,
    0x00000d38, 0x00000d45, 0x00000d53, 0x00000d61,
    0x00000d6e, 0x00000d7c, 0x00000d8a, 0x00000d98,
    0x00000da5, 0x00000db3, 0x00000dc1, 0x00000dce,
    0x00000ddc, 0x00000dea, 0x00000df7, 0x00000e05,
    0x00000e13, 0x00000e21, 0x00000e2e, 0x00000e3c,
    0x00000e4a, 0x00000e57, 0x00000e65, 0x00000e73,
    0x00000e80, 0x00000e8e, 0x00000e9c, 0x00000eaa,
    0x00000eb7, 0x00000ec5, 0x00000ed3, 0x00000ee0,
    0x00000eee, 0x00000efc, 0x00000f09, 0x00000f17,
    0x00000f25, 0x00000f33, 0x00000f40, 0x00000f4e,
    0x00000f5c, 0x00000f69, 0x00000f77, 0x00000f85,
    0x00000f92, 0x00000fa0, 0x00000fae, 0x00000fbc,
    0x00000fc9, 0x00000fd7, 0x00000fe5, 0x00000ff2,
    0x00001000,
};

CLPS_Block data_ov017_02111a94 = {
    0x53504c43, 0x00010008, 0x00000fe0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: ShipWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111484 loads 0x340 -- the class's own size -- into the header's leaf
 * operator new; 0x02111494 calls dBgActor_c's C2, the store at 0x021114a0
 * lays down this class's vptr, and dExtAnmTexSRT_c C1 runs on the member
 * at 0x320. The null check is the one `new` itself emits. The implicit
 * constructor is what keeps the intervening construction inlined; declaring
 * a constructor of our own would emit a `bl` the factory does not have. */
daObjKsWater_c *daObjKsWater_c_classInit() {
    return new daObjKsWater_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjKsWater_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov017_02111c88), 1, -1);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov017_02111c88.filePtr, data_ov017_02111a60);
    mTextureTransformer.SetFile(data_ov017_02111a60, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(data_ov017_02111c80), mClsnMat, 0x1000, mAngle.y,
                          data_ov017_02111a94);
    mMeshCollider.Enable(this);
    mOriginalPosY = mPos.y;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjKsWater_c::Behavior() {
    if (mChestsOpen == 0) {
        int ok = 1;
        dActor_c *p = dActor_c::FindWithActorID(0xd, 0);
        while (p != 0) {
            int state = ((daObjTbox_c *)p)->mState;
            int cond = 1;
            if (state != 1) {
                if (state != 2)
                    cond = 0;
            }
            if (cond == 0)
                ok = 0;
            p = dActor_c::FindWithActorID(0xd, p);
        }
        if (ok != 0)
            mChestsOpen = 1;
    } else {
        int d = mOriginalPosY - mPos.y;
        if (d < 0)
            d = -d;
        if (d < 0x92e000) {
            mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, mCamSpacePos, 0);
            mPos.y -= 0x5000;
        }
    }
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjKsWater_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
int daObjKsWater_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov017_02111c88.Release();
    data_ov017_02111c80.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKsWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */

/* File-scope objects, in __sinit_d_a_obj_ks_water.cpp construction order:
 * the model's SharedFilePtr, then the collision file's. */
KsWaterModelFilePtr data_ov017_02111c88(ASSET_KS_MIZU_BMD);
KsWaterClsnFileHandle data_ov017_02111c80(ASSET_KS_MIZU_KCL);

/* The BMD material the texture animation above drives. */
char data_ov017_02111a18[12] = "water_mat";
