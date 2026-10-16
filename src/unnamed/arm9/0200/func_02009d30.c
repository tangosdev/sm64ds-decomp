#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x8e];
    s16 mAngleY; /* 0x08e */
};

struct dCamera_c {
    char pad_000[0x110];
    struct dActor_c *mTargetPlayer; /* 0x110 */
    char pad_114[0x13c - 0x114];
    s32 mState_13c; /* 0x13c */
    char pad_140[0x154 - 0x140];
    u32 mFlags; /* 0x154 */
    char pad_158[0x186 - 0x158];
    s16 mTargetAngleY; /* 0x186 */
    char pad_188[0x1a6 - 0x188];
    u8 mTurningToTarget; /* 0x1a6 */
};

extern int data_0208729c;

int func_02009d30(int *p)
{
    struct dCamera_c *cam = (struct dCamera_c *)p;
    int one = 1;

    cam->mState_13c = (int)&data_0208729c;

    cam->mTurningToTarget = (unsigned char)one;

    {
        struct dActor_c *player = cam->mTargetPlayer;
        short h = player->mAngleY;
        cam->mTargetAngleY = (short)(h + 0x8000);
        cam->mFlags &= ~0x100u;
    }

    return one;
}
