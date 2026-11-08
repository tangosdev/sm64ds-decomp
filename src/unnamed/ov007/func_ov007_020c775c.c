#include "Ov007Spring.h"

extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(const Vector3 *v);

/* Re-measures each link of the list between its followers' targets. */
void func_ov007_020c775c(Ov007PointList *list) {
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
        for (i = 0; i < cnt; i++) {
            Ov007Link *e = list->links[i];
            SubVec3(e->from->target, e->to->target, &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    }
}
