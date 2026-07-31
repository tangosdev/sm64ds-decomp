//cpp
// @symbol _ZN4Fish6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Fish.h"
struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

int Fish::Render()
{
    if (unk_159 == 0) {
        Base *b = (Base*)((char *)&mModelAnim);
        b->m(0);
    }
    return 1;
}
