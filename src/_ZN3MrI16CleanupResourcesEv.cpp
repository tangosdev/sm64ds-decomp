//cpp
#include "SharedFilePtr.h"
extern "C" {
void UnloadBlueCoinModel(void);
extern int data_ov002_0210da38;
extern int data_ov071_02123050;
extern int* data_ov071_021226a4;
extern void* data_ov071_021226a0;
int _ZN3MrI16CleanupResourcesEv(void){
  int i;
  UnloadBlueCoinModel();
  ((SharedFilePtr *)(&data_ov002_0210da38))->Release();
  ((SharedFilePtr *)(&data_ov071_02123050))->Release();
  for(i=0;i<2;i++){
    ((SharedFilePtr *)((&data_ov071_021226a4)[i]))->Release();
  }
  ((SharedFilePtr *)(data_ov071_021226a0))->Release();
  return 1;
}
}
