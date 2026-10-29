//cpp
#include "types.h"
#include "math/Approach.h"

extern "C" {

/* File-local view of dCamera_c (include/dCamera_c.h is C++), with its names. */
typedef struct {
    unsigned char pad_000[0xb0];
    Vector3 savedLookAt; /* 0x0b0 */
    Vector3 savedPos;    /* 0x0bc */
    unsigned char pad_0c8[0xac];
    /* func_02007414 zeroes it; this state ramps it up to 0xf000 and paces
       the saved pair by it. */
    s32 mRampSpeed; /* 0x174 */
} CameraView;

#ifndef SM64DS_PLATFORM_PC
typedef char
    CameraView_ramp_speed_must_be_at_0x174[(int)&((CameraView *)0)->mRampSpeed == 0x174 ? 1 : -1];
#endif

int func_020072c4(char *self)
{
    CameraView *cam = (CameraView *)self;
    ApproachLinear(cam->mRampSpeed, 0xf000, 0x199);
    ApproachLinear(cam->savedLookAt.x, 0x7d0000, cam->mRampSpeed);
    ApproachLinear(cam->savedLookAt.y, 0x4b0000, cam->mRampSpeed / 10);
    ApproachLinear(cam->savedLookAt.z, 0x3e8000, cam->mRampSpeed / 10);
    ApproachLinear(cam->savedPos.x, 0, cam->mRampSpeed);
    ApproachLinear(cam->savedPos.y, 0x4b0000, cam->mRampSpeed >> 1);
    ApproachLinear(cam->savedPos.z, 0x3e8000, cam->mRampSpeed * 10 / 15);
    return 1;
}
}
