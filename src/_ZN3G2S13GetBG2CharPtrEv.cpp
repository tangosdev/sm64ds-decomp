//cpp
// @symbol _ZN3G2S13GetBG2CharPtrEv
/* G2S::GetBG2CharPtr() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). G2S is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */

namespace G2S {
unsigned GetBG2CharPtr(){
  int v1 = *(volatile int*)0x4001000;
  unsigned short v2 = *(volatile unsigned short*)0x400100c;
  if (!((v1 & 7) >= 5 && (v2 & 0x80)))
    return ((unsigned)(v2 & 0x3c) >> 2 << 0xe) + 0x6200000;
  return 0;
}
}
