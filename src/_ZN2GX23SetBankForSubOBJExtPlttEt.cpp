//cpp
// @symbol _ZN2GX23SetBankForSubOBJExtPlttEt
/* GX::SetBankForSubOBJExtPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
struct VramReg { u16 w0; u16 pad[0xb]; u16 f18; };
extern struct VramReg data_020a6088;

extern "C" {
extern void Vram__Map(u16 bits);
}

namespace GX {
void SetBankForSubOBJExtPltt(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f18) & ~x;
    data_020a6088.f18 = x;
    if (x != 0) { if (x == 0x100) { *(volatile unsigned int*)0x4001000 |= 0x80000000; *(volatile unsigned char*)0x4000249 = 0x83; } }
    else { *(volatile unsigned int*)0x4001000 &= ~0x80000000; }
    Vram__Map(data_020a6088.w0);
}
}
