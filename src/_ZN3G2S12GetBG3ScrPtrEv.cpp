//cpp
// @symbol _ZN3G2S12GetBG3ScrPtrEv
/* G2S::GetBG3ScrPtr() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). G2S is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */

namespace G2S {
unsigned int GetBG3ScrPtr(){
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
        return (r1 << 0xb) + 0x6200000;
    case 3:
    case 4:
    case 5:
        if (v & 0x80) return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}
}
