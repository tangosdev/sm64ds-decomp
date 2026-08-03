//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern void _ZN16MeshColliderBase7DisableEv(void* m);
extern int data_ov002_0210d9f0[];
extern void* data_ov036_02113f58[];
extern int data_ov036_0211419c[];
int func_ov036_02112318(char* c){
  if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  ((SharedFilePtr *)(data_ov002_0210d9f0))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113f58[1]))->Release();
  ((SharedFilePtr *)(data_ov036_0211419c))->Release();
  return 1;
}
}
