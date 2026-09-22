//cpp
// @symbol _ZN8Snowball8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Snowball.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct dCc_c;
struct dBgCh_Actr;
extern "C" unsigned short DecIfAbove0_Short(unsigned short *p);
extern "C" void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *self, dCc_c *cc);
extern "C" void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
extern "C" void _ZN5dCc_c5ClearEv(dCc_c *self);
extern "C" void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern "C" void _ZN5dCc_c6UpdateEv(dCc_c *self);

int Snowball::Behavior()
{
    DecIfAbove0_Short((unsigned short *)((char *)&mStateTimer));
    M *m = *(M **)((char *)&unk_378);
    if (m->pmf != 0)
        (((Klass *)((char *)this))->*(m->pmf))();
    int v = mVertSpeed + mVertAccel;
    int hi = mTerminalVelocity;
    if (v >= hi)
        hi = v;
    int tmp = unk_0ac;
    mVertSpeed = hi;
    unk_0ac = tmp;
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((char *)this), (dCc_c *)((char *)&mdCcAc_c));
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), (dBgCh_Actr *)((char *)&mWithMeshClsn), 0);
    mAngleY = mPrevAngleY;
    func_ov081_02126224(((char *)this));
    _ZN5dCc_c5ClearEv((dCc_c *)((char *)&mdCcAc_c));
    void *p = _ZN8dActor_c13ClosestPlayerEv(((char *)this));
    if (p != 0 && *(unsigned char *)((char *)p + 0x6fb) == 0)
        _ZN5dCc_c6UpdateEv((dCc_c *)((char *)&mdCcAc_c));
    return 1;
}
