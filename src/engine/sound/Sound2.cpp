//cpp
/* arm9/Sound2 -- the positional sound helpers, .text 0x02048528..0x020490b0.
 *
 * Everything here works out how loud a positioned sound is and where it
 * pans, from its distance to the listener. Sound.cpp is the only caller.
 *
 *   func_02049018  a position's distance from the origin (y counted
 *                  twice), through the hardware square-root unit
 *   SetSoundRange  load the near/far audible range for a sound mode
 *   func_02048f34  store the owner, clear the voice slots, select mode 0
 *   func_02048f24..func_02048eb4
 *                  eight tail-call thunks into SetSoundRange, r0 = 1..7, 0
 *   func_02048e94 / e74 / e54
 *                  set the slot-9 voice count back to four
 *   func_02048d80, func_02048c30, func_02048af4, func_02048908,
 *   func_02048528, func_020485f0
 *                  set a voice's volume from its distance (linear between
 *                  the near and far range, with per-sound overrides) and its
 *                  pan from the position's x
 *   func_02048a1c  is the sound inside its audible range?
 *   func_02048a18  returns its first argument unchanged
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
#include "nnsys/snd/player.h"
#include "nitro/cp/sqrt.h"
#include "private/sound_positional.h"

extern "C" {

/* A playing sound is an NNSSndHandle. Its NNSSndSeqPlayer carries the
   (seqNo, seqArcIndex) pair Sound.cpp passes to func_02048a1c as (kind, id),
   and the voice priority (prio). The voice slots are SoundSlots
   (private/sound_positional.h). */

extern u16 data_02099fa4;   /* pan channel */
extern u32 data_02099fa8;   /* near range: full volume inside it */
extern u32 data_02099fac;   /* far range: silent beyond it */
extern int data_02099fb0;   /* slot-9 voice count */
extern const u32 data_020821c0[16];
extern void *data_020a4bec; /* the registry owner */
extern SoundSlot data_020a4c18[];
extern SoundSlot data_020a4c08[];
extern SoundSlot data_020a4bf8[];
extern SoundSlot data_020a4bf0;

extern int func_0204fa2c(NNSSndHandle *handle, int v);
extern void func_0204f934(NNSSndHandle *handle);
extern void func_0204f94c(NNSSndHandle *handle);
extern void *func_02050cdc(int kind, int id);

int func_02049018(const Vector3 *v) {
    int tx = v->x >> 12;
    int ty = (v->y >> 12) * 2;
    int tz = v->z >> 12;
    int sum = tx * tx + ty * ty + tz * tz;
    CP_SetSqrt32((u32)sum);
    CP_WaitSqrt();
    return CP_GetSqrtResult32();
}

/* Copies pair mode (0..8) from data_020821c0 into the near/far range. */
void SetSoundRange(int mode) {
    volatile u32 *tbl;
    int j;
    if (mode < 0)
        return;
    if (mode > 8)
        return;
    tbl = (volatile u32 *)data_020821c0;
    j = mode * 2;
    data_02099fa8 = tbl[j];
    j++;
    data_02099fac = tbl[j];
}

void func_02048f34(void *owner) {
    int i;
    data_020a4bec = owner;
    for (i = 0; i < 6; i++)
        func_0204f94c(&data_020a4c18[i].handle);
    for (i = 0; i < 2; i++)
        func_0204f94c(&data_020a4c08[i].handle);
    for (i = 0; i < 2; i++)
        func_0204f94c(&data_020a4bf8[i].handle);
    func_0204f94c(&data_020a4bf0.handle);
    SetSoundRange(0);
}

void func_02048f24(void) {
    SetSoundRange(1);
}

void func_02048f14(void) {
    SetSoundRange(2);
}

void func_02048f04(void) {
    SetSoundRange(3);
}

void func_02048ef4(void) {
    SetSoundRange(4);
}
}

namespace Sound {

void Func_02048ee4() {
    SetSoundRange(5);
}

}

extern "C" void func_02048ed4(void) {
    SetSoundRange(6);
}

namespace Sound {

void Func_02048ec4() {
    SetSoundRange(7);
}

void Func_02048eb4() {
    SetSoundRange(0);
}

}

