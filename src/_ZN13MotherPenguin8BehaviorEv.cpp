//cpp
// @symbol _ZN10daPgMthr_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "daPgMthr_c.h"
extern "C" {
extern int func_ov018_02111d28(void*);
extern void func_ov018_0211235c(void*);
}

int daPgMthr_c::Behavior()
{
    func_ov018_0211235c(this);
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mModelAnim.UpdateVerts();
    func_ov018_02111d28(this);
    return 1;
}
