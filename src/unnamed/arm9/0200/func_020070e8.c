// @symbol func_020070e8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern int ApproachAngle(short *angle, int targetAngle, int invFactor, int maxDelta, int minDelta);

void func_020070e8(int *thiz, int p1, int p2, int p3, short flagV, short targetH, short flagH)
{
    int dist;
    short vertAng;
    short horzAng;
    struct Vector3 *trans = (struct Vector3*)((char*)*(int**)((char*)thiz + 0x110) + 0x5c);
    func_02007c9c(trans, (struct Vector3*)((char*)thiz + 0x8c), &dist, &vertAng, &horzAng);
    Math_Function_0203b0fc(&dist, p1, p2, 0x7fffffff);
    if (flagV != 0)
        ApproachAngle(&vertAng, p3, flagV, 0x4000, 0);
    if (flagH != 0)
        ApproachAngle(&horzAng, targetH, flagH, 0x4000, 0);
    func_02007c14((struct Vector3*)((char*)thiz + 0x8c), trans, dist, vertAng, horzAng);
}
