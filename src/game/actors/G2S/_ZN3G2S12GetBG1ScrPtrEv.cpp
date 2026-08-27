//cpp
// @symbol _ZN3G2S12GetBG1ScrPtrEv
/* G2S::GetBG1ScrPtr() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). G2S is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */

namespace G2S {
unsigned int GetBG1ScrPtr(){
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}
}
