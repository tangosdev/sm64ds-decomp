//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct GXState { u16 lcdc; u16 bg; u16 obj; };
extern "C" GXState data_020a6088;
extern "C" void Vram__Map(u16 bits);

namespace GX {

void SetBankForBG(u16 bank)
{
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.bg) & ~bank);
    data_020a6088.bg = bank;
    switch (bank) {
    case 0x00:
        break;
    case 0x08:
        *(volatile u8 *)0x4000243 = 0x81;
        break;
    case 0x0C:
        *(volatile u8 *)0x4000243 = 0x89;
        // fallthrough
    case 0x04:
        *(volatile u8 *)0x4000242 = 0x81;
        break;
    case 0x0E:
        *(volatile u8 *)0x4000243 = 0x91;
        // fallthrough
    case 0x06:
        *(volatile u8 *)0x4000242 = 0x89;
        // fallthrough
    case 0x02:
        *(volatile u8 *)0x4000241 = 0x81;
        break;
    case 0x0F:
        *(volatile u8 *)0x4000243 = 0x99;
        // fallthrough
    case 0x07:
        *(volatile u8 *)0x4000242 = 0x91;
        // fallthrough
    case 0x03:
        *(volatile u8 *)0x4000241 = 0x89;
        // fallthrough
    case 0x01:
        *(volatile u8 *)0x4000240 = 0x81;
        break;
    case 0x0B:
        *(volatile u8 *)0x4000240 = 0x81;
        *(volatile u8 *)0x4000241 = 0x89;
        *(volatile u8 *)0x4000243 = 0x91;
        break;
    case 0x0D:
        *(volatile u8 *)0x4000243 = 0x91;
        // fallthrough
    case 0x05:
        *(volatile u8 *)0x4000240 = 0x81;
        *(volatile u8 *)0x4000242 = 0x89;
        break;
    case 0x09:
        *(volatile u8 *)0x4000240 = 0x81;
        *(volatile u8 *)0x4000243 = 0x89;
        break;
    case 0x0A:
        *(volatile u8 *)0x4000241 = 0x81;
        *(volatile u8 *)0x4000243 = 0x89;
        break;
    case 0x70:
        *(volatile u8 *)0x4000246 = 0x99;
        // fallthrough
    case 0x30:
        *(volatile u8 *)0x4000245 = 0x91;
        // fallthrough
    case 0x10:
        *(volatile u8 *)0x4000244 = 0x81;
        break;
    case 0x50:
        *(volatile u8 *)0x4000246 = 0x91;
        *(volatile u8 *)0x4000244 = 0x81;
        break;
    case 0x60:
        *(volatile u8 *)0x4000246 = 0x89;
        // fallthrough
    case 0x20:
        *(volatile u8 *)0x4000245 = 0x81;
        break;
    case 0x40:
        *(volatile u8 *)0x4000246 = 0x81;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

}
