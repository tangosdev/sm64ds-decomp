#include "Ov007Picker.h"

extern void func_ov007_020c7694(Ov007PointList *list, Vector3 *rest);
extern void func_ov007_020c775c(Ov007PointList *list);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(const Vector3 *v);

/* Re-measures group idx's own links and, with more than one group, its head
 * and tail links. With flag set it measures from the rest copies
 * (groupPointCopies): the head link from the group's first point to the
 * point headLinkGroups/headLinkPoints names, the tail link from its last
 * point. Without it, every link is measured between its followers. */
void func_ov007_020c6174(Ov007Picker *t, int idx, int flag) {
    Ov007Link *e;
    if (flag) {
        func_ov007_020c7694(t->pointLists[idx], t->groupPointCopies[idx]);
        if (t->groupCount <= 1)
            return;
        e = t->headLinks[idx];
        SubVec3(t->groupPointCopies[idx],
                &t->groupPointCopies[t->headLinkGroups[idx]][t->headLinkPoints[idx]], &e->dir);
        e->length = LenVec3(&e->dir);
        e = t->tailLinks[idx];
        SubVec3(&t->groupPointCopies[idx][t->groups[idx]->pointCount - 1],
                &t->groupPointCopies[t->tailLinkGroups[idx]][t->tailLinkPoints[idx]], &e->dir);
        e->length = LenVec3(&e->dir);
    } else {
        func_ov007_020c775c(t->pointLists[idx]);
        if (t->groupCount <= 1)
            return;
        e = t->headLinks[idx];
        SubVec3(e->from->target, e->to->target, &e->dir);
        e->length = LenVec3(&e->dir);
        e = t->tailLinks[idx];
        SubVec3(e->from->target, e->to->target, &e->dir);
        e->length = LenVec3(&e->dir);
    }
}
