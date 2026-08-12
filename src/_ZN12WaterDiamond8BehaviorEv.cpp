//cpp
// @symbol _ZN9ArrowLift8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ArrowLift.h"
typedef short s16;
extern "C" {
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN12CylinderClsn5ClearEv(void* self);
extern void _ZN12CylinderClsn6UpdateEv(void* self);
}

int ArrowLift::Behavior()
{
    func_ov029_02111850(((char*)this));
    if (unk_158 == 0) return 1;
    func_ov029_021117ac(((char*)this));
    if (unk_15d != 0) {
        char* p;
        s16* a = (s16*)(((int)((char*)this) + 0x8e));
        *a = *a + 0x400;
        p = _ZN5Actor10FindWithIDEj(unk_158);
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
    _ZN12CylinderClsn5ClearEv((void*)((char*)&mMovingCylinderClsn));
    _ZN12CylinderClsn6UpdateEv((void*)((char*)&mMovingCylinderClsn));
    return 1;
}
