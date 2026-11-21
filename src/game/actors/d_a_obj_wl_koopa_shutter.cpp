//cpp
/**
 * Wet-Dry World's Koopa shutter (WL_KOOPA_SHUTTER).
 *
 * InitResources loads the model and collision files into the BSS
 * homes, sets them on the mesh collider, and enables it unless a
 * system state word vetoes; CleanupResources disables the collider
 * and releases both files; Render draws the model.
 *
 * Seven functions, ov026 .text 0x021116c8..0x021118b8: the D1/D0 pair,
 * the four virtual overrides, and the registry factory
 * daObjWlKoopaShutter_c_classInit last.
 *
 * NOTES:
 * - data_ov026_02112f74 is the unnamed CLPS block this TU feeds SetFile.
 * - gSaveBlock.file.flags1: either of its unnamed bits
 *   0x204 keeps the collider disabled.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjWlKoopaShutter_c_AssetId {
    ASSET_WL_KUPA_SHUTTER_BMD = 0x06d9,
    ASSET_WL_KUPA_SHUTTER_KCL = 0x06da
};

#include "SpawnInfo.h"
#include "daObjWlKoopaShutter_c.h"
#include "SharedFilePtr.h"
#include "SaveBlock.h"

/* BSS file homes (symbols.txt); Init loads them, Cleanup releases them.
 * The ctor/dtor are the ROM's SharedFilePtr veneer pairs (func_02017acc /
 * func_02017ab4 for the model, func_02017b4c / SharedFilePtr_Destruct_Clsn
 * for the collision), spelled through declared-only subclasses so the
 * static initializer names the real entry points. */
struct KoopaShutterModelFilePtr : SharedFilePtr {
    KoopaShutterModelFilePtr(u32 fileID);
    ~KoopaShutterModelFilePtr();
};
extern KoopaShutterModelFilePtr BowserShutter_ModelFile;
struct KoopaShutterCollisionFilePtr : SharedFilePtr {
    KoopaShutterCollisionFilePtr(u32 fileID);
    ~KoopaShutterCollisionFilePtr();
};
extern KoopaShutterCollisionFilePtr BowserShutter_ClsnFile;

struct CLPS_Block {
    u32 words[2];
};

CLPS_Block data_ov026_02112f74 = {
    0x53504c43,
    0x00010008,
};

/* The pointer table that follows the CLPS in this overlay's .data. */
extern u32 data_ov026_02112e60[];
extern u32 data_ov026_02112e70[];
extern u32 data_ov026_02112e90[];
extern u32 data_ov026_02112eb0[];
extern u32 data_ov026_02112ec0[];
extern u32 data_ov026_02112ed0[];
extern u32 data_ov026_021132b0[];
extern u32 data_ov026_02113600[];

u32 data_ov026_02112f7c[] = {
    0x00000fc0, 0x000000ff,
    0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff,
    0x00001a00, (u32)data_ov026_021132b0,
    0x00003805, (u32)data_ov026_02113600,
    0x00000140, (u32)data_ov026_02112e90,
    0x00000160, (u32)data_ov026_02112ed0,
    0x00000180, (u32)data_ov026_02112eb0,
    0x000001a0, (u32)data_ov026_02112ec0,
    0x000001c0, (u32)data_ov026_02112e60,
    0x000001e0, (u32)data_ov026_02112e70,
};

/* Tables that pointer block names. */
u32 data_ov026_021132b0[] = {
    0xe37c00a6, 0x012cfb1e, 0x00000000, 0xffff0000, 0xe3ae00a6, 0x00c8ff6a, 0x00000000, 0xffff0000,
    0xde680124, 0x060ef16e, 0x80000000, 0xff010000, 0xe444004e, 0xf9c0f16e, 0x00000000, 0xffff0000,
    0xde250107, 0x0411f16e, 0x40000000, 0xffff0000, 0xe37c004e, 0x0866f16e, 0x00000000, 0xffff0000,
    0xe9580124, 0x04b0f16e, 0x00000000, 0xff020000, 0xe7000124, 0x012bf369, 0x40000000, 0x02030000,
    0xdd030055, 0x03a3012c, 0x80000000, 0x00000000, 0xe3570055, 0xfa67f448, 0x00000000, 0x00090000,
    0xde250031, 0x0411f18c, 0x00000000, 0x00150000, 0xde250031, 0x0411f18c, 0x00000000, 0x00160000,
    0xe3a4013e, 0x015ef2e0, 0x00000000, 0xffff0000, 0xeb39001f, 0xf96d0672, 0xeaab0000, 0xff030000,
    0xed01002c, 0xf7520578, 0xeaab0000, 0x14050000, 0xed01011c, 0xf7520578, 0xeaab00b6, 0xffff0000,
    0xea66004e, 0x04b4f16e, 0x00000000, 0xffff0000, 0xe909004e, 0x0420f16e, 0x00000000, 0xffff0000,
    0xea3f004e, 0x03edf16e, 0x00000000, 0xffff0000, 0xdc4900aa, 0x038504de, 0x2eee0000, 0xffff0000,
    0xe35e00aa, 0x0b8104de, 0x93340000, 0xffff0000, 0xdfd700aa, 0x04ca04de, 0x80000000, 0xffff0000,
    0xd92300aa, 0x052604de, 0x56c10000, 0xffff0000, 0xe2e300aa, 0x079e04de, 0x38e30000, 0xffff0000,
    0xea50011c, 0xf70b0578, 0xeaab0000, 0xffff0000, 0xedf3011c, 0xfa210578, 0xeaab0000, 0xffff0000};

