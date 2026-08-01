//cpp
// @symbol _ZN12PiranhaPlant6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "PiranhaPlant.h"
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(void* p);
};
struct G2 { int w[2]; };
extern G2 data_ov084_02130df4;

int PiranhaPlant::Render()
{
    if (mScaleX == 0)
        return 1;
    ((Obj*)((char*)&mModelAnim))->m5((char*)&mScaleX);
    if (unk_170 == data_ov084_02130df4.w[1])
        ((Obj*)((char*)&mModel))->m5((char*)&unk_428);
    return 1;
}
