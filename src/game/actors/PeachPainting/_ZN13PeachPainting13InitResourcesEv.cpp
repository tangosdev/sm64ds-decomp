//cpp
// @symbol _ZN13PeachPainting13InitResourcesEv
#include "PeachPainting.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov010_02112d64;

int PeachPainting::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov010_02112d64), 1, -1);
    mOpacity = 0xff;
    UpdateModelTransform();
    return 1;
}
