//cpp
// @symbol _ZN4Trap16CleanupResourcesEv
/* recovered: renamed to Class_Method, declarations from a shared header
 *
 * Trap::CleanupResources -- vtable slot 3, ov010 0x02111554. Attributed by
 * the vtable: _ZTV4Trap carries this address at slot 3, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043bf0.
 *
 * Kept as an extern "C" free function under the literal mangled name rather
 * than a real method -- same shape as _ZN4Trap13InitResourcesEv.c. */
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" int _ZN4Trap16CleanupResourcesEv(char *self){
  if(((dBgW *)(self+0x124))->IsEnabled()){
    ((dBgW *)(self+0x124))->Disable();
  }
  if((*(unsigned int*)(self+8) & 0xff) != 0xff){
    ((SharedFilePtr *)(&data_ov010_02112d08))->Release();
    ((SharedFilePtr *)(&data_ov010_02112d00))->Release();
  }
  return 1;
}
