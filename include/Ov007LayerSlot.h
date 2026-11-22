#ifndef OV007_LAYER_SLOT_H
#define OV007_LAYER_SLOT_H

#include "types.h"

/* A reconstructed layout and name, not recovered class identity. */

struct Ov007MenuState;
struct Ov007BgLayer;

/* The 0xc-byte layer slot func_ov007_020adc08 builds around one of the
 * display's BG layers. func_ov007_020b6d40 keeps the eight in
 * Ov007Work::layerSlots, func_ov007_020ad660 updates one per frame and
 * func_ov007_020adbc8 frees one. */
typedef struct Ov007LayerSlot {
    struct Ov007MenuState *state; /* 0x00: func_ov007_020c9388(0) */
    struct Ov007BgLayer *layer;   /* 0x04: the layer func_ov007_020c0638 scrolls */
    u8 kind;                      /* 0x08: func_ov007_020b6d40's slot index, 0..7 */
} Ov007LayerSlot;

typedef char Ov007LayerSlot_size_must_be_0xc[sizeof(Ov007LayerSlot) == 0xc ? 1 : -1];

#endif
