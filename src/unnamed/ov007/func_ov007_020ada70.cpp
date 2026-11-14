//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Display.h"
#include "nitro/gx/gx_load.h"

#include "nitro/os/cache.h"

extern void func_ov007_020c32e8(Ov007Palette *pal, int target, unsigned int row, int skip);

extern "C" void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern "C" void func_ov007_020adb84(void);

/* For palette states 4..7, loads palette 2..5 into the sub BG palette, then
 * copies row 0xc..0xf (0xc by default) of palette 1 into sub OBJ palette
 * row 0xc and starts a screen fade into func_ov007_020adb84. */
void func_ov007_020ada70(void) {
    Ov007Work *g = WORK;
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
    const u16 *dst = r5->palettes[1]->colors + n * 16;
    DC_FlushRange(dst, 0x20);
    GXS_LoadOBJPltt(dst, 0x180, 0x20);
    RequestScreenFade(9, 2, func_ov007_020adb84);
}
