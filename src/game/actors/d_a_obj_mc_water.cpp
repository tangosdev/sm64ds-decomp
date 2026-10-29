//cpp
/* ov009/daObjMcWater_c -- CASTLE_WATER (338), the castle water plane.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI names this class daObjMcWater_c; the debug table names CASTLE_WATER.
 * This is the water plane, not daObjMc_Metalnet_c / daMcFlag_c / daSBird_c.
 *
 * This one file is the production source for the whole class. It licenses
 * the contiguous .text run 0x02111a70..0x02111dc4 in ov009
 * (config/tu_manifest.d/ov009/daObjMcWater_c.json).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST. The inline destructor in include/daObjMcWater_c.h emits
 * the retail D1/D0 pair first and emits no D2 body.
 *
 * Notes:
 * - data_ov009_02112bc4 / 02112c38 BTA/CLPS blocks: this TU consumes
 *   them, overlay .data owns them. The model/KCL handles and the three
 *   spawn tables are defined here.
 * - g_profile_MC_WATER stays outside the licensed .text.
 *
 * Absorbed these legacy one-function sources (ROM address order), all deleted
 * by tools/tu_promote.py:
 *   [0] 0x02111a70  src/_ZN14daObjMcWater_cD1Ev.cpp
 *   [1] 0x02111abc  src/_ZN14daObjMcWater_cD0Ev.cpp
 *   [2] 0x02111b1c  src/func_ov009_02111b1c.cpp
 *   [3] 0x02111bd4  src/_ZN14daObjMcWater_c16CleanupResourcesEv.cpp
 *   [4] 0x02111c18  src/_ZN14daObjMcWater_c6RenderEv.cpp
 *   [5] 0x02111c4c  src/_ZN14daObjMcWater_c8BehaviorEv.cpp
 *   [6] 0x02111c74  src/_ZN14daObjMcWater_c13InitResourcesEv.cpp
 *   [7] 0x02111d8c  src/daObjMcWater_c_classInit.c
 */

#include "common.h"
#include "daObjMcWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct BMD_File;
struct BTA_File;
struct KCL_File;
struct CLPS_Block;

extern BTA_File data_ov009_02112bc4;
extern CLPS_Block data_ov009_02112c38;
extern unsigned char data_0209f2d8[];
extern int data_0209caa0[];
extern int data_0209f32c;

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daObjMcWater_c_AssetId { ASSET_MC_WATER_BMD = 0x0693, ASSET_MC_WATER_KCL = 0x0694 };

/* The handles reach their files through these wrappers' constructor and
   destructor veneers; the manifest maps each to the retail destination.
   Descriptive local types, not recovered spellings. */
struct McWaterModelFile : SharedFilePtr {
    McWaterModelFile(u32 fileID);
    ~McWaterModelFile();
};

struct McWaterCollisionFile : SharedFilePtr {
    McWaterCollisionFile(u32 fileID);
    ~McWaterCollisionFile();
};

/* This TU's file handles and spawn tables, in the order the retail
   initializer (0x02112524) builds them. */
McWaterModelFile gMcWaterModelFile(ASSET_MC_WATER_BMD);
McWaterCollisionFile gMcWaterCollisionFile(ASSET_MC_WATER_KCL);

/* SpawnWaterfalls spawns actor 0xc5 at each of the seven positions in one
   of these tables. Its default table: */
Vector3 data_ov009_02113d8c[7] = {
    Vector3(-0x16b2000, -0x5a000, -0x9f6000),  Vector3(-0x15e0000, -0x5a000, -0xad2000),
    Vector3(-0x1540000, -0x5a000, -0xbc2000),  Vector3(-0x14aa000, -0x5a000, -0xcbc000),
    Vector3(-0x1450000, -0x5a000, -0xdca000),  Vector3(-0x141e000, -0x5a000, -0xeec000),
    Vector3(-0x141e000, -0x5a000, -0x100e000),
};

/* ... when bit 0x80000 of data_0209caa0[2] is set: */
Vector3 data_ov009_02113e34[7] = {
    Vector3(-0x152c000, -0x320000, -0x97e000), Vector3(-0x1450000, -0x320000, -0xa46000),
    Vector3(-0x13a6000, -0x320000, -0xb2c000), Vector3(-0x1324000, -0x320000, -0xc30000),
    Vector3(-0x12ac000, -0x320000, -0xd34000), Vector3(-0x12ac000, -0x320000, -0xda2000),
    Vector3(-0x1298000, -0x320000, -0xe56000),
};

/* ... and when data_0209f2d8[0] is 1: */
Vector3 data_ov009_02113de0[7] = {
    Vector3(-0x14d2000, -0x42e000, -0x960000), Vector3(-0x140a000, -0x42e000, -0xa32000),
    Vector3(-0x136a000, -0x42e000, -0xb18000), Vector3(-0x12f2000, -0x42e000, -0xc12000),
    Vector3(-0x1284000, -0x42e000, -0xd0c000), Vector3(-0x1266000, -0x42e000, -0xe1a000),
    Vector3(-0x128e000, -0x42e000, -0xf32000),
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjMcWater_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: CastleWater_Spawn. */
extern "C" daObjMcWater_c *daObjMcWater_c_classInit() {
    return new daObjMcWater_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c13InitResourcesEv
int daObjMcWater_c::InitResources() {
    int b = (int)(data_0209f2d8[0] == 1);
    if (b == 0) {
        if (data_0209caa0[2] & 0x80000) {
            mPosY = -0x2bc000;
        }
    }
    {
        BMD_File *bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(gMcWaterModelFile);
        mModel.SetFile(bmd, 1, 0x14);
    }
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)gMcWaterModelFile.filePtr, data_ov009_02112bc4);
    mTexTransformer.SetFile(data_ov009_02112bc4, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(gMcWaterCollisionFile);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov009_02112c38);
    }
    mMeshCollider.Enable(this);
    {
        int v = mPosY - 0x64000;
        if (data_0209f32c > v)
            data_0209f32c = v;
    }
    SpawnWaterfalls();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c8BehaviorEv
int daObjMcWater_c::Behavior() {
    mTexTransformer.speed = 0x1000;
    mTexTransformer.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c6RenderEv
int daObjMcWater_c::Render() {
    mTexTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c16CleanupResourcesEv
int daObjMcWater_c::CleanupResources() {
    gMcWaterModelFile.Release();
    gMcWaterCollisionFile.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c15SpawnWaterfallsEv
void daObjMcWater_c::SpawnWaterfalls() {
    Vector3 *vec;
    int i;
    int b = (int)(data_0209f2d8[0] == 1);
    if (b != 0) {
        if (mPosX > 0)
            return;
        vec = data_ov009_02113de0;
    } else {
        vec = (data_0209caa0[2] & 0x80000) ? data_ov009_02113e34 : data_ov009_02113d8c;
    }
    for (i = 0; i < 7; i++) {
        dActor_c::Spawn(0xc5, 0, vec[i], 0, mAreaId, -1);
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_cD1Ev
// @symbol _ZN14daObjMcWater_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjMcWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */
