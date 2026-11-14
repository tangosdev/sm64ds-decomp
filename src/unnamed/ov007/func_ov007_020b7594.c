#include "Ov007Display.h"
#include "Ov007MenuMem.h"

#define func_ov007_020c0354 _Z19func_ov007_020c0354PA2_ij
extern void func_ov007_020c0354(int (*words)[2], unsigned count);
extern Ov007OamList *func_ov007_020c1068(void);
extern Ov007BgList *func_ov007_020c0604(int engines, int mask);

/* Builds the display record: two OAM node lists, a BG layer list per
 * engine (each with its own layer mask) and two cleared 128-entry OAM
 * buffers; every table starts empty. */
Ov007Display *func_ov007_020b7594(int maskMain, int maskSub) {
    Ov007Display *d = func_ov007_020c3df4(0, sizeof(Ov007Display));
    d->oamMain = func_ov007_020c3df4(0, 0x400);
    d->oamSub = func_ov007_020c3df4(0, 0x400);
    func_ov007_020c0354((int(*)[2])d->oamMain, 0x80);
    func_ov007_020c0354((int(*)[2])d->oamSub, 0x80);
    d->oamListMain = func_ov007_020c1068();
    d->oamListSub = func_ov007_020c1068();
    d->bgListMain = func_ov007_020c0604(1, maskMain);
    d->bgListSub = func_ov007_020c0604(2, maskSub);
    d->transitionCount = 0;
    d->transitions = 0;
    d->characterCount = 0;
    d->characters = 0;
    d->paletteCount = 0;
    d->palettes = 0;
    d->spriteBankCount = 0;
    d->spriteBanks = 0;
    d->oamNodeCount = 0;
    d->oamNodes = 0;
    d->bgLayerCount = 0;
    d->bgLayers = 0;
    return d;
}
