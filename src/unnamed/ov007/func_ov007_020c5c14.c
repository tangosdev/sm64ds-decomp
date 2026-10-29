#include "types.h"
#include "Ov007StrokePath.h"
/* One group point: a Vector3 (x, y, z), as func_ov007_020c2f14 is declared. */
struct Elem {
    int x, y, z;
};

extern void func_ov007_020bfe4c(void* p, int b, int c, int d, int* e);
extern void func_ov007_020c2f14(struct Elem* arr, u16 n, int* o0, int* o1, int* o2, int* o3, int e, int f);
extern int func_01ffaa34(s64 v, s64 w);

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
 * func_ov007_020b9ca8, only the slot this file reads. */
typedef struct Ov007Camera {
    char pad_00[0x2c];
    int distance; /* 0x2c: func_ov007_020bffb8 places the eye this far from the target */
} Ov007Camera;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in
 * func_ov007_020c5a6c and func_ov007_020c690c, only the slots this file
 * reads. The tag keeps the definition's spelling. */
struct Self {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x1c];
    struct Elem **groupPoints; /* 0x28: per group (func_ov007_020c6e68) */
    char pad_2c[0xc];
    Ov007StrokePath **groups; /* 0x38: committed strokes (func_ov007_020c6d20) */
    char pad_3c[0x6c];
    Ov007Camera *camera; /* 0xa8: func_ov007_020c65bc unprojects through it */
};

typedef char Self_camera_must_be_at_0xa8[(int)&((struct Self *)0)->camera == 0xa8 ? 1 : -1];

/* Writes the centre of all groups' points' x/y bounding box to out. */
void func_ov007_020c5c14(struct Self* self, int* out)
{
    int i;
    int minx, maxx, miny, maxy;
    int o0, o2, o1, o3;
    minx = 0x7fffffff;
    maxx = 0x80000000;
    miny = 0x7fffffff;
    maxy = 0x80000000;
    func_ov007_020bfe4c(self->camera, 0, 0, -self->camera->distance, out);
    for (i = 0; i < self->groupCount; i++) {
        struct Elem *arr = self->groupPoints[i];
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
