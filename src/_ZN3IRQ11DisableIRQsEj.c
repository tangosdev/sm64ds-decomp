typedef unsigned int u32;typedef unsigned short u16;
#define IME (*(volatile u16*)0x4000208)
#define IE  (*(volatile u32*)0x4000210)
u32 _ZN3IRQ11DisableIRQsEj(u32 mask){u16 s=IME;IME=0;u32 o=IE;IE=o&~mask;(void)IME;IME=s;return o;}
