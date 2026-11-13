#include "types.h"
#include "Ov007Picker.h"

/* Sets each point's link direction to its rest offset from restPos, scaled
 * by m1 on x and m2 on y (Q12). */
void func_ov007_020c55bc(struct Ov007Picker *self, int m1, int m2) {
    int i;
    int j;
    int restX, restY;
    int x, y;
    int dx, dy;
    struct Ov007Link *p;
    int *q;
    long long M1, M2;

    i = 0;
    if (self->groupCount <= 0)
        return;

    M1 = m1;
    M2 = m2;

    do {
        j = 0;
        if (self->pointLists[i]->count > 0) {
            do {
                p = self->groupLinks[i][j];
                q = &p->dir.x;
                restX = self->restPos.x;
                restY = self->restPos.y;
                x = self->groupPointCopies[i][j].x;
                y = self->groupPointCopies[i][j].y;
                dx = restX - x;
                dy = restY - y;

                /* Not FX_MUL: its (fx64) recast of the s64 M1/M2 moves a register (6 words differ). */
                q[0] = ((M1 * dx + 0x800) >> 12);
                q[1] = ((M2 * dy + 0x800) >> 12);
                j++;
            } while (j < self->pointLists[i]->count);
        }
        i++;
    } while (i < self->groupCount);
}
