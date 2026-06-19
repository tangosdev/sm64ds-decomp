typedef unsigned int u32;typedef unsigned short u16;
#define IME (*(volatile u16*)0x4000208)
#define IE  (*(volatile u32*)0x4000210)
u32 _ZN3IRQ10EnableIRQsEj(u32 mask){u16 ime=IME;IME=0;u32 old=IE;IE=old|mask;(void)IME;IME=ime;return old;}
