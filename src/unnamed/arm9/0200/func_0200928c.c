#include "types.h"

struct dCamera_c;

extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    u8 pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    Vector3 pos;    /* 0x08c */
    u8 pad_098[0xe4];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
} CameraView;

void func_0200928c(struct dCamera_c *cam)
{
    CameraView *view = (CameraView *)cam;
    short h = Vec3_HorzAngle(&view->lookAt, &view->pos);
    view->mAngleY = h;

    short v = Vec3_VertAngle(&view->lookAt, &view->pos);
    view->mAngleX = v;
}
