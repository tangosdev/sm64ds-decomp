typedef unsigned int u32;
typedef void(*Fn)(void);
extern Fn _ZN3IRQ12IRQFunctionsE[];
struct Ent{ Fn fn; int en; int pad; };
extern struct Ent _ZN3IRQ15DmaTimFunctionsE[];
void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 mask, Fn handler){
 for(int i=0;i<0x16;i++){
   if(mask&1){
     struct Ent* e=0;
     if(i>=8 && i<=0xb) e=&_ZN3IRQ15DmaTimFunctionsE[i-8];
     else if(i>=3 && i<=6) e=&_ZN3IRQ15DmaTimFunctionsE[i+1];
     else _ZN3IRQ12IRQFunctionsE[i]=handler;
     if(e){ e->fn=handler; e->pad=0; e->en=1; }
   }
   mask>>=1;
 }
}
