#include "Ov007Picker.h"

extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);

/* Moves the picked point to src: its follower's velocity and force are
 * cleared and its x/y set. Then mode 0 moves pos to src, mode 1 moves it
 * to src plus hitOffset clamped to the bounds on x and y, and mode 2 moves
 * anchorPos to src. */
void func_ov007_020c5948(Ov007Picker *s, Vector3 *src) {
    int i;
    int j;
    Ov007Follower *f;
    Vector3 *vec;
    int mode;
    Vector3 *dst;
    Vector3 *min;
    Vector3 *max;

    i = s->hitGroup;
    if (i == -1)
        return;
    j = s->hitIndex;
    if (j == -1)
        return;

    f = s->pointLists[i]->items[j];
    vec = f->target;

    f->force.z = 0;
    f->force.y = f->force.z;
    f->force.x = f->force.y;
    f->velocity.z = f->force.x;
    f->velocity.y = f->velocity.z;
    f->velocity.x = f->velocity.y;

    vec->x = src->x;
    vec->y = src->y;

    mode = s->mode;
    switch (mode) {
    case 0:
        s->pos = *src;
        break;
    case 1:
        dst = &s->pos;
        min = &s->boundsMin;
        max = &s->boundsMax;
        AddVec3((Vector3 *)src, (Vector3 *)&s->hitOffset, (Vector3 *)dst);
        if (min->x > dst->x)
            dst->x = min->x;
        if (max->x < dst->x)
            dst->x = max->x;
        if (min->y > dst->y)
            dst->y = min->y;
        if (max->y < dst->y)
            dst->y = max->y;
        break;
    case 2:
        s->anchorPos = *src;
        break;
    }
}
