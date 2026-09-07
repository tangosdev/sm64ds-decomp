//cpp
// @symbol _ZN5Stump8BehaviorEv
#include "Stump.h"
#include "Player.h"

/* Stump::Behavior -- vtable slot 6, ov091 0x02133738.
 *
 * mState/mAngleDelta/mPrevAngle/mAttackCooldown are this class's own fields
 * (include/Stump.h); mPosX/Y/Z are dActor_c's own. ClosestPlayer,
 * GetBitInDeathTable, HorzAngleToCPlayer, DistToCPlayer and
 * TrackInDeathTable are dActor_c's own, called unqualified. SpawnCoins is
 * not yet declared on dActor_c, so it stays extern "C" under its exact ROM
 * symbol, same as DecIfAbove0_Byte and IsClsnInRangeOnScreen -- all three
 * the same as the pre-migration recovery.
 *
 * The by-value `V3` parameter below is the pre-migration recovery's own
 * local struct, kept private under a different name (types.h's real Vector3
 * is already in scope via Stump.h): its non-trivial ctor/copy-ctor/dtor is
 * load-bearing for the ABI shape (const Vector3& is passed as a caller-built
 * temporary, matched by the compiler's own by-value-with-non-trivial-copy
 * lowering) -- swapping in the real Vector3 changed the ABI enough to add
 * 4 bytes of register shuffling. */
struct V3 {
    int x, y, z;
    V3() {}
    V3(const V3 &o) { x = o.x; y = o.y; z = o.z; }
    ~V3() {}
};
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *c, int a, int b);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *c, V3 v, unsigned int n, int f, short s);
}

int Stump::Behavior()
{
    Player *player = ClosestPlayer();
    if (mState == 3
        && GetBitInDeathTable() == 0
        && player->mPosY < mPosY + 0x64000) {
        int angle = HorzAngleToCPlayer();
        if (DistToCPlayer() > 0x190000) {
            mAngleDelta = 0;
        } else {
            s16 diff = (s16)(angle - mPrevAngle);
            int val = mAngleDelta;
            int *p;
            if ((val > 0 && diff < -100) || (val < 0 && diff > 100))
                mAngleDelta = 0;
            p = &mAngleDelta;
            *p = *p + diff;
            {
                int a = mAngleDelta;
                if (a < 0) a = -a;
                if (a > 0x30000 && mBusy == 0
                    && GetBitInDeathTable() == 0) {
                    V3 v;
                    v.x = mPosX;
                    v.y = mPosY;
                    v.z = mPosZ;
                    v.y += 0xc8000;
                    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, v, 5, 0x5000, 0);
                    TrackInDeathTable();
                }
            }
        }
        mPrevAngle = angle;
    }
    DecIfAbove0_Byte((unsigned char *)&mAttackCooldown);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    return 1;
}
