#include "Ov007Mesh.h"

/* Pushes grid point (i, j) along z, lerping pushMin..pushMax by t (Q12),
 * unless the point is held; func_ov007_020bed60's pokes. */
void func_ov007_020ca578(Ov007Mesh *mesh, int i, int j, int t) {
    Ov007Follower *lr;
    int lo, hi;
    int val;

    lr = mesh->grid->rowLists[j]->items[i];

    if (lr->flags & 1)
        return;

    hi = mesh->params.pushMax;
    lo = mesh->params.pushMin;

    if (t >= 0x1000) {
        val = hi;
    } else if (t <= 0) {
        val = lo;
    } else {
        val = (((s64)(0x1000 - t) * lo + (s64)t * hi) >> 0xc);
    }

    lr->force.z += val;
}
