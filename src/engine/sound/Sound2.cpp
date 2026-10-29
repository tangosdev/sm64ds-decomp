//cpp
/* arm9/Sound2 -- the positional sound helpers, .text 0x02048528..0x020490b0.
 *
 * Everything here works out how loud a positioned sound is and where it
 * pans, from its distance to the listener. Sound.cpp is the only caller.
 *
 *   func_02049018  a position's distance from the origin (y counted
 *                  twice), through the hardware square-root unit
 *   func_02048fd4  load the near/far audible range for a sound mode
 *   func_02048f34  store the owner, clear the voice slots, select mode 0
 *   func_02048f24..func_02048eb4
 *                  eight tail-call thunks into func_02048fd4, r0 = 1..7, 0
 *   func_02048e94 / e74 / e54
 *                  set the slot-9 voice count back to four
 *   func_02048d80, func_02048c30, func_02048af4, func_02048908,
 *   func_02048528, func_020485f0
 *                  set a voice's volume from its distance (linear between
 *                  the near and far range, with per-sound overrides) and its
 *                  pan from the position's x
 *   func_02048a1c  is the sound inside its audible range?
 *   func_02048a18  empty
 *   func_02048720  pick a free (or the weakest) voice slot for a sound
 *
 * Edges: func_02048234 below is a matrix builder ModelComponents calls, and
 * func_020490b0 above starts the CP15/IRQ code. Folded from fourteen
 * shards; func_02049018 came from the root shard.
 *
 * The three Func_ wrappers are Sound:: entry points the compiler mangles
 * itself; the rest keep their ROM names under C linkage. mwccarm emits .text
 * in reverse source order, so the definitions below run ROM-descending.
 */

#include "types.h"
#include "Sound.h"

extern "C" {

/* The fields of a playing sound these functions read: the (kind, id) pair
   Sound.cpp passes to func_02048a1c, and the voice priority. */
typedef struct SoundHandle {
    u8 pad_00[0x38];
    u16 kind; /* 0x38 */
    u16 id;   /* 0x3a */
    u8 pad_3c;
    u8 priority; /* 0x3d */
} SoundHandle;

/* One voice slot: the sound handle and its last listener distance. */
typedef struct SoundSlot {
    void *obj;
    int dist;
} SoundSlot;

extern u16 data_02099fa4;   /* pan channel */
extern u32 data_02099fa8;   /* near range: full volume inside it */
extern u32 data_02099fac;   /* far range: silent beyond it */
extern int data_02099fb0;   /* slot-9 voice count */
extern u32 data_020821c0[]; /* near/far pairs, one per sound mode */
extern int *data_020a4bec;  /* the registry owner */
extern SoundSlot data_020a4c18[];
extern SoundSlot data_020a4c08[];
extern SoundSlot data_020a4bf8[];
extern SoundSlot data_020a4bf0;

extern void func_0204f924(void *obj, int volume);
extern void func_0204f7cc(void *obj, int channel, int pan);
extern void func_0204f7fc(void *obj, int a, int b);
extern int func_0204fa2c(int *obj, int v);
extern void func_0204f934(int **slot);
extern void func_0204f94c(void *slot);
extern void *func_02050cdc(int kind, int id);
extern int __aeabi_idiv(int a, int b);

// @symbol func_02049018
int func_02049018(Vector3 *v) {
    int tx = v->x >> 12;
    int ty = (v->y >> 12) * 2;
    int tz = v->z >> 12;
    int sum = tx * tx + ty * ty + tz * tz;
    volatile unsigned short *ime = (volatile unsigned short *)0x4000208;
    unsigned short saved = *ime;
    *ime = 0;
    *(volatile unsigned short *)0x40002b0 = 0;
    *(volatile unsigned int *)0x40002b8 = sum;
    *ime;
    *ime = saved;
    while (*(volatile unsigned short *)0x40002b0 & 0x8000)
        ;
    while (*(volatile unsigned short *)0x40002b0 & 0x8000)
        ;
    return *(volatile int *)0x40002b4;
}

// @symbol func_02048fd4
/* Copies pair i (i in 0..8) from data_020821c0 into the near/far range. */
void func_02048fd4(int i) {
    volatile u32 *tbl;
    int j;
    if (i < 0)
        return;
    if (i > 8)
        return;
    tbl = data_020821c0;
    j = i * 2;
    data_02099fa8 = tbl[j];
    j++;
    data_02099fac = tbl[j];
}

// @symbol func_02048f34
void func_02048f34(int *owner) {
    int i;
    data_020a4bec = owner;
    for (i = 0; i < 6; i++)
        func_0204f94c(&data_020a4c18[i]);
    for (i = 0; i < 2; i++)
        func_0204f94c(&data_020a4c08[i]);
    for (i = 0; i < 2; i++)
        func_0204f94c(&data_020a4bf8[i]);
    func_0204f94c(&data_020a4bf0);
    func_02048fd4(0);
}

// @symbol func_02048f24
void func_02048f24(void) {
    func_02048fd4(1);
}

// @symbol func_02048f14
void func_02048f14(void) {
    func_02048fd4(2);
}

// @symbol func_02048f04
void func_02048f04(void) {
    func_02048fd4(3);
}

// @symbol func_02048ef4
void func_02048ef4(void) {
    func_02048fd4(4);
}
}

