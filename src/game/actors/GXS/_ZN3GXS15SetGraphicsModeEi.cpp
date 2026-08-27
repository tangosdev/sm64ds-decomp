//cpp
// @symbol _ZN3GXS15SetGraphicsModeEi
/* GXS::SetGraphicsMode() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
//cpp
extern "C" void _ZN3GXS15SetGraphicsModeEi(int mode);
namespace GXS {
void SetGraphicsMode(int mode){
    *(volatile unsigned*)0x4001000 = (*(volatile unsigned*)0x4001000 & ~7) | mode;
}
}
