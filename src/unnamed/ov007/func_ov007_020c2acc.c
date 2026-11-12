#include "Ov007StrokePath.h"

extern Ov007StrokePath *func_ov007_020c2c68(int a, int b, int c);
extern void func_ov007_020c2d44(Ov007StrokePath *path, int i);

Ov007StrokePath *func_ov007_020c2b9c(int pointCount, const int *progressThresholds) {
    int flags = 0;
    Ov007StrokePath *path;
    if (progressThresholds != 0)
        flags |= OV007_PATH_HAS_PROGRESS;
    path = func_ov007_020c2c68(OV007_PATH_PLANAR, pointCount, flags);
    if ((flags & OV007_PATH_HAS_PROGRESS) != 0) {
        int i;
        for (i = 0; i < pointCount; i++) {
            path->progressThresholds[i] = progressThresholds[i];
        }
    }
    path->pointCount = (u16)pointCount;
    return path;
}

Ov007StrokePath *func_ov007_020c2b38(int pointCount, const int *xCoords, const int *yCoords,
                                     const int *progressThresholds) {
    Ov007StrokePath *path = func_ov007_020c2b9c(pointCount, progressThresholds);
    int i;
    for (i = 0; i < pointCount; i++) {
        path->xCoords[i] = xCoords[i];
        path->yCoords[i] = yCoords[i];
        func_ov007_020c2d44(path, i);
    }
    return path;
}

Ov007StrokePath *func_ov007_020c2acc(int pointCount, const unsigned char *xCoords,
                                     const unsigned char *yCoords, const int *progressThresholds) {
    Ov007StrokePath *path = func_ov007_020c2b9c(pointCount, progressThresholds);
    int i;
    for (i = 0; i < pointCount; i++) {
        path->xCoords[i] = xCoords[i] << 12;
        path->yCoords[i] = yCoords[i] << 12;
        func_ov007_020c2d44(path, i);
    }
    return path;
}
