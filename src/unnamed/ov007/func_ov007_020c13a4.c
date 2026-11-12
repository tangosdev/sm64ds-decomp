#include "Ov007SpriteBank.h"
#include "Ov007MenuMem.h"

/* Allocates a cell animation's frame and duration tables for `frameCount`
 * frames. */
void func_ov007_020c13a4(Ov007CellAnim *anim, int frameCount, u16 offsetX, u16 offsetY) {
    anim->frames = func_ov007_020c3df4(0, frameCount * 4);
    anim->durations = func_ov007_020c3df4(0, frameCount * 2);
    anim->frameCount = frameCount;
    anim->offsetX = offsetX;
    anim->offsetY = offsetY;
}
