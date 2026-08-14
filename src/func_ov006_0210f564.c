#include "types.h"
/* cMgSmartball_board_c slot 0 (SaveSnapshot). Opens with the base's own
 * SaveSnapshot body written out inline rather than called -- the same idiom
 * every sibling's slot 0 uses. Then runs the board's per-frame countdown:
 * every 60 ticks of the +0x94 timer it plays one of five warning sounds keyed
 * on +0x98, ages the eight +0x6c per-column timers, sweeps the manager's live
 * object list for anything standing in the board's y-band and inside one of
 * the nine column footprints, spawns the two effects for that column and
 * finally eases the nine +0x3c gauges up toward 0x8000. */

typedef struct V2 {
    s32 x;
    s32 y;
} V2;

/* One aggregate: the position read back from the object, its working copy,
 * and the three effect vectors. */
typedef struct W {
    V2 d;
    V2 v;
    V2 e;
    V2 f;
    V2 g;
} W;

typedef struct Board {
    char pad_000[4];    // 0x00
    char *unk_004;      // 0x04  owning manager
    s32 mCurrent0;      // 0x08
    s32 mCurrent1;      // 0x0c
    s32 mSnapshot0;     // 0x10
    s32 mSnapshot1;     // 0x14
    char pad_018[0x19]; // 0x18
    u8 byteA[9];        // 0x31
    char pad_03a[2];    // 0x3a
    s32 intB[9];        // 0x3c
    u8 byteC[9];        // 0x60
    char pad_069[3];    // 0x69
    s32 intD[8];        // 0x6c
    u8 byteE[8];        // 0x8c
    s32 unk_094;        // 0x94
    s32 unk_098;        // 0x98
} Board;

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
extern void func_02012718(void *a, int b);
extern void func_ov006_0211470c(int *a, int *b);
extern void func_ov006_02114800(void *a, int *p, int c);
extern int func_ov006_02111dcc(char *c, int value);
extern void func_ov006_0210ef48(void *self, int i);

extern s32 data_ov006_02142c1c[];

static inline u8 *GetObj(char *g, int i)
{
    return i >= 13 ? (u8 *)0 : ((u8 **)(g + 0x4688))[i];
}

static inline void SetXY(V2 *p, int x, int y)
{
    p->x = x;
    p->y = y;
}

void func_ov006_0210f564(Board *self)
{
    int i;
    int j;
    int n;
    W s;

    self->mSnapshot0 = self->mCurrent0;
    self->mSnapshot1 = self->mCurrent1;

    if (self->unk_094 > 0) {
        self->unk_094 -= 1;
        if (self->unk_094 > 0 && self->unk_094 % 60 == 0) {
            switch (self->unk_098) {
            case 0:
            case 1:
                _ZN5Sound12PlayBank2_2DEj(0x176);
                break;
            case 2:
                _ZN5Sound12PlayBank2_2DEj(0x177);
                break;
            case 3:
                _ZN5Sound12PlayBank2_2DEj(0x178);
                break;
            case 4:
            default:
                _ZN5Sound12PlayBank2_2DEj(0x179);
                break;
            }
        }
    }

    for (n = 0; n < 8; n++) {
        if (self->intD[n] > 0)
            self->intD[n] -= 1;
    }

    for (i = 0; i < *(s32 *)(self->unk_004 + 0x4668); i++) {
        if (*(u8 *)(self->unk_004 + 0x595c) != 0)
            break;
        if (GetObj(self->unk_004, i)[0x30] == 0)
            continue;
        func_ov006_0211470c((int *)&s.v, (int *)GetObj(self->unk_004, i));
        s.d = s.v;
        if (s.d.y < 0x74000)
            continue;
        if (s.d.y >= 0x7c000)
            continue;
        for (j = 0; j < 9; j++) {
            s32 idx = data_ov006_02142c1c[j];
            if (s.d.x < j * 0x18000 + 0xc000)
                continue;
            if (s.d.x >= j * 0x18000 + 0x14000)
                continue;
            if (func_ov006_02111dcc((char *)GetObj(self->unk_004, i), 0x14) == 0)
                continue;
            if (self->byteA[idx] == 0) {
                SetXY(&s.e, self->mCurrent0 + ((data_ov006_02142c1c[j] % 3 - 1) * 0x18 << 12),
                      self->mCurrent1 + ((data_ov006_02142c1c[j] / 3 - 1) * 0x18 << 12));
                func_ov006_02114800((void *)self->unk_004, (int *)&s.e, 0);
                SetXY(&s.f, (j * 0x18 + 0x10) << 12, 0x78000);
                func_ov006_02114800((void *)self->unk_004, (int *)&s.f, 0);
                func_02012718((void *)0x1be, j * 0x18000 + 0x10000);
            } else {
                SetXY(&s.g, (j * 0x18 + 0x10) << 12, 0x78000);
                func_ov006_02114800((void *)self->unk_004, (int *)&s.g, 1);
                func_02012718((void *)0x17a, j * 0x18000 + 0x10000);
            }
            func_ov006_0210ef48(self, j);
            break;
        }
    }

    for (i = 0; i < 9; i++) {
        if (self->byteA[i] == 1) {
            self->intB[i] += 0x400;
            if (self->intB[i] >= 0x8000)
                self->intB[i] = 0x8000;
        }
    }
}
