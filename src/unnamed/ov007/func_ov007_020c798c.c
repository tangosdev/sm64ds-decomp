#include "Ov007Spring.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c78b0(Ov007PointList *list);
extern Ov007Follower *func_ov007_020c844c(Vector3 *p, int q);
extern Ov007Link *func_ov007_020c80a4(void);

/* Builds a list of count points and the links between them. With flags bit
 * 0 it makes each point as a follower of mass pointMass (Q12); otherwise
 * the caller fills items in (func_ov007_020c73f0's column lists share the
 * row lists' points). */
Ov007PointList *func_ov007_020c798c(int springMode, int count, int pointMass, int flags) {
    int i;
    Ov007PointList *list;

    list = func_ov007_020c3df4(0, sizeof(Ov007PointList));
    func_ov007_020c78b0(list);
    list->springMode = springMode;
    list->count = count;
    list->flags = flags;
    list->items = func_ov007_020c3df4(0, list->count * 4);
    if (list->flags & 1) {
        for (i = 0; i < list->count; i++)
            list->items[i] = func_ov007_020c844c(0, pointMass);
    }
    if (count > 1) {
        int n;
        if (list->count <= 1)
            n = 0;
        else
            n = list->count - ((list->flags & 2) ? 0 : 1);
        list->links = func_ov007_020c3df4(0, n * 4);
        for (i = 0; i < n; i++)
            list->links[i] = func_ov007_020c80a4();
    }
    return list;
}
