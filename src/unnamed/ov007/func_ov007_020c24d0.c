#include "Ov007StrokePath.h"

extern void func_ov007_020c2d44(Ov007StrokePath *path, int n);

/* The alternate representation stores one Vector3 per point. */

/* Removes point idx from the path, shifting every later point (and its
 * segment length and progress threshold) down one; a planar path then
 * recomputes the segment that now ends at idx. */
void func_ov007_020c24d0(Ov007StrokePath *path, int idx) {
    int *lengths;
    int *xs;
    int *ys;
    unsigned short *angles;
    int i;

    if (path->pointCount == 0)
        return;
    if (idx >= path->pointCount)
        return;
    path->pointCount--;
    if (idx == path->pointCount)
        return;

    /* The round trip through long long is load-bearing: it gives i its own
     * register apart from idx, which the plain copy and a for loop do not. */
    i = (int)(long long)idx;
    while (i < path->pointCount) {
        switch (path->representation) {
        case OV007_PATH_PLANAR:
            xs = path->xCoords;
            xs[i] = xs[i + 1];
            ys = path->yCoords;
            ys[i] = ys[i + 1];
            angles = path->segmentAngles;
            angles[i] = angles[i + 1];
            break;
        case 1:
            ((Vector3 *)path->pointStorage)[i] =
                ((Vector3 *)path->pointStorage)[i + 1];
            break;
        }
        lengths = path->segmentLengths;
        lengths[i] = lengths[i + 1];
        if (path->flags & OV007_PATH_HAS_PROGRESS)
            path->progressThresholds[i] = path->progressThresholds[i + 1];
        i++;
    }

    switch (path->representation) {
    case OV007_PATH_PLANAR:
        func_ov007_020c2d44(path, idx);
        break;
    case 1:
        break;
    }
}
