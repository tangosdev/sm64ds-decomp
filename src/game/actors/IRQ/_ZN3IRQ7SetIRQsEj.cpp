//cpp
// @symbol _ZN3IRQ7SetIRQsEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
typedef unsigned int u32;typedef unsigned short u16;
#define IME (*(volatile u16*)0x4000208)
#define IE  (*(volatile u32*)0x4000210)

namespace IRQ {

u32 SetIRQs(u32 mask)
{u16 ime=IME;IME=0;u32 old=IE;IE=mask;(void)IME;IME=ime;return old;}

}
