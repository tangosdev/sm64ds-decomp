//cpp
// @symbol _ZN3G2S13GetBG3CharPtrEv
/* G2S::GetBG3CharPtr() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). G2S is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */

namespace G2S {
unsigned int GetBG3CharPtr(){
    int cfg = *(volatile int *)0x4001000;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    int m = cfg & 7;
    if (m >= 3) {
        if (m >= 6) goto zero;
        if (v & 0x80) goto zero;
    }
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
zero:
    return 0;
}
}
