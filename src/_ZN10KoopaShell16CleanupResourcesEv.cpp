//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int data_ov102_0214d70c[];
int _ZN10KoopaShell16CleanupResourcesEv(char* c){
  unsigned char i=*(unsigned char*)(c+0x3c4);
  ((SharedFilePtr *)((void*)data_ov102_0214d70c[i]))->Release();
  return 1;
}
}
