//cpp
// @symbol func_ov027_021116f0
/* ChillBully's star drop, shared with the Behavior above it.
 *
 * It used to carry the recovered name `daIDonketu_c::AfterClsn`, attributed "from
 * vtable slot identity" -- but this address is in NO slot of ChillBully's vtable.
 * All 31 were checked against daOts_c's and Enemy's; the four this class overrides
 * are 0 (InitResources), 6 (Behavior) and 16/17 (the destructor pair). So the name
 * is withdrawn and the placeholder kept until something actually names it.
 */
#include "ChillBully.h"
#include "decl_common.h"

extern "C" {
extern int func_ov064_0211616c(void *);

int func_ov027_021116f0(char *c)
{
    ChillBully *self = (ChillBully *)(void *)c;
    int r = func_ov064_0211616c(c);
    if (r == 0) return r;
    self->TriplePoofDust();
    int v[3];
    v[0] = self->mPosX;
    v[1] = self->mPosY;
    v[2] = self->mPosZ;
    v[1] += 0x64000;
    self->UntrackAndSpawnStar(self->mStarSlot, (self->mStarIdx | 0x40) & 0xff,
                              *(const Vector3 *)v, 4);
}
}
