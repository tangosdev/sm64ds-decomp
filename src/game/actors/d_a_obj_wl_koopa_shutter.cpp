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
 * - data_0209caa0.file.flags1: either of its unnamed bits
 *   0x204 keeps the collider disabled.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjWlKoopaShutter_c_AssetId {
    ASSET_WL_KUPA_SHUTTER_BMD = 0x06d9,
    ASSET_WL_KUPA_SHUTTER_KCL = 0x06da
};

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

extern CLPS_Block data_ov026_02112f74;

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
        mMeshCollider.SetFile(k, mClsnMat, 0x199, mAngleY, data_ov026_02112f74);
    }
    if (data_0209caa0.file.flags1 & 0x204)
        return 0;
    mMeshCollider.Enable(this);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjWlKoopaShutter_c through
 * RTTI, allocation size, vtable identity, and the WL_KOOPA_SHUTTER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: BowserShutter_Spawn. */
extern "C" daObjWlKoopaShutter_c *daObjWlKoopaShutter_c_classInit() {
    return new daObjWlKoopaShutter_c();
}

/* The retail initializer constructs the model first, then the collision
 * file. */
KoopaShutterModelFilePtr BowserShutter_ModelFile(ASSET_WL_KUPA_SHUTTER_BMD);    /* model */
KoopaShutterCollisionFilePtr BowserShutter_ClsnFile(ASSET_WL_KUPA_SHUTTER_KCL); /* collision */
