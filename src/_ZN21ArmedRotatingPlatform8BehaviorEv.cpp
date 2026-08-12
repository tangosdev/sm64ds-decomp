//cpp
// @symbol _ZN10DonutBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * Spin by unk_31e a frame; stop and switch the collider off once the actor is
 * flagged; otherwise follow the model and, when the player is close enough to
 * matter, the collision mesh too.
 *
 * Three shadow declarations are gone -- a `typedef int Fix12` and stand-in
 * `MeshColliderBase` and `Platform` structs -- along with the raw offsets they
 * served. `this + 0x8e` is Actor::mAngleY, and the `t[0x1e/2]` read off
 * `this + 0x300` was this class's own unk_31e reached the long way round.
 *
 * IsClsnInRange keeps its mangled spelling: the name carries Fix12<int> by
 * value, wall 6az, so a real call would not reproduce the call site.
 */
#include "DonutBlock.h"

extern "C" bool _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

int DonutBlock::Behavior()
{
    /* `+=`, not `mAngleY = mAngleY + ...`: the compound form CSEs the field
       address into a register and reuses it for the load and the store, which
       is what the ROM does here. That is a per-SITE fact -- the expanded
       spelling is the matching one in Player::St_WallJump_Init. */
    mAngleY += unk_31e;

    /* The temporary is load-bearing: the ROM materialises the predicate with
       movne/moveq and then tests it, where a direct `if` folds the two. */
    int flagged = (int)((mFlags & 8) != 0);
    if (flagged != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}
