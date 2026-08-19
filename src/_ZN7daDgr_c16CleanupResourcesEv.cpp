//cpp
// @symbol _ZN7daDgr_c16CleanupResourcesEv
/* daDgr_c::CleanupResources -- vtable slot 3. Extern "C" free function under
   the mangled name; see src/_ZN7daDgr_c13InitResourcesEv.cpp for why it is not
   converted to a true method body. */
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern int data_ov025_02113a68[];
extern int data_ov025_02113a60[];
int _ZN7daDgr_c16CleanupResourcesEv(char* c) {
  ((SharedFilePtr *)(data_ov025_02113a68))->Release();
  ((SharedFilePtr *)(data_ov025_02113a60))->Release();
  if (((dBgW *)(c+0x124))->IsEnabled())
    ((dBgW *)(c+0x124))->Disable();
  return 1;
}
}
