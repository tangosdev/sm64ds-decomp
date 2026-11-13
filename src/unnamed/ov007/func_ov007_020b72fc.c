#include "Ov007Display.h"
#include "Ov007MenuMem.h"
#define MenuMem_AllocClear _Z18MenuMem_AllocClearPKci

/* Sizes the sprite-bank table (func_ov007_020b6d40 asks for 6), zeroed. */
void func_ov007_020b72fc(Ov007Display *d, int n) {
    d->spriteBankCount = n;
    d->spriteBanks = MenuMem_AllocClear(0, n * sizeof(Ov007SpriteBank *));
}
