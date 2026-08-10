//cpp
// @symbol _ZN19FirePiranhaPlantBig6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "FirePiranhaPlantBig.h"
struct V3 { int x, y, z; };
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(V3* p);
};

int FirePiranhaPlantBig::Render()
{
    int v = mScale;
    int b;
    if (v == 0 || (b = (mFlags & 0x40000) != 0, b != 0)) {
        return 1;
    }
    V3 s;
    s.x = v;
    s.y = v;
    s.z = v;
    Obj* o = (Obj*)((char*)&mModelAnim);
    o->m5(&s);
    return 1;
}
