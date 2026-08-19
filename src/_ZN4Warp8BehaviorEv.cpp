//cpp
// @symbol _ZN4Warp8BehaviorEv
// recovered name: Warp::Behavior
/* Warp::Behavior - vtable slot 6, overriding fBase_c::Behavior(). Migrated
 * to a real member: `c + 0xf8` is mdCcAc_c.otherOwner
 * (include/dCc_c.h, 0x24 into the embedded dCcAc_c at
 * 0xd4 -- 0xd4 + 0x24 = 0xf8) and `c + 0xd4` casts to that same member's
 * base for Clear()/Update(); `c + 0x8e` and `c + 8` are dActor_c's own
 * mAngleY and param1, already recovered in dActor_c.h (mAngleY is
 * repurposed here as a warp-in-progress flag, not an angle). The found
 * actor's own fields (actorID 0x00c, mPlayerNo 0x6d8) are real Player
 * members once cast. */
#include "Warp.h"
#include "Player.h"

extern "C" int IsPlayerWarping(int a0);
extern "C" void WarpPlayer(int i, int val);

s32 Warp::Behavior()
{
    unsigned int id = mdCcAc_c.otherOwner;
    if (id != 0) {
        if (mAngleY == 0) {
            dActor_c *a = dActor_c::FindWithID(id);
            if (a != 0) {
                int isPlayer = (a->actorID == 0xbf) ? 1 : 0;
                if (isPlayer != 0) {
                    Player *p = (Player *)a;
                    if (IsPlayerWarping(p->mPlayerNo) != 0) {
                        mAngleY = 1;
                    } else if (p->CanWarp()) {
                        unsigned int param = (param1 >> 0xc) + 1;
                        WarpPlayer(p->mPlayerNo, param & 0xff);
                    }
                }
            }
        }
    } else {
        mAngleY = 0;
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
