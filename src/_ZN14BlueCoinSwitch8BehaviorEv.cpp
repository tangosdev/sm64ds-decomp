//cpp
// @symbol _ZN14BlueCoinSwitch8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BlueCoinSwitch.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern void _ZN5Event8ClearBitEj(u32 bit);
extern void _ZN5Event6SetBitEj(u32 bit);
extern void _ZN9ActorBase18MarkForDestructionEv(void* p);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);

int BlueCoinSwitch::Behavior()
{
    if (unk_10a != 0) {
        *(u16*)(((int)((char*)this) + 0x10a)) -= 1;
        if (unk_10a == 0) {
            *(u32*)(((int)((char*)this) + 0xec)) &= ~1;
            _ZN5Event8ClearBitEj(mEventID);
        }
    }
    if (unk_0f8 != 0) {
        *(u32*)(((long long)(int)((char*)&unk_0ec))) |= 1;
        _ZN5Event6SetBitEj(mEventID);
        if (unk_10c != 0) {
            unk_10a = unk_108;
        } else {
            _ZN9ActorBase18MarkForDestructionEv(((char*)this));
        }
    }
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    return 1;
}
