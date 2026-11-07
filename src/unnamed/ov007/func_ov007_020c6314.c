#include "Ov007Picker.h"

extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(const Vector3 *v);

/* Re-measures group idx's links: from restPos to each rest point (flag 0),
 * between their own end points (flag 1), or from restPos to each live point
 * (flag 2). */
void func_ov007_020c6314(Ov007Picker *self, int idx, int flag) {
    int i;
    Ov007Link *e;

    switch (flag) {
    case 0:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(&self->restPos, &self->groupPointCopies[idx][i], &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    case 1:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(e->from->target, e->to->target, &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    case 2:
        for (i = 0; i < self->groups[idx]->pointCount; i++) {
            e = self->groupLinks[idx][i];
            SubVec3(&self->restPos, &self->groupPoints[idx][i], &e->dir);
            e->length = LenVec3(&e->dir);
        }
        break;
    }
}
