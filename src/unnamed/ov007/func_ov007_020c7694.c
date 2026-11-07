#include "Ov007Spring.h"

extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(const Vector3 *v);

/* Sets each link's rest offset and length from the rest points: link i
 * runs from rest[i] to rest[i + 1], and in a closed list (flags bit 1) the
 * last one wraps to rest[0]. */
void func_ov007_020c7694(Ov007PointList *list, Vector3 *rest) {
    int n;
    int cnt;
    int i;

    n = list->count;
    if (n <= 1) {
        cnt = 0;
    } else {
        cnt = n - ((list->flags & 2) ? 0 : 1);
    }

    switch (list->springMode) {
    case 0:
    case 1:
    case 2:
    case 3:
        i = 0;
        if (cnt <= 0)
            return;
        do {
            int j = (i < list->count - 1) ? i + 1 : 0;
            Ov007Link *e = list->links[i];
            SubVec3(&rest[i], &rest[j], &e->dir);
            e->length = LenVec3(&e->dir);
            i++;
        } while (i < cnt);
        break;
    }
}
