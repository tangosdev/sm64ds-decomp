//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. Then, while
 * mActionTimer is still counting down from a previous trigger, it just
 * spends the timer and returns without scanning. Otherwise it scans the
 * shared manager's up-to-13 tracked objects for one within a fixed-point
 * distance of 0x4000 and, on a hit, arms mActionTimer, flips unk_031
 * off, fires an effect, deactivates the object it found, and returns. If
 * the whole scan comes up empty, mIdleTimer counts the miss and wraps at
 * 0x3c, toggling unk_031. */
#include "cMgSmartball_pakkun_c.h"

typedef int Fix12i;

extern "C" void func_ov006_0211470c(int *a, int *b);
extern "C" void Vec2_Sub(int *o, int *a, int *b);
extern "C" Fix12i Vec2_Len(const void *v);
extern "C" void func_ov006_0210d8bc(char *c);
extern "C" void func_02012718(void *a, int b);

inline int *GetObj(char *g, int i)
{
    return i >= 13 ? 0 : *(int **)(g + 0x4688 + i * 4);
}

#pragma opt_strength_reduction off
void cMgSmartball_pakkun_c::SaveSnapshot()
{
    int s[5];
    int i;
    int b;

    if (mIsActive == 0) return;
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mActionTimer > 0) {
        mActionTimer -= 1;
        if (mActionTimer == 0) mIsActive = 0;
        return;
    }
    for (i = 0; i < *(int *)((char *)mpManager + 0x4668); i++) {
        func_ov006_0211470c(&s[0], GetObj((char *)mpManager, i));
        Vec2_Sub(&s[2], &mCurrent0, &s[0]);
        b = Vec2_Len(&s[2]) < 0x4000;
        if (b) {
            mActionTimer = 0x61;
            unk_031 = 0;
            func_ov006_0210d8bc(*(char **)((char *)mpManager + 0x4780));
            *((char *)GetObj((char *)mpManager, i) + 0x30) = 0;
            func_02012718((void *)0x1a0, mCurrent0);
            return;
        }
    }
    mIdleTimer += 1;
    if (mIdleTimer < 0x3c) return;
    mIdleTimer = 0;
    if (unk_031 == 0) unk_031 = 1;
    else unk_031 = 0;
}
