//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int data_ov002_020ff480[];
void func_ov002_020bebd4(void){
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+1]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+2]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+3]))->Release();
  }
}
}
