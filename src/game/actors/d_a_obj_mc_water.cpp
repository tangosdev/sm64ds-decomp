//cpp
/* ov009/daObjMcWater_c -- CASTLE_WATER (338), the castle water plane.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI names this class daObjMcWater_c; the debug table names CASTLE_WATER.
 * This is the water plane, not daObjMc_Metalnet_c / daMcFlag_c / Bird.
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
 * deslop leftovers:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - SharedFilePtr +4: InitResources' TextureTransformer::Prepare reads the
 *   BMD at data_ov009_02113c68 +4; SharedFilePtr.h has no fields.
 * - data_ov009_02113c68 / 02113c70 / 02112bc4 / 02112c38 model/KCL/BTA/CLPS
 *   handles and data_ov009_02113d8c / 02113de0 / 02113e34 spawn tables;
 *   this TU consumes them, overlay .bss/.data owns them.
 * - S14: g_profile_MC_WATER stays outside the licensed .text.
 * - func_ov009_02111b1c stays a C-linkage free function; the cartridge
 *   gives it a global symbol other config still names.
 * - no Player.h.
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

extern "C" {
void func_ov009_02111b1c(daObjMcWater_c *self);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    void *self, void *bta, int a, int fix, unsigned b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);

extern void *data_ov009_02113c68[];
extern SharedFilePtr data_ov009_02113c70;
extern BTA_File data_ov009_02112bc4;
extern CLPS_Block data_ov009_02112c38;
extern Vector3 data_ov009_02113d8c[];
extern Vector3 data_ov009_02113de0[];
extern Vector3 data_ov009_02113e34[];
extern unsigned char data_0209f2d8[];
extern int data_0209caa0[];
extern int data_0209f32c;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjMcWater_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: CastleWater_Spawn. */
extern "C" daObjMcWater_c *daObjMcWater_c_classInit()
{
    return new daObjMcWater_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c13InitResourcesEv
int daObjMcWater_c::InitResources()
{
    int b = (int)(data_0209f2d8[0] == 1);
    if (b == 0) {
        if (data_0209caa0[2] & 0x80000) {
            mPosY = -0x2bc000;
        }
    }
    {
        BMD_File *bmd = (BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov009_02113c68);
        mModel.SetFile(bmd, 1, 0x14);
    }
    TextureTransformer::Prepare(*(BMD_File *)data_ov009_02113c68[1], data_ov009_02112bc4);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTexTransformer, &data_ov009_02112bc4, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov009_02113c70);
        /* 6az: header SetFile takes Fix12<int> by value. */
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov009_02112c38);
    }
    mMeshCollider.Enable(this);
    {
        int v = mPosY - 0x64000;
        if (data_0209f32c > v) data_0209f32c = v;
    }
    func_ov009_02111b1c(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c8BehaviorEv
int daObjMcWater_c::Behavior()
{
    mTexTransformer.speed = 0x1000;
    mTexTransformer.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c6RenderEv
int daObjMcWater_c::Render()
{
    mTexTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjMcWater_c16CleanupResourcesEv
int daObjMcWater_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov009_02113c68)->Release();
    data_ov009_02113c70.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov009_02111b1c
extern "C" void func_ov009_02111b1c(daObjMcWater_c *self)
{
    Vector3 *vec;
    int i;
    int b = (int)(data_0209f2d8[0] == 1);
    if (b != 0) {
        if (self->mPosX > 0) return;
        vec = data_ov009_02113de0;
    } else {
        vec = (data_0209caa0[2] & 0x80000) ? data_ov009_02113e34 : data_ov009_02113d8c;
    }
    for (i = 0; i < 7; i++) {
        dActor_c::Spawn(0xc5, 0, vec[i], 0, self->mAreaId, -1);
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
