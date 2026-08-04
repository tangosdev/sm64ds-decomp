//cpp
// @symbol func_ov006_020c1804
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern void func_0203cd80(struct Vector3 *m, short angle);
void func_ov006_020c1804(void *self);
}

namespace ShadowModel { void RenderAll(); }

struct Obj {
    virtual void f0() = 0;
    virtual void f1() = 0;
    virtual void f2() = 0;
    virtual void f3() = 0;
    virtual void f4() = 0;
    virtual void f5(int a) = 0;
};

void func_ov006_020c1804(void *self)
{
    char *c = (char*)self;
    struct Vector3 v1, v2;
    unsigned int p1, p2;
    short ang;
    unsigned char b[3];
    int i;

    v1.x = 0;
    v1.y = 0;
    v1.z = 0xfffff008;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0xfffff008;
    func_0203cd80(&v1, -0x1000);
    func_0203ccd4(&v1, 0x1000);

    ang = *(short*)(c + 0x1e4);
    func_0203ccd4(&v2, ang);

    p1 = (((short)v1.x >> 3) & 0x3ff)
       | ((((short)v1.y >> 3) & 0x3ff) << 10)
       | ((((short)v1.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int*)0x40004c8 = p1;

    p2 = (((short)v2.x >> 3) & 0x3ff)
       | ((((short)v2.y >> 3) & 0x3ff) << 10)
       | ((((short)v2.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int*)0x40004c8 = p2 | 0x40000000;

    func_02016a14(c + 0x1c, 0x7fff);
    func_02016a04(c + 0x1c, 0x4210);

    ang = *(short*)(c + 0x1e2);
    {
        unsigned char *p = b;
        for (i = 0; i < 3; i++) {
            if ((ang >> i) & 1)
                *p = 0x1f;
            else
                *p = 0x14;
            p++;
        }
    }

    func_020169d8(c + 0x8c, 0, (unsigned short)(b[0] | (b[1] << 5) | (b[2] << 10)));
    func_020169d8(c + 0x8c, 4, 0x6318);
    func_020169d8(c + 0x8c, 5, (unsigned short)(b[1] | (b[2] << 5) | (b[0] << 10)));

    ((Obj*)(c + 0x1c))->f5(0);
    func_ov006_020c07a0(c + 0xdc);
    ((Obj*)(c + 0x8c))->f5(0);

    ShadowModel::RenderAll();
}
