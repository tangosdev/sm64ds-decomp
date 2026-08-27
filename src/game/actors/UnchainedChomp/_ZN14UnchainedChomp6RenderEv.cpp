//cpp
// @symbol _ZN14UnchainedChomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "UnchainedChomp.h"
struct A;
struct B {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(A* arg);
};

int UnchainedChomp::Render()
{
    B *b = (B*)((char *)&mModelAnim);
    b->m5((A*)((char *)&mScaleX));
    
    int j = 0;
    char *p2 = ((char *)this) + 0x370;
    for (;;) {
        B *b2 = (B*)p2;
        b2->m5((A*)0);
        j++;
        p2 += 0x50;
        if (j >= 5) break;
    }
    return 1;
}
