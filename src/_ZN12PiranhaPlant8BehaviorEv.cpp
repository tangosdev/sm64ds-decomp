//cpp
// @symbol _ZN12PiranhaPlant8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PiranhaPlant.h"
struct Cls { virtual void dummy(); };
typedef void (Cls::*PMF)();
extern PMF data_ov084_02130e80[];
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void*, void*, void*, unsigned int);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void*, void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void*, void*);
}

int PiranhaPlant::Behavior()
{
    int r;
    int old;
    int cur;
    r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(((char*)this), ((char*)this) + 0x1c4, ((char*)this) + 0x110, 1);
    if (r != 0) {
        if (r == 2) {
            unk_108 = 0;
            mState = 7;
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPosX = unk_44c;
            mPosY = unk_450;
            mPosZ = unk_454;
        }
        return 1;
    }
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char*)this), ((char*)this) + 0x380);
    _ZN9Animation7AdvanceEv((char*)&mAnimation);
    func_ov084_0212f204(((char*)this));
    old = mState;
    (((Cls*)((char*)this))->*data_ov084_02130e80[old])();
    {
        unsigned short* p100 = (unsigned short*)((char*)&unk_100);
        *p100 = (unsigned short)(*p100 + 1);
    }
    cur = mState;
    if (old != cur) {
        if (cur == 5) {
            int* pb0 = (int*)((char*)&unk_0b0);
            *pb0 = *pb0 & ~0x10000000;
        }
        unk_100 = 0;
        unk_478 = 0;
    }
    func_ov084_0212ec60(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn1);
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn2);
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
    if (unk_45c != 0) {
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn1);
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn2);
        if (mState == 2) {
            _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x3e8, ((char*)this) + 0x440);
            _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
        }
    }
    return 1;
}
