#include "types.h"
typedef struct EchoChan {
    int l;
    int r;
} EchoChan;

typedef struct EchoWork {
    char pad[0xc];
    EchoChan ch[24];
} EchoWork;

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);

void func_020497a0(s16* p0, s16* p1, u32 len, EchoWork* st)
{
    int n = len >> 1;
    int j;
    int pos = 0;
    int rem;
    int shift;
    int i;

    while (n > pos + 24) {
        for (j = 0; j < 24; j++) {
            int idx = j + pos;
            int v0 = p0[idx] + st->ch[j].l;
            int v1 = p1[idx] + st->ch[j].r;
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
            st->ch[j].l = (v1 + 1) >> 2;
            st->ch[j].r = (v0 + 1) >> 2;
        }
        pos += 24;
    }

    rem = n - pos;
    for (n = 0; n < rem; n++) {
        int idx = n + pos;
        int v0 = p0[idx] + st->ch[n].l;
        int v1 = p1[idx] + st->ch[n].r;
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
        st->ch[i].l = st->ch[i + rem].l;
        st->ch[i].r = st->ch[i + rem].r;
    }

    for (i = 0; i < rem; i++) {
        int idx = i + pos;
        int dst = i + 24 - rem;
        st->ch[dst].l = (p1[idx] + 1) >> 2;
        st->ch[dst].r = (p0[idx] + 1) >> 2;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p0, len);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p1, len);
}
