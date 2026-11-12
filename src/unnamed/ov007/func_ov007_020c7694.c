#include "Ov007Spring.h"

/* local extern: math/CalcLib.h is C++ only; this matches its declaration. */
extern void VEC_Subtract(Vector3 *, Vector3 *, Vector3 *);
/* local extern: as above */
extern int VEC_Mag(const Vector3 *);

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
            VEC_Subtract(&rest[i], &rest[j], &e->dir);
            e->length = VEC_Mag(&e->dir);
            i++;
        } while (i < cnt);
        break;
    }
}
