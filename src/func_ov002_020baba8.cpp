//cpp
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
int func_ov002_020baba8(char* c, void** p){
  if(((dBgW *)(c+0x124))->IsEnabled())
    ((dBgW *)(c+0x124))->Disable();
  ((SharedFilePtr *)(p[0]))->Release();
  ((SharedFilePtr *)(p[1]))->Release();
  return 1;
}
}
