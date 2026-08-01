#include "types.h"
struct VramReg { u16 w0; u16 pad[0xb]; u16 f18; };
extern struct VramReg data_020a6088;
extern void Vram__Map(u16 bits);
void _ZN2GX23SetBankForSubOBJExtPlttEt(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f18) & ~x;
    data_020a6088.f18 = x;
    if (x != 0) { if (x == 0x100) { *(volatile unsigned int*)0x4001000 |= 0x80000000; *(volatile unsigned char*)0x4000249 = 0x83; } }
    else { *(volatile unsigned int*)0x4001000 &= ~0x80000000; }
    Vram__Map(data_020a6088.w0);
}
