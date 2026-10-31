#include "Ov007Sprite.h"

extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);

/* Draws sprite `child` relative to sprite `parent`: child takes parent's
 * scale, colour and alpha, is offset by parent's screen position and depth
 * for the screen -> world pass through the camera, drawn with its world
 * position relative to parent's, and then has its own screen position and
 * depth restored. */
void func_ov007_020c4128(Ov007Sprite *parent, Ov007Sprite *child, struct Ov007Camera *camera) {
    int savedDepth, savedX, savedY;

    child->scaleX = parent->scaleX;
    child->scaleY = parent->scaleY;
    child->color = parent->color;
    child->alpha = parent->alpha;

    savedDepth = child->depth;
    savedX = child->screenX;
    savedY = child->screenY;

    child->screenX += parent->screenX;
    child->screenY += parent->screenY;
    child->depth += parent->depth;

    func_ov007_020c4388(child, camera);
    SubVec3((Vector3 *)&child->pos, (Vector3 *)&parent->pos, (Vector3 *)&child->pos);
    func_ov007_020c421c(child);

    child->screenX = savedX;
    child->screenY = savedY;
    child->depth = savedDepth;
}
