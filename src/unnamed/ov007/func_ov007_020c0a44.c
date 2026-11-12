#include "Ov007BgLayer.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c0a44(Ov007BgLayer *layer);

/* Allocates a BG layer for BG bg at priority, over the width x height-tile
 * screen buffer screen. */
Ov007BgLayer *func_ov007_020c0aa8(void *screen, int bg, int priority, u16 width, u16 height) {
    Ov007BgLayer *layer = func_ov007_020c3df4(0, sizeof(Ov007BgLayer));
    func_ov007_020c0a44(layer);
    layer->screen = screen;
    layer->width = width;
    layer->height = height;
    layer->bg = bg;
    layer->priority = priority;
    return layer;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
void func_ov007_020c0a9c(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

/* Resets a BG layer: no BG, visible, dirty, no buffers, no scroll, unlinked. */
void func_ov007_020c0a44(Ov007BgLayer *layer) {
    layer->fineScrollX = layer->fineScrollY = 0;
    layer->scrollX = layer->scrollY = 0;
    layer->bg = -1;
    layer->visible = 1;
    layer->screen = 0;
    layer->source = 0;
    layer->width = layer->height = 0;
    layer->screenOffset = 0;
    layer->next = layer->prev = 0;
    layer->dirty = 1;
}
