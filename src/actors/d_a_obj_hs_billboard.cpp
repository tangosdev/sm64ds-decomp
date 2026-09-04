//cpp
/* ov031/daObjHsBillboard_c -- the slide's four decoration billboards.
 *
 * PRODUCTION translation unit. It owns every function the ROM placed between
 * 0x021111a0 and 0x02111344, which is the whole class plus one file-local
 * helper. The four spawn factories above it (0x02111344..0x02111424) and the
 * static initialiser below it stay where they are -- they are not members and
 * the ROM did not group them with the class.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm 2004/b56 emits one
 * .text section per function in the reverse of source order, so the
 * highest-address ROM function is written first.
 *
 * THE DESTRUCTOR IS DEFINED INLINE IN THE HEADER, declared first, which makes
 * this TU the home of _ZTV18daObjHsBillboard_c. Both ROM bodies are empty; an
 * out-of-line body would emit D0 ahead of D1 and add the D2 variant the
 * cartridge never carried.
 */

#include "daObjHsBillboard_c.h"
#include "SharedFilePtr.h"
#include "BMD_File.h"

extern "C" {

/* The four SharedFilePtrs the static initialiser at 0x02111434 fills in with
 * file ids 1564/1565/1566/1569, in actor-id order. */
extern SharedFilePtr *data_ov031_02111424[];

void func_ov031_02111214(daObjHsBillboard_c *self);

void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angY);

}

/* ROM 0x021112a8, size 0x9c */
// @symbol _ZN18daObjHsBillboard_c13InitResourcesEv
int daObjHsBillboard_c::InitResources()
{
    switch (actorID) {
    case 0x12e: mVariant = 0; break;
    case 0x12f: mVariant = 1; break;
    case 0x130: mVariant = 2; break;
    case 0x131: mVariant = 3; break;
    }

    BMD_File *file = (BMD_File *)Model::LoadFile(*data_ov031_02111424[mVariant]);
    mModel.SetFile(file, 1, -1);
    func_ov031_02111214(this);
    return 1;
}

/* ROM 0x02111280, size 0x28 */
// @symbol _ZN18daObjHsBillboard_c6RenderEv
int daObjHsBillboard_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM 0x02111254, size 0x2c */
// @symbol _ZN18daObjHsBillboard_c16CleanupResourcesEv
int daObjHsBillboard_c::CleanupResources()
{
    data_ov031_02111424[mVariant]->Release();
    return 1;
}

/* ROM 0x02111214, size 0x40 -- file-local, no mangled name in the cartridge.
 * Points the model at the actor's yaw and drops its translation to the
 * actor's position scaled down by 8. */
// @symbol func_ov031_02111214
extern "C" void func_ov031_02111214(daObjHsBillboard_c *self)
{
    Matrix4x3_FromRotationY(&self->mModel.mat4x3, self->mAngleY);
    self->mModel.mat4x3.t.x = self->mPosX >> 3;
    self->mModel.mat4x3.t.y = self->mPosY >> 3;
    self->mModel.mat4x3.t.z = self->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN18daObjHsBillboard_cD0Ev, 0x021111d0, size 0x44        */
/* ROM ordinal 0 -- _ZN18daObjHsBillboard_cD1Ev, 0x021111a0, size 0x30        */
/* -------------------------------------------------------------------------- */
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
