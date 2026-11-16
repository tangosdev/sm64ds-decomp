//cpp
/**
 * Wet-Dry World's submarine platform (WL_SUBMARINE).
 *
 * InitResources loads the model and collision files into the BSS
 * homes, sets them on the mesh collider, and enables it unless a
 * system state word vetoes; CleanupResources disables the collider
 * and releases both files; Render draws the model.
 *
 * Six functions, ov026 .text 0x021118b8..0x02111aa0: the D1/D0 pair,
 * the three virtual overrides, and the registry factory
 * daObjWlSubmarine_c_classInit last.
 *
 * NOTES:
 * - data_ov026_02112fd4 is the unnamed CLPS block this TU feeds SetFile.
 * - data_0209caa0.file.flags1: either of its unnamed bits
 *   0x204 keeps the collider disabled.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjWlSubmarine_c_AssetId {
    ASSET_WL_SUBMARINE_BMD = 0x06dc,
    ASSET_WL_SUBMARINE_KCL = 0x06dd
};

#include "daObjWlSubmarine_c.h"
#include "SharedFilePtr.h"
#include "SaveBlock.h"

/* BSS file homes (symbols.txt); Init loads them, Cleanup releases them.
 * The ctor/dtor are the ROM's SharedFilePtr veneer pairs (func_02017acc /
 * func_02017ab4 for the model, func_02017b4c / SharedFilePtr_Destruct_Clsn
 * for the collision), spelled through declared-only subclasses so the
 * static initializer names the real entry points. */
struct SubmarineModelFilePtr : SharedFilePtr {
    SubmarineModelFilePtr(u32 fileID);
    ~SubmarineModelFilePtr();
};
extern SubmarineModelFilePtr Submarine_ModelFile;
struct SubmarineCollisionFilePtr : SharedFilePtr {
    SubmarineCollisionFilePtr(u32 fileID);
    ~SubmarineCollisionFilePtr();
};
extern SubmarineCollisionFilePtr Submarine_ClsnFile;

extern CLPS_Block data_ov026_02112fd4;

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * dExtSimpleModel_c and dBgW_KcMbg, then dActor_c. */
daObjWlSubmarine_c::~daObjWlSubmarine_c() {}

s32 daObjWlSubmarine_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    Submarine_ModelFile.Release();
    Submarine_ClsnFile.Release();
    return 1;
}

s32 daObjWlSubmarine_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daObjWlSubmarine_c::InitResources() {
    BMD_File *m = dExtSimpleModel_c::LoadFile(Submarine_ModelFile);
    mModel.SetFile(m, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *k = dBgW_Kc::LoadFile(Submarine_ClsnFile);
        mMeshCollider.SetFile(k, mClsnMat, 0x199, mAngleY, data_ov026_02112fd4);
    }
    if (data_0209caa0.file.flags1 & 0x204)
        return 0;
    mMeshCollider.Enable(this);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjWlSubmarine_c through
 * RTTI, allocation size, vtable identity, and the WL_SUBMARINE registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: Submarine_Spawn. */
daObjWlSubmarine_c *daObjWlSubmarine_c_classInit() {
    return new daObjWlSubmarine_c();
}

/* The retail initializer constructs the model first, then the collision
 * file. */
SubmarineModelFilePtr Submarine_ModelFile(ASSET_WL_SUBMARINE_BMD);    /* model */
SubmarineCollisionFilePtr Submarine_ClsnFile(ASSET_WL_SUBMARINE_KCL); /* collision */
