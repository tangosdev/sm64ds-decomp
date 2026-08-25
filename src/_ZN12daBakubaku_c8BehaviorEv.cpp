//cpp
// @symbol _ZN12daBakubaku_c8BehaviorEv
/* recovered: named members + shared header, real C++ method -- vtable slot 6
 *
 * THE POINTER-TO-MEMBER SHADOW BELOW STAYS, and it is the whole reason this file
 * looks the way it does. mState points at a state-table entry whose +8 is a
 * pointer-to-member; a PMF's representation depends on what the compiler knows
 * about the class it belongs to, so `Klass` must stay INCOMPLETE. Giving it a
 * definition -- or, worse, naming it daBakubaku_c -- makes mwccarm abort with an
 * internal compiler error rather than a diagnostic.
 *
 * What was wrong before was only the NAME: the stand-in never had to be called
 * Klass or to sit in the way of the real class. Everything else in this function
 * migrated once the header named its fields.
 */
#include "daBakubaku_c.h"
#include "dCc_c.h"
#include "Player.h"

/* Deliberately never defined. See above. */
struct Klass;
typedef void (Klass::*PMF)();
struct StateEntry { char pad[8]; PMF handler; };

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_ov032_02112044(daBakubaku_c *self);
void func_ov032_021113fc(daBakubaku_c *self);
extern char data_ov032_02113aac[];
}

s32 daBakubaku_c::Behavior()
{
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&unk_42a);

    StateEntry *state = (StateEntry *)mState;
    if (state->handler != 0)
        (((Klass *)this)->*(state->handler))();

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos(&mBodyClsn);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov032_02112044(this);

    if (mState != (void *)data_ov032_02113aac) {
        mModelAnim.speed = 0x1000;
    } else {
        mModelAnim.speed = 0x2000;
    }

    mModelAnim.Advance();
    func_ov032_021113fc(this);
    mBodyClsn.Clear();
    mHeadClsn.Clear();

    Player *p = ClosestPlayer();
    if (p != 0 && p->mIsVanish == 0) {
        mBodyClsn.Update();
        mHeadClsn.Update();
    }

    return 1;
}
