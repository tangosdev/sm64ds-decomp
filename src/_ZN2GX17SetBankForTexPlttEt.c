#include "types.h"
struct VramReg { u16 w0; u16 pad[4]; u16 fa; };
extern struct VramReg data_020a6088;
extern void Vram__Map(u16 bits);
void _ZN2GX17SetBankForTexPlttEt(u16 x){
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
