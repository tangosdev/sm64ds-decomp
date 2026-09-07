//cpp
// @symbol _ZN9LightBeam13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LightBeam.h"
#include "SharedFilePtr.h"
extern "C" {
extern unsigned char NumStars(void);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *, void *, void *, int, int, unsigned int, unsigned int);
}
extern int data_0209caa0[];

int LightBeam::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)data_ov010_02112d50);
    if (NumStars() < 0xe) return 0;
    if (data_0209caa0[1] & 0x80000000) return 0;
    mModel.SetFile((BMD_File *)data_ov010_02112d50[1], 1, 0x13);
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylinder, this, &mPosX, 0xf0000, 0x8c000, 0x4800002, 0);
    func_ov010_02111a54((char *)this);
    return 1;
}
