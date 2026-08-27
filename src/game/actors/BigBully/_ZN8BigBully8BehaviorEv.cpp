//cpp
#include "types.h"
// @symbol _ZN8BigBully8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBully.h"
extern "C" {
extern int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void* a, u16* p);
extern int func_ov064_02116d1c(void* c);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* c, void* clsn, unsigned f);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
}

int BigBully::Behavior()
{
    u8 s = mNumBulliesKilled;
    if (s >= 4) {
        if (mSecretSoundCounter != 0) {
            if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (u16*)((char*)&mSecretSoundCounter)) != 0)
                mSecretSoundCounter = 0;
        }
        return func_ov064_02116d1c(((char*)this));
    }
    if (s == 3) {
        int t;
        int m;
        int* p;
        if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (u16*)((char*)&mSecretSoundCounter)) != 0)
            mSecretSoundCounter = 0;
        t = mVertSpeed + mVertAccel;
        m = mTerminalVelocity;
        if (t >= m) m = t;
        mVertSpeed = m;
        p = (int*)(((int)((char*)this) + 0x60));
        *p = *p + mVertSpeed;
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this) + 0x174, 0);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)&mWithMeshClsn) != 0) {
            u8* q;
            func_0200fa8c(((char*)this), 0);
            q = (u8*)(((int)((char*)this) + 0x3fe));
            *q = *q + 1;
        }
        func_ov064_02116bac(((char*)this));
    }
    return 1;
}
