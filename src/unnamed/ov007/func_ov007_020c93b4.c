#include "Ov007PaletteBuf.h"

#include "nitro/gx/g3x.h"

/* Hands the buffer's colours to G3X_SetToonTable. */
void func_ov007_020c93b4(Ov007PaletteBuf *buf) {
    G3X_SetToonTable(buf->colors);
}
