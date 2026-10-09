//cpp
// @symbol _ZN2GX15SetGraphicsModeEiii
/* GX::SetGraphicsMode() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
extern u16 data_02099fcc;
extern u16 data_020a6084;
namespace GX {
void SetGraphicsMode(int a, int b, int c){
 u16 t = data_02099fcc;
 unsigned reg = *(volatile unsigned*)0x4000000;
 data_020a6084 = (u16)a;
 if(t==0) a=0;
 reg = (unsigned)b | ((reg & 0xfff0fff0) | ((unsigned)a<<16));
 reg = ((unsigned)c<<3) | reg;
 *(volatile unsigned*)0x4000000 = reg;
 if(data_020a6084==0) data_02099fcc=0;
}
}
