//cpp
// @symbol _ZN12WaterDiamond8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterDiamond.h"
typedef short s16;
extern "C" {
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dCc_c5ClearEv(void* self);
extern void _ZN5dCc_c6UpdateEv(void* self);
}

int WaterDiamond::Behavior()
{
    func_ov029_02111850(((char*)this));
    if (unk_158 == 0) return 1;
    func_ov029_021117ac(((char*)this));
    if (unk_15d != 0) {
        char* p;
        s16* a = (s16*)(((int)((char*)this) + 0x8e));
        *a = *a + 0x400;
        p = _ZN8dActor_c10FindWithIDEj(unk_158);
        if (p != 0) {
            if (mPosY == *(int*)(p+0x60)) {
                if (mAngleY == 0) unk_15d = 0;
            }
            if (mPosY != *(int*)(p+0x334)) {
                unk_15d = 0;
                mAngleY = 0;
            }
        }
    }
    func_ov029_021118c8(((char*)this));
    _ZN5dCc_c5ClearEv((void*)((char*)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((void*)((char*)&mdCcAc_c));
    return 1;
}
