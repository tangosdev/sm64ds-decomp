//cpp
// @symbol _ZN13PrincessPeach8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PrincessPeach.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void *a);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
extern int func_ov085_02129fdc(void *c);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); };

int PrincessPeach::Behavior()
{
    func_ov085_0212a430(((char *)this));
    func_ov085_02129dbc(((char *)this));
    if (unk_354 != 1)
        _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    ((Sub*)((char *)&mModelAnim))->g3();
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    func_ov085_02129fdc(((char *)this));
    return 1;
}
