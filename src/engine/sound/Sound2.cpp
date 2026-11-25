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

/* A playing sound is an NNSSndHandle. Its NNSSndSeqPlayer carries the
   (seqNo, seqArcIndex) pair Sound.cpp passes to func_02048a1c as (kind, id),
   and the voice priority (prio). The voice slots are SoundSlots
   (private/sound_positional.h). */

extern u16 data_02099fa4;
/* pan channel */
extern u32 data_02099fa8;
/* near range: full volume inside it */
extern u32 data_02099fac;
/* far range: silent beyond it */
extern int data_02099fb0;
/* Near/far range pairs for the sound modes SetSoundRange copies. */
extern const u32 data_020821c0[16] = {
    90, 550, 200, 1500, 200, 750, 150, 400, 40, 400, 90, 1000, 50, 400, 200, 1500,
};
extern void *data_020a4bec;
/* the registry owner */
extern SoundSlot data_020a4c18[];
extern SoundSlot data_020a4c08[];
extern SoundSlot data_020a4bf8[];
extern SoundSlot data_020a4bf0;

extern "C" void NNS_SndPlayerStopSeq(NNSSndHandle *handle, int v);
extern "C" void NNS_SndHandleReleaseSeq(NNSSndHandle *handle);
extern "C" void NNS_SndHandleInit(NNSSndHandle *handle);
extern "C" void *NNS_SndArcGetSeqArcParam(int kind, int id);

extern "C" int func_02049018(const Vector3 *v) {
    int tx = v->x >> 12;
    int ty = (v->y >> 12) * 2;
    int tz = v->z >> 12;
    int sum = tx * tx + ty * ty + tz * tz;
    CP_SetSqrt32(sum);
    CP_WaitSqrt();
    return CP_GetSqrtResult32();
}

/* Copies pair mode (0..8) from data_020821c0 into the near/far range. */
void SetSoundRange(int mode) {
    const u32 *tbl;
    int j;
    if (mode < 0)
        return;
    if (mode > 8)
        return;
    tbl = data_020821c0;
    j = mode * 2;
    data_02099fa8 = tbl[j];
    j++;
    data_02099fac = tbl[j];
}

extern "C" void func_02048f34(void *owner) {
    int i;
    data_020a4bec = owner;
    for (i = 0; i < 6; i++)
        NNS_SndHandleInit(&data_020a4c18[i].handle);
    for (i = 0; i < 2; i++)
        NNS_SndHandleInit(&data_020a4c08[i].handle);
    for (i = 0; i < 2; i++)
        NNS_SndHandleInit(&data_020a4bf8[i].handle);
    NNS_SndHandleInit(&data_020a4bf0.handle);
    SetSoundRange(0);
}

extern "C" void func_02048f24(void) {
    SetSoundRange(1);
}

extern "C" void func_02048f14(void) {
    SetSoundRange(2);
}

extern "C" void func_02048f04(void) {
    SetSoundRange(3);
}

extern "C" void func_02048ef4(void) {
    SetSoundRange(4);
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

extern "C" void func_02048e94(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

extern "C" void func_02048e74(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

extern "C" void func_02048e54(void) {
    data_02099fb0 = 4;
    Sound::Player::SetPlayableSeqCount(9, 4);
}

extern "C" void func_02048d80(NNSSndHandle *handle, const Vector3 *p) {
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
        volume = 127;
    else
        volume = (farRange - dist) * 127 / (farRange - nearRange);
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan / 2;
    if (pan > 63)
        pan = 63;
    else if (pan < -64)
        pan = -64;
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
}

extern "C" void func_02048c30(NNSSndHandle *handle, const Vector3 *p, int q) {
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
                t = 127;
            } else {
                t = (int)((hi - n) * 127) / (int)(hi - lo);
            }
        } else {
            NNS_SndPlayerStopSeq(handle, 0);
        }
    } else {
        t = 127;
        a = 64;
    }
    if (b < 0) {
        t = 24;
    } else if (b <= 40) {
        t = t * (b * 103 / 40 + 24) / 127;
    }
    NNS_SndPlayerSetVolume(handle, t);
    a = a / 2;
    if (a > 63) {
        a = 63;
    } else if (a < -64) {
        a = -64;
    }
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, a);
}

