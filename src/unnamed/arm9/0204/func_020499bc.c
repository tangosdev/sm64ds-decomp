typedef short s16;

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *addr, unsigned int size);

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_020499bc(s16 *a0, s16 *a1, unsigned int a2, int *a3) {
    s16 local[2];
    int half = a2 >> 1;
    int i;

    for (i = 0; i < 2; i++) {
        int idx = i + half - 2;
        int d = a0[idx] - a1[idx];
        if (d < -0x8000) d = -0x8000;
        else if (d >= 0x8000) d = 0x7fff;
        local[i] = d;
    }

    for (i = half - 1; i >= 2; i--) {
        int d = a0[i - 2] - a1[i - 2];
        if (d >= 0) {
            int hi = d + a0[i];
            a0[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            {
                int lo = a1[i] - d;
                a1[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            }
        } else {
            int lo = d + a0[i];
            a0[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            {
                int hi = a1[i] - d;
                a1[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            }
        }
    }

    for (i = 1; i >= 0; i--) {
        int v = a0[i] + *(int *)((char *)(a3 + i) + 0xc);
        if (v < -0x8000) v = -0x8000;
        else if (v >= 0x8000) v = 0x7fff;
        a0[i] = v;
        {
            int w = a1[i] - *(int *)((char *)(a3 + i) + 0xc);
            if (w < -0x8000) w = -0x8000;
            else if (w >= 0x8000) w = 0x7fff;
            a1[i] = w;
        }
    }

    for (i = 0; i < 2; i++) {
        *(int *)((char *)(a3 + i) + 0xc) = local[i];
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj(a0, a2);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(a1, a2);
}
