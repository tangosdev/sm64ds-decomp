#include "Ov007Spring.h"

extern void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to);

/* Binds each link i to points i and i + 1 (the last wraps to point 0 when
 * the list is closed), for the four spring modes func_ov007_020c7560 runs. */
void func_ov007_020c7804(Ov007PointList *list) {
    int count = list->count;
    int n;
    int i;

    if (count <= 1) {
        n = 0;
    } else {
        n = count - !(list->flags & 2);
    }

    switch (list->springMode) {
    case 0:
    case 1:
    case 2:
    case 3:
        for (i = 0; i < n; i++) {
            int j;
            if (i < list->count - 1)
                j = i + 1;
            else
                j = 0;
            func_ov007_020c8010(list->links[i], list->items[i], list->items[j]);
        }
        break;
    default:
        break;
    }
}
