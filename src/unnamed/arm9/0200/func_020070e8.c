// @symbol func_020070e8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern int ApproachAngle(short *angle, int targetAngle, int invFactor, int maxDelta, int minDelta);

/* The fields used here, named as in include/dActor_c.h and include/dCamera_c.h. */
struct dActor_c {
    char pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
};

struct dCamera_c {
    char pad_000[0x8c];
    Vector3 pos; /* 0x08c */
    char pad_098[0x110 - 0x98];
    struct dActor_c *mTargetPlayer; /* 0x110 */
};

void func_020070e8(int *thiz, int p1, int p2, int p3, short flagV, short targetH, short flagH)
{
    int dist;
    short vertAng;
    short horzAng;
    struct dCamera_c *cam = (struct dCamera_c *)thiz;
    struct Vector3 *trans = &cam->mTargetPlayer->mPos;
    func_02007c9c(trans, &cam->pos, &dist, &vertAng, &horzAng);
    Math_Function_0203b0fc(&dist, p1, p2, 0x7fffffff);
    if (flagV != 0)
        ApproachAngle(&vertAng, p3, flagV, 0x4000, 0);
    if (flagH != 0)
        ApproachAngle(&horzAng, targetH, flagH, 0x4000, 0);
    func_02007c14(&cam->pos, trans, dist, vertAng, horzAng);
}
