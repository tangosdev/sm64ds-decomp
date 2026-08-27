//cpp
// @symbol _ZN6Snufit8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Snufit.h"

/* This file used to open with `struct dEnemyBase_c { char pad[0x800]; };` and work a
 * `char *c` through raw offsets. Snufit.h now supplies the real chain, so the
 * stand-in is gone and every offset below is a named field.
 *
 * dEnemyBase_c::UpdateYoshiEat is still reached by its mangled name -- unlike
 * UpdateDeath, UpdateWMClsn and UpdateKillByInvincibleChar, it is not declared
 * in dEnemyBase_c.h yet.
 */
extern "C" {
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(dEnemyBase_c *thiz, dBgCh_Actr *c);
extern int ApproachAngle(short *target, short from, short start, short speed, short max);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);
extern int func_ov065_0211691c(void *c, void *p);
extern unsigned short DecIfAbove0_Short(unsigned short *p);

extern short data_02082214[];
extern int data_ov065_0211d670[];
}

int Snufit::Behavior()
{
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(this, &mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov065_0211696c((char *)this);
        mHomePosX = mPosX;
        mHomePosY = mPosY;
        mHomePosZ = mPosZ;
        func_ov065_0211691c(this, data_ov065_0211d670);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        ApproachAngle(&mAngleX, -0x4000, 0xa, 0x200, 0x100);
        UpdateDeath(mWithMeshClsn);
        func_ov065_0211696c((char *)this);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) {
            (this->*(q->mMain))();
        }
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped) {
            clamped = fallSpeed;
        }
        int keep = unk_0ac;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    if (mCurrentState != (State *)data_ov065_0211d650) {
        int *pAngle;
        int ang;
        int idx;
        short tbl;
        int result;
        /* The add sits INSIDE the integer cast, which is load-bearing here:
           `(int)this + 0x3d8` is not interchangeable with `&unk_3d8`. */
        pAngle = (int *)(((int)this + 0x3d8));
        *pAngle += 0x200;
        ang = unk_3d8;
        /* The shift must be LOGICAL so the angle wraps -- writing it on the
           signed s16 would read the wrong table entry for negative angles. */
        idx = ((unsigned short)(short)ang >> 4) * 2;
        tbl = data_02082214[idx];
        result = (int)(((long long)tbl * 0x46000 + 0x800) >> 12);
        _Z14ApproachLinearRiii(&mPosY, mHomePosY + (result + 0xb4000), 0x3000);
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov065_0211696c((char *)this);
    if (mCurrentState != (State *)data_ov065_0211d660) {
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        func_ov065_02115ff0((char *)this);
    }
    mdCcAc_c.Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mdCcAc_c.Update();
        }
    }
    mModelAnim.Advance();
    return 1;
}
