#include "types.h"
struct VramReg { u16 w0; u16 pad[0xa]; u16 f16; };
extern struct VramReg data_020a6088;
extern void Vram__Map(u16 bits);
void _ZN2GX22SetBankForSubBGExtPlttEt(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f16) & ~x;
    data_020a6088.f16 = x;
    if (x != 0) { if (x == 0x80) { *(volatile unsigned int*)0x4001000 |= 0x40000000; *(volatile unsigned char*)0x4000248 = 0x82; } }
    else { *(volatile unsigned int*)0x4001000 &= ~0x40000000; }
    Vram__Map(data_020a6088.w0);
}
