//cpp
// @symbol _ZN11daTBasket_c13InitResourcesEv
/* Vtable slot 0. Loads the shared cage model (module file 0x6c9, which daTrs_c
 * loads too), builds the 100-unit collision cylinder and the mesh collider,
 * and zeroes the particle/sound tail. */

#include "daTBasket_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov063_0211edec;

/* dCcAc_c::Init / dBgCh_Actr::Init stay scalar: the header member forms take
 * Fix12<int> BY VALUE but the tree's Fix12 has no int constructor (measured:
 * neither a bare 0x64000 nor Fix12<int>(0x64000) converts), so only the scalar
 * spelling reproduces the ROM's raw-word passing. dBgCh_Actr::Init's header
 * additionally takes Fix12i, which mangles as i while the ROM is Fix12<int>.
 * da1up_c calls them in this same spelling. */
extern "C" {
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor, Fix12i radius, Fix12i height, unsigned int flags, unsigned int vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *actor, Fix12i radius, Fix12i height, Vector3_16 *a, Vector3_16 *b);
}

int daTBasket_c::InitResources()
{
    BMD_File *bmd = (BMD_File *)Model::LoadFile(data_ov063_0211edec);
    if (mModel.SetFile(bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0x64000, 0x200004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mParticleID = 0;
    mSoundTimer = 0;
    mMuteSecretSound = 0;
    return 1;
}
