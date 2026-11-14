#include "Ov007Picker.h"
#include "Ov007Camera.h"

extern void func_ov007_020bfe4c(Ov007Camera *camera, int x, int y, int z, void *out);
/* The index of the first point at each axis minimum and maximum. */
extern void func_ov007_020c2f14(void *points, int pointCount, int *outMinX, int *outMaxX,
                                int *outMinY, int *outMaxY, int *outMinZ, int *outMaxZ);
extern int func_01ffaa34(s64 v, s64 w);

/* Writes the centre of all groups' points' x/y bounding box to out, at the
 * camera's depth. */
void func_ov007_020c5c14(Ov007Picker *self, int *out) {
    int i;
    int minx, maxx, miny, maxy;
    int o0, o2, o1, o3;
    minx = 0x7fffffff;
    maxx = 0x80000000;
    miny = 0x7fffffff;
    maxy = 0x80000000;
    func_ov007_020bfe4c(self->camera, 0, 0, -self->camera->distance, out);
    for (i = 0; i < self->groupCount; i++) {
        Vector3 *arr = self->groupPoints[i];
        func_ov007_020c2f14(arr, self->groups[i]->pointCount, &o0, &o1, &o2, &o3, 0, 0);
        if (arr[o0].x < minx)
            minx = arr[o0].x;
        if (arr[o1].x > maxx)
            maxx = arr[o1].x;
        if (arr[o2].y < miny)
            miny = arr[o2].y;
        if (arr[o3].y > maxy)
            maxy = arr[o3].y;
    }
    out[0] = func_01ffaa34((s64)minx + (s64)maxx, 2);
    out[1] = func_01ffaa34((s64)miny + (s64)maxy, 2);
}
