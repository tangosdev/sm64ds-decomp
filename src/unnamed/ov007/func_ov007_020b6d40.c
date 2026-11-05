#include "types.h"
#include "Ov007Work.h"
#include "Ov007Display.h"
extern int data_ov007_020d7e28[];
extern int data_ov007_020d7e38[];
extern int data_ov007_020d7e50[];
extern int data_ov007_020d7ec8[];
extern u8 data_ov007_020d7a24[][2];

extern Ov007Display *func_ov007_020b7594(int maskMain, int maskSub);
extern void func_ov007_020b7320(Ov007Display *d, int a, int b);
extern Ov007CharRecord *func_ov007_020c99d8(int x);
extern Ov007Palette *func_ov007_020c9988(int x);
extern void func_ov007_020b72fc(Ov007Display *d, int n);
extern Ov007SpriteBank *func_ov007_020c9688(int arg);
extern struct Ov007ItemNode *func_ov007_020ade58(int index);
extern void func_ov007_020b72d8(Ov007Display *d, int n);
extern Ov007BgLayer *func_ov007_020c95fc(int a, int b, int c);
extern struct Ov007LayerSlot *func_ov007_020adc08(int type, Ov007BgLayer *layer);

void func_ov007_020b6d40(void) {
    Ov007Display *c;
    int i;

    WORK->display = func_ov007_020b7594(0x1f, 0x1f);
    c = WORK->display;
    func_ov007_020b7320(c, 4, 6);

    for (i = 0; i < 4; i++) {
        c->characters[i] = func_ov007_020c99d8(data_ov007_020d7e28[i]);
    }

    for (i = 0; i < 6; i++) {
        c->palettes[i] = func_ov007_020c9988(data_ov007_020d7e38[i]);
        if (i >= 2 && i <= 5) {
            c->palettes[i]->colorCount = 0xd0;
        }
    }

    func_ov007_020b72fc(c, 6);

    for (i = 0; i < 6; i++) {
        c->spriteBanks[i] = func_ov007_020c9688(data_ov007_020d7e50[i]);
    }

    for (i = 0; i < 9; i++) {
        WORK->itemNodes[i] = func_ov007_020ade58(i);
    }

    func_ov007_020b72d8(c, 8);

    for (i = 0; i < 8; i++) {
        c->bgLayers[i] = func_ov007_020c95fc(data_ov007_020d7ec8[i], data_ov007_020d7a24[i][0],
                                             data_ov007_020d7a24[i][1]);
        WORK->layerSlots[i] = func_ov007_020adc08(i, c->bgLayers[i]);
    }
}
