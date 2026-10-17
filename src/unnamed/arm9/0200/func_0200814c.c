#include "nitro/fx/fx.h"
/* func_0200814c at 0x0200814c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef struct Vec3 { int x, y, z; } Vec3;

extern struct dActor_c *func_0200e6d8(unsigned int arg0);
extern void Vec3_RotateYAndTranslate(Vec3 *out, const Vec3 *trans, short ang, const Vec3 *add);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    Vec3 pos;    /* 0x08c */
} CameraView;

int func_0200814c(int self)
{
    CameraView *cam = (CameraView *)self;
    struct dActor_c *a;
    int diff, absdiff, val;
    Vec3 v;

    a = func_0200e6d8(0);
    diff = *(int *)((char *)a + 0x60) - cam->pos.y;
    v.x = 0;
    v.y = 0x50000;
    v.z = diff;
    absdiff = diff < 0 ? -diff : diff;
    val = FX_MUL(absdiff, -0x199);
    v.z = val;
    if (val > -0x64000)
        v.z = -0x64000;
    if (diff > -0x300000)
        Vec3_RotateYAndTranslate(&cam->lookAt, (const Vec3 *)((char *)a + 0x5c),
                                 *(short *)((char *)a + 0x8e), &v);
    return 1;
}
