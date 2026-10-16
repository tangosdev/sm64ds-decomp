/* storage 0x02397ED4 runtime 0x027C9E4C linked 0x2FC main ARM.
 * Eight 0x69 self-test loops over two halfword index tables sharing one
 * fail label; a nonzero fail count sets bit 3 at base+0x302. Returns
 * nothing: R0 on exit is whatever the last loop left there. */

extern int fn_02395BE0(int a1, int a2); /* =0x027C7B58 */
extern int fn_02395C74(int a1); /* =0x027C7BEC */
extern unsigned short dat_27D6734[]; /* =0x027D6734 */
extern unsigned short dat_27D6768[]; /* =0x027D6768 */

void fn_02397ED4(void) {
    unsigned int v0;
    unsigned short *v1;

    v0 = 0;
    if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x5B0) == 5)
        v1 = dat_27D6734;
    else
        v1 = dat_27D6768;
    {
        unsigned int i;
        int vv;
        int minus1;
        int result;
        i = 0;
        vv = 255;
        minus1 = -1;
        while (i < 0x69) {
            result = fn_02395BE0(i, vv);
            if (result == minus1) {
                v0 = 1;
                goto fail;
            }
            i++;
        }
    }
    {
        unsigned int idx;
        unsigned int outer;
        int t;
        outer = 0;
        idx = outer;
        while (outer < 0x69) {
            if (outer == v1[idx])
                idx++;
            else {
                t = fn_02395C74(outer);
                t = (unsigned short)t;
                if (t != 255) {
                    if (v0++ > 0x20)
                        goto fail;
                }
            }
            outer++;
        }
    }
    {
        unsigned int j;
        int zero;
        j = 0;
        zero = j;
        while (j < 0x69) {
            fn_02395BE0(j, zero);
            j++;
        }
    }
    {
        unsigned int outer;
        unsigned int idx;
        int t;
        outer = 0;
        idx = outer;
        while (outer < 0x69) {
            if (outer == v1[idx])
                idx++;
            else {
                t = fn_02395C74(outer);
                t = (unsigned short)t;
                if (t != 0) {
                    if (v0++ > 0x20)
                        goto fail;
                }
            }
            outer++;
        }
    }
    {
        unsigned short v10;
        unsigned int k;
        v10 = 0x55;
        k = 0;
        while (k < 0x69) {
            fn_02395BE0(k, v10);
            k++;
            v10 = ~v10;
        }
    }
    {
        unsigned int outer;
        unsigned short v12;
        unsigned int idx;
        int t;
        v12 = 0x55;
        outer = 0;
        idx = outer;
        while (outer < 0x69) {
            if (outer == v1[idx])
                idx++;
            else {
                t = fn_02395C74(outer);
                t = (unsigned short)t;
                if (t != v12) {
                    if (v0++ > 0x20)
                        goto fail;
                }
            }
            outer++;
            t = ~v12;
            t &= 0xFF;
            v12 = t;
        }
    }
    {
        unsigned int m;
        unsigned short v15;
        int t;
        v15 = 0xFF;
        m = 0;
        while (m < 0x69) {
            fn_02395BE0(m, v15);
            m++;
            t = v15 - 1;
            v15 = t;
        }
    }
    {
        unsigned int outer;
        unsigned short v17;
        unsigned int idx;
        int t;
        v17 = 0xFF;
        outer = 0;
        idx = outer;
        while (outer < 0x69) {
            if (outer == v1[idx])
                idx++;
            else {
                t = fn_02395C74(outer);
                t = (unsigned short)t;
                if (t != v17) {
                    if (v0++ > 0x20)
                        goto fail;
                }
            }
            outer++;
            t = v17 - 1;
            v17 = t;
        }
    }
    {
        int zero;
        unsigned int n;
        unsigned short v22;
        unsigned int outer;
        unsigned int idx;
        int one;
        int t;
        outer = 0;
        idx = outer;
        one = 1;
        zero = outer;
        while (outer < 0x69) {
            if (outer == v1[idx])
                idx++;
            else {
                v22 = one;
                n = zero;
                while (n < 9) {
                    fn_02395BE0(outer, v22);
                    t = fn_02395C74(outer);
                    t = (unsigned short)t;
                    if (t != v22) {
                        if (v0++ > 0x20)
                            goto fail;
                    }
                    t = 2 * v22;
                    t &= 0xFF;
                    v22 = t;
                    n++;
                }
            }
            outer++;
        }
    }
fail:
    if (v0 != 0)
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x302) |= 8;
}
