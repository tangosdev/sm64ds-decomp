//cpp
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
extern int data_ov002_0210d9f0[];
extern void* data_ov036_02113f58[];
extern int data_ov036_0211419c[];
int func_ov036_02112318(char* c){
  if (((MeshColliderBase *)(c+0x124))->IsEnabled())
    ((MeshColliderBase *)(c+0x124))->Disable();
  ((SharedFilePtr *)(data_ov002_0210d9f0))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[1]))->Release();
  ((SharedFilePtr *)(data_ov036_0211419c))->Release();
  return 1;
}
}
