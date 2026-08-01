#include "types.h"
struct VramReg { u16 w0; u16 pad[9]; u16 f14; };
extern struct VramReg data_020a6088;
extern void Vram__Map(u16 bits);
void _ZN2GX16SetBankForSubOBJEt(u16 x){
    data_020a6088.w0 = (data_020a6088.w0 | data_020a6088.f14) & ~x;
    data_020a6088.f14 = x;
    switch (x) {
    case 0: break;
    case 8: *(volatile unsigned char*)0x4000243 = 0x84; break;
    case 0x100: *(volatile unsigned char*)0x4000249 = 0x82; break;
    }
    Vram__Map(data_020a6088.w0);
}
