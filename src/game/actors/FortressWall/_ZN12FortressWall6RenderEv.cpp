//cpp
// @symbol _ZN12FortressWall6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
struct Base {
    virtual int vf0(int);
    virtual int vf1(int);
    virtual int vf2(int);
    virtual int vf3(int);
    virtual int vf4(int);
    virtual int vfunc(int a);
};

int FortressWall::Render()
{
    if (mBroken != 0) return 1;
    ((Base *)&mModel)->vfunc(0);
    return 1;
}
