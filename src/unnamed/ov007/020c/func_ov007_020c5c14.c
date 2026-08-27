#include "types.h"
struct Elem { int a, b, c; };

extern void func_ov007_020bfe4c(void* p, int b, int c, int d, int* e);
extern void func_ov007_020c2f14(struct Elem* arr, u16 n, int* o0, int* o1, int* o2, int* o3, int e, int f);
extern int func_01ffaa34(s64 v, s64 w);

struct Self {
    char pad8[8];
    int count;       /* 0x8 */
    char pad[0x28-0xc];
    struct Elem** f28;   /* 0x28 */
    char pad2[0x38-0x2c];
    void** f38;          /* 0x38 */
    char pad3[0xa8-0x3c];
    int* fa8;            /* 0xa8 */
};

void func_ov007_020c5c14(struct Self* self, int* out)
{
    int i;
    int minx, maxx, miny, maxy;
    int o0, o2, o1, o3;
    minx = 0x7fffffff;
    maxx = 0x80000000;
    miny = 0x7fffffff;
    maxy = 0x80000000;
    func_ov007_020bfe4c(self->fa8, 0, 0, -self->fa8[0x2c / 4], out);
    for (i = 0; i < self->count; i++) {
        struct Elem* arr = self->f28[i];
        func_ov007_020c2f14(arr, *(u16*)((char*)self->f38[i] + 8), &o0, &o1, &o2, &o3, 0, 0);
        if (arr[o0].a < minx) minx = arr[o0].a;
        if (arr[o1].a > maxx) maxx = arr[o1].a;
        if (arr[o2].b < miny) miny = arr[o2].b;
        if (arr[o3].b > maxy) maxy = arr[o3].b;
    }
    out[0] = func_01ffaa34((s64)minx + (s64)maxx, 2);
    out[1] = func_01ffaa34((s64)miny + (s64)maxy, 2);
}
