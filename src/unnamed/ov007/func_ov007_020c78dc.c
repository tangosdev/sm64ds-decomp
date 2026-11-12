#include "Ov007Spring.h"
#include "Ov007MenuMem.h"

/* func_ov007_020c8440 and func_ov007_020c8098 are long-branch veneers to
 * func_ov007_020c3d1c, the free hook. */
extern void func_ov007_020c8440(void *p);
extern void func_ov007_020c8098(void *p);

/* Frees a point list: its own followers (flags bit 0) and the items array,
 * then its links and the links array, then the list itself. */
void func_ov007_020c78dc(Ov007PointList *c) {
    int i;
    if (c->flags & 1) {
        for (i = 0; i < c->count; i++)
            func_ov007_020c8440(c->items[i]);
    }
    func_ov007_020c3d1c(c->items);
    if (c->count > 1) {
        int m;
        if (c->count <= 1)
            m = 0;
        else
            m = c->count - ((c->flags & 2) ? 0 : 1);
        for (i = 0; i < m; i++)
            func_ov007_020c8098(c->links[i]);
        func_ov007_020c3d1c(c->links);
    }
    func_ov007_020c3d1c(c);
}
