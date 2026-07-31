// @symbol func_ov006_02109834
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgRoulette_c.h"
// @emits dScMgRoulette_c_Render
/* recovered: renamed to Class_Method */
/* dScMgRoulette_c::Render - recovered from vtable slot identity */
/* dScMgRoulette_c_Render @ 0x02109834 (ov006, size 0x26c)
 * Race minigame frame update: updates the racers back-to-front, plays the
 * countdown beep (volume ramps over the last 3 seconds), shows the winner
 * banner with per-rank colors, and refreshes the board.
 */
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

typedef struct Racer {
    char b[0x34];
} Racer;

typedef struct Obj9 {
    char _p0[0x51a8];
    Racer racers[3]; /* 0x51a8 */
} Obj9;

extern int data_020a0db0;

extern void func_ov004_020b1bc8(char *, int, int, int);
extern void func_ov004_020af68c(int, int, int, int, int);
extern void func_ov004_020b2220(int, int, int, int, int, int, int);
extern void Camera_UpdateMatrices(char *);
extern void func_ov006_020c0aa8(char *);
extern void func_ov006_020c1804(char *);

int dScMgRoulette_c_Render(char *c)
{
    struct dScMgRoulette_c *self = (struct dScMgRoulette_c *)(void *)c;
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();

    {
        int i = self->unk_53fc - 1;
        for (; i >= 0; i--)
            func_ov006_02108cc0(((Obj9 *)c)->racers[i].b);
    }

    func_ov006_02109aa0(c);

    {
        int idle = (int)(((long long)(self->unk_53c4 == 0)));
        if (idle != 0) {
            if (self->unk_53e6 < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = self->unk_53d6;
                    func_ov004_020af68c(data_ov006_02138c18,
                                        *(int *)(data_ov006_02142ab4 + idx * 8) >> 12,
                                        *(int *)(data_ov006_02142ab8 + idx * 8) >> 12,
                                        -1, 1);
                }
            }
        }
    }

    if (self->unk_53e6 == 2) {
        int t = self->unk_53e8;
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
        s16 k = self->unk_53e4;
        if (k != 0) {
            int off = k * 0xc;
            s16 hval = *(s16 *)(data_ov006_0213e2e0 + k * 2);
            {
                int d1 = *(int *)(data_ov006_0213e354 + off);
                int b1 = *(int *)(data_ov006_0213e350 + off);
                int a1 = *(int *)(data_ov006_0213e34c + off);
                self->unk_470c = a1;
                self->unk_4710 = b1;
                self->unk_4714 = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                self->unk_4700 = d2;
                self->unk_4704 = e2;
                self->unk_4708 = f2;
            }
            self->unk_4718 = hval;
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
