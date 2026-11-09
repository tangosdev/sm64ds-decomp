//cpp
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "CP15.h"
#include "Ov007BgLayer.h"

extern "C" {

extern void func_02056554(const void *src, int offset, int count);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void func_020564f4(const void *src, int offset, int count);
extern void func_02056434(const void *src, int offset, int count);
extern void func_02056374(const void *src, u32 offset, u32 count);
extern void func_020562b4(const void *src, u32 offset, u32 count);

/* Shows a BG layer on the engines in `engines` (&1 main, &2 sub): re-uploads
 * screen to its BG while it is dirty, then writes the scroll (fine + coarse,
 * in pixels) to BGnHOFS/BGnVOFS. */
void func_ov007_020c076c(Ov007BgLayer *layer, int engines) {
    int x = (layer->scrollX + layer->fineScrollX) >> 12;
    int y = (layer->scrollY + layer->fineScrollY) >> 12;
    int size;

    if (layer->screen == 0)
        return;

    if (layer->dirty != 0) {
        size = layer->width * layer->height * 2;
        CP15::FlushAndInvalidateDataCache((u32)layer->screen, size);
        if (engines & 1) {
            switch (layer->bg) {
            case 0:
                func_02056554(layer->screen, layer->screenOffset, size);
                break;
            case 1:
                GX_LoadBG1Scr(layer->screen, layer->screenOffset, size);
                break;
            case 2:
                func_020563d4(layer->screen, layer->screenOffset, size);
                break;
            case 3:
                func_02056314(layer->screen, layer->screenOffset, size);
                break;
            }
        }
        if (engines & 2) {
            switch (layer->bg) {
            case 0:
                func_020564f4(layer->screen, layer->screenOffset, size);
                break;
            case 1:
                func_02056434(layer->screen, layer->screenOffset, size);
                break;
            case 2:
                func_02056374(layer->screen, layer->screenOffset, size);
                break;
            case 3:
                func_020562b4(layer->screen, layer->screenOffset, size);
                break;
            }
        }
        layer->dirty = 0;
    }

    if (engines & 1) {
        switch (layer->bg) {
        case 0:
            *(volatile u32 *)0x4000010 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 1:
            *(volatile u32 *)0x4000014 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 2:
            *(volatile u32 *)0x4000018 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 3:
            *(volatile u32 *)0x400001c = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        }
    }

    if (engines & 2) {
        switch (layer->bg) {
        case 0:
            *(volatile u32 *)0x4001010 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 1:
            *(volatile u32 *)0x4001014 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 2:
            *(volatile u32 *)0x4001018 = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        case 3:
            *(volatile u32 *)0x400101c = (x & 0x1ff) | ((y << 16) & 0x1ff0000);
            break;
        }
    }
}
}
