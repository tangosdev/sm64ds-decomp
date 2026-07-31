//cpp
// @symbol _ZN5Swoop6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Swoop.h"
struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

int Swoop::Render()
{
    int flag = (unk_0b0 & 0x40000) != 0;
    if (flag != 0) return 1;
    if (unk_43c == 1) {
        Base *b = (Base *)((char *)&mModelAnim1);
        b->m(0);
    } else {
        Base *b = (Base *)((char *)&mModelAnim2);
        b->m(0);
    }
    return 1;
}
