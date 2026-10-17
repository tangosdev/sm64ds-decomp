#include "types.h"

struct dCamera_c;

extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern short AngleDiff(short a, short b);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    u8 pad_000[0x8c];
    Vector3 pos; /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    Vector3 mTargetPos; /* 0x098 */
} CameraView;

void func_020071a8(struct dCamera_c *cam, short ang, int threshold)
{
    CameraView *view = (CameraView *)cam;
    short diff = AngleDiff(Vec3_HorzAngle(&view->mTargetPos, &view->pos), ang);
    /* 0x19a stays raw: its writers store yaws, but no reader shows its role. */
    if (diff > threshold)
        *(short *)((char *)cam + 0x19a) = ang;
}
