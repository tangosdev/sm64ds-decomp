//cpp
// @symbol _ZN2GX6DispOnEv
/* GX::DispOn() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
extern u16 data_02099fcc;
extern u16 data_020a6084;
namespace GX {
void DispOn(){
 data_02099fcc = 1;
 u16 t = data_020a6084;
 if(t!=0){
   volatile unsigned *r = (volatile unsigned*)0x4000000;
   *r = (*r & ~0x30000u) | ((unsigned)t<<16);
   return;
 }
 *(volatile unsigned*)0x4000000 |= 0x10000;
}
}
