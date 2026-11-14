/* storage 0x0238231C runtime 0x037FA1B4 size 0x12C + pool 0x130 wram ARM.
 * Deadline-ordered timer queue insert: bump the stamp past now via the
 * 64-bit scale helper, store it, then splice the node into the list and
 * rearm. Key shapes: stamp carried as one ull from param-formation (prologue
 * MOV R8/R7/R6, no ORRs); ull-mediated compares/subtracts/adds (fused
 * CMP/CMPEQ, SUBS/SBC, ADDS/ADC); split `w += 1` increment (steers the
 * w-home coloring to ROM's); `dd - zero` 64-bit sign test (second
 * SUBS/SBCS); for-loop walk (B-guard + bottom test); i==0 arm in source
 * order (a1[6]=0, v12, link, store, test) with pool-addr return on the
 * linked path. Three calls (one stays asm, abs row in syms.d). */
extern unsigned long long fn_02381EB0(void);
extern unsigned long long fn_0238D38C(unsigned, unsigned, unsigned, unsigned);
extern int fn_023824BC(int);
extern int dword_3806FE4[];
int *fn_0238231C(int *a1, long long st) {
    if (*(unsigned long long *)((int *)a1 + 7) != 0) {
        unsigned long long v6 = fn_02381EB0();
        st = *(unsigned long long *)((int *)a1 + 9);
        if (st < v6) {
            unsigned long long vv = *(unsigned long long *)((int *)a1 + 7);
            unsigned long long d = v6 - st;
            unsigned long long w = fn_0238D38C((unsigned)d, (unsigned)(d >> 32), (unsigned)vv, (unsigned)(vv >> 32));
            w += 1;
            st += vv * w;
        }
    }
    a1[3] = (unsigned)st;
    a1[4] = (unsigned)(st >> 32);
    {
        int *i;
        int zero = 0;
        for (i = (int *)dword_3806FE4[0]; i != 0; i = (int *)((int *)i)[6]) {
            unsigned long long m = *(unsigned long long *)((int *)i + 3);
            long long dd = (long long)(st - m);
            if (dd - zero < 0) {
                a1[5] = ((int *)i)[5];
                ((int *)i)[5] = (int)a1;
                a1[6] = (int)i;
                {
                    int *r = (int *)a1[5];
                    if (r != 0) {
                        r[6] = (int)a1;
                        return r;
                    } else {
                        dword_3806FE4[0] = (int)a1;
                        return (int *)fn_023824BC((int)a1);
                    }
                }
            }
        }
        {
            int v12;
            a1[6] = 0;
            v12 = dword_3806FE4[1];
            dword_3806FE4[1] = (int)a1;
            a1[5] = v12;
            if (v12 != 0) {
                *(int *)(v12 + 24) = (int)a1;
                return (int *)dword_3806FE4;
            } else {
                dword_3806FE4[1] = (int)a1;
                dword_3806FE4[0] = (int)a1;
                return (int *)fn_023824BC((int)a1);
            }
        }
    }
}
