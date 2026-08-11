//cpp
// @symbol _ZN8Snowball8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Snowball.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
extern "C" unsigned short DecIfAbove0_Short(unsigned short *p);
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, CylinderClsn *cc);
extern "C" void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
extern "C" void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *self);
extern "C" void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);

int Snowball::Behavior()
{
    DecIfAbove0_Short((unsigned short *)((char *)&unk_100));
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
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((char *)this), (CylinderClsn *)((char *)&mMovingCylinderClsn));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), (WithMeshClsn *)((char *)&mWithMeshClsn), 0);
    mAngleY = mPrevAngleY;
    func_ov081_02126224(((char *)this));
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)((char *)&mMovingCylinderClsn));
    void *p = _ZN5Actor13ClosestPlayerEv(((char *)this));
    if (p != 0 && *(unsigned char *)((char *)p + 0x6fb) == 0)
        _ZN12CylinderClsn6UpdateEv((CylinderClsn *)((char *)&mMovingCylinderClsn));
    return 1;
}
