//cpp
// @symbol _ZN3IRQ13GetIRQHandlerEj
// Shared typed IRQ ownership: the namespace declaration, callback type, and
// handler-table entry shape now come from IRQ.h. Codegen remains exact.
#include "IRQ.h"

extern "C" {
extern IRQ::Handler data_02099fe4[];
extern IRQ::HandlerEntry data_020a60c4[];
}

namespace IRQ {

Handler GetIRQHandler(u32 mask)
{
 int i=0;
 Handler* p=data_02099fe4;
 for(;i<0x16;i++){
   if(mask&1){
     if(i>=8 && i<=0xb) return data_020a60c4[i-8].handler;
     if(i>=3 && i<=6) return data_020a60c4[i+1].handler;
     return *p;
   }
   mask>>=1; p++;
 }
 return 0;
}

}
