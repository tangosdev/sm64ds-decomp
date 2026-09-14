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
 */

#include "daObjHsBillboard_c.h"
#include "SharedFilePtr.h"
#include "BMD_File.h"

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
