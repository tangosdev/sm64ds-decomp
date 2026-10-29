//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"

namespace CP15 {
void FlushAndInvalidateDataCache(unsigned int a, unsigned int b);
}

namespace GXS {
void LoadOBJPltt(const void *p, unsigned int a, unsigned int b);
}
extern "C" {
extern void func_ov007_020c32e8(void *a, int b, int c, int d);

extern void RequestScreenFade(int a, int b, int c);
extern void *data_ov007_0210342c;
extern void func_ov007_020adb84(void);

/* The 8-byte palette record func_ov007_020c3398 allocates:
 * func_ov007_020c32e8 loads colorCount colors from `colors`. Only the
 * slot this file reads. */
struct Ov007Palette {
    char *colors; /* 0x00: 16-color (0x20-byte) rows */
};

/* The manager's +0x74: the 0x48-byte 2D display record func_ov007_020b7594
 * allocates; only the slot this file reads. */
struct Ov007Display {
    char pad_00[0x2c];
    /* 0x2c: func_ov007_020b7320 allocates the table, func_ov007_020b6d40
     * fills it with func_ov007_020c9988 palettes */
    Ov007Palette **palettes;
};

typedef char Ov007Display_size_must_be_0x30[sizeof(Ov007Display) == 0x30 ? 1 : -1];

/* For palette states 4..7, loads palette 2..5 into the sub BG palette, then
 * copies row 0xc..0xf (0xc by default) of palette 1 into sub OBJ palette
 * row 0xc and starts a screen fade into func_ov007_020adb84. */
void func_ov007_020ada70(void) {
    Ov007Work *g = (Ov007Work *)*(void **)&data_ov007_0210342c;
    short st = g->paletteState->currentState;
    Ov007Display *r5 = g->display;
    int n = 0xc;
    switch (st) {
        case 4:
            func_ov007_020c32e8(r5->palettes[2], 3, 0, 0);
            break;
        case 5:
            func_ov007_020c32e8(r5->palettes[3], 3, 0, 0);
            n = 0xd;
            break;
        case 6:
            func_ov007_020c32e8(r5->palettes[4], 3, 0, 0);
            n = 0xe;
            break;
        case 7:
            func_ov007_020c32e8(r5->palettes[5], 3, 0, 0);
            n = 0xf;
            break;
    }
    char *base = r5->palettes[1]->colors;
    char *dst = base + (n << 5);
    CP15::FlushAndInvalidateDataCache((unsigned int)dst, 0x20);
    GXS::LoadOBJPltt(dst, 0x180, 0x20);
    RequestScreenFade(9, 2, (int)&func_ov007_020adb84);
}
}
