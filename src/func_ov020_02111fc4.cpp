//cpp
// @symbol func_ov020_02111fc4
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;
typedef short s16;


struct dActor_c {
    virtual void v0();
};
extern "C" char* _ZN8dActor_c13ClosestPlayerEv(void);
extern "C" Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern "C" int _ZN8dActor_c14GetSubtractionEss(void* thiz, short a, short b);
extern "C" void func_0201267c(int a, void* p);

extern "C" void func_ov020_02111fc4(char* thiz)
{
    char* c = thiz;
    Vector3 v;
    char* p = _ZN8dActor_c13ClosestPlayerEv();
    {
        int* s = (int*)(int)(((int)p + 0x5c));
        v.x = s[0];
        v.y = s[1];
        v.z = s[2];
    }
    if (Vec3_Dist((Vector3*)(c + 0x5c), &v) >= 0x190000) return;
    {
        short ang = Vec3_HorzAngle((Vector3*)(c + 0x5c), &v);
        if (_ZN8dActor_c14GetSubtractionEss(c, *(short*)(c + 0x94), ang) >= 0x3000) return;
    }
    *(int*)(c + 0x424) = 1;
    *(int*)(c + 0x98) = 0x5000;
    *(short*)(c + 0x100) = 0;
    {
        int* p234 = (int*)(int)(((int)c + 0x234));
        *p234 = *p234 & ~1;
    }
    func_0201267c(0x166, c + 0x74);
}
