//cpp
// @symbol _ZN25SlideDecorationSilverStar13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlideDecorationSilverStar.h"
#include "SharedFilePtr.h"

extern "C" void func_ov031_02111214(SlideDecorationSilverStar *);
extern "C" SharedFilePtr *data_ov031_02111424[];

int SlideDecorationSilverStar::InitResources()
{
    switch (actorID) {
    case 0x12e: mVariant = 0; break;
    case 0x12f: mVariant = 1; break;
    case 0x130: mVariant = 2; break;
    case 0x131: mVariant = 3; break;
    }

    BMD_File *file = (BMD_File *)Model::LoadFile(
        *data_ov031_02111424[mVariant]);
    mModel.SetFile(file, 1, -1);
    func_ov031_02111214(this);
    return 1;
}
