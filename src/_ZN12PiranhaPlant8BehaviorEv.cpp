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
extern int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void*, void*, void*, unsigned int);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void*, void*);
extern void _ZN5dCc_c5ClearEv(void*);
extern void _ZN5dCc_c6UpdateEv(void*);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void*, void*);
}

int PiranhaPlant::Behavior()
{
    int r;
    int old;
    int cur;
    r = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(((char*)this), &mWithMeshClsn, &mModelAnim, 1);
    if (r != 0) {
        if (r == 2) {
            unk_108 = 0;
            mState = 7;
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mPosX = mHomePos.x;
            mPosY = mHomePos.y;
            mPosZ = mHomePos.z;
        }
        return 1;
    }
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), &mdCcAc_c1);
    mModelAnim.Advance();
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
            int* pb0 = (int*)((char*)&mFlags);
            *pb0 = *pb0 & ~0x10000000;
        }
        unk_100 = 0;
        unk_478 = 0;
    }
    func_ov084_0212ec60(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c1);
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c2);
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (mClsnEnabled != 0) {
        _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c1);
        _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c2);
        if (mState == 2) {
            _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(&mdCcAcPos_c, &mSpawnPos);
            _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
        }
    }
    return 1;
}
