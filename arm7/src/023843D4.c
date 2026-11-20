/* storage 0x023843D4 runtime 0x037FC26C size 0x134 (listed 0x124 + pool) wram ARM.
 * Best-entry selector: mask a1 in place by ~*A4 (and ~*A0 when a3 == 0),
 * walk the 16-slot byte table at 0x3805964, and keep the entry with the
 * smallest byte-34 key (ties broken via fn_02383EBC). Then run the entry
 * callback if present, mark it (byte 4 = 2, clear byte-3 bit 1), and re-init
 * it via fn_02383FCC(v8, a4, a5, a2). Four pools, two calls (one indirect).
 * Modifying a1 in place (no v7 copy) is what forces the early save of R0 and
 * the R0-based pool loads; the shift-base and entry stride are hoisted loop
 * constants (R5/R4); the counted scan is a do-while (fallthrough entry). */

extern int unk_38075A4;
extern int unk_38075A0;
extern unsigned char dt_3805964[];
extern int unk_38075C4[];
extern int fn_02383EBC(int a1, int a2);
extern int fn_02383FCC(unsigned char *a1, int a2, int a3, int a4);

char *fn_023843D4(int a1, int a2, int a3, int a4, int a5) {
    unsigned char *v8;
    int i;
    int v10;
    unsigned char *v11;
    unsigned v12;
    unsigned v13;
    int v15;
    int one;
    int stride;
    a1 &= ~unk_38075A4;
    if (a3 == 0)
        a1 &= ~unk_38075A0;
    v8 = 0;
    i = (int)v8;
    one = 1;
    stride = 0x54;
    do {
        v10 = dt_3805964[i];
        if ((a1 & (one << v10)) != 0) {
            v11 = (unsigned char *)unk_38075C4 + stride * v10;
            if (v8 == 0) {
                v8 = v11;
            } else {
                v12 = v8[34];
                v13 = v11[34];
                if (v13 <= v12) {
                    if (v13 != v12 || fn_02383EBC((int)v8, (int)v11) < 0)
                        v8 = v11;
                }
            }
        }
        ++i;
    } while (i < 16);
    if (v8 == 0)
        return 0;
    if (a2 < (int)v8[34])
        return 0;
    v15 = ((int *)v8)[18];
    if (v15 != 0)
        ((void (*)(unsigned char *, int, int))v15)(v8, 0, ((int *)v8)[19]);
    v8[4] = 2;
    v8[3] &= ~1;
    fn_02383FCC(v8, a4, a5, a2);
    return (char *)v8;
}
