#include "Ov007Item.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c937c(void *state);

/* Frees an item: its menu state, then the record. */
void func_ov007_020aea4c(Ov007Item *item) {
    func_ov007_020c937c(item->state);
    func_ov007_020c3d1c(item);
}
