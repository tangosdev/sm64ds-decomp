#include "Ov007Item.h"

/* Puts the item at its layout row's home position. */
void func_ov007_020ae810(Ov007Item *item) {
    item->posX = item->layout->homeX << 12;
    item->posY = item->layout->homeY << 12;
}
