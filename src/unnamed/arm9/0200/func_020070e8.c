// @symbol func_020070e8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern int ApproachAngle(short *angle, int targetAngle, int invFactor, int maxDelta, int minDelta);

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields
 * this camera step touches, under that header's names. */
typedef struct {
    u8 pad_000[0x80];
    struct Vector3 lookAt; /* 0x080 */
    struct Vector3 pos;    /* 0x08c */
    u8 pad_098[0x78];
    char *mTargetPlayer; /* 0x110 -- its position (+0x5c) is the orbit centre */
} CameraView;

void func_020070e8(int *thiz, int p1, int p2, int p3, short flagV, short targetH, short flagH)
{
    int dist;
    short vertAng;
    short horzAng;
    CameraView *cam = (CameraView *)thiz;
    struct Vector3 *trans = (struct Vector3 *)(cam->mTargetPlayer + 0x5c);
    func_02007c9c(trans, &cam->pos, &dist, &vertAng, &horzAng);
    Math_Function_0203b0fc(&dist, p1, p2, 0x7fffffff);
    if (flagV != 0)
        ApproachAngle(&vertAng, p3, flagV, 0x4000, 0);
    if (flagH != 0)
        ApproachAngle(&horzAng, targetH, flagH, 0x4000, 0);
    func_02007c14(&cam->pos, trans, dist, vertAng, horzAng);
}
