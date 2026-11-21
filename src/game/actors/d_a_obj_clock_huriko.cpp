//cpp
/* daObjClockHuriko_c -- the swinging pendulum inside the clock painting.
 * ov013/daObjClockHuriko_c, one genuine translation unit, 8 functions,
 * .text 0x021111a0..0x021113bc.
 *
 * ov013 is the clock painting overlay (not TTC / ov065): CLOCK_PAINTING_PENDULUM
 * (294) is this class; CLOCK_PAINTING_HAND_SHORT (292) and
 * CLOCK_PAINTING_HAND_LONG (293) share the sibling daObjClock_c.
 *
 * THE CLASS NAME COMES OUT OF THE CARTRIDGE. The vtable header at ov013
 * 0x02112120 is {0, 0x021120e0}. That _ZTI is three words --
 * {0x0209a764, 0x021120ec, 0x0208e390} -- whose first word is
 * __si_class_type_info's vtable+8; whose second points at the length-prefixed
 * mangled string "18daObjClockHuriko_c"; and whose third names the DIRECT BASE,
 * 0x0208e390 = _ZTI8dActor_c.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 * The destructor is the documented exception and is not written here at all:
 * it is inline in include/daObjClockHuriko_c.h so that D1/D0 come out in ROM
 * order with no homeless D2.
 *
 * Members, in ROM address order:
 *   [0] 0x021111a0  daObjClockHuriko_c::~daObjClockHuriko_c  (D1)
 *   [1] 0x021111d0  daObjClockHuriko_c::~daObjClockHuriko_c  (D0)
 *   [2] 0x02111214  daObjClockHuriko_c::CleanupResources
 *   [3] 0x02111238  daObjClockHuriko_c::UpdateModelTransform
 *   [4] 0x02111280  daObjClockHuriko_c::Render
 *   [5] 0x021112a8  daObjClockHuriko_c::Behavior
 *   [6] 0x0211133c  daObjClockHuriko_c::InitResources
 *   [7] 0x02111384  daObjClockHuriko_c_classInit  (factory, actor 294)
 *
 * Notes:
 * - data_ov013_02112280 is the pendulum model's SharedFilePtr; this TU's
 *   static initializer constructs it. symbols.txt has no recovered name.
 * - Matrix4x3_FromRotationZXYExt stays a local C extern; no shared header
 *   declares it.
 * - UpdateModelTransform is the same model-matrix rebuild as the sibling
 *   daObjClock_c::UpdateModelTransform (ov013 0x02111430).
 * - g_profile_CLOCK_HURIKO is this file's profile, next to the class typeinfo.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_clock_huriko_AssetId { ASSET_C2_HURIKO_BMD = 0x05b3 };

#include "daObjClockHuriko_c.h"
#include "SharedFilePtr.h"
#include "SpawnInfo.h"
#include "Sound.h"
#include "math/MtxStack.h"

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4. The manifest aliases those undefined members onto the ROM
 * symbols. */
struct ClockHurikoModelFilePtr : SharedFilePtr {
    ClockHurikoModelFilePtr(u32 fileID);
    ~ClockHurikoModelFilePtr();
};

extern signed char gNextSublevel;
extern ClockHurikoModelFilePtr data_ov013_02112280;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjClockHuriko_c through RTTI,
 * allocation size, vtable identity, and the CLOCK_HURIKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjClockHuriko_c_Spawn. */
