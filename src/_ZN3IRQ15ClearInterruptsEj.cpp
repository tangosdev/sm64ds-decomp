//cpp
// @symbol _ZN3IRQ15ClearInterruptsEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
#include "types.h"
#define IME (*(volatile u16*)0x4000208)
#define IF  (*(volatile u32*)0x4000214)

namespace IRQ {

u32 ClearInterrupts(u32 mask)
{u16 ime=IME;IME=0;u32 old=IF;IF=mask;(void)IME;IME=ime;return old;}

}
