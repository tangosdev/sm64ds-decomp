#include "types.h"

/* A point list as func_ov007_020c798c builds it; only the count is read. */
struct Ov007PointList {
    int items;
    int count; /* 0x4 */
};

/* A spring between two points, as func_ov007_020c5a6c spells it. */
struct Ov007Link {
    int from;    /* 0x0 */
    int to;      /* 0x4 */
    int length;  /* 0x8 */
    Vector3 dir; /* 0xc */
};

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
struct Ov007Picker {
    int _0[2];
    int groupCount; /* 0x08 */
    int _c[8];
    Vector3 **groupPointCopies; /* 0x2c */
    int _30;
    struct Ov007Link ***groupLinks; /* 0x34: per group, one link per point from posPoint */
    int _38;
    struct Ov007PointList **pointLists; /* 0x3c */
    int _40[15];
    Vector3 restPos; /* 0x7c */
};

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
                q[0] = (int)((M1 * dx + 0x800) >> 12);
                q[1] = (int)((M2 * dy + 0x800) >> 12);
                j++;
            } while (j < self->pointLists[i]->count);
        }
        i++;
    } while (i < self->groupCount);
}
