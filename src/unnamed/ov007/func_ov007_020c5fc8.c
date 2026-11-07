#include "types.h"
/* recovered: shared common types */
#include "common.h"
#include "Ov007StrokePath.h"
#include "Ov007Picker.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(const Vector3 *v);

/* Re-measures group idx's links at a pose blended between groupPoints
 * (weight t, Q12) and groupPointCopies: x and y blended, z from the copies.
 * Each link gets the offset between consecutive blended points as its dir,
 * and that offset's length. */
void func_ov007_020c5fc8(Ov007Picker *self, int idx, int t) {
    int i;
    int inv;
    struct Vector3 cur;
    struct Vector3 prev;

    i = 0;
    if ((int)self->groups[idx]->pointCount <= 0)
        return;

    inv = 0x1000 - t;
    do {
        struct Vector3 *a = self->groupPoints[idx];
        struct Vector3 *b = self->groupPointCopies[idx];
        int va = a[i].x;
        int vb = b[i].x;
        int r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.x = r;

        a = self->groupPoints[idx];
        b = self->groupPointCopies[idx];
        va = a[i].y;
        vb = b[i].y;
        r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.y = r;

        cur.z = self->groupPointCopies[idx][i].z;

        if (i > 0) {
            Ov007Link *link = self->pointLists[idx]->links[i - 1];
            SubVec3(&prev, &cur, &link->dir);
            link->length = LenVec3(&link->dir);
        }

        prev = cur;

        i++;
    } while (i < (int)self->groups[idx]->pointCount);
}
