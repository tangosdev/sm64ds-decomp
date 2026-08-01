//cpp
// @symbol _ZN6Klepto8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Klepto.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, CylinderClsn *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
void func_ov062_0211c6a8(void *self);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void func_02012790(int a, int b);
void func_ov062_0211c658(void *c, PMF *p);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
void func_ov062_0211b51c(void *self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
extern char data_ov062_0211e17c[];
}

int Klepto::Behavior()
{
    M *m;
    int b;

    DecIfAbove0_Short((unsigned short *)((char *)&unk_100));
    DecIfAbove0_Short((unsigned short *)((char *)&unk_444));

    m = *(M **)((char *)&unk_42c);
    if (m->pmf != 0)
        (((Klass *)((char *)this))->*(m->pmf))();

    {
        int accum = unk_0a8;
        int a0 = unk_09c;
        int lim = unk_0a0;
        int sum = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = unk_0ac;
        unk_0a8 = lim;
        unk_0ac = t;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((char *)this), (CylinderClsn *)((char *)&mMovingCylinderClsn1));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char *)this), (WithMeshClsn *)((char *)&mWithMeshClsn), 0);

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov062_0211c6a8(((char *)this));

    unsigned int actorId = mHeldActorID;
    if (actorId != 0) {
        void *p = _ZN5Actor10FindWithIDEj(actorId);
        if (p != 0) {
            if (mCarriedItem == 1) {
                *(int *)((char *)p + 0x5c) = unk_450;
                *(int *)((char *)p + 0x60) = unk_454;
                *(int *)((char *)p + 0x64) = unk_458;
                goto skip_destroy;
            } else if (*(unsigned char *)((char *)p + 0x403) == 0) {
                *(int *)((char *)p + 0x5c) = unk_450;
                *(int *)((char *)p + 0x60) = unk_454;
                *(int *)((char *)p + 0x64) = unk_458;
                goto skip_destroy;
            } else {
                mHeldActorID = 0;
                func_02012790(0xa, 0);
                *(unsigned short *)(((char *)this) + 0x400 + 0x44) = 0x1e;
                func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
                goto skip_destroy;
            }
        } else {
            mHeldActorID = 0;
            func_02012790(0xa, 0);
            *(unsigned short *)(((char *)this) + 0x400 + 0x44) = 0x1e;
            func_ov062_0211c658(((char *)this), (PMF *)data_ov062_0211e17c);
            goto skip_destroy;
        }
    }

    if (mCarriedItem == 1 && unk_448 != 2) {
        b = (unk_0b0 & 8) != 0;
        if (b != 0) {
            _ZN9ActorBase18MarkForDestructionEv(((char *)this));
        }
    }
skip_destroy:
    _ZN14BlendModelAnim7AdvanceEv((void *)((char *)&mBlendModelAnim));
    if (*(void **)((char *)&unk_42c) != (void *)data_ov062_0211e14c) {
        func_ov062_0211b51c(((char *)this));
    }

    _ZN12CylinderClsn5ClearEv((CylinderClsn *)((char *)&mMovingCylinderClsn1));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)((char *)&mMovingCylinderClsn1));
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)((char *)&mMovingCylinderClsn2));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)((char *)&mMovingCylinderClsn2));

    return 1;
}
