//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "CaptureEffect.h"

/* Capture effect 2, a feedback cross-echo: each output sample is the input
   plus the delay-line entry from 24 samples earlier, saturated to s16. What
   goes back into the delay line is a quarter of the other channel's mixed
   output (the input with its own echo already added; the main loop takes it
   before saturation, the tail after), so each echo feeds the next, swapping
   sides each time. The 24-entry delay line carries over between buffers.
   Both buffers are then flushed from the data cache for the sound
   hardware. */

void func_020497a0(s16 *p0, s16 *p1, u32 len, CaptureEffect *st) {
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
