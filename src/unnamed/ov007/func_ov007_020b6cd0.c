#include "Ov007Work.h"

extern void func_ov007_020ade14(struct Ov007ItemNode *node);
extern void func_ov007_020adbc8(void *slot);
extern void func_ov007_020b73b4(struct Ov007Display *d);

/* Frees the nine item nodes, the eight layer slots and the display record. */
void func_ov007_020b6cd0(void) {
    int i;
    for (i = 0; i < 9; i++) {
        func_ov007_020ade14(gMenuWork->itemNodes[i]);
    }
    for (i = 0; i < 8; i++) {
        func_ov007_020adbc8(gMenuWork->layerSlots[i]);
    }
    func_ov007_020b73b4(gMenuWork->display);
}
