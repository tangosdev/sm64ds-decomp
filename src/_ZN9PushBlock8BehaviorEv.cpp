//cpp
// @symbol _ZN9PushBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PushBlock.h"
extern void _ZN12CylinderClsn5ClearEv(char* t);
extern void _ZN12CylinderClsn6UpdateEv(char* t);
extern void _ZN5Actor13SmallPoofDustEv(char* c);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);

int PushBlock::Behavior()
{
    int b = (int)((unk_0b0 & 0x40000) != 0);
    if (b != 0) return 1;
    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;
    func_ov002_020b9750(((char*)this));
    func_ov002_020b979c(((char*)this));
    func_ov002_020b9a1c(((char*)this));
    func_ov002_020b993c(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    if (_ZN8SaveData16HasPlayerLostCapEv()) {
        _ZN5Actor13SmallPoofDustEv(((char*)this));
        _ZN9ActorBase18MarkForDestructionEv(((char*)this));
    }
    return 1;
}
