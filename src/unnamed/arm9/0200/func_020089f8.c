#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
};

struct dCamera_c {
    char pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    char pad_08c[0x98 - 0x8c];
    Vector3 mTargetPos;    /* 0x098 */
    Vector3 mLookAtOffset; /* 0x0a4 */
    char pad_0b0[0x110 - 0xb0];
    struct dActor_c *mTargetPlayer; /* 0x110 */
    char pad_114[0x130 - 0x114];
    s32 mFocusBlend; /* 0x130 */
};

extern void Vec3_Sub(int *dest, const int *src1, const int *src2);

void func_020089f8(int *obj)
{
    struct dCamera_c *cam = (struct dCamera_c *)obj;
    struct dActor_c *player;
    Vector3 *p;
    int tmp[3];

    player = cam->mTargetPlayer;
    if (player == 0) {
        return;
    }
    p = &player->mPos;
    cam->mTargetPos.x = p->x;
    cam->mTargetPos.y = p->y;
    cam->mTargetPos.z = p->z;
    Vec3_Sub(tmp, &cam->lookAt.x, &cam->mTargetPos.x);
    cam->mLookAtOffset.x = tmp[0];
    cam->mLookAtOffset.y = tmp[1];
    cam->mLookAtOffset.z = tmp[2];
    cam->mFocusBlend = 0;
}
