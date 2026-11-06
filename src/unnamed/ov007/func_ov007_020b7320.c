#include "Ov007Display.h"

extern void *func_ov007_020c3d40(const char *name, int size);

/* Sizes the character and palette tables (func_ov007_020b6d40 asks for 4
 * and 6), zeroed. */
void func_ov007_020b7320(Ov007Display *d, int a, int b) {
    d->characterCount = a;
    d->paletteCount = b;
    d->characters = func_ov007_020c3d40(0, a * sizeof(Ov007CharRecord *));
    d->palettes = func_ov007_020c3d40(0, b * sizeof(Ov007Palette *));
}
