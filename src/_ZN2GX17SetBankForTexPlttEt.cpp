//cpp
// @symbol _ZN2GX17SetBankForTexPlttEt
/* GX::SetBankForTexPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
struct VramReg { u16 w0; u16 pad[4]; u16 fa; };
extern struct VramReg data_020a6088;

extern "C" {
extern void Vram__Map(u16 bits);
}

namespace GX {
void SetBankForTexPltt(u16 x){
    data_020a6088.w0 = ~x & (data_020a6088.w0 | data_020a6088.fa);
    data_020a6088.fa = x;
    switch (x) {
    case 0x40:
        *(volatile unsigned char*)0x4000246 = 0x83;
        break;
    case 0x60:
        *(volatile unsigned char*)0x4000246 = 0x8b;
    case 0x20:
        *(volatile unsigned char*)0x4000245 = 0x83;
        break;
    case 0x70:
        *(volatile unsigned char*)0x4000246 = 0x9b;
    case 0x30:
        *(volatile unsigned char*)0x4000245 = 0x93;
    case 0x10:
        *(volatile unsigned char*)0x4000244 = 0x83;
    case 0:
        break;
    }
    Vram__Map(data_020a6088.w0);
}
}
