//cpp
// @symbol _ZN13RacingPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"
extern "C" void _ZN9Animation7AdvanceEv(char *c);
extern "C" void _ZN5dCc_c5ClearEv(char *c);
extern "C" void _ZN5dCc_c6UpdateEv(char *c);

int RacingPenguin::Behavior()
{
    func_ov019_02112268(((char *)this));
    _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    _ZN9Animation7AdvanceEv((char *)&mTextureSequence);
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    func_ov019_021114ec(((char *)this));
    return 1;
}
