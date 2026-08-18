//cpp
// @symbol _ZN10ChillBully8BehaviorEv
/* recovered: real C++ method -- vtable slot 6, previously func_ov027_02111770
 *
 * daOts_c leaves slot 6 pure virtual, so each of its three children supplies its own.
 * ChillBully's is the star-carrying one: if an invincible character killed it, it
 * poofs, drops its star a little above itself, and marks itself for destruction;
 * otherwise it falls through to the shared behaviour in ov064.
 *
 * Every call below is a real method now, so the compiler mangles the names and the
 * signatures become falsifiable -- including UntrackAndSpawnStar's `s8 &`, which is
 * why mStarSlot is signed. See the long note on that symbol in dActor_c.h.
 */
#include "ChillBully.h"

extern "C" int func_ov064_02116d1c(char *c);

int ChillBully::Behavior()
{
    int r = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 0);
    if (r != 0) {
        if (r == 2) {
            Vector3 v;
            TriplePoofDust();
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + 0x64000;
            UntrackAndSpawnStar(mStarSlot, (mStarIdx | 0x40) & 0xff, v, 4);
            MarkForDestruction();
        }
        return 1;
    }
    return func_ov064_02116d1c((char *)this);
}
