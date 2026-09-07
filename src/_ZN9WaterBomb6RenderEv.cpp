//cpp
// @symbol _ZN9WaterBomb6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "WaterBomb.h"
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(void *arg);
};

int WaterBomb::Render()
{
    if (unk_3c8 != 0) {
        Obj *o = (Obj *)((char *)&mModel);
        o->m((char *)&mScaleX);
    }
    return 1;
}