namespace Sound {

// @symbol _ZN5Sound13Func_02048ee4Ev
void Func_02048ee4() {
    func_02048fd4(5);
}

}

// @symbol func_02048ed4
extern "C" void func_02048ed4(void) {
    func_02048fd4(6);
}

namespace Sound {

// @symbol _ZN5Sound13Func_02048ec4Ev
void Func_02048ec4() {
    func_02048fd4(7);
}

// @symbol _ZN5Sound13Func_02048eb4Ev
void Func_02048eb4() {
    func_02048fd4(0);
}

}

extern "C" {

// @symbol func_02048e94
void func_02048e94(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

// @symbol func_02048e74
void func_02048e74(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

// @symbol func_02048e54
void func_02048e54(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

// @symbol func_02048d80
void func_02048d80(void *obj, Vector3 *p) {
    int s = p->x >> 12;
    int hi = data_02099fac;
    int lo = data_02099fa8;
    int r1;
    int cur;
    if (obj == 0)
        return;
    if (*(int *)obj == 0)
        return;
    cur = func_02049018(p);
    if ((unsigned)cur >= (unsigned)hi)
        return;
    if ((unsigned)lo > (unsigned)cur)
        r1 = 0x7f;
    else
        r1 = (hi - cur) * 0x7f / (hi - lo);
    func_0204f924(obj, r1);
    s = s / 2;
    if (s > 0x3f)
        s = 0x3f;
    else if (s < -0x40)
        s = -0x40;
    func_0204f7cc(obj, data_02099fa4, s);
}

// @symbol func_02048c30
void func_02048c30(int *self, Vector3 *p, int q) {
    int t;
    int a;
    int b = q >> 12;
    unsigned int n, lo, hi;
    if (self == 0)
        return;
    if (*self == 0)
        return;
    if (p != 0) {
        a = p->x >> 12;
        n = func_02049018(p);
        hi = data_02099fac;
        if (n < hi) {
            lo = data_02099fa8;
            if (lo > n) {
                t = 0x7f;
            } else {
                t = (int)((hi - n) * 0x7f) / (int)(hi - lo);
            }
        } else {
            func_0204fa2c(self, 0);
        }
    } else {
        t = 0x7f;
        a = 0x40;
    }
    if (b < 0) {
        t = 0x18;
    } else if (b <= 0x28) {
        t = t * (b * 103 / 40 + 0x18) / 0x7f;
    }
    func_0204f924(self, t);
    a = a / 2;
    if (a > 0x3f) {
        a = 0x3f;
    } else if (a < -0x40) {
        a = -0x40;
    }
    func_0204f7cc(self, data_02099fa4, a);
}

// @symbol func_02048af4
void func_02048af4(int *self, Vector3 *p, int q) {
    int a = p->x >> 12;
    int b = q >> 12;
    unsigned int n, lo, hi;
    int t;
    if (self == 0)
        return;
    if (*self == 0)
        return;
    n = func_02049018(p);
    hi = data_02099fac;
    if (n < hi) {
        lo = data_02099fa8;
        if (lo > n) {
            t = 0x7f;
        } else {
            t = (int)((hi - n) * 0x7f) / (int)(hi - lo);
        }
        if (b < 0) {
            t = 0x3f;
        } else if (b <= 0x28) {
            t = t * (b * 64 / 40 + 0x3f) / 0x7f;
        }
        func_0204f924(self, t);
        func_0204f7fc(self, 3, (b - 0x28) * 2);
        a = a / 2;
        if (a > 0x3f) {
            a = 0x3f;
        } else if (a < -0x40) {
            a = -0x40;
        }
        func_0204f7cc(self, data_02099fa4, a);
        return;
    }
    func_0204fa2c(self, 0);
}

// @symbol func_02048a1c
int func_02048a1c(Vector3 *pos, int kind, int id) {
    u32 lim = data_02099fac;
    u32 dist;
    if (kind == 3) {
        if (id >= 0x100 && id <= 0x103) {
            lim = 0x8bc;
        } else if (id == 0x18d) {
            lim = 0x1f4;
        } else if (id == 0x81) {
            lim = 0x4b0;
        } else {
            if (id >= 0x99 && id <= 0x9a) {
                lim = 0x384;
            }
        }
    } else if (kind == 1) {
        if (id >= 0xea) {
            if (id <= 0x2b) {
                lim = 0x44c;
            }
        }
    }
    dist = (u32)func_02049018(pos);
    if (lim < dist) {
        return 0;
    }

    if (data_020a4bec == 0) {
        return 1;
    }
    func_02050cdc(kind, id);
    return 1;
}

// @symbol func_02048a18
void func_02048a18(void) {}

// @symbol func_02048908
void func_02048908(void *obj, Vector3 *p) {
    int num = data_02099fac;
    int den = data_02099fa8;
    int a = 0x7f;
    int b = 0;
    SoundHandle *s;
    int typ, val;
    int cur, arg;
    int half;

    if (obj == 0)
        return;
    s = *(SoundHandle **)obj;
    if (s == 0)
        return;

    typ = s->kind;
    val = s->id;

    if (typ == 3) {
        if (val >= 0x4b && val <= 0x4c) {
            num = 0x384;
            den = 0xc8;
            b = 0x28;
        } else if (val == 0x28) {
            b = 0x78;
            a = 0x7f;
        }
    } else if (typ == 1) {
        if (val >= 0x2a && val <= 0x2b) {
            num = 0x44c;
            den = 0xc8;
        }
    }

    cur = ((int *)obj)[1];
    if ((unsigned)cur < (unsigned)den)
        arg = a;
    else
        arg = b + __aeabi_idiv((a - b) * (num - cur), num - den);
    func_0204f924(obj, arg);

    half = p->x >> 12;
    half = half / 2;
    if (half > 0x3f)
        half = 0x3f;
    else if (half < -0x40)
        half = -0x40;
    func_0204f7cc(obj, data_02099fa4, half);
}

// @symbol func_02048720
void *func_02048720(Vector3 *v, int kind, int id) {
    u32 lim;
    void *elem;
    u8 typ;
    u32 pri;
    int i;
    u32 dist;
    u32 p;
    u32 best_pri;
    u32 best_dist;
    int best_idx;
    int pick;
    int count;
    SoundSlot *table;

    lim = data_02099fac;
    best_idx = -1;
    pick = best_idx;
    best_pri = 0xff;
    best_dist = 0;

    elem = func_02050cdc(kind, id);
    if (elem == 0) {
        return 0;
    }

    typ = ((u8 *)elem)[5];
    pri = ((u8 *)elem)[4];

    switch (typ) {
    case 2:
        count = 2;
        table = data_020a4bf8;
        break;
    case 9:
        count = data_02099fb0;
        table = data_020a4c18;
        break;
    default:
        return 0;
    }

    dist = (u32)func_02049018(v);

    if (id >= 0x4b && id <= 0x4c) {
        lim = 0x384;
    }
    if (dist > lim) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (table[i].obj == 0)
            pick = i;
    }

    if (pick < 0) {
        for (i = 0; i < count; i++) {
            p = ((SoundHandle *)table[i].obj)->priority;
            if (best_pri > p) {
                best_pri = p;
                best_idx = i;
                best_dist = (u32)table[i].dist;
            } else if (best_pri == p) {
                if (best_dist < (u32)table[i].dist) {
                    best_dist = (u32)table[i].dist;
                    best_idx = i;
                }
            }
        }

        if (pri > best_pri) {
            pick = best_idx;
        } else if (pri == best_pri && dist <= best_dist) {
            pick = best_idx;
        }
    }

    if (pick < 0) {
        return 0;
    }
    if (pick >= 0 && pick < count) {
        func_0204fa2c((int *)&table[pick], 1);
        func_0204f934((int **)&table[pick]);
        table[pick].dist = (int)dist;
        return &table[pick];
    }
    return 0;
}

// @symbol func_020485f0
void func_020485f0(int *thiz, Vector3 *a) {
    int r5 = a->x >> 12;
    unsigned int x;
    u16 r4;
    int v;
    if (thiz == 0)
        return;
    if (thiz[0] == 0)
        return;
    x = func_02049018(a);
    r4 = ((SoundHandle *)thiz[0])->id;
    if (r4 == 0x81) {
        if (x > 0x4b0)
            v = 0;
        else if (x <= 0x64)
            v = 0x7f;
        else
            v = (int)(0x4b0 - x) * 0x7f / 1100;
    } else {
        if (x >= 0x8bc)
            v = 0;
        else if (x < 0x3e8)
            v = 0x7f;
        else
            v = (int)(0x8bc - x) * 0x7f / 1236;
    }
    func_0204f924(thiz, v);
    r5 = r5 >> 4;
    if (r5 > 0x3f)
        r5 = 0x3f;
    else if (r5 < -0x40)
        r5 = -0x40;
    if (r4 == 0x103) {
        func_0204f7cc(thiz, 1, r5);
    } else {
        func_0204f7cc(thiz, data_02099fa4, r5);
    }
}

// @symbol func_02048528
void func_02048528(int *thiz, Vector3 *a) {
    int r4 = a->x >> 12;
    unsigned int x;
    int v;
    if (thiz == 0)
        return;
    if (thiz[0] == 0)
        return;
    x = func_02049018(a);
    if (x >= 0x1f4) {
        v = 0x1e;
    } else if (x < 0x14) {
        v = 0x7f;
    } else {
        v = (int)(0x1f4 - x) * 0x61 / 0x1e0 + 0x1e;
    }
    func_0204f924(thiz, v);
    r4 = r4 >> 2;
    if (r4 > 0x3f)
        r4 = 0x3f;
    else if (r4 < -0x40)
        r4 = -0x40;
    func_0204f7cc(thiz, data_02099fa4, r4);
}
}
