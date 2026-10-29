//cpp
/* Production translation unit for ov102/daObjSimpleBg_c.
 * 12 function(s), .text 0x02148a80..0x02148fbc. One mesh and one moving
 * collider, shared by the six profiles named in the header. BK_TOWER (actor
 * 0x31) in sublevel 7 does not spawn above y 0xdac000 until star 1 is
 * collected, unless the level was entered for that star.
 *
 * NAME: _ZTS15daObjSimpleBg_c is "15daObjSimpleBg_c" at ov102 0x0214e0cc;
 * _ZTI at 0x0214e0c0 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class FortressTower
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02148a80), D0
 * (0x02148ac4), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * The six daObjSimpleBg_c_classInit_* factories (0x02148e9c..0x02148fbc) are
 * reconstructed names (RTTI daObjSimpleBg_c, the six registry profiles above);
 * retail does not store them. Historical aliases: RopeBarrier_Spawn
 * (MC_HAZAD), CgStairs_Spawn (MC_DODAI), WarpPipe_Spawn (DOKAN),
 * FortressTower_Spawn (BK_TOWER), RockTriangle_Spawn (KI_HASIRA_DAI),
 * StaticRock_Spawn (KI_IWA). Each factory is `operator new(0x320)` + the
 * inherited dBgActor_c ctor + this class's vtable store -- exactly
 * `new daObjSimpleBg_c()`, since the class has no user-declared constructor
 * of its own. They append here, after InitResources, in ROM order, following
 * the same '#pragma defer_codegen off' source-order emission as the rest of
 * this file (compare src/game/actors/d_a_obj_hatena_block.cpp immediately downstream
 * at 0x02148fbc, which has no such pragma and so emits in the reverse of
 * source order -- its classInit block sits at the top of its file instead).
 *
 * What stays:
 * - The 0xc-stride file rows are local: only this TU indexes those tables.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSimpleBg_c_AssetId {
    ASSET_OBJ_DOKAN_BMD = 0x0461,
    ASSET_OBJ_DOKAN_KCL = 0x0462,
    ASSET_BK_TOWER_BMD = 0x059d,
    ASSET_BK_TOWER_KCL = 0x059e,
    ASSET_KI_HASIRA_DAI_BMD = 0x063f,
    ASSET_KI_HASIRA_DAI_KCL = 0x0640,
    ASSET_KI_IWA_BMD = 0x0643,
    ASSET_KI_IWA_KCL = 0x0644,
    ASSET_MC_DODAI_BMD = 0x068b,
    ASSET_MC_DODAI_KCL = 0x068c,
    ASSET_MC_HAZAD_BMD = 0x068f,
    ASSET_MC_HAZAD_KCL = 0x0690
};

#include "decl_common.h"
#include "daObjSimpleBg_c.h"
#include "SharedFilePtr.h"

/* Six model handles construct through func_02017acc and destroy through
 * func_02017ab4. Six collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The file tables above these
 * objects stay ROM data. */
struct SimpleBgModelFilePtr : SharedFilePtr {
    SimpleBgModelFilePtr(unsigned int fileId);
    ~SimpleBgModelFilePtr();
};
struct SimpleBgClsnFilePtr : SharedFilePtr {
    SimpleBgClsnFilePtr(unsigned int fileId);
    ~SimpleBgClsnFilePtr();
};
typedef char SimpleBgModelFilePtr_size_must_be_8[sizeof(SimpleBgModelFilePtr) == 8 ? 1 : -1];
typedef char SimpleBgClsnFilePtr_size_must_be_8[sizeof(SimpleBgClsnFilePtr) == 8 ? 1 : -1];

struct FileRow {
    SharedFilePtr *file;
    u8 pad[8];
};
typedef char FileRow_size_must_be_0xc[sizeof(FileRow) == 0xc ? 1 : -1];

struct ClpsRow {
    CLPS_Block *block;
    u8 pad[8];
};
typedef char ClpsRow_size_must_be_0xc[sizeof(ClpsRow) == 0xc ? 1 : -1];

extern FileRow data_ov102_0214e188[];
extern FileRow data_ov102_0214e18c[];
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8[];

/* local extern: no shared header declares it. */
extern "C" int IsStarCollectedInCurLevel(int starID);

/* Actor ID 0x31 is BK_TOWER; data_0209f2d8[0] is CURRENT_GAMEMODE
 * (symbols/verified.tsv). These int-returning predicates are what make the
 * ROM materialize 0/1 before testing it. */
static inline int IsBkTower(dActor_c *actor) {
    return actor->actorID == 0x31;
}

static inline int IsGameMode(int mode) {
    return data_0209f2d8[0] == mode;
}

/* ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN15daObjSimpleBg_cD1Ev
// @symbol _ZN15daObjSimpleBg_cD0Ev
daObjSimpleBg_c::~daObjSimpleBg_c() {}

// @symbol _ZN15daObjSimpleBg_c16CleanupResourcesEv
int daObjSimpleBg_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov102_0214e188[mVariant].file->Release();
    data_ov102_0214e18c[mVariant].file->Release();
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c6RenderEv
int daObjSimpleBg_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c8BehaviorEv
int daObjSimpleBg_c::Behavior() {
    int range = 0;
    switch (actorID) {
    case 0x31:
        range = 0x900000;
        break;
    case 0x3b:
        range = 0x600000;
        break;
    case 0x3d:
        range = 0x600000;
        break;
    case 0x12a:
        range = 0x600000;
        break;
    case 0x154:
        range = 0x300000;
        break;
    case 0x155:
        range = 0x300000;
        break;
    }
    if (IsGameMode(1)) {
        if (!mMeshCollider.IsEnabled())
            mMeshCollider.Enable(this);
    } else {
        IsClsnInRangeOnScreen(range, 0);
    }
    return 1;
}

// @symbol _ZN15daObjSimpleBg_c13InitResourcesEv
int daObjSimpleBg_c::InitResources() {
    u16 id = actorID;
    u8 idx;

    switch (id) {
    case 0x31:
        mVariant = 2;
        break;
    case 0x3b:
        mVariant = 1;
        break;
    case 0x3d:
        mVariant = 0;
        break;
    case 0x12a:
        mVariant = 3;
        break;
    case 0x154:
        mVariant = 4;
        break;
    case 0x155:
        mVariant = 5;
        break;
    }

    idx = mVariant;
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov102_0214e188[idx].file), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    idx = mVariant;
    if (idx == 0 || idx == 2) {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov102_0214e18c[idx].file);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngleY,
                              *((ClpsRow *)data_ov102_0214e190)[idx].block);
    } else {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov102_0214e18c[idx].file);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY,
                              *((ClpsRow *)data_ov102_0214e190)[idx].block);
    }

    if (IsBkTower(this) && data_0209f2f8 == 7) {
        if (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0) {
            if (mPosY >= 0xdac000)
                return 0;
        }
    }
    return 1;
}

// @symbol daObjSimpleBg_c_classInit_MC_HAZAD
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_MC_HAZAD() {
    return new daObjSimpleBg_c();
}

// @symbol daObjSimpleBg_c_classInit_MC_DODAI
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_MC_DODAI() {
    return new daObjSimpleBg_c();
}

// @symbol daObjSimpleBg_c_classInit_DOKAN
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_DOKAN() {
    return new daObjSimpleBg_c();
}

// @symbol daObjSimpleBg_c_classInit_BK_TOWER
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_BK_TOWER() {
    return new daObjSimpleBg_c();
}

// @symbol daObjSimpleBg_c_classInit_KI_HASIRA_DAI
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_KI_HASIRA_DAI() {
    return new daObjSimpleBg_c();
}

// @symbol daObjSimpleBg_c_classInit_KI_IWA
extern "C" daObjSimpleBg_c *daObjSimpleBg_c_classInit_KI_IWA() {
    return new daObjSimpleBg_c();
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_simple_bg.cpp from these. */
SimpleBgModelFilePtr gKiIwaModelFile(ASSET_KI_IWA_BMD);
SimpleBgModelFilePtr gKiHasiraDaiModelFile(ASSET_KI_HASIRA_DAI_BMD);
SimpleBgModelFilePtr gBkTowerModelFile(ASSET_BK_TOWER_BMD);
SimpleBgModelFilePtr gdaObjSimpleBgObjDokanModelFile(ASSET_OBJ_DOKAN_BMD);
SimpleBgModelFilePtr gMcDodaiModelFile(ASSET_MC_DODAI_BMD);
SimpleBgModelFilePtr gMcHazadModelFile(ASSET_MC_HAZAD_BMD);
SimpleBgClsnFilePtr gKiIwaCollisionFile(ASSET_KI_IWA_KCL);
SimpleBgClsnFilePtr gKiHasiraDaiCollisionFile(ASSET_KI_HASIRA_DAI_KCL);
SimpleBgClsnFilePtr gBkTowerCollisionFile(ASSET_BK_TOWER_KCL);
SimpleBgClsnFilePtr gObjDokanCollisionFile(ASSET_OBJ_DOKAN_KCL);
SimpleBgClsnFilePtr gMcDodaiCollisionFile(ASSET_MC_DODAI_KCL);
SimpleBgClsnFilePtr gMcHazadCollisionFile(ASSET_MC_HAZAD_KCL);
