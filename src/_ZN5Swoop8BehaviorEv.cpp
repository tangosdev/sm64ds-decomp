//cpp
// @symbol _ZN5Swoop8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Swoop.h"

/* This file used to open with `struct Enemy { char pad[0x800]; };` and work a
 * `char *c` through raw offsets. Swoop.h now supplies the real chain, so the
 * stand-in is gone and every offset below is a named field.
 *
 * The two ModelAnims are what the tail is choosing between: `c + 0x350` and
 * `c + 0x3b4` are each one's Animation base (+0x50), so the branch advances
 * mModelAnim1 or mModelAnim2.
 *
 * Enemy::UpdateYoshiEat is still reached by its mangled name -- unlike
 * UpdateDeath, UpdateWMClsn and UpdateKillByInvincibleChar, it is not declared
 * in Enemy.h yet.
 */
extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_02012694(int, void *);

extern char data_ov065_0211d6f0[];
}

int Swoop::Behavior()
{
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(this, &mWithMeshClsn) != 0) {
        mMovingCylinderClsn.Clear();
        if (unk_107 != 0) {
            if (unk_104 == 0) {
                mMovingCylinderClsn.Update();
            }
        }
        func_ov065_02117994((char *)this);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim1, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov065_02117994((char *)this);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&unk_100);
    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) (this->*(q->mMain))();
    }
    {
        State *m = mCurrentState;
        if (m == (State *)data_ov065_0211d6e0 || m == (State *)data_ov065_0211d6f0) {
            /* Three wingbeat frames, one flap sound, at the camera-space
               position. */
            if (mModelAnim1.WillHitFrame(3) != 0 ||
                mModelAnim1.WillHitFrame(0xf) != 0 ||
                mModelAnim1.WillHitFrame(0x1b) != 0) {
                func_02012694(0xe1, &mCamSpacePosX);
            }
        }
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        int keep = unk_0ac;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mMovingCylinderClsn);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov065_02117994((char *)this);
    if (unk_43c == 1) {
        func_ov065_0211704c((char *)this);
    }
    mMovingCylinderClsn.Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mMovingCylinderClsn.Update();
        }
    }
    if (unk_43c == 1) {
        mModelAnim1.Advance();
    } else {
        mModelAnim2.Advance();
    }
    return 1;
}
