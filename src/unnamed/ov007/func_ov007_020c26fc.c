#include "Ov007StrokePath.h"

extern void func_ov007_020c3054(int *arr, int n, int *pmin, int *pmax);

/* Recomputes the stroke path's extrema: clears both index triples, then for a
 * planar path stores the indices of its smallest and largest X and Y. */
void func_ov007_020c26fc(Ov007StrokePath *path) {
    int mn0 = 0, mn1 = 0, mx0 = 0, mx1 = 0;
    path->minPointIndices[0] = path->minPointIndices[1] = path->minPointIndices[2] = 0;
    path->maxPointIndices[0] = path->maxPointIndices[1] = path->maxPointIndices[2] = 0;
    int x = path->representation;
    if (x != 0) {
        if (x == 1)
            return;
        return;
    }
    func_ov007_020c3054(path->xCoords, path->pointCount, &mn0, &mx0);
    func_ov007_020c3054(path->yCoords, path->pointCount, &mn1, &mx1);
    path->minPointIndices[0] = mn0;
    path->minPointIndices[1] = mn1;
    path->maxPointIndices[0] = mx0;
    path->maxPointIndices[1] = mx1;
}
