//cpp
// @symbol func_ov002_020c47f4
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

extern "C" {
short Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
int _Z15ApproachLinear2Rsss(short& dst, short to, short step);
short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
int ApproachAngle(short* angle, short targetAngle, short invFactor, short maxDelta, short minDelta);

int func_ov002_020c47f4(char* c)
{
    Vector3 vec;
    int r, a;
    Fix12i y;
    vec.x = *(Fix12i*)(c + 0x5c);
    y = *(Fix12i*)(c + 0x60);
    vec.y = y;
    vec.z = *(Fix12i*)(c + 0x64);
    vec.y = y + 0x64000;
    *(short*)(c + 0x762) = 0;
    *(short*)(c + 0x764) = 0;
    r = _Z15ApproachLinear2Rsss(*(short*)(c + 0x69c), (short)(-Vec3_VertAngle((Vector3*)(c + 0x744), &vec)), 0x200);
    *(short*)(c + 0x762) = *(short*)(c + 0x69c);
    a = ApproachAngle((short*)(c + 0x8e), (short)(Vec3_HorzAngle((Vector3*)(c + 0x744), &vec) + 0x8000), 4, 0x1000, 0x200);
    *(char*)(c + 0x742) = 3;
    if (r == 0) goto ret0;
    if (a == 0) return 1;
ret0:
    return 0;
}
}
