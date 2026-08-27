//cpp
// @symbol _ZN15daObjRcCarpet_c16CleanupResourcesEv
/* daObjRcCarpet_c::CleanupResources() -- vtable slot 3. Real C++ method over
 * the shared header. The dBgW at +0x124 is PathLift's, read by raw offset (the
 * header does not name it), and the SharedFilePtr globals are released. */
#include "daObjRcCarpet_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern int data_ov002_0210d9f0[];
extern void* data_ov036_02113f58[];
extern int data_ov036_0211419c[];
}
int daObjRcCarpet_c::CleanupResources(){
  if (((dBgW *)((char*)this+0x124))->IsEnabled())
    ((dBgW *)((char*)this+0x124))->Disable();
  ((SharedFilePtr *)(data_ov002_0210d9f0))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[1]))->Release();
  ((SharedFilePtr *)(data_ov036_0211419c))->Release();
  return 1;
}
