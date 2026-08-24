//cpp
// @symbol _ZN6Klepto8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Klepto.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct dCc_c;
struct dBgCh_Actr;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *self, dCc_c *cc);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, dBgCh_Actr *wm, unsigned int j);
void func_ov062_0211c6a8(void *self);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_02012790(int a, int b);
void func_ov062_0211c658(void *c, PMF *p);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void _ZN5dCc_c5ClearEv(dCc_c *self);
void _ZN5dCc_c6UpdateEv(dCc_c *self);
extern char data_ov062_0211e17c[];
}

int Klepto::Behavior()
{
    M *m;
    int b;

    DecIfAbove0_Short((unsigned short *)&unk_100);
    DecIfAbove0_Short((unsigned short *)&mTimer);

    m = (M *)mState;
    if (m->pmf != 0)
        (((Klass *)((char *)this))->*(m->pmf))();

    {
        int accum = mVertSpeed;
        int a0 = mVertAccel;
        int lim = mTerminalVelocity;
        int sum = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = unk_0ac;
        mVertSpeed = lim;
        unk_0ac = t;
    }
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((char *)this), (dCc_c *)&mdCcAc_c1);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), (dBgCh_Actr *)&mWithMeshClsn, 0);

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov062_0211c6a8(((char *)this));

    unsigned int actorId = mHeldActorID;
    if (actorId != 0) {
        void *p = _ZN8dActor_c10FindWithIDEj(actorId);
        if (p != 0) {
            if (mCarriedItem == 1) {
                *(int *)((char *)p + 0x5c) = mHeldPosX;
                *(int *)((char *)p + 0x60) = mHeldPosY;
                *(int *)((char *)p + 0x64) = mHeldPosZ;
                goto skip_destroy;
            } else if (*(unsigned char *)((char *)p + 0x403) == 0) {
                *(int *)((char *)p + 0x5c) = mHeldPosX;
                *(int *)((char *)p + 0x60) = mHeldPosY;
                *(int *)((char *)p + 0x64) = mHeldPosZ;
                goto skip_destroy;
            } else {
                mHeldActorID = 0;
                func_02012790(0xa, 0);
                mTimer = 0x1e;
                func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
                goto skip_destroy;
            }
        } else {
            mHeldActorID = 0;
            func_02012790(0xa, 0);
            mTimer = 0x1e;
            func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
            goto skip_destroy;
        }
    }

    if (mCarriedItem == 1 && unk_448 != 2) {
        b = (mFlags & 8) != 0;
        if (b != 0) {
            _ZN7fBase_c18MarkForDestructionEv(((char *)this));
        }
    }
skip_destroy:
    mBlendModelAnim.Advance();
    if (mState != (void *)data_ov062_0211e14c) {
        func_ov062_0211b51c(((char *)this));
    }

    _ZN5dCc_c5ClearEv((dCc_c *)&mdCcAc_c1);
    _ZN5dCc_c6UpdateEv((dCc_c *)&mdCcAc_c1);
    _ZN5dCc_c5ClearEv((dCc_c *)&mdCcAc_c2);
    _ZN5dCc_c6UpdateEv((dCc_c *)&mdCcAc_c2);

    return 1;
}
