//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_ks_water_AssetId {
    ASSET_KS_MIZU_BMD = 0x0689,
    ASSET_KS_MIZU_KCL = 0x068a
};

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
 * Leftover:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - Sound::PlayLong: Behavior's long water sound; Sound.h has no PlayLong.
 * - SharedFilePtr +4: InitResources' TextureTransformer::Prepare reads the
 *   BMD at data_ov017_02111c88 +4; SharedFilePtr is the 8-byte handle.
 * - BTA_File in the header is only the leading numFrames halfword. The
 *   object at data_ov017_02111a60 is the 0x18 bytes func_020469e8 reads.
 * - data_ov017_02111b74 and data_ov017_02111b88 stay the ROM copy. Nothing
 *   this table points at lands there, and _ZTI14daObjKsWater_c follows them.
 *   g_profile_KS_MIZU and _ZTV14daObjKsWater_c stay outside.
 * - no Player.h.
 * - homeless _ZN10dBgActor_cD2Ev from the inline dtor chain (deadstrip).
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN14daObjKsWater_cD1Ev.cpp
 *   [1] 0x021111ec  src/_ZN14daObjKsWater_cD0Ev.cpp
 *   [2] 0x0211124c  src/_ZN14daObjKsWater_c16CleanupResourcesEv.cpp
 *   [3] 0x02111290  src/_ZN14daObjKsWater_c6RenderEv.cpp
 *   [4] 0x021112c4  src/_ZN14daObjKsWater_c8BehaviorEv.cpp
 *   [5] 0x021113c0  src/_ZN14daObjKsWater_c13InitResourcesEv.cpp
 *   [6] 0x02111480  src/daObjKsWater_c_classInit.c
 */

#include "daObjKsWater_c.h"
#include "SharedFilePtr.h"
#include "daObjTbox_c.h"
#include "CLPS.h"

struct BMD_File;
struct KCL_File;

/* Header CLPS_Block is incomplete. This object is the 8-byte header
   (stride at +4) plus three 8-byte entries. */
struct CLPS_Block {
    u32 header0;
    u16 stride;
    u16 header6;
    CLPS entries[3];
};
typedef char CLPS_Block_size_must_be_0x20[sizeof(CLPS_Block) == 0x20 ? 1 : -1];

struct ClpsHeader {
    u32 header0;
    u16 stride;
    u16 header6;
};
typedef char ClpsHeader_size_must_be_0x8[sizeof(ClpsHeader) == 0x8 ? 1 : -1];

/* func_020469e8 reads these fields through the BTA_File* it is passed.
   func_02046b64 reads the name pointer at +4 of each entry. */
struct BtaEntry {
    u16 id;
    u16 pad;
    char *name;
    u16 flag1;
    u16 idx1;
    u16 flag2;
    u16 idx2;
    u16 flag3;
    u16 idx3;
    u16 flag4;
    u16 idx4;
    u16 flag5;
    u16 idx5;
};
typedef char BtaEntry_size_must_be_0x1c[sizeof(BtaEntry) == 0x1c ? 1 : -1];

struct BtaDesc {
    u16 numFrames;
    u16 pad;
    const s32 *tableA;
    const u16 *tableB;
    const s32 *tableC;
    u16 count;
    u16 pad2;
    const BtaEntry *entries;
};
typedef char BtaDesc_size_must_be_0x18[sizeof(BtaDesc) == 0x18 ? 1 : -1];

/* Model handles construct through func_02017acc and destroy through
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

extern "C" {
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File *file, int flags, int speed, u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);

extern KsWaterClsnFileHandle data_ov017_02111c80;
extern KsWaterModelFilePtr data_ov017_02111c88;
extern BtaDesc data_ov017_02111a60;
extern CLPS_Block data_ov017_02111a94;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjKsWater_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: ShipWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111484 loads 0x340 -- the class's own size -- into the header's leaf
 * operator new; 0x02111494 calls dBgActor_c's C2, the store at 0x021114a0
 * lays down this class's vptr, and TextureTransformer C1 runs on the member
 * at 0x320. The null check is the one `new` itself emits. The implicit
 * constructor is what keeps the intervening construction inlined; declaring
 * a constructor of our own would emit a `bl` the factory does not have. */
extern "C" daObjKsWater_c *daObjKsWater_c_classInit()
{
    return new daObjKsWater_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c13InitResourcesEv
int daObjKsWater_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov017_02111c88), 1, -1);
    /* SharedFilePtr is the 8-byte handle; +4 is the loaded BMD pointer. */
    TextureTransformer::Prepare(*(BMD_File *)data_ov017_02111c88.filePtr,
                                *(BTA_File *)&data_ov017_02111a60);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, (BTA_File *)&data_ov017_02111a60, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov017_02111c80),
        &mClsnMat, 0x1000, mAngleY, &data_ov017_02111a94);
    mMeshCollider.Enable(this);
    mOriginalPosY = mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c8BehaviorEv
