//cpp
// @symbol _ZN10KingBobOmb16CleanupResourcesEv

#include "KingBobOmb.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov078_02126f38;
extern SharedFilePtr data_ov078_02126f00;
extern SharedFilePtr data_ov078_02126f20;
extern SharedFilePtr data_ov078_02126f10;
extern SharedFilePtr data_ov078_02126f08;
extern SharedFilePtr data_ov078_02126f18;
extern SharedFilePtr data_ov078_02126ee0;
extern SharedFilePtr data_ov078_02126ef0;
extern SharedFilePtr data_ov078_02126f40;
extern SharedFilePtr data_ov078_02126f30;
extern SharedFilePtr data_ov078_02126ee8;
extern SharedFilePtr data_ov078_02126f28;
extern SharedFilePtr data_ov078_02126ef8;
}

int KingBobOmb::CleanupResources()
{
    data_ov078_02126f38.Release();
    data_ov078_02126f00.Release();
    data_ov078_02126f20.Release();
    data_ov078_02126f10.Release();
    data_ov078_02126f08.Release();
    data_ov078_02126f18.Release();
    data_ov078_02126ee0.Release();
    data_ov078_02126ef0.Release();
    data_ov078_02126f40.Release();
    data_ov078_02126f30.Release();
    data_ov078_02126ee8.Release();
    data_ov078_02126f28.Release();
    data_ov078_02126ef8.Release();
    return 1;
}
