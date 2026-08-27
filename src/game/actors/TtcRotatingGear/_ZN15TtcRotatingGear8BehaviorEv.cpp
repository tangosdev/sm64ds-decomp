//cpp
// @symbol _ZN15TtcRotatingGear8BehaviorEv
/* TtcRotatingGear::Behavior -- Tick Tock Clock's lifts.
 *
 * data_0209f2c0 is the level's clock hand setting. 3 ("random") pins the lift
 * at the top of its travel; otherwise it runs on a timer, and each time the
 * timer expires the gear clamps itself back inside [mHomePosY, mHomePosY +
 * 0x14a000], flips mMoveDir, and reloads speed and timer from the two 16-byte
 * per-setting tables. Setting 2 overrides the timer with a random multiple of
 * 0x14 in [0xa, 0x6e].
 *
 * The control flow is the ROM's, gotos and all -- the clamp reads as an
 * if/else but compiles to the interleaved form only in this shape.
 *
 * IsClsnInRange carries Fix12<int> by value (wall 6az), so it stays an extern-C
 * mangled free function; UpdateModelPosAndRotY, UpdateClsnPosAndRot and
 * UpdatePos are members.
 */
#include "TtcRotatingGear.h"

extern "C" {
extern u8  data_0209f2c0;      /* clock hand setting */
extern int data_0209e650;      /* the shared RNG seed */
extern u8  data_ov065_0211c0d4[];  /* per-setting timer table, 16-byte stride */
extern u8  data_ov065_0211c0d0[];  /* per-setting speed table, 16-byte stride */

u16 DecIfAbove0_Short(void *p);
int RandomIntInternal(int *seed);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
}

int TtcRotatingGear::Behavior()
{
    if (data_0209f2c0 == 3) {
        mPosY = mHomePosY + 0x14a000;
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
            UpdateClsnPosAndRot();
        goto ret1;
    }

    if (DecIfAbove0_Short(&mMoveTimer) == 0) {

        s32 lower, upper, y, in;
        UpdatePos(0);
        lower = mHomePosY;
        y = mPosY;
        upper = lower + 0x14a000;
        if (y < lower)
            goto Lzero;
        if (y <= upper) {
            in = 1;
            goto Lchk;
        }
Lzero:
        in = 0;
Lchk:
        if (in != 0)
            goto tail;
        {
            s32 v = lower;
            if (y >= lower) {
                s32 t = upper;
                if (y <= upper)
                    t = y;
                v = t;
            }
            mPosY = v;
        }

        {
            /* THE LAUNDERED ADDRESS IS LOAD-BEARING, per-site. Spelling this
               as `mMoveDir ^= 1' or as a load/store pair through the member
               changes the function's SIZE (999 words, i.e. a length mismatch),
               not just its allocation -- the same RMW-address CSE described in
               notes/mwccarm-codegen.md. Taking the address through an integer
               first defeats it. */
            u8 *tog = (u8 *)((int)this + 0x32e);
            u8 dir = *tog;
            *tog = dir ^ 1;
            {
                int idx = data_0209f2c0;
                u8 t = mMoveDir;
                mMoveTimer = *(u16 *)(&data_ov065_0211c0d4[idx * 16] + t * 8);
                t = mMoveDir;
                mVertSpeed = *(s32 *)(&data_ov065_0211c0d0[idx * 16] + t * 8);
                if (data_0209f2c0 != 2)
                    goto tail;
                {
                    u32 r = (u32)RandomIntInternal(&data_0209e650);
                    mMoveTimer = (u16)((r % 6) * 0x14 + 0xa);
                }
            }
        }

    }

tail:
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
ret1:
    return 1;
}
