//cpp
// @symbol _ZN19daPropeller_Heyho_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"

/* This file used to open with `struct dEnemyBase_c { char pad[0x800]; };` and work a
 * `char *c` through raw offsets. daPropeller_Heyho_c.h now supplies the real chain, so the
 * stand-in is gone and every offset is a named field.
 *
 * dEnemyBase_c::UpdateYoshiEat is still reached by its mangled name -- unlike
 * UpdateDeath and UpdateWMClsn, it is not declared in dEnemyBase_c.h yet.
 */
extern "C" {
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(dEnemyBase_c *thiz, dBgCh_Actr *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
}

int daPropeller_Heyho_c::Behavior()
{
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(this, &mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov070_02120070((char *)this);
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov070_02120070((char *)this);
        return 1;
    }

    if (mCurrentState != (State *)data_ov070_021235cc) {
        DecIfAbove0_Short((unsigned short *)&mStateTimer);
    }
    DecIfAbove0_Short(&unk_3cc);

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
        int v = mVertSpeed + mVertAccel;
        int hi = mTerminalVelocity;
        if (v >= hi) {
            hi = v;
        }
        int tmp = unk_0ac;
        mVertSpeed = hi;
        unk_0ac = tmp;
    }

    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (mCurrentState != (State *)data_ov070_021235bc) {
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    func_ov070_02120070((char *)this);

    if (mCurrentState != (State *)data_ov070_021235bc) {
        func_ov070_0211f100((char *)this);
    }

    mdCcAc_c.Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mdCcAc_c.Update();
        }
    }

    mModelAnim.speed = 0x1000;
    mModelAnim.Advance();
    return 1;
}
