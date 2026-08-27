//cpp
// @symbol func_ov100_021463b0
/* recovered: shared common types */
#include "common.h"

extern "C" {
int _Z14ApproachLinearRsss(short&, short, short);
char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
int Vec3_HorzLen(Vector3* v);
int RandomIntInternal(int* seed);
extern int data_0209e650;

void func_ov100_021463b0(char* c)
{
    Vector3 v;
    char* o = c + 0x100;
    if (!_Z14ApproachLinearRsss(*(short*)(c + 0x94), *(short*)(o + 0x56), *(short*)(o + 0x54)))
        return;
    {
        char* a = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x13c));
        Vec3_Sub(&v, (Vector3*)(c + 0x5c), (Vector3*)(a + 0x5c));
    }
    if (Vec3_HorzLen(&v) >= 0xfa000) return;
    *(int*)(c + 0x98) = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
    *(int*)(c + 0x130) = 0x1000;
    *(unsigned char*)(c + 0x15a) = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
    *(int*)(c + 0x14c) = 5;
}
}
