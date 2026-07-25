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
extern "C" void Vram__Map(int bits);

#define VRAMCNT_A (*(volatile u8 *)0x4000240)
#define VRAMCNT_B (*(volatile u8 *)0x4000241)
#define VRAMCNT_C (*(volatile u8 *)0x4000242)
#define VRAMCNT_D (*(volatile u8 *)0x4000243)

namespace GX {

void SetBankForTex(u16 tex)
{
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.tex) & ~tex);
    data_020a6088.tex = tex;
    if (tex == 0) {
        *(volatile u16 *)0x4000060 &= 0xcffe;
    } else {
        *(volatile u16 *)0x4000060 =
            (u16)((*(volatile u16 *)0x4000060 & ~0x3000u) | 1u);
        switch (tex) {
        case 0:
            break;
        case 5:
            VRAMCNT_A = 0x83;
            VRAMCNT_C = 0x8b;
            break;
        case 9:
            VRAMCNT_A = 0x83;
            VRAMCNT_D = 0x8b;
            break;
        case 10:
            VRAMCNT_B = 0x83;
            VRAMCNT_D = 0x8b;
            break;
        case 11:
            VRAMCNT_A = 0x83;
            VRAMCNT_B = 0x8b;
            VRAMCNT_D = 0x93;
            break;
        case 13:
            VRAMCNT_A = 0x83;
            VRAMCNT_C = 0x8b;
            VRAMCNT_D = 0x93;
            break;
        case 8:
            VRAMCNT_D = 0x83;
            break;
        case 12:
            VRAMCNT_D = 0x8b;
        case 4:
            VRAMCNT_C = 0x83;
            break;
        case 14:
            VRAMCNT_D = 0x93;
        case 6:
            VRAMCNT_C = 0x8b;
        case 2:
            VRAMCNT_B = 0x83;
            break;
        case 15:
            VRAMCNT_D = 0x9b;
        case 7:
            VRAMCNT_C = 0x93;
        case 3:
            VRAMCNT_B = 0x8b;
        case 1:
            VRAMCNT_A = 0x83;
            break;
        }
    }
    Vram__Map(data_020a6088.lcdc);
}

}
