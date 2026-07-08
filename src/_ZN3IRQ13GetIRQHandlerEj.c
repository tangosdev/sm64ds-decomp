typedef unsigned int u32;
extern void* _ZN3IRQ12IRQFunctionsE;
extern char _ZN3IRQ15DmaTimFunctionsE[];
void* _ZN3IRQ13GetIRQHandlerEj(u32 mask){
 int i=0;
 char* p=(char*)&_ZN3IRQ12IRQFunctionsE;
 for(;i<0x16;i++){
   if(mask&1){
     if(i>=8 && i<=0xb) return *(void**)(_ZN3IRQ15DmaTimFunctionsE+(i-8)*12);
     if(i>=3 && i<=6) return *(void**)(_ZN3IRQ15DmaTimFunctionsE+(i+1)*12);
     return *(void**)p;
   }
   mask>>=1; p+=4;
 }
 return 0;
}
