//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
int func_ov002_020baba8(char* c, void** p){
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  ((SharedFilePtr *)(p[0]))->Release();
  ((SharedFilePtr *)(p[1]))->Release();
  return 1;
}
}
