//cpp
// @symbol _ZN11PyramidLift6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidLift.h"
extern "C" {
struct Mtx { int w[12]; };
extern void Matrix4x3_FromTranslation(Mtx* m, int x, int y, int z);
}

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

struct Elem { int w[3]; };

int PyramidLift::Render()
{
    {
        Sub* s = (Sub*)((char*)&mModel);
        s->m(0);
    }
    int i = mNextBullet;
    if (i < 0xa) {
        Elem* e = (Elem*)(((char*)this) + i * 0xc);
        do {
            Matrix4x3_FromTranslation((Mtx*)((char*)&mModel2.mat4x3),
                e->w[(0x37c)/4] >> 3,
                e->w[(0x380)/4] >> 3,
                e->w[(0x384)/4] >> 3);
            Sub* s2 = (Sub*)((char*)&mModel2);
            s2->m(0);
            i++;
            e = (Elem*)((char*)e + 0xc);
        } while (i < 0xa);
    }
    return 1;
}
