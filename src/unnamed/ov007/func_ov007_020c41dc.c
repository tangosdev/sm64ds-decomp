#include "Ov007Sprite.h"

extern void func_ov007_020c43bc(void *self, int arg);

/* Sizes sprite a1 (func_ov007_020c43bc) as if it sat a0's depth further
 * away, then restores its own depth. func_ov007_020b42d8 passes an item
 * view's sprite as a0. The parameters keep the tree's char * spelling;
 * `base` and `s` are the typed views. */
void func_ov007_020c41dc(char *a0, char *a1, int a2)
{
    Ov007Sprite *base = (Ov007Sprite *)a0;
    Ov007Sprite *s = (Ov007Sprite *)a1;
    int *slot = &s->depth;
    int addend = base->depth;
    int value = *slot;
    int old = s->depth;

    *slot = value + addend;
    func_ov007_020c43bc(a1, a2);
    s->depth = old;
}
