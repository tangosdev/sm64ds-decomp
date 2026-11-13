#include "Ov007Work.h"
#include "Ov007Display.h"
#include "Ov007LayerSlot.h"
#include "Ov007MenuMem.h"
#define MenuText_GetSourceScreen _Z24MenuText_GetSourceScreenv
#define MenuText_GetSubScreen _Z21MenuText_GetSubScreenv
#define MenuText_GetMainScreen _Z22MenuText_GetMainScreenv

extern void *func_ov007_020c9388(int a);
extern void *MenuText_GetMainScreen(void);
extern void *MenuText_GetSourceScreen(void);
extern void func_ov007_020c0384(Ov007BgList *list, Ov007BgLayer *layer);
extern void *MenuText_GetSubScreen(void);
extern void func_ov007_020c02d8(void *screen, int rows, unsigned short tile);

/* Builds layer slot `type` around `layer` and links it into the main or sub
 * BG list: 6/7 take prebuilt screen pairs, 1 keeps the loaded screen as its
 * source behind a fresh buffer, 5 starts from a blank (tile 0x300) screen. */
Ov007LayerSlot *func_ov007_020adc08(int type, Ov007BgLayer *layer) {
    Ov007LayerSlot *slot = func_ov007_020c3df4(0, sizeof(Ov007LayerSlot));
    Ov007Display *display = WORK->display;
    slot->kind = type;
    slot->layer = layer;
    slot->state = func_ov007_020c9388(0);
    switch (type) {
    case 6:
        layer->screen = MenuText_GetMainScreen();
        layer->source = MenuText_GetSourceScreen();
        func_ov007_020c0384(display->bgListMain, layer);
        break;
    case 7:
        layer->screen = MenuText_GetSubScreen();
        layer->source = MenuText_GetSourceScreen();
        func_ov007_020c0384(display->bgListSub, layer);
        break;
    case 1:
        layer->source = layer->screen;
        layer->screen = func_ov007_020c3df4(0, 0x800);
        func_ov007_020c0384(display->bgListMain, layer);
        break;
    case 2:
        func_ov007_020c0384(display->bgListSub, layer);
        break;
    case 3:
    case 4:
        func_ov007_020c0384(display->bgListSub, layer);
        break;
    case 5:
        layer->screen = func_ov007_020c3df4(0, 0x800);
        func_ov007_020c02d8(layer->screen, 0x18, 0x300);
        func_ov007_020c0384(display->bgListSub, layer);
        break;
    default:
        break;
    }
    return slot;
}
