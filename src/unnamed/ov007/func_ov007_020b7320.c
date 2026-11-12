#include "Ov007Display.h"
#include "Ov007MenuMem.h"

/* Sizes the character and palette tables (func_ov007_020b6d40 asks for 4
 * and 6), zeroed. */
void func_ov007_020b7320(Ov007Display *d, int a, int b) {
    d->characterCount = a;
    d->paletteCount = b;
    d->characters = MenuMem_AllocClear(0, a * sizeof(Ov007CharRecord *));
    d->palettes = MenuMem_AllocClear(0, b * sizeof(Ov007Palette *));
}
