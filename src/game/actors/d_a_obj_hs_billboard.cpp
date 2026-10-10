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
 * deslop leftovers:
 * - func_ov031_02111214 (yaw mModel.mat4x3, drop translation to pos >> 3):
 *   manifest ordinal 2 of this TU, no other src/ consumer; kept as a
 *   C-linkage ROM label. Member/static form unmeasured.
 * - data_ov031_02111424 is the extern four-variant SharedFilePtr table
 *   InitResources indexes by mVariant (this TU's manifest is text-only);
 *   the individual BMD link names stay data_ov031_*.
 * - Matrix4x3_FromRotationY stays a TU-local extern: no header declares it.
 * - The actorID switch stays defaultless in matched form; the registry
 *   only spawns the four HS_* profiles (0x12e-0x131).
 * - pad_0d0 stays: untouched anywhere in this TU (Model opens at 0x0d4).
 */

#include "daObjHsBillboard_c.h"
#include "SharedFilePtr.h"
#include "BMD_File.h"

/* Four model handles. Each constructs through func_02017acc and destroys
 * through func_02017ab4. The four-pointer table stays ROM rodata. */
struct HsBillboardModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    HsBillboardModelFilePtr(unsigned int fileId);
    ~HsBillboardModelFilePtr();
};
typedef char HsBillboardModelFilePtr_size_must_be_8[
    sizeof(HsBillboardModelFilePtr) == 8 ? 1 : -1];

extern "C" {
extern SharedFilePtr *data_ov031_02111424[];
void func_ov031_02111214(daObjHsBillboard_c *self);
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angY);
}

/* Four C-linkage factories: one `new`, four names. The EAD classInit spelling
 * would collide, so each profile keeps its HS_* suffix. */
// @symbol daObjHsBillboard_c_classInit_HS_MOON
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_MOON()
{
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_STAR()
{
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_Y_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_Y_STAR()
{
    return new daObjHsBillboard_c();
}

// @symbol daObjHsBillboard_c_classInit_HS_B_STAR
extern "C" daObjHsBillboard_c *daObjHsBillboard_c_classInit_HS_B_STAR()
{
    return new daObjHsBillboard_c();
}

// @symbol _ZN18daObjHsBillboard_c13InitResourcesEv
int daObjHsBillboard_c::InitResources()
{
    switch (actorID) {
    case 0x12e: mVariant = 0; break; /* HS_MOON */
    case 0x12f: mVariant = 1; break; /* HS_STAR */
    case 0x130: mVariant = 2; break; /* HS_Y_STAR */
    case 0x131: mVariant = 3; break; /* HS_B_STAR */
    }

    BMD_File *file = (BMD_File *)Model::LoadFile(*data_ov031_02111424[mVariant]);
    mModel.SetFile(file, 1, -1);
    func_ov031_02111214(this);
    return 1;
}

// @symbol _ZN18daObjHsBillboard_c6RenderEv
int daObjHsBillboard_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjHsBillboard_c16CleanupResourcesEv
int daObjHsBillboard_c::CleanupResources()
{
    data_ov031_02111424[mVariant]->Release();
    return 1;
}

/* File-local: yaw the model and drop its translation to position >> 3. */
// @symbol func_ov031_02111214
extern "C" void func_ov031_02111214(daObjHsBillboard_c *self)
{
    Matrix4x3_FromRotationY(&self->mModel.mat4x3, self->mAngleY);
    self->mModel.mat4x3.t.x = self->mPosX >> 3;
    self->mModel.mat4x3.t.y = self->mPosY >> 3;
    self->mModel.mat4x3.t.z = self->mPosZ >> 3;
}
// @symbol _ZN18daObjHsBillboard_cD1Ev
// @symbol _ZN18daObjHsBillboard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjHsBillboard_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Written out of
   line here instead they come out D0-before-D1 and the isolation step rejects
   the object.

   Both bodies are short because the chain is short: this class's vptr store,
   then the owned Model at 0xd4, then dActor_c's own teardown, which is where
   the actor-list unlink lives. D0's trailing deallocation is the inherited
   inline `operator delete`, which is why nothing here names a heap. */

/* Retail construction order. mwcc emits __sinit_d_a_obj_hs_billboard.cpp. */
HsBillboardModelFilePtr data_ov031_02111a08(1565);
HsBillboardModelFilePtr data_ov031_02111a00(1566);
HsBillboardModelFilePtr data_ov031_02111a10(1569);
HsBillboardModelFilePtr data_ov031_02111a18(1564);
