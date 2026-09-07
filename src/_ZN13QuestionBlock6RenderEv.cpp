//cpp
// @symbol _ZN13QuestionBlock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"
extern "C" { extern int data_0209caa0; }

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

#pragma optimize_for_size on

int QuestionBlock::Render()
{
    if (mState == 2)
        goto done;
    if ((*(int*)((char*)&data_0209caa0 + 4) & 0x80000000) == 0) {
        int b = (actorID == 0x14);
        if (b != 0) {
            Sub* s = (Sub*)&mModelAnim;
            s->m(0);
            goto done;
        }
    }
    {
        Sub* s2 = (Sub*)&mModel;
        s2->m((int)((char*)&mScaleX));
    }
done:
    return 1;
}
