//cpp
// @symbol _ZN5Whomp8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
struct dActor_c; typedef int (dActor_c::*PMF)();
struct dBgCh_Actr;
struct dCc_c;

extern "C" {
int _ZN8dActor_c13DistToCPlayerEv(dActor_c* self);
void func_ov079_02123f34(dActor_c* self);
void _ZN8dActor_c9UpdatePosEP5dCc_c(dActor_c* self, dCc_c* c);
int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(dActor_c* self, dBgCh_Actr* w, int a, short b, int c, int d, void* e);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(dActor_c* self, dBgCh_Actr* w, unsigned int n);
void func_ov079_02124188(dActor_c* self);
int func_ov079_021243e0(char* c, int r4);
int func_ov079_02123a8c(dActor_c* self);
void func_ov079_02124008(dActor_c* self);
}

extern int data_0209f318;
extern PMF data_ov079_02128280[];

int Whomp::Behavior()
{
    if (mIsKing != 0 && mState != 9) {
        if (_ZN8dActor_c13DistToCPlayerEv(((dActor_c*)this)) < 0x1770000) {
            *(int*)(*(int*)&data_0209f318 + 0x114) = (int)((dActor_c*)this);
        }
    }

    func_ov079_02123f34(((dActor_c*)this));
    _ZN8dActor_c9UpdatePosEP5dCc_c(((dActor_c*)this), 0);

    if (mHorzSpeed != 0) {
        if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(((dActor_c*)this), &mWithMeshClsn, 0x3c000, (short)0x2888, 0, 0, (void*)0x32000)) {
            /* The step would leave the ledge: rewind to last frame's position. */
            mPosX = mSafePosX;
            mPosY = mSafePosY;
            mPosZ = mSafePosZ;
        } else {
            mSafePosX = mPosX;
            mSafePosY = mPosY;
            mSafePosZ = mPosZ;
        }
    } else {
        mSafePosX = mPosX;
        mSafePosY = mPosY;
        mSafePosZ = mPosZ;
    }

    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((dActor_c*)this), &mWithMeshClsn, 0);

    {
        int idx = mState;
        PMF* pmf = &data_ov079_02128280[idx];
        (((dActor_c*)this)->**pmf)();

        {
            /* dEnemyBase_c's 0x100 counts frames spent in the current state: it is
               incremented here and reset the moment the handler changed mState. */
            unsigned short* ctr = (unsigned short*)&mStateTimer;
            *ctr = *ctr + 1;
            if (idx != mState) {
                *ctr = 0;
                unk_40c = 0;
            }
        }
    }

    func_ov079_02124188(((dActor_c*)this));

    if (func_ov079_021243e0((char*)((dActor_c*)this), 0) == 0 || func_ov079_02123a8c(((dActor_c*)this)) != 0) {
        func_ov079_02124008(((dActor_c*)this));
    }

    unk_403 = 0;
    return 1;
}
