//cpp
// @symbol _ZN10HootTheOwl8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HootTheOwl.h"
extern "C" {
extern void DecIfAbove0_Short(void*);
extern void func_02012694(int, void*);

extern char data_ov094_02136b40[];
extern char data_ov094_02136b60[];
extern void func_ov094_021357a4(void*);
}

/* The four states Behavior tests against live in ov094's data and are still
   unnamed, so each comparison casts. decl_common.h already declares three of
   them as `char[]`, which is why they are not simply declared as State. */

int HootTheOwl::Behavior()
{
    DecIfAbove0_Short(&unk_100);
    {
        State *o = mCurrentState;
        /* Reads the handler's pointer word directly rather than as
           `&o->mMain`: taking the ADDRESS of a pointer-to-member makes mwcc
           materialise the whole 8-byte pmf. Reading one to CALL it, below, is
           free. */
        if (*(int*)((char*)o+8) != 0) {
            (this->*(o->mMain))();
        }
    }
    if (mCurrentState == (State *)data_ov094_02136b40) return 1;
    mModelAnim.speed = mAnimSpeed;
    mModelAnim.Advance();
    {
        State *m = mCurrentState;
        if ((m == (State *)data_ov094_02136b50 || m == (State *)data_ov094_02136b60 ||
             m == (State *)data_ov094_02136b30) &&
            (unsigned short)(mModelAnim.currFrame >> 0xc) == 0) {
            func_02012694(0x139, &mCamSpacePosX);
        }
    }
    /* The ROM really does test the same state twice, so the else branch below
       is unreachable. Kept because removing it changes the bytes. */
    if (mCurrentState == (State *)data_ov094_02136b70) {
        if (mCurrentState == (State *)data_ov094_02136b70) {
            func_ov094_021362e0(this);
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            UpdateWMClsn(mWithMeshClsn, 0);
        } else {
            func_ov094_021361d8(this);
        }
        return 1;
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep = unk_0ac;
        if (fallSpeed >= clamped) clamped = fallSpeed;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mMovingCylinderClsnWithPos);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov094_021361d8(this);
    if (mCurrentState == (State *)data_ov094_02136b60 && unk_3d4 == 2) {
        func_ov094_021357a4(this);
    }
    mMovingCylinderClsnWithPos.Clear();
    mMovingCylinderClsnWithPos.Update();
    return 1;
}
