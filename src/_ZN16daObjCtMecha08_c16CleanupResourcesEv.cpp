//cpp
// @symbol _ZN16daObjCtMecha08_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha08_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int TtcRotatingGear_ClsnFile[];
extern int TtcRotatingGear_ModelFile[];

int daObjCtMecha08_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(TtcRotatingGear_ModelFile))->Release();
    ((SharedFilePtr *)(TtcRotatingGear_ClsnFile))->Release();
    return 1;
}
