#include "Ov007Display.h"

extern void *MenuMem_AllocClear(const char *name, int size);

/* Sizes the BG layer table (func_ov007_020b6d40 asks for 8), zeroed. */
void func_ov007_020b72d8(Ov007Display *d, int n) {
    d->bgLayerCount = n;
    d->bgLayers = MenuMem_AllocClear(0, n * sizeof(Ov007BgLayer *));
}