extern "C" void func_02048af4(NNSSndHandle *handle, const Vector3 *p, int q) {
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
            t = 127;
        } else {
            t = (int)((hi - n) * 127) / (int)(hi - lo);
        }
        if (b < 0) {
            t = 63;
        } else if (b <= 40) {
            t = t * (b * 64 / 40 + 63) / 127;
        }
        NNS_SndPlayerSetVolume(handle, t);
        NNS_SndPlayerSetTrackPitch(handle, 3, (b - 40) * 2);
        a = a / 2;
        if (a > 63) {
            a = 63;
        } else if (a < -64) {
            a = -64;
        }
        NNS_SndPlayerSetTrackPan(handle, data_02099fa4, a);
        return;
    }
    NNS_SndPlayerStopSeq(handle, 0);
}

extern "C" int func_02048a1c(const Vector3 *pos, int kind, int id) {
    u32 lim = data_02099fac;
    u32 dist;
    if (kind == 3) {
        if (id >= 0x100 && id <= 0x103) {
            lim = 2236;
        } else if (id == 0x18d) {
            lim = 500;
        } else if (id == 0x81) {
            lim = 1200;
        } else {
            if (id >= 0x99 && id <= 0x9a) {
                lim = 900;
            }
        }
    } else if (kind == 1) {
        if (id >= 0xea) {
            if (id <= 0x2b) {
                lim = 1100;
            }
        }
    }
    dist = func_02049018(pos);
    if (lim < dist) {
        return 0;
    }

    if (data_020a4bec == 0) {
        return 1;
    }
    NNS_SndArcGetSeqArcParam(kind, id);
    return 1;
}

extern "C" int func_02048a18(int a, int b) {
    return a;
}

extern "C" void func_02048908(SoundSlot *slot, const Vector3 *p) {
    int num = data_02099fac;
    int den = data_02099fa8;
    int a = 127;
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
            num = 900;
            den = 200;
            b = 40;
        } else if (val == 0x28) {
            b = 120;
            a = 127;
        }
    } else if (typ == 1) {
        if (val >= 0x2a && val <= 0x2b) {
            num = 1100;
            den = 200;
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
    if (half > 63)
        half = 63;
    else if (half < -64)
        half = -64;
    NNS_SndPlayerSetTrackPan(&slot->handle, data_02099fa4, half);
}

extern "C" SoundSlot *func_02048720(const Vector3 *v, int kind, int id) {
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

    elem = NNS_SndArcGetSeqArcParam(kind, id);
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

    dist = func_02049018(v);

    if (id >= 0x4b && id <= 0x4c) {
        lim = 900;
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
                best_dist = table[i].dist;
            } else if (best_pri == p) {
                if (best_dist < (u32)table[i].dist) {
                    best_dist = table[i].dist;
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
        NNS_SndPlayerStopSeq(&table[pick].handle, 1);
        NNS_SndHandleReleaseSeq(&table[pick].handle);
        table[pick].dist = dist;
        return &table[pick];
    }
    return 0;
}

extern "C" void func_020485f0(NNSSndHandle *handle, const Vector3 *pos) {
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
        if (dist > 1200)
            volume = 0;
        else if (dist <= 100)
            volume = 127;
        else
            volume = (int)(1200 - dist) * 127 / 1100;
    } else {
        if (dist >= 2236)
            volume = 0;
        else if (dist < 1000)
            volume = 127;
        else
            volume = (int)(2236 - dist) * 127 / 1236;
    }
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan >> 4;
    if (pan > 63)
        pan = 63;
    else if (pan < -64)
        pan = -64;
    if (id == 0x103) {
        NNS_SndPlayerSetTrackPan(handle, 1, pan);
    } else {
        NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
    }
}

extern "C" void func_02048528(NNSSndHandle *handle, const Vector3 *pos) {
    int pan = pos->x >> 12;
    unsigned int dist;
    int volume;
    if (handle == 0)
        return;
    if (handle->player == 0)
        return;
    dist = func_02049018(pos);
    if (dist >= 500) {
        volume = 30;
    } else if (dist < 20) {
        volume = 127;
    } else {
        volume = (int)(500 - dist) * 97 / 480 + 30;
    }
    NNS_SndPlayerSetVolume(handle, volume);
    pan = pan >> 2;
    if (pan > 63)
        pan = 63;
    else if (pan < -64)
        pan = -64;
    NNS_SndPlayerSetTrackPan(handle, data_02099fa4, pan);
}
