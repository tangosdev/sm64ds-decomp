//cpp
// @symbol _ZN13RacingPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"
extern "C" void _ZN9Animation7AdvanceEv(char *c);
extern "C" void _ZN12CylinderClsn5ClearEv(char *c);
extern "C" void _ZN12CylinderClsn6UpdateEv(char *c);

int RacingPenguin::Behavior()
{
    func_ov019_02112268(((char *)this));
    _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    _ZN9Animation7AdvanceEv((char *)&mTextureSequence);
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    func_ov019_021114ec(((char *)this));
    return 1;
}
