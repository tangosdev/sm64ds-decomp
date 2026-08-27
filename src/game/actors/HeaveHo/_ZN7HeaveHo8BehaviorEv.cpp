//cpp
#include "types.h"
// @symbol _ZN7HeaveHo8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HeaveHo.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct dCc_c;
struct dBgCh_Actr;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, dCc_c *cc);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *v);
int func_02010844(void *unused, Vector3 *v, s16 angle);
int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void *self, dBgCh_Actr *wm, Fix12i a, s16 b, int c, int d, void *e);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
void func_ov077_02126dac(char *t);
void func_ov077_02126528(char *c);
void _ZN5dCc_c5ClearEv(dCc_c *self);
void _ZN5dCc_c6UpdateEv(dCc_c *self);
void _ZN9Animation7AdvanceEv(void *self);
extern int data_0209f32c;
}

int HeaveHo::Behavior()
{
    int b;
    Vector3 v;
    int r5;
    M *m;

    if (mPosY < data_0209f32c) {
        mPosX = unk_404;
        mPosY = unk_408;
        mPosZ = unk_40c;
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)((char *)&mStateTimer));
    DecIfAbove0_Short((unsigned short *)((char *)&unk_426));

    m = *(M **)((char *)&unk_3fc);
    if (m->pmf != 0)
        (((Klass *)((char *)this))->*(m->pmf))();

    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), (dCc_c *)((char *)&mdCcAcPos_c));

    r5 = 0;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn)) {
        void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &v);
        r5 = func_02010844(((char *)this), &v, mAngleY);
    }

    b = _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(((char *)this), (dBgCh_Actr *)((char *)&mWithMeshClsn), 0x3c000, (s16)0x2888, 0, 1, (void *)0x32000);
    if (b == 0) {
        if (r5 < 0)
            r5 = (s16)-r5;
        if (r5 <= 0x100)
            goto writeback;
    }
    mPosX = unk_410;
    mPosY = unk_414;
    mPosZ = unk_418;
writeback:
    unk_410 = mPosX;
    unk_414 = mPosY;
    unk_418 = mPosZ;
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), (dBgCh_Actr *)((char *)&mWithMeshClsn), 2);

    mAngleY = mPrevAngleY;
    func_ov077_02126dac(((char *)this));

    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) && *(void **)((char *)&unk_3fc) != (void *)data_ov077_02127cd8) {
        func_ov077_02126528(((char *)this));
    }
    _ZN5dCc_c5ClearEv((dCc_c *)((char *)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((dCc_c *)((char *)&mdCcAc_c));
    _ZN5dCc_c5ClearEv((dCc_c *)((char *)&mdCcAcPos_c));
    _ZN5dCc_c6UpdateEv((dCc_c *)((char *)&mdCcAcPos_c));

    mModelAnim.Advance();
    return 1;
}
