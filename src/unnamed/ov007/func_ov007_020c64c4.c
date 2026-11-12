#include "Ov007Picker.h"
#include "math/CalcLib.h"

/* Re-measures every group's rest links (func_ov007_020c6314 with `b`); in
 * link mode also drops the anchor onto pos and re-measures `link`. */
void func_ov007_020c64c4(Ov007Picker *self, int b) {
    Ov007Link *link;
    int i;
    for (i = 0; i < self->groupCount; i++) {
        func_ov007_020c6314(self, i, b);
    }
    if (self->mode != 2)
        return;
    self->anchorPos = self->pos;
    link = self->link;
    VEC_Subtract(link->from->target, link->to->target, &link->dir);
    link->length = VEC_Mag(&link->dir);
}
