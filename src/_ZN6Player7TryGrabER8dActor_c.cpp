//cpp
// @symbol _ZN6Player7TryGrabER8dActor_c
/* recovered: named members + shared header, real C++ method
 *
 * Refuses if already holding something or if the target does not carry the
 * grabbable flag (0x80). Grabbing is allowed from the punch state past step 2,
 * from swimming, or -- for actor IDs other than 0xbd/0xbe -- from the crouch
 * state.
 */
#include "Player.h"
extern "C" {
extern int func_ov002_020dacb4(void* p, void* actor);
extern Player::State data_ov002_02110574;   /* punch */
extern Player::State data_ov002_0211067c;   /* swim */
extern Player::State data_ov002_021105bc;   /* crouch */
}

int Player::TryGrab(dActor_c & actor_)
{
    dActor_c *actor = &actor_;
    int b = (mHeldObj != 0);
    if (b) return 0;
    if ((actor->mFlags & 0x80) == 0) return 0;
    if (!(IsState(data_ov002_02110574) && mPunchKickStep != 2)) {
        if (!IsState(data_ov002_0211067c)) goto bd;
    }
    func_ov002_020dacb4(this, actor);
    return 1;
bd:
    {
        unsigned short t = actor->actorID;
        int b0 = (t == 0xbd);
        if (!b0) {
            int b1 = (t == 0xbe);
            if (!b1) {
                if (IsState(data_ov002_021105bc)) {
                    func_ov002_020dacb4(this, actor);
                    return 1;
                }
            }
        }
    }
    return 0;
}
