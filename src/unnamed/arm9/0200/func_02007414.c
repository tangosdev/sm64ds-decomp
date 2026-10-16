#include "types.h"

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x60];
    s32 mPosY; /* 0x060 */
};

struct dCamera_c {
    char pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    char pad_08c[0xb0 - 0x8c];
    Vector3 savedLookAt; /* 0x0b0 */
    Vector3 savedPos;    /* 0x0bc */
    char pad_0c8[0x110 - 0xc8];
    struct dActor_c *mTargetPlayer; /* 0x110 */
    char pad_114[0x170 - 0x114];
    s32 mScriptParam; /* 0x170 */
    s32 mRampSpeed;   /* 0x174 */
};

/* Script op: clears savedLookAt, sets savedPos to (0, lookAt's height above
   the target player, 0), and clears mScriptParam and mRampSpeed. */
int func_02007414(struct dCamera_c *c) {
    c->savedLookAt.x = 0;
    c->savedLookAt.y = 0;
    c->savedLookAt.z = 0;

    struct dActor_c *player = c->mTargetPlayer;
    int lookAtY = c->lookAt.y;
    int playerY = player->mPosY;

    c->savedPos.x = 0;
    c->savedPos.y = lookAtY - playerY;
    c->savedPos.z = 0;
    c->mScriptParam = 0;
    c->mRampSpeed = 0;

    return 1;
}
