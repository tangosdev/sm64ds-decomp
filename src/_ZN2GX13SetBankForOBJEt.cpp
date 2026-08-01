//cpp
#include "types.h"
struct GXState { u16 lcdc; u16 pad; u16 obj; };
extern "C" GXState data_020a6088;
extern "C" void Vram__Map(u16 bits);

namespace GX {

void SetBankForOBJ(u16 obj)
{
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.obj) & ~obj);
    data_020a6088.obj = obj;
    switch (obj) {
    case 0x00:
        break;
    case 0x03:
        *(volatile u8 *)0x4000241 = 0x8a;
        // fallthrough
    case 0x01:
        *(volatile u8 *)0x4000240 = 0x82;
        break;
    case 0x02:
        *(volatile u8 *)0x4000241 = 0x82;
        break;
    case 0x70:
        *(volatile u8 *)0x4000246 = 0x9a;
        // fallthrough
    case 0x30:
        *(volatile u8 *)0x4000245 = 0x92;
        // fallthrough
    case 0x10:
        *(volatile u8 *)0x4000244 = 0x82;
        break;
    case 0x50:
        *(volatile u8 *)0x4000246 = 0x92;
        *(volatile u8 *)0x4000244 = 0x82;
        break;
    case 0x60:
        *(volatile u8 *)0x4000246 = 0x8a;
        // fallthrough
    case 0x20:
        *(volatile u8 *)0x4000245 = 0x82;
        break;
    case 0x40:
        *(volatile u8 *)0x4000246 = 0x82;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

}
