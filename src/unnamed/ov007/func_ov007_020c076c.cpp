//cpp
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "nitro/os/cache.h"
#include "nitro/gx/g2.h"
#include "Ov007BgLayer.h"



/* local extern: nitro/gx/gx_load.h leaves GX_LoadBG3Scr out until
 * decl_common.h's non-const spelling of it goes. */
extern "C" void GX_LoadBG3Scr(void *dst, u32 offset, u32 len);

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
        DC_FlushRange(layer->screen, size);
        if (engines & 1) {
            switch (layer->bg) {
            case 0:
                GX_LoadBG0Scr(layer->screen, layer->screenOffset, size);
                break;
            case 1:
                GX_LoadBG1Scr(layer->screen, layer->screenOffset, size);
                break;
            case 2:
                GX_LoadBG2Scr(layer->screen, layer->screenOffset, size);
                break;
            case 3:
                GX_LoadBG3Scr(layer->screen, layer->screenOffset, size);
                break;
            }
        }
        if (engines & 2) {
            switch (layer->bg) {
            case 0:
                GXS_LoadBG0Scr(layer->screen, layer->screenOffset, size);
                break;
            case 1:
                GXS_LoadBG1Scr(layer->screen, layer->screenOffset, size);
                break;
            case 2:
                GXS_LoadBG2Scr(layer->screen, layer->screenOffset, size);
                break;
            case 3:
                GXS_LoadBG3Scr(layer->screen, layer->screenOffset, size);
                break;
            }
        }
        layer->dirty = 0;
    }

    if (engines & 1) {
        switch (layer->bg) {
        case 0:
            G2_SetBG0Offset(x, y);
            break;
        case 1:
            G2_SetBG1Offset(x, y);
            break;
        case 2:
            G2_SetBG2Offset(x, y);
            break;
        case 3:
            G2_SetBG3Offset(x, y);
            break;
        }
    }

    if (engines & 2) {
        switch (layer->bg) {
        case 0:
            G2S_SetBG0Offset(x, y);
            break;
        case 1:
            G2S_SetBG1Offset(x, y);
            break;
        case 2:
            G2S_SetBG2Offset(x, y);
            break;
        case 3:
            G2S_SetBG3Offset(x, y);
            break;
        }
    }
}

