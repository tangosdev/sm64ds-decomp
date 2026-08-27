//cpp
// @symbol _ZN3IRQ13GetIRQHandlerEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
#include "types.h"
extern "C" {
extern void* data_02099fe4;
extern char data_020a60c4[];
}

namespace IRQ {

void* GetIRQHandler(u32 mask)
{
 int i=0;
 char* p=(char*)&data_02099fe4;
 for(;i<0x16;i++){
   if(mask&1){
     if(i>=8 && i<=0xb) return *(void**)(data_020a60c4+(i-8)*12);
     if(i>=3 && i<=6) return *(void**)(data_020a60c4+(i+1)*12);
     return *(void**)p;
   }
   mask>>=1; p+=4;
 }
 return 0;
}

}
