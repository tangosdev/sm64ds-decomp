//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here).
 *
 * Then: spend the countdown at mFanfareTimer and, once a second, replay the jingle
 * selected by mLinesScored; age the eight per-line timers in mLineTimer; scan the
 * shared manager's up-to-13 tracked balls for one sitting in the board's
 * horizontal strip, map its x to one of the nine cells through the shuffle
 * table data_ov006_02142c1c and claim that cell; finally spin every claimed
 * cell's angle in mCellFlipAngle up to its 0x8000 stop.
 *
 * The nine cell bytes are reached as (&unk_031)[k] -- the array begins at the
 * BASE's unk_031 and runs into this class's mCellsTail[6], so no member can name
 * it. See cMgSmartball_board_c.h. */
#include "cMgSmartball_board_c.h"

typedef struct { int x; int y; } V2;

extern "C" int data_ov006_02142c1c[];

extern "C" void func_ov006_0211470c(int *a, int *b);
extern "C" int func_ov006_02111dcc(char *p, int val);
extern "C" void func_ov006_02114800(void *c, int *p, int f);
extern "C" void func_ov006_0210ef48(void *c, int i);
extern "C" void func_02012718(void *a, int b);
namespace Sound { void PlayBank2_2D(u32 id); }

static inline u8 *GetObj(char *g, int i)
{
    return i >= 13 ? 0 : ((u8 **)(g + 0x4688))[i];
}

static inline void SetV2(V2 *p, int x, int y)
{
    p->x = x;
    p->y = y;
}

/* Load-bearing for this spelling: without it the function is 5 words too long.
 * Not a property of the function itself -- match/board-slot0-a reproduces the
 * same bytes with different statement order and no pragma at all. */
#pragma opt_strength_reduction off
void cMgSmartball_board_c::SaveSnapshot()
{
    int i;
    int n;
    int j;
    int lo;
    int hi;
    V2 v;
    V2 t;
    V2 a;
    V2 b;
    V2 d;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mFanfareTimer > 0) {
        mFanfareTimer--;
        if (mFanfareTimer > 0 && mFanfareTimer % 60 == 0) {
            switch (mLinesScored) {
            case 0:
            case 1:
                Sound::PlayBank2_2D(0x176);
                break;
            case 2:
                Sound::PlayBank2_2D(0x177);
                break;
            case 3:
                Sound::PlayBank2_2D(0x178);
                break;
            case 4:
            default:
                Sound::PlayBank2_2D(0x179);
                break;
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0)
            mLineTimer[i]--;
    }
    for (n = 0; n < *(int *)((char *)mpManager + 0x4668); n++) {
        if (*(u8 *)((char *)mpManager + 0x595c) != 0)
            break;
        if (GetObj((char *)mpManager, n)[0x30] == 0)
            continue;
        func_ov006_0211470c((int *)&t, (int *)GetObj((char *)mpManager, n));
        /* COMPILER STEER, and the only one in this file. `v = t;` is what the
           ROM does and it is what the pre-migration C said -- but mwcc
           scalarizes a whole-struct assignment in C++ where it kept it in
           memory in C, which deletes v's stack slot, its two stores and the
           reload of v.x in the loop below: 0x3a4 bytes instead of 0x3b0.
           Casting the DESTINATION alone restores the memory-to-memory copy;
           casting the source instead does nothing. Measured, not guessed --
           six other spellings (memberwise, copy-init, a reference alias, an
           sret-returning helper, a non-POD V2, a V2 with a base) were all
           tried and all diverge. See launder.h for the family this belongs
           to. */
        *(V2 *)&v = t;
        if (v.y < 0x74000)
            continue;
        if (v.y >= 0x7c000)
            continue;
        for (j = 0, lo = 0xc000, hi = 0x14000; j < 9; j++) {
            int slot = data_ov006_02142c1c[j];
            if (v.x >= lo && v.x < hi) {
                if (func_ov006_02111dcc((char *)GetObj((char *)mpManager, n), 0x14)) {
                    if ((&unk_031)[slot] == 0) {
                        SetV2(&a, mCurrent0 + ((data_ov006_02142c1c[j] % 3 - 1) * 0x18 << 12),
                              mCurrent1 + ((data_ov006_02142c1c[j] / 3 - 1) * 0x18 << 12));
                        func_ov006_02114800((char *)mpManager, (int *)&a, 0);
                        SetV2(&b, (j * 0x18 + 0x10) << 12, 0x78000);
                        func_ov006_02114800((char *)mpManager, (int *)&b, 0);
                        func_02012718((void *)0x1be, j * 0x18000 + 0x10000);
                    } else {
                        SetV2(&d, (j * 0x18 + 0x10) << 12, 0x78000);
                        func_ov006_02114800((char *)mpManager, (int *)&d, 1);
                        func_02012718((void *)0x17a, j * 0x18000 + 0x10000);
                    }
                    func_ov006_0210ef48(this, j);
                    break;
                }
            }
            lo += 0x18000;
            hi += 0x18000;
        }
    }
    for (i = 0; i < 9; i++) {
        if ((&unk_031)[i] == 1) {
            mCellFlipAngle[i] += 0x400;
            if (mCellFlipAngle[i] >= 0x8000)
                mCellFlipAngle[i] = 0x8000;
        }
    }
}
