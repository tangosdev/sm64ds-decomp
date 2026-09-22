//cpp
// @symbol _ZN2GX16SetBankForSubOBJEt
/* GX::SetBankForSubOBJ() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
struct VramReg { u16 w0; u16 pad[9]; u16 f14; };
extern struct VramReg data_020a6088;

extern "C" {
extern void Vram__Map(u16 bits);
}

namespace GX {
void SetBankForSubOBJ(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f14) & ~x;
    data_020a6088.f14 = x;
    switch (x) {
    case 0: break;
    case 8: *(volatile unsigned char*)0x4000243 = 0x84; break;
    case 0x100: *(volatile unsigned char*)0x4000249 = 0x82; break;
    }
    Vram__Map(data_020a6088.w0);
}
}
