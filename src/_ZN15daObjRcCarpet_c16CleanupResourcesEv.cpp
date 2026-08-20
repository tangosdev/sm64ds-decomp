//cpp
// @symbol _ZN15daObjRcCarpet_c16CleanupResourcesEv
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern int data_ov002_0210d9f0[];
extern void* data_ov036_02113f58[];
extern int data_ov036_0211419c[];
int _ZN15daObjRcCarpet_c16CleanupResourcesEv(char* c){
  if (((dBgW *)(c+0x124))->IsEnabled())
    ((dBgW *)(c+0x124))->Disable();
  ((SharedFilePtr *)(data_ov002_0210d9f0))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[1]))->Release();
  ((SharedFilePtr *)(data_ov036_0211419c))->Release();
  return 1;
}
}
