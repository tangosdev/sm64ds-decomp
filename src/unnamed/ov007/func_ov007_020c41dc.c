#include "Ov007Sprite.h"

/* Sizes sprite s for the camera (func_ov007_020c43bc) as if it sat base's
 * depth further away, then restores its own depth. NumberSprite_Create
 * passes an item view's sprite as base. */
void func_ov007_020c41dc(Ov007Sprite *base, Ov007Sprite *s, struct Ov007Camera *camera) {
    int *slot = &s->depth;
    int addend = base->depth;
    int value = *slot;
    int old = s->depth;

    *slot = value + addend;
    func_ov007_020c43bc(s, camera);
    s->depth = old;
}
