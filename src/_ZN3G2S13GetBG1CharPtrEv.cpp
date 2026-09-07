//cpp
// @symbol _ZN3G2S13GetBG1CharPtrEv
/* G2S::GetBG1CharPtr() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). G2S is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */

namespace G2S {
unsigned int GetBG1CharPtr(){
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}
}
