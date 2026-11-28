//cpp
/* Music track-volume ramps and the sound-capture effect helpers. mwccarm
 * emits the bodies in reverse, so the capture startup function comes first. */
#include "types.h"
#include "nnsys/snd/player.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/mi/memory.h"
#include "nitro/snd/interface.h"
#include "CaptureEffect.h"

extern "C" int NNS_SndCaptureStartEffect(void *buffer, unsigned int size, int format, int rate,
                                         int interval, void *callback, void *arg);

u16 data_020a4c48[2];
int data_020a4c4c[2];
int data_020a4c54[2];
int data_020a4c5c[2];
CaptureEffect gCaptureEffect;

extern "C" void func_02049cd8(void *buffer, int size, int mode) {
    volatile unsigned short fill = 0;
    MI_CpuFill16(&gCaptureEffect.work, fill, sizeof(gCaptureEffect.work));
    gCaptureEffect.kind = mode;
    if (mode == 1)
        SNDi_SetSurroundDecay(0x3000);
    NNS_SndCaptureStartEffect(buffer, size, 0, 32000, 2, (void *)func_02049ba8, &gCaptureEffect);
}

void func_02049c48(int kind) {
    int current = gCaptureEffect.kind;
    volatile unsigned short fill;
    OSIntrMode savedIRQ;
    if (kind == current)
        return;
    if (current == 1) {
        SNDi_SetSurroundDecay(0);
    }
    savedIRQ = OS_DisableInterrupts();
    fill = 0;
    MI_CpuFill16(&gCaptureEffect.work, fill, sizeof(gCaptureEffect.work));
    gCaptureEffect.kind = kind;
    OS_RestoreInterrupts(savedIRQ);
    if (kind == 1) {
        SNDi_SetSurroundDecay(0x3000);
    }
}

extern "C" void func_02049ba8(void *left, void *right, unsigned int len, int format,
                              CaptureEffect *ctx) {
    if (ctx->fn)
        ctx->fn(left, right, len, format, ctx->arg);
    switch (ctx->kind) {
    case 0:
        return;
    case 1:
        func_020499bc((s16 *)left, (s16 *)right, len, ctx);
        return;
    case 2:
        func_020497a0((s16 *)left, (s16 *)right, len, ctx);
        return;
    }
}

extern "C" void func_020499bc(s16 *left, s16 *right, unsigned int len, CaptureEffect *ctx) {
    s16 carry[2];
    int n = len >> 1;
    int i;

    for (i = 0; i < 2; i++) {
        int idx = i + n - 2;
        int d = left[idx] - right[idx];
        if (d < -0x8000)
            d = -0x8000;
        else if (d >= 0x8000)
            d = 0x7fff;
        carry[i] = d;
    }

    for (i = n - 1; i >= 2; i--) {
        int d = left[i - 2] - right[i - 2];
        if (d >= 0) {
            int hi = d + left[i];
            left[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            {
                int lo = right[i] - d;
                right[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            }
        } else {
            int lo = d + left[i];
            left[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            {
                int hi = right[i] - d;
                right[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            }
        }
    }

    for (i = 1; i >= 0; i--) {
        int v = left[i] + ctx->work.history[i];
        if (v < -0x8000)
            v = -0x8000;
        else if (v >= 0x8000)
            v = 0x7fff;
        left[i] = v;
        {
            int w = right[i] - ctx->work.history[i];
            if (w < -0x8000)
                w = -0x8000;
            else if (w >= 0x8000)
                w = 0x7fff;
            right[i] = w;
        }
    }

    for (i = 0; i < 2; i++) {
        ctx->work.history[i] = carry[i];
    }

    DC_FlushRange(left, len);
    DC_FlushRange(right, len);
}

extern "C" void func_020497a0(s16 *p0, s16 *p1, u32 len, CaptureEffect *st) {
    int n = len >> 1;
    int j;
    int pos = 0;
    int rem;
    int shift;
    int i;

    while (n > pos + 24) {
        for (j = 0; j < 24; j++) {
            int idx = j + pos;
            int v0 = p0[idx] + st->work.echo[j].l;
            int v1 = p1[idx] + st->work.echo[j].r;
            int s0 = v0;
            int s1;
            if (v0 < -0x8000) {
                s0 = -0x8000;
            } else if (v0 >= 0x8000) {
                s0 = 0x7fff;
            }
            p0[idx] = s0;
            s1 = v1;
            if (v1 < -0x8000) {
                s1 = -0x8000;
            } else if (v1 >= 0x8000) {
                s1 = 0x7fff;
            }
            p1[idx] = s1;
            st->work.echo[j].l = (v1 + 1) >> 2;
            st->work.echo[j].r = (v0 + 1) >> 2;
        }
        pos += 24;
    }

    rem = n - pos;
    for (n = 0; n < rem; n++) {
        int idx = n + pos;
        int v0 = p0[idx] + st->work.echo[n].l;
        int v1 = p1[idx] + st->work.echo[n].r;
        if (v0 < -0x8000) {
            v0 = -0x8000;
        } else if (v0 >= 0x8000) {
            v0 = 0x7fff;
        }
        p0[idx] = v0;
        if (v1 < -0x8000) {
            v1 = -0x8000;
        } else if (v1 >= 0x8000) {
            v1 = 0x7fff;
        }
        p1[idx] = v1;
    }

    shift = 24 - rem;
    for (i = 0; i < shift; i++) {
        st->work.echo[i].l = st->work.echo[i + rem].l;
        st->work.echo[i].r = st->work.echo[i + rem].r;
    }

    for (i = 0; i < rem; i++) {
        int idx = i + pos;
        int dst = i + 24 - rem;
        st->work.echo[dst].l = (p1[idx] + 1) >> 2;
        st->work.echo[dst].r = (p0[idx] + 1) >> 2;
    }

    DC_FlushRange(p0, len);
    DC_FlushRange(p1, len);
}

extern "C" void func_02049764(void) {
    data_020a4c5c[0] = 0x7f000;
    data_020a4c54[0] = 0x7f000;
    data_020a4c4c[0] = 0;
    data_020a4c5c[1] = 0x7f000;
    data_020a4c54[1] = 0x7f000;
    data_020a4c4c[1] = 0;
}

extern const int data_0208220c;
extern const int data_02082208;
extern const int data_02082204;
extern const int data_02082200;
extern const int data_02082210;

extern "C" void func_020494cc(NNSSndHandle *a, int mode) {
    NNSSndSeqPlayer *p;
    int id;

    if (a == 0)
        return;
    p = a->player;
    if (p == 0)
        return;

    id = p->seqNo;

    if (mode == 0) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 1) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 2) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082204;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082204;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 3) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082210;
        data_020a4c48[0] = 0x70;
    } else if (mode == 4) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_0208220c;
        data_020a4c48[0] = 0x70;
    } else if (mode == 5) {
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082208;
        data_020a4c48[1] = 0x100;
    } else {
        if (mode != 6)
            return;
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082208;
        data_020a4c48[1] = 0x100;
    }
}

