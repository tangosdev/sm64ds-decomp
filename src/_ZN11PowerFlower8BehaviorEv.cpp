//cpp
// @symbol _ZN11PowerFlower8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerFlower.h"
extern "C" {
extern void _ZN5dCc_c5ClearEv(char* t);
extern void _ZN5dCc_c6UpdateEv(char* t);
extern void _ZN8dActor_c13SmallPoofDustEv(char* c);
extern void _ZN7fBase_c18MarkForDestructionEv(char* c);
}

int PowerFlower::Behavior()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0) return 1;
    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;
    func_ov002_020b9750(((char*)this));
    func_ov002_020b979c(((char*)this));
    func_ov002_020b9a1c(((char*)this));
    func_ov002_020b993c(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
    if (_ZN8SaveData16HasPlayerLostCapEv()) {
        _ZN8dActor_c13SmallPoofDustEv(((char*)this));
        _ZN7fBase_c18MarkForDestructionEv(((char*)this));
    }
    return 1;
}
