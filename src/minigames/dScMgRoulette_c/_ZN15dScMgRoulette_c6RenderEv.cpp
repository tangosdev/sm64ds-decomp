//cpp
// @symbol _ZN15dScMgRoulette_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgRoulette_c.h"
/* dScMgRoulette_c::Render -- vtable slot 9, ov006 0x02109834.
 *
 * Named from the table: 0x02109834 is the word slot 9 of
 * _ZTV15dScMgRoulette_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares. The
 * pre-migration file had already retyped the receiver as dScMgRoulette_c and
 * named its fields; only the symbol was still a func_ov006_ one.
 *
 * Race minigame frame update: updates the racers back-to-front, plays the
 * countdown beep (volume ramps over the last 3 seconds), shows the winner
 * banner with per-rank colors, and refreshes the board.
 *
 * `c` and `self` are BOTH kept from the pre-migration file rather than folded
 * into `this`. The byte offsets that go through `c` -- 0x4660, 0x4f38, 0x51a8,
 * 0x530c -- land inside spans include/dScMgRoulette_c.h holds as opaque
 * members (mTable, mArray) or inside the base's own body, so spelling them as
 * members would be a layout claim this rename has no evidence for. */

typedef struct Racer {
    char b[0x34];
} Racer;

typedef struct Obj9 {
    char _p0[0x51a8];
    Racer racers[3]; /* 0x51a8 */
} Obj9;

extern "C" {
extern int data_020a0db0;

void func_ov004_020b1bc8(char *, int, int, int);
void Hud_RenderSprite(int, int, int, int, int);
void func_ov004_020b2220(int, int, int, int, int, int, int);
void Camera_UpdateMatrices(char *);
void func_ov006_020c0aa8(char *);
void func_ov006_020c1804(char *);
}

s32 dScMgRoulette_c::Render()
{
    char *c = (char *)this;
    struct dScMgRoulette_c *self = (struct dScMgRoulette_c *)(void *)c;
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();

    {
        int i = self->mRacerCount - 1;
        for (; i >= 0; i--)
            func_ov006_02108cc0(((Obj9 *)c)->racers[i].b);
    }

    func_ov006_02109aa0(c);

    {
        int idle = (int)(((long long)(self->unk_53c4 == 0)));
        if (idle != 0) {
            if (self->mPhase < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = self->mSelectedTile;
                    Hud_RenderSprite(data_ov006_02138c18,
                                        *(int *)(data_ov006_02142ab4 + idx * 8) >> 12,
                                        *(int *)(data_ov006_02142ab8 + idx * 8) >> 12,
                                        -1, 1);
                }
            }
        }
    }

    if (self->mPhase == 2) {
        int t = self->mPhaseTimer;
        if (t <= 0xb4) {
            int u = t + 0x3b;
            s16 rem = u % 60;
            int q = u / 60;
            int vol = rem * 0x88 + 0x1000;
            if (vol > 0x2000)
                vol = 0x2000;
            func_ov004_020b2220(0x80, 0x60, q, 1, -1, func_02053200(vol), 0);
        }
    }

    {
        s16 k = self->mCameraPreset;
        if (k != 0) {
            int off = k * 0xc;
            s16 hval = *(s16 *)(data_ov006_0213e2e0 + k * 2);
            {
                int d1 = *(int *)(data_ov006_0213e354 + off);
                int b1 = *(int *)(data_ov006_0213e350 + off);
                int a1 = *(int *)(data_ov006_0213e34c + off);
                self->mCameraTargetX = a1;
                self->mCameraTargetY = b1;
                self->mCameraTargetZ = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                self->mCameraEyeX = d2;
                self->mCameraEyeY = e2;
                self->mCameraEyeZ = f2;
            }
            self->mCameraAngle = hval;
            Camera_UpdateMatrices(c + 0x4660);
        } else {
            func_ov006_020c0aa8(c + 0x4660);
        }
        if (k != 0)
            func_ov006_02107d80(c + 0x530c);
        else
            func_ov006_020c1804(c + 0x4f38);
    }

    return 1;
}
