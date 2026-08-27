//cpp
// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha03_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern int data_ov065_0211d894[];
extern int data_ov065_0211d88c[];

int daObjCtMecha03_c::CleanupResources()
{
    if (((dBgW *)((char *)this + 0x124))->IsEnabled())
        ((dBgW *)((char *)this + 0x124))->Disable();
    ((SharedFilePtr *)data_ov065_0211d88c)->Release();
    ((SharedFilePtr *)data_ov065_0211d894)->Release();
    return 1;
}
