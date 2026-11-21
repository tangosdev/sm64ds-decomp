#ifndef OV007_PALETTE_BUF_H
#define OV007_PALETTE_BUF_H

#include "types.h"

/* A reconstructed layout and name, not recovered class identity. */

/* The 0x44-byte palette buffer func_ov007_020c946c allocates; Ov007Work
 * keeps three in paletteBufs. func_ov007_020c940c copies 0x40 bytes of
 * colours from source into colors, or clears them when source is 0;
 * func_ov007_020c93c4 blends two buffers' colours into a third and
 * func_ov007_020c93b4 hands colors to func_020555a4 as the toon table. */
typedef struct Ov007PaletteBuf {
    const void *source; /* 0x00 */
    u16 colors[0x20];   /* 0x04 */
} Ov007PaletteBuf;

typedef char Ov007PaletteBuf_size_must_be_0x44[sizeof(Ov007PaletteBuf) == 0x44 ? 1 : -1];

#endif
