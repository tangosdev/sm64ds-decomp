//cpp
// @symbol _ZN6Bullet8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bullet.h"

/* This file used to carry `struct Actor { char pad[0x350]; Holder* h; }` and
 * cast `this` to it. That stand-in was how the state pointer at 0x350 was
 * reached; Bullet.h declares it now, so the cast and the three dummy structs
 * are gone.
 */
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
}

int Bullet::Behavior()
{
    DecIfAbove0_Short((unsigned short*)&unk_100);
    State* h = mCurrentState;
    /* Reads the handler's pointer word directly rather than as `&h->mMain`:
       taking the ADDRESS of a pointer-to-member makes mwcc materialise the
       whole 8-byte pmf. Reading one to CALL it is free. */
    if (*(int*)((char*)h + 8) != 0) {
        (this->*(h->mMain))();
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int spd = mVertSpeed;
        int pos = mVertAccel;
        int lim = mTerminalVelocity;
        int ac = unk_0ac;
        int np = spd + pos;
        if (np >= lim) lim = np;
        mVertSpeed = lim;
        unk_0ac = ac;
        UpdatePosWithOnlySpeed(&mMovingCylinderClsn);
    }
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleY = mPrevAngleY;
    func_ov002_020fed7c((char*)this);
    mMovingCylinderClsn.Clear();
    mMovingCylinderClsn.Update();
    return 1;
}
