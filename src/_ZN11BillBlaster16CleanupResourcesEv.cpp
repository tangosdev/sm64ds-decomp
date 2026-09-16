//cpp
#include "dBgW.h"
#include "SharedFilePtr.h"
// @symbol _ZN11BillBlaster16CleanupResourcesEv
#include "BillBlaster.h"
// recovered name: daObjBkKillerdai_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjBkKillerdai_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void* data_ov079_02127f64[];
extern void* data_ov079_02128300;
extern void* data_ov079_021282f0;
}

s32 BillBlaster::CleanupResources() {
    char * t = (char *)this;
  if(((dBgW *)(t+0x124))->IsEnabled())
    ((dBgW *)(t+0x124))->Disable();
  ((SharedFilePtr *)data_ov079_02127f64[0])->Release();
  ((SharedFilePtr *)data_ov079_02127f64[1])->Release();
  ((SharedFilePtr *)&data_ov079_02128300)->Release();
  ((SharedFilePtr *)&data_ov079_021282f0)->Release();
  return 1;
}
