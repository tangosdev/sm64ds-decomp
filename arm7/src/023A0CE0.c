// NONMATCHING: best C 134/158 words match the ROM.
// @ 0x023A0CE0 size 0x26C (runtime 0x027D2C58, main). Residue class: register allocation / frame shape.
// Closest C attempt from AH54 (tried, closed); not enrolled, so the build keeps the original bytes for this range.
extern void fn_02382C98(void *a1, void *a2, int a3) /* =0x037FAB30 */;
extern int fn_0238D594(int a1, int a2) /* =0x0380542C */;
extern int fn_02394D84(void *a1, void *a2, unsigned short *node) /* =0x027C6CFC */;
extern void fn_0239477C(int a1, int a2) /* =0x027C66F4 */;
extern int fn_02394DF8(void *list, unsigned short *node) /* =0x027C6D70 */;

#pragma opt_propagation off
#pragma opt_common_subs off
void fn_023A0CE0(unsigned short *a1, unsigned short *a2, int a3) {
    int off;
    unsigned int i;
    char *base;
    unsigned short *v12;
    unsigned int sum;
    int pa2;
    int off2;
    int pa3;
    unsigned int w9;
    int diff;
    int d2;
    base = *(char **)0x0380FFF4 + 0x4A0;
    a1[4] -= 24;
    for (i = 0; i < 3; i++) {
        off = i * 0x18;
        unsigned short *lslot = (unsigned short *)(base + off);
        if (*(unsigned short *)(base + off) != 0
            && a2[2] == lslot[4]
            && a2[1] == lslot[3]
            && a2[0] == lslot[2]
            && a2[5] == lslot[7]
            && a2[4] == lslot[6]
            && a2[3] == lslot[5]) {
            if ((((unsigned int)a2[6] << 16) >> 20) == (((unsigned int)lslot[8] << 16) >> 20)) {
                diff = (int)lslot[8] - (int)a2[6];
                if ((diff & 0x80000000) != 0)
                    return;
                pa2 = diff * lslot[1];
                pa3 = (int)a1[4] - pa2;
                if (pa3 == 0)
                    return;
                if ((pa3 & 0x80000000) == 0) {
                    d2 = diff;
                    break;
                }
                return;
            }
        }
    }
    if (i != 3) {
        char *slot16;
        unsigned short cur;
        int r;
        char *slot;
        unsigned int sh;
        unsigned int nib;
        unsigned int bic;
        off2 = i * 0x18;
        slot = base + off2;
        v12 = (unsigned short *)(*(int *)(slot + 20) + 16);
        w9 = v12[8];
        sum = w9 + pa3;
        if (sum > 0x5E4) {
            fn_02394DF8((*(char **)0x0380FFF4 + 0x14C), (v12 - 8));
            *(unsigned short *)(base + off2) = 0;
        } else {
            fn_02382C98((char *)&a1[18] + pa2, (void *)((char *)v12 + 44 + w9), pa3 + 1);
            v12[8] = (unsigned short)sum;
            r = fn_0238D594(*a1 & 0xF0, 16);
            slot = base + off2;
            slot16 = slot + 16;
            cur = *(unsigned short *)slot16;
            bic = cur & ~0xF;
            sh = (unsigned int)cur << 28;
            *(unsigned short *)slot16 = (unsigned short)(bic | ((unsigned short)(r - d2 + (sh >> 28)) & 0xF));
            *(int *)((char *)*(char **)0x0380FFF4 + 0x518) += r;
            if ((*a1 & 0x100) == 0) {
                *(unsigned short *)slot = 0;
                v12[4] = (unsigned short)((v12[4] & ~0xF0) + 0x10);
                v12[8] += 24;
                nib = v12[4] & 0xF;
                if (nib == 0)
                    goto do17;
                if (nib != 8)
                    goto doC;
                fn_02394D84((*(char **)0x0380FFF4 + 0x14C), (*(char **)0x0380FFF4 + 0x188), (v12 - 8));
                fn_0239477C(2, 6);
                goto done;
            do17:
                fn_02394D84((*(char **)0x0380FFF4 + 0x14C), (*(char **)0x0380FFF4 + 0x1A0), (v12 - 8));
                fn_0239477C(1, 7);
                goto done;
            doC:
                fn_02394DF8((*(char **)0x0380FFF4 + 0x14C), (v12 - 8));
            done:;
            }
        }
    }
}