extern "C" {

void func_02048e94(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

void func_02048e74(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

void func_02048e54(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

void func_02048d80(NNSSndHandle *handle, const Vector3 *p) {
    int pan = p->x >> 12;
    int farRange = data_02099fac;
    int nearRange = data_02099fa8;
    int volume;
    int dist;
    if (handle == 0)
        return;
    if (handle->player == 0)
        return;
    dist = func_02049018(p);
    if ((unsigned)dist >= (unsigned)farRange)
        return;
    if ((unsigned)nearRange > (unsigned)dist)
        volume = 0x7f;
    else
        volume = (farRange - dist) * 0x7f / (farRange - nearRange);
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan / 2;
    if (pan > 0x3f)
        pan = 0x3f;
    else if (pan < -0x40)
        pan = -0x40;
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
}

void func_02048c30(NNSSndHandle *handle, const Vector3 *p, int q) {
    int t;
    int a;
    int b = q >> 12;
    unsigned int n, lo, hi;
    if (handle == 0)
        return;
    if (handle->player == 0)
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
            func_0204fa2c(handle, 0);
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
    NNS_SndPlayerSetVolume(handle, t);
    a = a / 2;
    if (a > 0x3f) {
        a = 0x3f;
    } else if (a < -0x40) {
        a = -0x40;
    }
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, a);
}

void func_02048af4(NNSSndHandle *handle, const Vector3 *p, int q) {
    int a = p->x >> 12;
    int b = q >> 12;
    unsigned int n, lo, hi;
    int t;
    if (handle == 0)
        return;
    if (handle->player == 0)
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
        NNS_SndPlayerSetVolume(handle, t);
        NNS_SndPlayerSetTrackPitch(handle, 3, (b - 0x28) * 2);
        a = a / 2;
        if (a > 0x3f) {
            a = 0x3f;
        } else if (a < -0x40) {
            a = -0x40;
        }
        NNS_SndPlayerSetTrackPan(handle, data_02099fa4, a);
        return;
    }
    func_0204fa2c(handle, 0);
}

int func_02048a1c(const Vector3 *pos, int kind, int id) {
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

int func_02048a18(int a, int b) {
    return a;
}

void func_02048908(SoundSlot *slot, const Vector3 *p) {
    int num = data_02099fac;
    int den = data_02099fa8;
    int a = 0x7f;
    int b = 0;
    NNSSndSeqPlayer *s;
    int typ, val;
    int cur, arg;
    int half;

    if (slot == 0)
        return;
    s = slot->handle.player;
    if (s == 0)
        return;

    typ = s->seqNo;
    val = s->seqArcIndex;

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

    cur = slot->dist;
    if ((unsigned)cur < (unsigned)den)
        arg = a;
    else
        arg = b + (a - b) * (num - cur) / (num - den);
    NNS_SndPlayerSetVolume(&slot->handle, arg);

    half = p->x >> 12;
    half = half / 2;
    if (half > 0x3f)
        half = 0x3f;
    else if (half < -0x40)
        half = -0x40;
    NNS_SndPlayerSetTrackPan(&slot->handle, data_02099fa4, half);
}

SoundSlot *func_02048720(const Vector3 *v, int kind, int id) {
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
        if (table[i].handle.player == 0)
            pick = i;
    }

    if (pick < 0) {
        for (i = 0; i < count; i++) {
            p = table[i].handle.player->prio;
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
        func_0204fa2c(&table[pick].handle, 1);
        func_0204f934(&table[pick].handle);
        table[pick].dist = (int)dist;
        return &table[pick];
    }
    return 0;
}

void func_020485f0(NNSSndHandle *handle, const Vector3 *pos) {
    int pan = pos->x >> 12;
    unsigned int dist;
    u16 id;
    int volume;
    if (handle == 0)
        return;
    if (handle->player == 0)
        return;
    dist = func_02049018(pos);
    id = handle->player->seqArcIndex;
    if (id == 0x81) {
        if (dist > 0x4b0)
            volume = 0;
        else if (dist <= 0x64)
            volume = 0x7f;
        else
            volume = (int)(0x4b0 - dist) * 0x7f / 1100;
    } else {
        if (dist >= 0x8bc)
            volume = 0;
        else if (dist < 0x3e8)
            volume = 0x7f;
        else
            volume = (int)(0x8bc - dist) * 0x7f / 1236;
    }
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan >> 4;
    if (pan > 0x3f)
        pan = 0x3f;
    else if (pan < -0x40)
        pan = -0x40;
    if (id == 0x103) {
        NNS_SndPlayerSetTrackPan(handle, 1, pan);
    } else {
        NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
    }
}

void func_02048528(NNSSndHandle *handle, const Vector3 *pos) {
    int pan = pos->x >> 12;
    unsigned int dist;
    int volume;
    if (handle == 0)
        return;
    if (handle->player == 0)
        return;
    dist = func_02049018(pos);
    if (dist >= 0x1f4) {
        volume = 0x1e;
    } else if (dist < 0x14) {
        volume = 0x7f;
    } else {
        volume = (int)(0x1f4 - dist) * 0x61 / 0x1e0 + 0x1e;
    }
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan >> 2;
    if (pan > 0x3f)
        pan = 0x3f;
    else if (pan < -0x40)
        pan = -0x40;
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
}
}
