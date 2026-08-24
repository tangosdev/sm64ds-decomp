//cpp
// @symbol _ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Yoshi eats the cap off this enemy. ReleaseCap does the spawning; this
 * function's job is to HAND OVER the eater.
 *
 * The pointer compare is the whole trick: ReleaseCap returns `this` when the
 * cap was already released and the new actor when it was not, so `p != c` is
 * how "a cap actually came off just now" is tested without a second flag.
 *
 * On success mEatingPlayer moves to the new cap actor, the eater's own back-
 * reference at +0x360 is repointed at it, and flag 0x20000 moves across too --
 * the cap is now the thing being eaten, and this enemy is not.
 *
 * The model is only re-bound for bank-0 caps (mCapBank == 0).
 */
#include "dCapEnemy_c.h"

struct dActor_c;

extern "C" {
extern void func_ov001_020ab228(unsigned char *a, unsigned char *b, unsigned char idx, int e, unsigned char f);
}

int dCapEnemy_c::GetCapEatenOffIt(const Vector3 & v_)
{
    unsigned char *c = (unsigned char *)this;
    const Vector3 *v = &v_;
    Vector3 local;
    unsigned char *p;
    local.x = v->x;
    local.y = v->y;
    local.z = v->z;
    p = (unsigned char *)ReleaseCap(local);
    if (p != 0 && p != c) {
        unsigned char idx;
        *(int *)(p + 0xd0) = mEatingPlayer;
        *(unsigned char **)(*(unsigned char **)(c + 0xd0) + 0x360) = p;
        *(int *)(((int)p + 0xb0)) |= 0x20000;
        *(int *)(((int)c + 0xb0)) &= ~0x20000;
        mEatingPlayer = 0;
        if (mCapBank == 0) {
            idx = mCapId & 7;
            func_ov001_020ab228((unsigned char *)&mCapIcon, c, idx, 0, 0);
        }
        return 1;
    }
    return 0;
}
