#include "types.h"
extern int func_020092c4(void *a, void *b, int c);
extern void func_0200928c(void *cam);
extern int ApproachAngle(void *p, int target, int a, int b, int c);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    u8 pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    u8 pad_08c[0xf0];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
} CameraView;

int func_020091f8(void *a, int b, int c, int d)
{
    CameraView *f = (CameraView *)a;
    int r4;
    int t;
    r4 = (func_020092c4(a, &f->lookAt, b) != 1);
    func_0200928c(a);
    t = ApproachAngle(&f->mAngleY, c, 8, 0x4000, 0x80);
    if (t != 0) r4 = 1;
    t = ApproachAngle(&f->mAngleX, d, 8, 0x4000, 0x80);
    return (r4 | t) == 0;
}
