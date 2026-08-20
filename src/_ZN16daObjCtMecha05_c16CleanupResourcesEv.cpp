//cpp
// @symbol _ZN16daObjCtMecha05_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha05_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* The ROM's own bss symbol names for this class's model/collision files really
   are spelled TTC_MovingBar_ModelFile / TTC_MovingBar_ClsnFile -- not a naming
   error carried over from the neighbouring class, just the shared data this
   overlay's un-migrated func_ov065_0211ad04 already referenced. Left as-is. */
extern int TTC_MovingBar_ModelFile[];
extern int TTC_MovingBar_ClsnFile[];

int daObjCtMecha05_c::CleanupResources()
{
    if (((dBgW *)((char *)this + 0x124))->IsEnabled())
        ((dBgW *)((char *)this + 0x124))->Disable();
    ((SharedFilePtr *)TTC_MovingBar_ModelFile)->Release();
    ((SharedFilePtr *)TTC_MovingBar_ClsnFile)->Release();
    return 1;
}
