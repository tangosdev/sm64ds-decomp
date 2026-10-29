//cpp
/* ov031/daObjHsBillboard_c -- the slide's four decoration billboards.
 *
 * One class, four profiles (HS_MOON / HS_STAR / HS_Y_STAR / HS_B_STAR).
 * .text 0x021111a0..0x02111424: D1/D0, a file-local helper, Cleanup/Render/
 * InitResources, then the four factories.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * HS_MOON factory first. The inline destructor emits the retail D1/D0 pair
 * first and emits no leaf D2 body.
 *
 * Notes:
 * - data_ov031_02111424 is the extern four-variant SharedFilePtr table
 *   InitResources indexes by mVariant (this TU's manifest is text-only);
 *   the individual BMD link names stay data_ov031_*.
 * - The actorID switch stays defaultless in matched form; the registry
 *   only spawns the four HS_* profiles (0x12e-0x131).
 * - pad_0d0 stays: untouched anywhere in this TU (dExtSimpleModel_c opens at 0x0d4).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_hs_billboard_AssetId {
    ASSET_HS_B_STAR_BMD = 0x061c,
    ASSET_HS_MOON_BMD = 0x061d,
    ASSET_HS_STAR_BMD = 0x061e,
    ASSET_HS_Y_STAR_BMD = 0x0621
};

#include "daObjHsBillboard_c.h"
#include "SharedFilePtr.h"
#include "BMD_File.h"
#include "math/MtxStack.h"

/* Four model handles. Each constructs through func_02017acc and destroys
 * through func_02017ab4. The four-pointer table stays ROM rodata. */
struct HsBillboardModelFilePtr : SharedFilePtr {
    HsBillboardModelFilePtr(unsigned int fileId);
    ~HsBillboardModelFilePtr();
};
typedef char HsBillboardModelFilePtr_size_must_be_8[sizeof(HsBillboardModelFilePtr) == 8 ? 1 : -1];

extern SharedFilePtr *data_ov031_02111424[];

/* Four C-linkage factories: one `new`, four names. The EAD classInit spelling
 * would collide, so each profile keeps its HS_* suffix. */
// @symbol daObjHsBillboard_c_classInit_HS_MOON
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_MOON() {
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_STAR() {
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_Y_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_Y_STAR() {
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_B_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_B_STAR() {
    return new daObjHsBillboard_c();
}

// @symbol _ZN18daObjHsBillboard_c13InitResourcesEv
int daObjHsBillboard_c::InitResources() {
    switch (actorID) {
    case 0x12e:
        mVariant = 0;
        break; /* HS_MOON */
    case 0x12f:
        mVariant = 1;
        break; /* HS_STAR */
    case 0x130:
        mVariant = 2;
        break; /* HS_Y_STAR */
    case 0x131:
        mVariant = 3;
        break; /* HS_B_STAR */
    }

    BMD_File *file = (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov031_02111424[mVariant]);
    mModel.SetFile(file, 1, -1);
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN18daObjHsBillboard_c6RenderEv
int daObjHsBillboard_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjHsBillboard_c16CleanupResourcesEv
int daObjHsBillboard_c::CleanupResources() {
    data_ov031_02111424[mVariant]->Release();
    return 1;
}

/* Yaw the model and set its translation to the position >> 3. */
// @symbol _ZN18daObjHsBillboard_c20UpdateModelTransformEv
void daObjHsBillboard_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
// @symbol _ZN18daObjHsBillboard_cD1Ev
// @symbol _ZN18daObjHsBillboard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjHsBillboard_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Written out of
   line here instead they come out D0-before-D1 and the isolation step rejects
   the object.

   Both bodies are short because the chain is short: this class's vptr store,
   then the owned dExtSimpleModel_c at 0xd4, then dActor_c's own teardown, which is where
   the actor-list unlink lives. D0's trailing deallocation is the inherited
   inline `operator delete`, which is why nothing here names a heap. */

/* Retail construction order. mwcc emits __sinit_d_a_obj_hs_billboard.cpp. */
HsBillboardModelFilePtr gHsMoonModelFile(ASSET_HS_MOON_BMD);
HsBillboardModelFilePtr gHsStarModelFile(ASSET_HS_STAR_BMD);
HsBillboardModelFilePtr gHsYStarModelFile(ASSET_HS_Y_STAR_BMD);
HsBillboardModelFilePtr gHsBStarModelFile(ASSET_HS_B_STAR_BMD);
