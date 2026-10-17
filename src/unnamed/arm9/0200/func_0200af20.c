// @symbol func_0200af20
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
typedef long long s64;


extern signed char data_0209f2f8;
extern short data_02082214[];
extern char data_0209f43c[];

extern short Vec3_HorzAngle(struct Vector3 *v0, struct Vector3 *v1);
/* local extern: dClipper::Func_020156DC takes a by-value Fix12<int> (the Fix12 wall), so it stays spelled out with scalar arguments. */
extern void _ZN8dClipper13Func_020156DCEitii(void *self, int a, int b, int c, int d);

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * helper touches. */
typedef struct {
    u8 pad_000[0x98];
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    struct Vector3 mTargetPos; /* 0x098 */
    u8 pad_0a4[0x54];
    s32 mAspect; /* 0x0f8 */
    s32 mNear;   /* 0x0fc */
    s32 mFar;    /* 0x100 */
    u8 pad_104[0x44];
    struct Vector3 *mFixedViewPos; /* 0x148 */
    u8 pad_14c[0x8];
    u32 mFlags; /* 0x154 */
} CameraView;

void func_0200af20(char *c, struct Vector3 *v1, struct Vector3 *v2, short *out)
{
    CameraView *cam = (CameraView *)c;
    struct Vector3 *f = cam->mFixedViewPos;
    int *b;
    unsigned short ang;
    int idx;
    int y;
    int z;

    if (f == 0)
        return;

    b = 0;
    {
        signed char g = data_0209f2f8;
        if (g == 6) {
            b = data_02086f38;
        } else if (g == 0x15) {
            if (cam->mTargetPos.y < 0)
                return;
            b = data_02086f48;
        }
    }

    z = f->z;
    y = f->y;
    y = y + cam->mTargetPos.y;
    v1->x = f->x;
    v1->y = y >> 1;
    v1->z = z;
    ang = Vec3_HorzAngle(v1, &cam->mTargetPos);
    idx = (ang >> 4) * 2;

    v2->y = cam->mTargetPos.y + 0x8f7690;
    v2->x = cam->mTargetPos.x + FX_MUL(data_02082214[idx], 0x15a6f80);
    v2->z = cam->mTargetPos.z + FX_MUL(data_02082214[idx + 1], 0x15a6f80);

    if (b != 0) {
        {
            int lo = b[1];
            int hi = b[0];
            int x = v2->x;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->x = lo;
        }
        {
            int lo = b[3];
            int hi = b[2];
            int x = v2->z;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->z = lo;
        }
    }

    *out = 0xe38;
    _ZN8dClipper13Func_020156DCEitii(data_0209f43c, cam->mAspect, *out, cam->mNear, cam->mFar);
    cam->mFlags &= ~1;
}