daObjClockHuriko_c *daObjClockHuriko_c_classInit() {
    return new daObjClockHuriko_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// recovered name: daObjClockHuriko_c::InitResources
/* daObjClockHuriko_c::InitResources -- vtable slot 0, ov013 0x0211133c.
 * A REAL MEMBER ON PURPOSE: the destructor is inline in the header, so this
 * class's key function is its first declared non-inline virtual. */
s32 daObjClockHuriko_c::InitResources() {
    BMD_File *f = dExtSimpleModel_c::LoadFile(data_ov013_02112280);
    mModel.SetFile(f, 1, -1);
    UpdateModelTransform();
    mAngSpeed = 0x100;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* daObjClockHuriko_c::Behavior -- vtable slot 6, ov013 0x021112a8. The
 * pendulum swings while gNextSublevel is not positive: the swing
 * speed reverses at each end of the arc, the angle integrates it, and the tick
 * sound plays as the speed passes +-0x10.
 *
 * The swing angle is dActor_c::mAngle.z. */
s32 daObjClockHuriko_c::Behavior() {
    if (gNextSublevel <= 0) {
        if (mAngle.z > 0) {
            mAngSpeed -= 8;
        } else {
            mAngSpeed += 8;
        }
        mAngle.z += mAngSpeed;
        s16 speed = mAngSpeed;
        if (speed == 0x10 || speed == -0x10) {
            Sound::PlayBank3(0x16, mCamSpacePos);
        }
    }
    UpdateModelTransform();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* daObjClockHuriko_c::Render -- vtable slot 9, ov013 0x02111280. */
s32 daObjClockHuriko_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Rebuilds mModel.mat4x3 from the actor angles and from position >> 3.
 * The marker above is not decoration -- without it tools/tiers.py folds this
 * body into the preceding member's fragment. */
void daObjClockHuriko_c::UpdateModelTransform() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* daObjClockHuriko_c::CleanupResources -- vtable slot 3, ov013 0x02111214.
 * Releases the one shared file the painting holds; it never touches `this`. */
s32 daObjClockHuriko_c::CleanupResources() {
    data_ov013_02112280.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Both destructors are emitted from the INLINE `~daObjClockHuriko_c() {}` in
 * include/daObjClockHuriko_c.h -- there is deliberately no body here.
 *
 * Defining ~daObjClockHuriko_c() out of line HERE instead would break the TU
 * two ways: mwccarm would emit D0 at 0x021111d0's slot before D1 at
 * 0x021111a0's, reversing ROM order so objisolate refuses the entire TU, and
 * it would emit a third symbol, D2, which has no address anywhere in the
 * cartridge. See the vtable comment in the header.
 */

/* File-scope object: the pendulum model's SharedFilePtr, the one
 * construction __sinit_d_a_obj_clock_huriko.cpp performs. */
ClockHurikoModelFilePtr data_ov013_02112280(ASSET_C2_HURIKO_BMD);

/* Pendulum animation tables and payload, stored in .data ahead of the
 * class RTTI. Every relocated word names the symbol it loads. */

extern u32 data_ov013_02111784;
extern u32 data_ov013_02111794;
extern u32 data_ov013_021117bc;
extern u32 data_ov013_021117cc;
extern u32 data_ov013_021117d4;
extern u32 data_ov013_021117dc;
extern u32 data_ov013_0211178c;
extern u32 data_ov013_0211179c;
extern u32 data_ov013_021117a4;
extern u32 data_ov013_021117ac;
extern u32 data_ov013_021117b4;
extern u32 data_ov013_021117c4;
extern u32 data_ov013_021117e4;
extern u32 data_ov013_021117ec;
extern u32 data_ov013_021117f8;
extern u32 data_ov013_02111804;
extern u32 data_ov013_02111814;
extern u32 data_ov013_02111824;
extern u32 data_ov013_02111834;
extern u32 data_ov013_02111860;
extern u32 data_ov013_02111890;
extern u32 data_ov013_021118d0;
extern u32 data_ov013_02111918;
extern u32 data_ov013_0211196c;
extern u32 data_ov013_021119cc;
extern u32 data_ov013_02111a3c;
extern u32 data_ov013_02111ae4;
extern u32 data_ov013_02111b94;
extern u32 data_ov013_02111c54;
extern u32 data_ov013_02111d28;
extern u32 data_ov013_02111e18;
extern u32 data_ov013_02111f18;

/* 0x02111780..0x021120e0: the pendulum animation tables and payload, one
 * object ahead of the class RTTI. The four zero bytes at 0x02111928 are
 * the alignment pad between the two table groups. Relocated words name
 * the symbol they load. */

u32 data_ov013_02111780[600] = {
    0x00000004,
    0x00000002,
    (u32)&data_ov013_02111804,
    0x00000400,
    (u32)&data_ov013_02111890,
    0x00000002,
    (u32)&data_ov013_02111824,
    0x00000105,
    (u32)&data_ov013_021117c4,
    0x00000009,
    (u32)&data_ov013_021118d0,
    0x1db60a3d,
    0xf76805c3,
    0x00000105,
    (u32)&data_ov013_021117ac,
    0x00000001,
    (u32)&data_ov013_0211178c,
    0xff39ff44,
    0xfa160c1c,
    0x00000001,
    (u32)&data_ov013_021117e4,
    0x00000001,
    (u32)&data_ov013_0211179c,
    0x00000001,
    (u32)&data_ov013_021117b4,
    0x00001000,
    (u32)&data_ov013_02111e18,
    0x06f506f4,
    0x06f706f6,
    0x06f906f8,
    0x08980898,
    0x08980898,
    0x08980898,
    0x00000b00,
    (u32)&data_ov013_02111ae4,
    0x00000205,
    (u32)&data_ov013_02111834,
    0xfbaf483f,
    0x0a2908ca,
    0xfa58e83c,
    0x07a10a11,
    0x00000700,
    (u32)&data_ov013_021119cc,
    0x00000205,
    (u32)&data_ov013_02111814,
    0xfb3f463f,
    0xfad504c9,
    0xfcc8e63c,
    0xfd8105d8,
    (u32)&data_ov013_02111b94,
    (u32)&data_ov013_021117a4,
    0x06f306fa,
    0x06fc06fb,
    (u32)&data_ov013_02111918,
    0x06400007,
    0x0000000f,
    0xff100000,
    0x0010ffff,
    0x7fa5ff02,
    0xff020012,
    0x001480fe,
    0xffffff0f,
    0xff070023,
    0x002affff,
    0xffffff0b,
    0xff0d0035,
    0x0042ffff,
    0xffffff0a,
    0xedb5002a,
    0xe3d105c3,
    0x00000000,
    0x0cff0000,
    0xedb5002a,
    0xfbd70523,
    0x80000000,
    0x0c330000,
    0xe462002a,
    0xf7590564,
    0x40000000,
    0x4c770000,
    0xe57000ed,
    0xf89404fb,
    0x40000000,
    0x24000000,
    0x00004c02,
    (u32)&data_ov013_02111f18,
    0x00000803,
    (u32)&data_ov013_02111860,
    0x00000f04,
    (u32)&data_ov013_02111c54,
    0x0000060b,
    (u32)&data_ov013_021117ec,
    0x00000f01,
    (u32)&data_ov013_02111d28,
    0x00000809,
    (u32)&data_ov013_0211196c,
    0x0000060c,
    (u32)&data_ov013_021117f8,
    0x0000010e,
    (u32)&data_ov013_02111780,
    0x00000c0a,
    (u32)&data_ov013_02111a3c,
    0x00000000,
    0x00000000,
    0x06ef0100,
    (u32)&data_ov013_02111784,
    0x00000000,
    0x0b9f0301,
    (u32)&data_ov013_021117bc,
    0x00000000,
    0x0c8f0202,
    (u32)&data_ov013_021117cc,
    0x00000000,
    0x08e30203,
    (u32)&data_ov013_02111794,
    0x00000000,
    0x11300304,
    (u32)&data_ov013_021117d4,
    0x00000000,
    0x16440105,
    (u32)&data_ov013_021117dc,
    0x00000000,
    0x08e30203,
    0x01f4fcae,
    0x8000fbe6,
    0x00070000,
    0x01f4fb82,
    0x0000fbe6,
    0x00070000,
    0x0497fbe6,
    0x0000f4de,
    0x000f0100,
    0x0497f5b7,
    0x4000f7a4,
    0x00021200,
    0x0497065e,
    0xc000f70e,
    0x00021300,
    0x0898ff3b,
    0x8000028a,
    0x000b1400,
    0x0c1cff39,
    0x0000fecd,
    0x000c4500,
    0x04971aae,
    0x4000f7a4,
    0x00016300,
    0xff3800a9,
    0x0bb809f6,
    0x00000000,
    0xffff0000,
    0xff3800a8,
    0x0bf40b22,
    0x80000000,
    0xffff0000,
    0xff3800a7,
    0x0bf40b22,
    0x80000000,
    0xffff0000,
    0xff38002a,
    0x0bf70a5a,
    0x80000000,
    0x0d330000,
    0xf9c00050,
    0x0bb80898,
    0x8000ff04,
    0x0af90000,
    0x08b400ed,
    0x06d70b54,
    0xc0000000,
    0x26040000,
    0xff3800ed,
    0x03840898,
    0x00000000,
    0x25060000,
    0x05c3edb5,
    0x0000e3d1,
    0x00180000,
    0xfd7cff07,
    0xefd504fb,
    0x00000000,
    0x55040015,
    0x0514fd6d,
    0x0000feec,
    0x00168000,
    0x0d7f2205,
    0xef5705dc,
    0x00000000,
    0x44060013,
    0x0523edb5,
    0x0000fbd7,
    0x00198000,
    0xff383307,
    0x0bf60a5a,
    0x80000000,
    0x4408001b,
    0x1298ff1f,
    0xc000e3fb,
    0x00270000,
    0xfbb41109,
    0xfc4a01f4,
    0x80000000,
    0x20ff0b02,
    0x0af0f2ea,
    0xc000073a,
    0x001c0000,
    0x0b74110a,
    0x073a0afa,
    0x0000c000,
    0x110c0022,
    0x060e138d,
    0x0000ff5b,
    0x00308000,
    0xfc7c330b,
    0xfc4a01f4,
    0x80000000,
    0x20ff1002,
    0xfd7c002a,
    0xefd504fb,
    0x00000000,
    0x09550000,
    0xfd6d002a,
    0xfeec0514,
    0x80000000,
    0x0a220000,
    0xfa1d002a,
    0xfe180561,
    0x6aaa0000,
    0x41330000,
    0x00e2002a,
    0xfe0f055f,
    0x95560000,
    0x43330000,
    0x042b002a,
    0xfab904fb,
    0xaaab0000,
    0x40550000,
    0xfc7c0050,
    0xfa2404b0,
    0x0000ff03,
    0x0af70000,
    0x03200050,
    0xfbb40497,
    0xaaabffff,
    0x0af40000,
    0x00a0004b,
    0xf9110514,
    0x4000ffff,
    0x07100000,
    0xfce000ed,
    0xf1280497,
    0x00000000,
    0x06030000,
    0x01f400ed,
    0xf3800497,
    0xa0000000,
    0x15050000,
    0xf6ba002a,
    0xfab2052d,
    0x55550000,
    0x42230000,
    0x53504c43,
    0x00150008,
    0x04000fc0,
    0x000000ff,
    0x00000fc0,
    0x000000ff,
    0x00040380,
    0x000000ff,
    0x00040fc1,
    0x000000ff,
    0x00040fc4,
    0x000000ff,
    0x00042fc1,
    0x000000ff,
    0x00942fc1,
    0x000000ff,
    0x01058fc4,
    0x000000ff,
    0x00058fc4,
    0x000000ff,
    0x01040fc5,
    0x000000ff,
    0x01043fc5,
    0x000000ff,
    0x00040fc5,
    0x000000ff,
    0x01040304,
    0x000000ff,
    0x00040301,
    0x000000ff,
    0x00040304,
    0x000000ff,
    0x00042301,
    0x000000ff,
    0x008c0fc4,
    0x000000ff,
    0x01040fc4,
    0x000000ff,
    0x00030fc1,
    0x000000ff,
    0x00000fc1,
    0x000000ff,
    0x000401c1,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xfc18ff02,
    0xfb8202bc,
    0x80000000,
    0xff020000,
    0x05dcfc18,
    0x0000f380,
    0x00007555,
    0x0064ff02,
    0xfd120578,
    0xcaab0000,
    0xff020000,
    0x060e0ce4,
    0x0000f380,
    0x00008000,
    0xefbcff02,
    0xf7680578,
    0xc0000000,
    0xff020000,
    0x092e0032,
    0x000007d0,
    0x0000f556,
    0xfea2ff02,
    0xe8f413ec,
    0x78e30000,
    0xff050000,
    0x03b6fb1e,
    0x0000f704,
    0x00000000,
    0xff380106,
    0xfa150e74,
    0x80000000,
    0xff020000,
    0x07080dde,
    0x0000fc18,
    0x00000000,
    0xf15aff02,
    0x079e0c1c,
    0x471c0000,
    0xff020000,
    0x0c1c0d16,
    0x0000079e,
    0x0000b8e4,
    0x0e2e0206,
    0xf7590753,
    0x40000000,
    0xff020000,
    0x055f03f4,
    0x0000f759,
    0x00004000,
    0x2269ff04,
    0xf92305e0,
    0xb8e40000,
    0x00000000,
    0xfbe60000,
    0xfa5601f4,
    0x80000000,
    0x07000000,
    0xfc7c0000,
    0xf18c04b0,
    0x00000000,
    0x00090000,
    0xfda80000,
    0xf18c04b0,
    0x00000000,
    0x00090000,
    0xfed40000,
    0xf18c04b0,
    0x00000000,
    0x00090000,
    0xfd7c0000,
    0xefd50622,
    0x00000000,
    0x05890000,
    0xfd760000,
    0xff0605aa,
    0x80000000,
    0x05910000,
    0x0d7a0000,
    0xef5206b0,
    0x00000000,
    0x059b0000,
    0xede80000,
    0xf7670834,
    0x40000000,
    0x02220000,
    0xff380000,
    0x0bea0b22,
    0x80000000,
    0x05ac0000,
    0xff1f0000,
    0xe39a12c0,
    0x00000000,
    0x04350000,
    0xf2ea0000,
    0x06d60abe,
    0x40000000,
    0x04540000,
    0x0d7f0000,
    0xff5b06bd,
    0x80000000,
    0x05cb0000,
    0x0b860000,
    0x06d60abe,
    0xc0000000,
    0x045c0000,
    0xfc4a0000,
    0xfa5601f4,
    0x80000000,
    0x07000000,
    0x05b70000,
    0xf7590497,
    0xc0000000,
    0x07690000,
    0x0d7f002a,
    0xef57055f,
    0x00000000,
    0x08660000,
    0x06a4002a,
    0xf7590627,
    0x40000000,
    0x41220000,
    0x0a8c002a,
    0xfc3b05c8,
    0x40000000,
    0x4c330000,
    0x1680002a,
    0xfc3b05c8,
    0xc0000000,
    0x6c330000,
    0x1a68002a,
    0xf7590627,
    0xc0000000,
    0x61220000,
    0x1680002a,
    0xf2770659,
    0xc0000000,
    0x60220000,
    0x1388002a,
    0xef7005dc,
    0x00000000,
    0x63440000,
    0x0a8c002a,
    0xf2770659,
    0x40000000,
    0x40220000,
    0x190900f4,
    0xf7540677,
    0xbd280000,
    0x00ff0000,
    0x0c70001a,
    0xf5670627,
    0x00000000,
    0xffff0000,
    0x0e61001a,
    0xfae40627,
    0x00000000,
    0xffff0000,
    0x14a5001a,
    0xf56b0627,
    0x80000000,
    0xffff0000,
    0x12ae001a,
    0xfade0627,
    0x80000000,
    0xffff0000,
    0x138d002a,
    0xff5b060e,
    0x80000000,
    0x10330000,
    0x0d7f002a,
    0xff38060e,
    0x80000000,
    0x4f330000,
    0x0e1000ed,
    0xf128055f,
    0x00000000,
    0x02070000,
    0x04fbe570,
    0xe570f63c,
    0xf89404fb,
    0x04fbe7c8,
    0xea20f894,
    0xf63c04fb,
    0x052decdc,
    0xeda4f7cc,
    0xfb5004fb,
    0x04fbee08,
    0xee9efb50,
    0xf7cc052d,
    0x0497f47a,
    0xf47af7cc,
    0xf7040497,
    0x052dee9e,
    0xf060f704,
    0xe4a804fb,
    0x04fbeae8,
    0xed40e4a8,
    0xf704052d,
    0x04fbea20,
    0xe7c8f894,
    0xf63c04fb,
    0x0ed8ff38,
    0xff38fa15,
    0xec3615e0,
    0x07530e2e,
    0x0e2ef52e,
    0xf9b60753,
    0x0497f8f8,
    0xfce0f380,
    0xf1280497,
    0x0497ff9c,
    0xfed4f704,
    0xfed40898,
    0x08980064,
    0xff2f00c8,
    0x01900898,
    0x0898fe0c,
    0xff9c00c8,
    0xfed40898,
    0x0497fed4,
    0xff9cf704,
    0xf4480497,
    0x049702bc,
    0x02bcf6a0,
    0xfaec0497,
    0x0497fd44,
    0xf8f8fda8,
    0xfbb40497,
    0x0497f704,
    0x0a5af768,
    0x06400b54,
    0x0b5408b9,
    0x0a5806d7,
    0x07400b54,
    0x0b540aba,
    0x0c650806,
    0x08060b54,
    0x0b540c65,
    0x0abf05aa,
    0x05a40b54,
    0x0497ff2c,
    0x01ebf261,
    0xf3b60497,
    0x04970320,
    0x0514f6a0,
    0xf7680497,
    0x04970320,
    0x0320f95c,
    0xfaec0497,
    0x04970190,
    0x00c8fc18,
    0xfaec0497,
    0x049701f4,
    0x04b0fa88,
    0xf7680497,
    0x04970190,
    0xfb50f6a0,
    0x05dc0898,
    0x0898ff38,
    0x03200384,
    0x05dc0898,
    0x089806a4,
    0x044c06a4,
    0x08340898,
    0x0898044c,
    0x012c0a8c,
    0x0a280898,
    0x0898fe0c,
    0xff380898,
    0x05780898,
    0x08980064,
    0xfd440898,
    0x0a280898,
    0x0898f9c0,
    0xf7680834,
    0x06a40898,
    0x055f0bb8,
    0x0e10f380,
    0xf128055f,
    0x055f0e10,
    0x0c80f3e4,
    0xf6a0055f,
    0x055f0e74,
    0x0c1cf7cc,
    0xfa88055f,
    0x055f0e74,
    0x0f3cfd44,
    0xfaec055f,
    0x055f0a28,
    0x0e74f768,
    0xf574055f,
};

extern "C" SpawnInfo<daObjClockHuriko_c> g_profile_CLOCK_HURIKO = {daObjClockHuriko_c_classInit,
                                                                   294,
                                                                   164,
                                                                   0x00000006,
                                                                   0x00064000,
                                                                   0x000fa000,
                                                                   0x00c80000,
                                                                   0x00640000};
