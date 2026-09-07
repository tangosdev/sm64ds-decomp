//cpp
// @symbol _ZN2GX22SetBankForSubBGExtPlttEt
/* GX::SetBankForSubBGExtPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
struct VramReg { u16 w0; u16 pad[0xa]; u16 f16; };
extern struct VramReg data_020a6088;

extern "C" {
extern void Vram__Map(u16 bits);
}

namespace GX {
void SetBankForSubBGExtPltt(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f16) & ~x;
    data_020a6088.f16 = x;
    if (x != 0) { if (x == 0x80) { *(volatile unsigned int*)0x4001000 |= 0x40000000; *(volatile unsigned char*)0x4000248 = 0x82; } }
    else { *(volatile unsigned int*)0x4001000 &= ~0x40000000; }
    Vram__Map(data_020a6088.w0);
}
}
