#include "nitro/fx/fx.h"
#include "Ov007Picker.h"
#include "Matrix2x2.h"

/* Applies the 2x2 Q12 matrix m to every rest point's x/y about restPos. */
void func_ov007_020c5490(char *obj, struct Matrix2x2 *m) {
    Ov007Picker *self = (Ov007Picker *)obj;
    int i;
    int j;
    int restX, restY;
    VecFx32 *e;
    int x, y;
    int dx, dy;

    i = 0;
    if (self->groupCount <= 0)
        return;

    do {
        j = 0;
        if ((int)self->groups[i]->pointCount > 0) {
            do {
                e = &self->groupPointCopies[i][j];
                restX = self->restPos.x;
                x = e->x;
                restY = self->restPos.y;
                y = e->y;
                dx = x - restX;
                dy = y - restY;
                e->x = (FX_MUL(m->m00, dx) + FX_MUL(m->m01, dy)) + restX;
                j++;
                e->y = (FX_MUL(m->m10, dx) + FX_MUL(m->m11, dy)) + self->restPos.y;
            } while (j < self->groups[i]->pointCount);
        }
        i++;
    } while (i < self->groupCount);
}
