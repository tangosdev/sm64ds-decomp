//cpp
// @symbol _ZN14daObjC1Peach_c13InitResourcesEv
#include "daObjC1Peach_c.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov010_02112d64;

int daObjC1Peach_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov010_02112d64), 1, -1);
    mOpacity = 0xff;
    UpdateModelTransform();
    return 1;
}
