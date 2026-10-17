#include "types.h"

struct dCamera_c;

extern int data_02086ec0[];
extern void Vec3_RotateYAndTranslate(void *src, void *dst, short angle, void *unk);

/* File-local views (the class headers are C++): only the fields this state
 * touches, under include/dCamera_c.h's and include/dActor_c.h's names. */
typedef struct {
    u8 pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
    u8 pad_068[0x26];
    short mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    u8 pad_000[0xb0];
    Vector3 savedLookAt; /* 0x0b0 */
    u8 pad_0bc[0x54];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_0200762c(struct dCamera_c *self)
{
    CameraView *cam = (CameraView *)self;
    int stack[3];
    stack[0] = data_02086ec0[0];
    stack[1] = data_02086ec0[1];
    stack[2] = data_02086ec0[2];

    short angle = cam->mTargetPlayer->mAngleY;
    angle = (short)(angle + 0x8000);

    Vec3_RotateYAndTranslate(&cam->savedLookAt, &cam->mTargetPlayer->mPos, angle, (void *)stack);

    cam->savedLookAt.y = 0x1c9000;
    return 1;
}
