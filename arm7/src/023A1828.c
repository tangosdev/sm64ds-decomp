/* storage 0x023A1828 runtime 0x027D37A0 main ARM, matched 2004/b56 -O4,s.
 * 802.11 IE (TLV) parser: while (v3 > 0) walks tag,len pairs via
 * fn_02395334, jump-table switch on tags 0-5 plus case 0xDD (vendor IE gated
 * len >= 8 with Nintendo OUI 00 09 BF 00). void (T *a1).
 * Key shape (if-conversion wall, 2-word gap): cases 0/1/3 spell the fC RMW
 * as a1->fC (like siblings 4/5/DD) with NO named &a1->fC variable. A
 * `v4 = &a1->fC` local lets the if-converter re-emit the trailing DD tail
 * through R4 (5 predicated insns, 0x378); without it, loop-invariant
 * motion still homes a1+12 in R4 for the hot cases while the 6-insn DD
 * tail keeps its `bne` + fresh `add r1,sb,#0xc` (0x380 exact). C only. */
extern unsigned int fn_02395334(unsigned char *p); /* =0x027C72AC */
extern void fn_02396440(unsigned char *a1, unsigned short *a2); /* =0x027C83B8 */
extern unsigned int fn_023964FC(unsigned int a1, unsigned char *a2); /* =0x027C8474 */
extern int dword_380FFF4[]; /* =0x0380FFF4, read as base pointer per use */

typedef struct {
    unsigned char *cur;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fA;
    unsigned short fC;
    unsigned short fE;
    unsigned short f10;
    unsigned short f12;
    unsigned short f14;
    unsigned short f16;
    unsigned char *f18;
    unsigned char *f1C;
    unsigned char *f20;
    unsigned char *f24;
} T;

void fn_023A1828(T *a1) {
    unsigned char *v2 = *(unsigned char **)a1;
    int v3;
    unsigned char *v7;
    unsigned int len;
    unsigned int tag;
    char *q;

    q = *(char **)dword_380FFF4 + 0x300;
    a1->f12 = *(unsigned short *)(q + 0x76);
    if ((a1->fC & 0x800) != 0)
        a1->fA |= 1;
    v3 = a1->f8;
    while (v3 > 0) {
        tag = fn_02395334(v2);
        v7 = v2 + 1;
        v2 += 2;
        len = fn_02395334(v7);
        switch (tag) {
        case 0:
            if (len <= 0x20) {
                a1->fC |= 1;
                a1->f18 = v2 - 2;
                if (len == 0 && (a1->fC & 0x800)) {
                    a1->fA |= 1;
                } else {
                    a1->fA &= ~1;
                    if (fn_023964FC((unsigned short)len, v2) != 0)
                        a1->fA |= 1;
                }
            } else {
                a1->fC &= ~1;
                a1->f18 = 0;
            }
            break;
        case 2:
            break;
        case 3:
            if (len >= 1) {
                a1->fC |= 2;
                a1->f12 = fn_02395334(v2);
                if (a1->f12 == *(unsigned short *)((char *)(*(int **)(*(char **)dword_380FFF4 + 0x3D4)) + 0x4A))
                    a1->fA |= 2;
                else
                    a1->fA &= ~2;
            }
            break;
        case 1:
            if (len >= 1) {
                unsigned int t14;
                unsigned int x;
                unsigned int y;
                a1->fC |= 4;
                fn_02396440(v2 - 2, &a1->f14);
                q = *(char **)dword_380FFF4;
                t14 = a1->f14;
                q += 0x300;
                x = *(unsigned short *)(q + 0x5C);
                y = *(unsigned short *)(q + 0x5E);
                if ((t14 & ~(x | y)) == 0 &&
                    x == (x & (t14 | a1->f16)))
                    a1->fA |= 4;
                else
                    a1->fA &= ~4;
            }
            break;
        case 5:
            if (len >= 3) {
                a1->fC |= 0x100;
                a1->f20 = v2 - 2;
            }
            break;
        case 4:
            if (len >= 6) {
                a1->fC |= 0x200;
                a1->f1C = v2 - 2;
            }
            break;
        case 0xDD:
            if (len < 8)
                break;
            if (fn_02395334(v2) != 0)
                break;
            if (fn_02395334(v2 + 1) != 9)
                break;
            if (fn_02395334(v2 + 2) != 0xBF)
                break;
            if (fn_02395334(v2 + 3) != 0)
                break;
            a1->fC |= 0x400;
            a1->f24 = v2 - 2;
        }
        v2 += len;
        v3 -= len + 2;
    }
    if ((a1->fC & 8) != 0) {
        q = *(char **)dword_380FFF4 + 0x300;
        if (((*(unsigned short *)(q + 0x60) & 1) != 0) ||
            ((a1->f4 & 0x8000) != 0))
            a1->fA |= 8;
    }
    if ((a1->fC & 0x30) != 0) {
        if ((a1->f6 & 3) == (*(unsigned short *)(*(char **)dword_380FFF4 + 0x300 + 0x78) & 3))
            a1->fA |= 0x10;
        if ((a1->f6 & 0x10) == (*(unsigned short *)(*(char **)dword_380FFF4 + 0x300 + 0x78) & 0x10))
            a1->fA |= 0x20;
    }
}
