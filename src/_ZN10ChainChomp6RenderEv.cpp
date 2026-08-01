//cpp
// @symbol _ZN10ChainChomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
struct A;
struct B {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(A* arg);
};

int ChainChomp::Render()
{
    B *b = (B*)((char *)&mModelAnim);
    b->m5((A*)((char *)&mScaleX));
    
    int j = 0;
    char *p2 = ((char *)this) + 0x1dc;
    for (;;) {
        B *b2 = (B*)p2;
        b2->m5((A*)0);
        j++;
        p2 += 0x50;
        if (j >= 7) break;
    }
    return 1;
}
