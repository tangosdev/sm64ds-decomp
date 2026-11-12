#include "Ov007Spring.h"
#include "math/CalcLib.h"

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
            VEC_Subtract(e->from->target, e->to->target, &e->dir);
            e->length = VEC_Mag(&e->dir);
        }
        break;
    }
}
