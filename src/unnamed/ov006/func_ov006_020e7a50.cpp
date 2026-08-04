//cpp
#include "SharedFilePtr.h"
extern "C" {
extern void* data_ov006_02141e94;
extern void* data_ov006_02141e6c;
extern void __destroy_arr(void*,int,int,void*);
void _ZN15MaterialChangerD1Ev(void);
void _ZN9ModelAnimD1Ev(void);
void* func_ov006_020e7a50(char* c){
  ((SharedFilePtr *)(&data_ov006_02141e94))->Release();
  ((SharedFilePtr *)(&data_ov006_02141e6c))->Release();
  __destroy_arr(c+0x12c,3,0x14,(void*)_ZN15MaterialChangerD1Ev);
  __destroy_arr(c,3,0x64,(void*)_ZN9ModelAnimD1Ev);
  return c;
}
}
