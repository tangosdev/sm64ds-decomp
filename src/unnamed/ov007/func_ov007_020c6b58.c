#include "Ov007Picker.h"

extern int func_ov007_020c31e0(int *xs, int *ys, int n, int x, int y, int maxDist, int *dist);
extern void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to);

/* Ties each group's ends to the other groups: the head link joins its first
 * point to the nearest point of any other group, the tail link its last
 * point. func_ov007_020c31e0 returns the point of a stroke nearest (x, y)
 * no farther than maxDist (any distance when it is negative) and stores
 * that distance, so each search narrows the next. */
void func_ov007_020c6b58(Ov007Picker *c) {
    int i;
    Ov007StrokePath *self;
    int j;
    int best1;
    int best2;
    Ov007StrokePath *other;
    int bj1;
    int bj2;
    int d0;
    int d1;
    int res;

    for (i = 0; i < c->groupCount; i++) {
        self = c->groups[i];
        best1 = -1;
        d0 = -1;
        d1 = -1;
        best2 = -1;
        for (j = 0; j < c->groupCount; j++) {
            other = c->groups[j];
            if (i != j) {
                res = func_ov007_020c31e0(other->xCoords, other->yCoords, other->pointCount,
                                          self->xCoords[0], self->yCoords[0], d0, &d0);
                if (res != -1) {
                    best1 = res;
                    bj1 = j;
                }
                res = func_ov007_020c31e0(other->xCoords, other->yCoords, other->pointCount,
                                          self->xCoords[self->pointCount - 1],
                                          self->yCoords[self->pointCount - 1], d1, &d1);
                if (res != -1) {
                    best2 = res;
                    bj2 = j;
                }
            }
        }
        if (best1 != -1) {
            c->headLinkGroups[i] = bj1;
            c->headLinkPoints[i] = best1;
            func_ov007_020c8010(c->headLinks[i], c->pointLists[i]->items[0],
                                c->pointLists[bj1]->items[best1]);
        }
        if (best2 != -1) {
            c->tailLinkGroups[i] = bj2;
            c->tailLinkPoints[i] = best2;
            func_ov007_020c8010(c->tailLinks[i], c->pointLists[i]->items[self->pointCount - 1],
                                c->pointLists[bj2]->items[best2]);
        }
    }
}
