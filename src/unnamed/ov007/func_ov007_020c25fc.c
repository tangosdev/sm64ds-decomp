#include "Ov007StrokePath.h"

extern int func_ov007_020c2e90(Ov007StrokePath *path, int *a, int *b, int minLen, int maxLen);
extern void func_ov007_020c2d44(Ov007StrokePath *path, int n);
extern int func_ov007_020c2dfc(Ov007StrokePath *path, int idx, int angle);
extern void func_ov007_020c24d0(Ov007StrokePath *path, int idx);

/* Appends point (x, y) to a planar path when func_ov007_020c2e90 accepts its
 * distance from the last point, and computes its segment. If the path
 * then turns by less than minTurn degrees at the previous point
 * (func_ov007_020c2dfc), that point is dropped. Returns whether the point
 * was added. */
int func_ov007_020c25fc(Ov007StrokePath *path, int x, int y, int minLen, int maxLen, int minTurn) {
    int ok = 1;
    if (path->pointCount != 0) {
        int point[3], last[3];
        point[0] = x;
        point[1] = y;
        last[0] = path->xCoords[path->pointCount - 1];
        last[1] = path->yCoords[path->pointCount - 1];
        last[2] = 0;
        point[2] = 0;
        ok = func_ov007_020c2e90(path, point, last, minLen, maxLen);
    }
    if (ok != 0) {
        path->xCoords[path->pointCount] = x;
        path->yCoords[path->pointCount] = y;
        func_ov007_020c2d44(path, path->pointCount);
        path->pointCount++;
        if (func_ov007_020c2dfc(path, path->pointCount - 1, minTurn) != 0)
            func_ov007_020c24d0(path, path->pointCount - 2);
        return 1;
    }
    return 0;
}
