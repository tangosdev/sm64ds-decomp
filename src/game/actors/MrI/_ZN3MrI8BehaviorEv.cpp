//cpp
// @symbol _ZN3MrI8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MrI.h"
extern "C" {
    void func_ov071_021215c0(void *c);
    void func_0200f760(void *c, void *p);
    void func_ov071_02120c90(void *c);
}

/* dCc_c comes from the real dCcAc_c chain now that MrI.h types mdCcAcPos_c;
   the ad-hoc redeclaration that used to stand in for it ICEd mwccarm
   (CClass.c:3328) once the real class was visible. Clear and Update are
   non-virtual there, so the direct bl is unchanged. */

int MrI::Behavior()
{
    func_ov071_021215c0(((char *)this));
    func_0200f760(((char *)this), ((char *)this) + 0x174);
    mTurnRefAngleY = mAngleY;
    ((dCc_c*)((char *)&mdCcAcPos_c))->Clear();
    ((dCc_c*)((char *)&mdCcAcPos_c))->Update();
    func_ov071_02120c90(((char *)this));
    return 1;
}
