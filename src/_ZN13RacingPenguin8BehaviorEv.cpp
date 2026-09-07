//cpp
// @symbol _ZN13RacingPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"

int RacingPenguin::Behavior()
{
    func_ov019_02112268((char *)this);
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov019_021114ec((char *)this);
    return 1;
}
