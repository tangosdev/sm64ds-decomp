typedef unsigned int u32;
typedef unsigned short u16;
#define IME (*(volatile u16*)0x4000208)
#define IF  (*(volatile u32*)0x4000214)
u32 _ZN3IRQ15ClearInterruptsEj(u32 mask){u16 ime=IME;IME=0;u32 old=IF;IF=mask;(void)IME;IME=ime;return old;}