int daObjKsWater_c::Behavior()
{
    if (mChestsOpen == 0) {
        int ok = 1;
        dActor_c *p = dActor_c::FindWithActorID(0xd, 0);
        while (p != 0) {
            int state = ((daObjTbox_c *)p)->mState;
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = dActor_c::FindWithActorID(0xd, p);
        }
        if (ok != 0) mChestsOpen = 1;
    } else {
        int d = mOriginalPosY - mPosY;
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, *(const Vector3 *)&mCamSpacePosX, 0);
            mPosY -= 0x5000;
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
// @symbol _ZN14daObjKsWater_c6RenderEv
int daObjKsWater_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c16CleanupResourcesEv
int daObjKsWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov017_02111c88.Release();
    data_ov017_02111c80.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_cD1Ev
// @symbol _ZN14daObjKsWater_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKsWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */

struct FourTagged {
    u32 tag0;
    u32 *slot0;
    u32 tag1;
    u32 *slot1;
    u32 tag2;
    u32 *slot2;
    u32 tag3;
    u32 *slot3;
};
typedef char FourTagged_size_must_be_0x20[sizeof(FourTagged) == 0x20 ? 1 : -1];

struct WordA50 {
    u32 word;
    u32 *rest;
};
struct WordA10 {
    u32 word;
    WordA50 *next;
};
struct NodeA24 {
    WordA10 *head;
    u32 word;
    u32 packed;
};
struct WordA08 {
    u32 word;
    FourTagged *rows;
};
struct WordA04 {
    u32 word;
    u32 *slot;
};
struct WordA58 {
    u32 word;
    u32 *rest;
};
struct BlockA78 {
    ClpsHeader *header;
    WordA08 *list;
    u32 packed0;
    u32 packed1;
    NodeA24 *node;
    u32 packed2;
    u32 tail;
};
typedef char BlockA78_size_must_be_0x1c[sizeof(BlockA78) == 0x1c ? 1 : -1];

extern "C" const u16 data_ov017_021114b8[2] = {0, 0};
extern "C" const s32 data_ov017_021114bc = 0x1000;
extern "C" const s32 data_ov017_021114dc[12] = {
    0, 14, 27, 41, 55, 68, 82, 96, 110, 123, 137, 151,
};
extern "C" char data_ov017_02111a18[12] = "water_mat";
extern "C" const BtaEntry data_ov017_021114c0 = {
    0xffff, 0, data_ov017_02111a18, 1, 0, 1, 0, 1, 0, 300, 0, 300, 0,
};

extern "C" u32 data_ov017_02111a00 = 0x765;
extern "C" u32 data_ov017_02111a04 = 4;
extern "C" u32 data_ov017_02111a30[4] = {0x0384ff02, 0xfc1803b6, 0x80000000, 0};
extern "C" u32 data_ov017_02111a40[3] = {0xba875701, 0xb3b02af8, 0xc0700001};
extern "C" u32 data_ov017_02111afc[6] = {
    0x1f, 0x0b5405c6, 0, 0x01010000, 0x124, 0xf862fea9,
};
extern "C" u32 data_ov017_02111b14[6] = {
    0, 0xff010000, 0xb9, 0x809, 0, 0xffff0000,
};
extern "C" u32 data_ov017_02111b2c[2] = {0x03840000, 0xfa880374};
extern "C" u32 data_ov017_02111b34[4] = {0x80000000, 0x00800000, 0x028a0000, 0xfa880374};
extern "C" u32 data_ov017_02111b44[8] = {
    0x80000000, 0x00800000, 0x047e0000, 0xfa880374,
    0x80000000, 0x00800000, 0x05460000, 0xfa880374,
};
extern "C" u32 data_ov017_02111b64 = 0x80000000;
extern "C" u32 data_ov017_02111b68 = 0x00800000;
extern "C" u32 data_ov017_02111b6c[2] = {0x423f, 0x0b5405c6};

extern "C" WordA50 data_ov017_02111a50 = {0x300, data_ov017_02111afc};
extern "C" WordA10 data_ov017_02111a10 = {2, &data_ov017_02111a50};
extern "C" NodeA24 data_ov017_02111a24 = {&data_ov017_02111a10, 0, 0x0c2b0000};
extern "C" FourTagged data_ov017_02111ad4 = {
    0x104, data_ov017_02111a30,
    0x10b, &data_ov017_02111a00,
    0x401, data_ov017_02111b2c,
    0x208, data_ov017_02111a40,
};
extern "C" WordA08 data_ov017_02111a08 = {5, &data_ov017_02111ad4};
extern "C" WordA04 data_ov017_02111af4 = {0x10e, &data_ov017_02111a04};
extern "C" u32 data_ov017_02111a4c = 0x94700000;
extern "C" WordA58 data_ov017_02111a58 = {0x905, data_ov017_02111b6c};
extern "C" ClpsHeader data_ov017_02111ab4 = {0x53504c43, 8, 2};
extern "C" CLPS data_ov017_02111abc[3] = {
    {0x00030fc0, 0x000000ff},
    {0x00030fc5, 0x000000ff},
    {0xffffffff, 0xffffffff},
};
extern "C" BlockA78 data_ov017_02111a78 = {
    &data_ov017_02111ab4,
    &data_ov017_02111a08,
    0x07640766,
    0x07680767,
    &data_ov017_02111a24,
    0x03200001,
    15,
};
extern "C" BtaDesc data_ov017_02111a60 = {
    300, 0, &data_ov017_021114bc, data_ov017_021114b8, data_ov017_021114dc,
    1, 0, &data_ov017_021114c0,
};
extern "C" CLPS_Block data_ov017_02111a94 = {
    0x53504c43, 8, 1,
    {{0x00000fe0, 0x000000ff}, {0xffffffff, 0xffffffff}, {0xffffffff, 0xffffffff}},
};

/* File-scope objects, in __sinit_d_a_obj_ks_water.cpp construction order:
 * the model's SharedFilePtr, then the collision file's. */
KsWaterModelFilePtr data_ov017_02111c88(ASSET_KS_MIZU_BMD);
KsWaterClsnFileHandle data_ov017_02111c80(ASSET_KS_MIZU_KCL);
