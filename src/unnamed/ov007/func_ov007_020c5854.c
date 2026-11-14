#include "Ov007Picker.h"

extern int func_ov007_020c704c(Ov007Follower *f, Vector3 *target, int pull, int reach,
                               int snapRadius);

inline long long mul_add(long long a, int b, long long c) {
    return a * b + c;
}

/* Eases the picker's pos x/y toward target by scale (Q12) and sets its z,
 * then pulls every point of every group toward target with
 * func_ov007_020c704c: a point within snapRadius snaps onto it, one within
 * reach is pulled with strength pull. Returns how many points snapped. */
int func_ov007_020c5854(Ov007Picker *self, Vector3 *target, int pull, int snapRadius, int reach,
                        int scale) {
    long long v;
    int i, j, cnt = 0;
    v = mul_add(scale, target->x, ((long long)(0x1000 - scale)) * self->pos.x);
    self->pos.x = (int)(v >> 12);
    v = mul_add(scale, target->y, ((long long)(0x1000 - scale)) * self->pos.y);
    self->pos.y = (int)(v >> 12);

    self->pos.z = target->z;
    for (i = 0; i < self->groupCount; i++) {
        j = 0;
        if (self->pointLists[i]->count > 0) {
            do {
                if (func_ov007_020c704c(self->pointLists[i]->items[j], target, pull, reach,
                                        snapRadius) == 2)
                    cnt++;
                j++;
            } while (j < self->pointLists[i]->count);
        }
    }
    return cnt;
}
