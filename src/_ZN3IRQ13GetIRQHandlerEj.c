typedef unsigned int u32;
extern void* data_02099fe4;
extern char data_020a60c4[];
void* _ZN3IRQ13GetIRQHandlerEj(u32 mask){
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