u32 data_ov026_02113600[] = {
    0xe4760a36, 0x0226fda8, 0xe2821436, 0x015efa24, 0xe8c2e225, 0x00faf60a, 0xe8c2e225, 0x0032f5d8,
    0xe8c2e225, 0x0000f510, 0xe8c2e225, 0x0032f448, 0xe8c2e225, 0x00faf416, 0xe8c2e225, 0x01c2f448,
    0xe8c2e225, 0x01f4f510, 0xe8c2e225, 0x01c2f5d8, 0xebb0e225, 0x00faf60a, 0xebb0e225, 0x0032f5d8,
    0xebb0e225, 0x0000f510, 0xebb0e225, 0x0032f448, 0xebb0e225, 0x00faf416, 0xebb0e225, 0x01c2f448,
    0xebb0e225, 0x01f4f510, 0xebb0e225, 0x01c2f5d8, 0xf254e225, 0x00faf60a, 0xf254e225, 0x0032f5d8,
    0xf254e225, 0x0000f510, 0xf254e225, 0x0032f448, 0xf254e225, 0x00faf416, 0xf254e225, 0x01c2f448,
    0xf254e225, 0x01f4f510, 0xf254e225, 0x01c2f5d8, 0xde49e225, 0x0538f286, 0xde49e225, 0x0538f31c,
    0xde49e225, 0x0538f3b2, 0xde49e225, 0x0538f448, 0xde49e225, 0x0538f4de, 0xe188e225, 0x0352f3ed,
    0xe188e225, 0x0352f483, 0xe188e225, 0x0352f519, 0xe188e225, 0x0352f5af, 0xe188e225, 0x0352f645,
    0xeb1ae225, 0x041af16e, 0xea20e225, 0x05dcf16e, 0xe728e225, 0x07a3f16e, 0xe95f0a36, 0x04b4f286,
    0xe4440a36, 0x0064f63c, 0xe890443f, 0xfbb4f510, 0xe3ae4e3f, 0x015ef7cc, 0xdc74e225, 0x06d6fe70,
    0xdc10e225, 0x073afed4, 0xdbace225, 0x079eff6a, 0xdb48e225, 0x08020000, 0xdae4e225, 0x08660096,
    0xdcceca3f, 0x0b220672, 0xe3afed43, 0x015df340, 0xe8b9e225, 0x05d8f16e, 0xe64ce225, 0x0811f16e,
    0xe805e225, 0x0719f16e, 0xe731e225, 0x0084f56d, 0xe732e225, 0x0084f3de, 0xe732e225, 0x0084f4a7};

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * dExtSimpleModel_c and dBgW_KcMbg, then dActor_c. */
daObjWlKoopaShutter_c::~daObjWlKoopaShutter_c() {}

s32 daObjWlKoopaShutter_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    BowserShutter_ModelFile.Release();
    BowserShutter_ClsnFile.Release();
    return 1;
}

s32 daObjWlKoopaShutter_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daObjWlKoopaShutter_c::Behavior() {
    return 1;
}

s32 daObjWlKoopaShutter_c::InitResources() {
    BMD_File *m = dExtSimpleModel_c::LoadFile(BowserShutter_ModelFile);
    mModel.SetFile(m, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *k = dBgW_Kc::LoadFile(BowserShutter_ClsnFile);
        mMeshCollider.SetFile(k, mClsnMat, 0x199, mAngle.y, data_ov026_02112f74);
    }
    if (gSaveBlock.file.flags1 & 0x204)
        return 0;
    mMeshCollider.Enable(this);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjWlKoopaShutter_c through
 * RTTI, allocation size, vtable identity, and the WL_KOOPA_SHUTTER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: BowserShutter_Spawn. */
daObjWlKoopaShutter_c *daObjWlKoopaShutter_c_classInit() {
    return new daObjWlKoopaShutter_c();
}

/* The retail initializer constructs the model first, then the collision
 * file. */
KoopaShutterModelFilePtr BowserShutter_ModelFile(ASSET_WL_KUPA_SHUTTER_BMD);    /* model */
KoopaShutterCollisionFilePtr BowserShutter_ClsnFile(ASSET_WL_KUPA_SHUTTER_KCL); /* collision */

extern "C" SpawnInfo<daObjWlKoopaShutter_c> g_profile_WL_KOOPA_SHUTTER = {
    daObjWlKoopaShutter_c_classInit,
    91,
    252,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000};