/* The per-track volume deltas, one constant per ramp above. */
const int data_0208220c = 0x1451;
const int data_02082208 = 0x1451;
const int data_02082204 = 0x0a28;
const int data_02082200 = 0x1451;
const int data_02082210 = 0xcb33;

extern "C" void func_0204921c(NNSSndHandle *a, int mode) {
    int i;
    int id;
    NNSSndSeqPlayer *p;

    if (a == 0)
        return;
    p = a->player;
    if (p == 0)
        return;

    id = p->seqNo;

    if (mode == 0) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 1) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 2) {
        if (id != 0x3b)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 3;
        data_020a4c5c[1] = 0x7f000;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 3) {
        if (id != 0x3d)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 0x70;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0;
    } else if (mode == 4) {
        if (id != 0x3d)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 0x70;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0;
    } else if (mode == 5) {
        if (id != 0x3e)
            return;
        data_020a4c5c[0] = 0;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 8;
        data_020a4c5c[1] = 0x7f000;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x100;
    } else {
        if (mode != 6)
            return;
        if (id != 0x3e)
            return;
        data_020a4c5c[0] = 0x7f000;
        data_020a4c4c[0] = 0;
        data_020a4c48[0] = 8;
        data_020a4c5c[1] = 0;
        data_020a4c4c[1] = 0;
        data_020a4c48[1] = 0x100;
    }

    for (i = 0; i < 2; i++) {
        if (data_020a4c5c[i] < 0)
            data_020a4c5c[i] = 0;
        else if (data_020a4c5c[i] > 0x7f000)
            data_020a4c5c[i] = 0x7f000;
        NNS_SndPlayerSetTrackVolume(a, data_020a4c48[i], data_020a4c5c[i] >> 12);
        if (data_020a4c5c[i] == 0)
            NNS_SndPlayerSetTrackMute(a, data_020a4c48[i], 1);
    }
}

void func_020490b0(NNSSndHandle *c) {
    s32 t[4];
    s32 i;
    s32 a;
    s32 e;
    s32 w;
    s32 flag;

    if (c == 0) {
        return;
    }
    if (c->player == 0) {
        return;
    }

    t[0] = 0;
    t[1] = 0x7f000;
    t[2] = 0;
    t[3] = 0;

    for (i = 0; i < 2; i++) {
        flag = 0;
        a = data_020a4c4c[i];
        if (a > 0) {
            e = data_020a4c5c[i];
            if (e == 0) {
                flag = 1;
            }
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] >= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = t[0];
            }
        } else if (a < 0) {
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] <= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = 0;
            }
        } else {
            continue;
        }

        w = data_020a4c5c[i];
        if (w > 0x7f000) {
            data_020a4c5c[i] = t[1];
        } else if (w < 0) {
            data_020a4c5c[i] = t[2];
            NNS_SndPlayerSetTrackMute(c, data_020a4c48[i], 1);
        }
        if (flag != 0) {
            NNS_SndPlayerSetTrackMute(c, data_020a4c48[i], t[3]);
        }
        NNS_SndPlayerSetTrackVolume(c, data_020a4c48[i], data_020a4c5c[i] >> 12);
    }
}
