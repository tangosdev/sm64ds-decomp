#include "Ov007Sprite.h"

typedef struct { int x, y, z; } Vec3;

extern void func_ov007_020c4388(char* r0, int r1);
extern void SubVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void func_ov007_020c421c(char *r4);

/* Draws sprite `dest` relative to sprite `self`: dest takes self's scale,
 * colour and alpha, is offset by self's screen position for the screen ->
 * world pass, drawn with its world position relative to self's, and then has
 * its own screen position restored. */
void func_ov007_020c4128(char *self, char *dest, int arg2)
{
    Ov007Sprite *parent = (Ov007Sprite *)self;
    Ov007Sprite *child = (Ov007Sprite *)dest;
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

    func_ov007_020c4388(dest, arg2);
    SubVec3((Vec3 *)&child->pos, (Vec3 *)&parent->pos, (Vec3 *)&child->pos);
    func_ov007_020c421c(dest);

    child->screenX = savedX;
    child->screenY = savedY;
    child->depth = savedDepth;
}
