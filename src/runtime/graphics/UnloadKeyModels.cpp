//cpp
#include "SharedFilePtr.h"
extern "C" {
extern void* data_ov089_02132894[];
extern void* data_ov089_021328b4[];
void UnloadKeyModels(int i){
  if(i>=8) return;
  ((SharedFilePtr *)(data_ov089_02132894[i]))->Release();
  if(data_ov089_021328b4[i]==0) return;
  ((SharedFilePtr *)(data_ov089_021328b4[i]))->Release();
}
}
