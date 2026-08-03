//cpp
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
int func_ov002_020baba8(char* c, void** p){
  if(((MeshColliderBase *)(c+0x124))->IsEnabled())
    ((MeshColliderBase *)(c+0x124))->Disable();
  ((SharedFilePtr *)(p[0]))->Release();
  ((SharedFilePtr *)(p[1]))->Release();
  return 1;
}
}
