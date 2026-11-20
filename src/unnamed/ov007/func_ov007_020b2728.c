#include "Ov007Work.h"
#include "Ov007Sprite.h"

/* Draws the three screen sprites, each with its drop shadow
 * (func_ov007_020c421c). */
void func_ov007_020b2728(void) {
    int i;
    for (i = 0; i < 3; i++) {
        func_ov007_020c421c(gMenuWork->screenSprites[i]);
    }
}
