//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct GXState {
    u16 lcdc;
    u16 pad;
    u16 obj;
    u16 pad2;
    u16 tex;
};
extern "C" GXState data_020a6088;
extern "C" void Vram__Map(u16 bits);

namespace GX {

void SetBankForTex(u16 tex)
{
    data_020a6088.lcdc = (u16)(~tex & (data_020a6088.lcdc | data_020a6088.tex));
    data_020a6088.tex = tex;
    if (tex == 0) {
        *(volatile u16 *)0x4000060 &= 0xcffe;
    } else {
        *(volatile u16 *)0x4000060 =
            (u16)((*(volatile u16 *)0x4000060 & ~0x3000) | 1);
        switch (tex) {
        case 0:
            break;
        case 5:
            *(volatile u8 *)0x4000240 = 0x83;
            *(volatile u8 *)0x4000242 = 0x8b;
            break;
        case 9:
            *(volatile u8 *)0x4000240 = 0x83;
            *(volatile u8 *)0x4000243 = 0x8b;
            break;
        case 10:
            *(volatile u8 *)0x4000241 = 0x83;
            *(volatile u8 *)0x4000243 = 0x8b;
            break;
        case 11:
            *(volatile u8 *)0x4000240 = 0x83;
            *(volatile u8 *)0x4000241 = 0x8b;
            *(volatile u8 *)0x4000243 = 0x93;
            break;
        case 13:
            *(volatile u8 *)0x4000240 = 0x83;
            *(volatile u8 *)0x4000242 = 0x8b;
            *(volatile u8 *)0x4000243 = 0x93;
            break;
        case 8:
            *(volatile u8 *)0x4000243 = 0x83;
            break;
        case 12:
            *(volatile u8 *)0x4000243 = 0x8b;
            // fallthrough
        case 4:
            *(volatile u8 *)0x4000242 = 0x83;
            break;
        case 14:
            *(volatile u8 *)0x4000243 = 0x93;
            // fallthrough
        case 6:
            *(volatile u8 *)0x4000242 = 0x8b;
            // fallthrough
        case 2:
            *(volatile u8 *)0x4000241 = 0x83;
            break;
        case 15:
            *(volatile u8 *)0x4000243 = 0x9b;
            // fallthrough
        case 7:
            *(volatile u8 *)0x4000242 = 0x93;
            // fallthrough
        case 3:
            *(volatile u8 *)0x4000241 = 0x8b;
            // fallthrough
        case 1:
            *(volatile u8 *)0x4000240 = 0x83;
            break;
        }
    }
    Vram__Map(data_020a6088.lcdc);
}

}
