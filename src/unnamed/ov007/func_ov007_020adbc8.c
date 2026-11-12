#include "Ov007BgLayer.h"
#include "Ov007LayerSlot.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c937c(void *state);

/* Frees a layer slot: kinds 1 and 5 own the 0x800-byte screen buffer
 * func_ov007_020adc08 allocated for them; then the state and the record. */
void func_ov007_020adbc8(Ov007LayerSlot *slot) {
    unsigned char kind = slot->kind;
    if (kind == 1 || kind == 5)
        func_ov007_020c3d1c(slot->layer->screen);
    func_ov007_020c937c(slot->state);
    func_ov007_020c3d1c(slot);
}
