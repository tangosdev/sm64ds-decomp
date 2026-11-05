#include "Ov007Picker.h"

extern int func_ov007_020c3ba8(int n);

/* Jitters the rest offsets (dir x/y) of the groups' per-point links. Each
 * link is picked with chance percent / 100 (func_ov007_020c3ba8(1000) <
 * percent * 10) and given an offset in [-rangeX, rangeX) x [-rangeY,
 * rangeY), times scale. mode 0 sets dir to it, 1 adds it to dir, 2 sets dir
 * to it plus restPos minus the point's rest copy (groupPointCopies). The
 * rest copy is reached by byte offset: indexing it does not match. */
void func_ov007_020c56bc(char *sl, int mode, int percent, int rangeX, int rangeY, int scale) {
    Ov007Picker *self = (Ov007Picker *)sl;
    int i, j;
    int threshold, minX, spanX, minY, spanY;
    Vector3 *dir;
    int dx;
    int offset;

    i = 0;
    if (self->groupCount <= 0)
        return;

    threshold = percent * 10 - 1;
    minX = -rangeX;
    minY = -rangeY;
    spanX = rangeX - minX;
    spanY = rangeY - minY;

    do {
        j = 0;
        if (self->pointLists[i]->count > 0) {
            offset = 0;
            do {
                int dy;

                if (func_ov007_020c3ba8(1000) <= threshold) {
                    dir = &self->groupLinks[i][j]->dir;
                    dx = scale * (minX + func_ov007_020c3ba8(spanX));
                    dy = scale * (minY + func_ov007_020c3ba8(spanY));

                    switch (mode) {
                    case 0:
                        dir->x = dx;
                        dir->y = dy;
                        break;
                    case 1:
                        dir->x += dx;
                        dir->y += dy;
                        break;
                    case 2:
                        dir->x =
                            dx + (self->restPos.x -
                                  ((Vector3 *)((char *)self->groupPointCopies[i] + offset))->x);
                        dir->y =
                            dy + (self->restPos.y -
                                  ((Vector3 *)((char *)self->groupPointCopies[i] + offset))->y);
                        break;
                    }
                }

                offset += 0xc;
                j++;
            } while (j < self->pointLists[i]->count);
        }
        i++;
    } while (i < self->groupCount);
}
