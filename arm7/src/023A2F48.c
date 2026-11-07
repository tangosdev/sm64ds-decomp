// NONMATCHING: closest C attempt, not a byte match (storage 0x023A2F48 runtime 0x027D4EC0 code 0x78C + pool 0x3C main ARM.).
// @ 0x023A2F48 size 0x78C (runtime 0x027D4EC0, main). Residue class: see the notes below.
// Tried by AH61; not enrolled, so the build keeps the original bytes for this range.
/* storage 0x023A2F48 runtime 0x027D4EC0 code 0x78C + pool 0x3C main ARM.
 * NOT matched (best C: frame 6 callee vs ROM 5, ~999 raw diffs incl. BL
 * slots). WM parent request (a1): w+0x528 counter++, aid fetch via
 * fn_023A4D04(a1+15), 40-byte buf via fn_023A17C8, MAC reg programming
 * (0x048080F0..0x0480810C), 64-bit tick math via fn_0238D38C, child-record
 * updates, v51/v52 blocks. No BL reloc issues (all externs banked).
 * Load-bearing fixes in this attempt: v7 is a POINTER (work+776), not a
 * halfword load; fetch temp v6 is unsigned int (no lsl/lsr extend, matches
 * ROM add+strh); no tptr variable (buf[0]=a1+28, t-load=a1+22 recomputed
 * like ROM). WALL (frame fixed-point, ~30 probes): candidate pushes
 * {r4-r8,sb,lr}+0x44, ROM {r4-r8,lr}+0x48 (same 96-byte total: one value
 * in reg vs on stack). P1 diagnostic: spilling v7 (fresh reloads) gives
 * EXACT ROM frame, so v7 is the contested reg; but ROM keeps v7 in r7
 * (add r7,#0x308) with only 5 callee, i.e. ROM spills something this
 * source keeps in-reg (aid-reload, v5-reload, cnt-reuse, decl-order,
 * 8 opt pragmas all inert). C only. */
extern int fn_023A4D04(short *a1); /* =0x027CCB7C */
extern int fn_0239A96C(int a1, short a2); /* =0x027CC8E4 */
extern int fn_02382CB4(int v, int *p, int n); /* =0x037FAB4C */
extern void fn_023A17C8(void *a1); /* =0x027D37A0 */
extern unsigned int fn_02395334(unsigned int addr); /* =0x027C72AC */
extern void fn_023AC444(int a1, void *a2); /* =0x027D42BC */
extern void fn_023954C4(void); /* =0x027C743C */
extern int fn_023970C8(unsigned int a1, unsigned int a2); /* =0x027C9040 */
extern int fn_023979B0(unsigned short a1, int a2); /* =0x027C9928 */
extern int fn_02397468(unsigned short a1); /* =0x027C93E0 */
extern int fn_023974A8(unsigned short a1); /* =0x027C9420 */
extern unsigned short *fn_023969D8(unsigned short *result, unsigned short *a2); /* =0x027C8950 */
extern void fn_0239477C(int a1, int a2); /* =0x027C66F4 */
extern int fn_02396EFC(int a1); /* =0x027C8E74 */
extern unsigned long long fn_0238D38C(unsigned long long a1, unsigned long long a2); /* =0x03805224 */
extern int fn_02382AC4(void); /* =0x037FA95C */
extern int fn_02382AD8(int a1); /* =0x037FA970 */
extern void fn_02382C98(void *a1, void *a2, unsigned int a3); /* =0x037FAB30 */
extern void fn_023A97D8(void); /* =0x027D1650 */
extern void fn_023A535C(int a1); /* =0x027CD1D4 */
extern void fn_023AE1DC(int a1, int a2); /* =0x027D6054 */

void fn_023A2F48(unsigned short *a1) {
    unsigned long long t;
    unsigned long long v40;
    unsigned long long v41;
    volatile int zero;
    int buf[10];
    int aid;
    unsigned int cnt;
    int v5;
    unsigned int v6;
    int v7;
    unsigned int v8;
    int v18;
    unsigned short *v52s;
    unsigned int v26;
    unsigned int v32;
    unsigned int v33;
    unsigned int v34;
    unsigned long long v27;
    unsigned long long v36;
    unsigned long long v37;
    int v28;
    unsigned int v29;
    unsigned int v30;

    ++*(unsigned int *)((char *)*(char **)0x0380FFF4 + 0x528);
    aid = fn_023A4D04((short *)(a1 + 15));
    a1[1] = (unsigned short)aid;
    if (aid == 255)
        return;
    fn_0239A96C(aid, a1[9]);
    cnt = a1[3];
    if (cnt > 12) {
        zero = 0;
        fn_02382CB4(zero, buf, 40);
        buf[0] = (int)(a1 + 28);
        ((unsigned short *)buf)[4] = (unsigned short)(cnt - 12);
        ((unsigned short *)buf)[5] = 2;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x1A) == 0)
            ((unsigned short *)buf)[5] |= 1;
        ((unsigned short *)buf)[6] = 56;
        ((unsigned short *)buf)[2] = a1[4];
        ((unsigned short *)buf)[3] = a1[27];
        fn_023A17C8(buf);
        v5 = buf[7];
        if (v5 != 0 && (a1[11] & 0x8000) != 0) {
            v6 = fn_02395334((unsigned int)(v5 + 6));
            *(unsigned short *)0x480810C = (unsigned short)(v6 + (fn_02395334((unsigned int)(v5 + 7)) << 8));
        }
        v7 = (int)((char *)*(char **)0x0380FFF4 + 776);
        v8 = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xBC);
        if (v8 == 19 && *(unsigned short *)(*(unsigned int *)(*(char **)0x0380FFF4 + 980) + 56) == 1) {
            if ((((unsigned short *)buf)[5] & 9) == 9)
                fn_023AC444((int)a1, buf);
            goto tail;
        }
        if ((((unsigned short *)buf)[5] & 8) == 0)
            goto tail;
        if ((((unsigned short *)buf)[6] & 1) != 0) {
            if ((((unsigned short *)buf)[6] & 1) == 0)
                goto tail;
            if ((((unsigned short *)buf)[5] & 1) == 0)
                goto tail;
        }
        if (v8 == 33) {
            fn_023954C4();
            if ((((unsigned short *)buf)[5] & 0x30) == 0x30) {
                if ((((unsigned short *)buf)[5] & 4) != 0) {
                    if (a1[26] <= 0x3E8) {
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 0;
                        if ((((unsigned short *)buf)[6] & 2) != 0 && (((unsigned short *)buf)[5] & 2) == 0)
                            fn_023970C8(((unsigned short *)buf)[9], 0);
                        if (*(unsigned short *)(v7 + 12) == 2) {
                            v52s = (unsigned short *)buf[9];
                            if (v52s != 0) {
                                fn_023979B0((unsigned short)(fn_02395334((unsigned int)(v52s + 3)) + (fn_02395334((unsigned int)((char *)v52s + 7)) << 8)), 1);
                                *(unsigned short *)0x380FFF0 = (unsigned short)(fn_02395334((unsigned int)(v52s + 4)) + (fn_02395334((unsigned int)((char *)v52s + 9)) << 8));
                            } else {
                                fn_023979B0(0xFFFF, 1);
                                *(unsigned short *)0x380FFF0 = 0;
                            }
                        }
                        fn_02397468((unsigned short)fn_02395334((unsigned int)((char *)buf[8] + 3)));
                        *(unsigned short *)(v7 + 106) = (unsigned short)fn_02395334((unsigned int)((char *)buf[8] + 2));
                        fn_023974A8(a1[26]);
                        *(unsigned short *)(v7 + 14) |= 4;
                        *(unsigned short *)(v7 + 14) |= 0x40;
                        if (*(unsigned short *)(v7 + 12) == 2)
                            *(unsigned short *)0x4808048 = 3;
                        *(unsigned short *)0x4808038 |= 1;
                    } else {
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 1;
                    }
                } else {
                    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 18;
                }
            } else {
                *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 10;
            }
            fn_023969D8((unsigned short *)(*(unsigned int *)(*(char **)0x0380FFF4 + 984) + 8), (unsigned short *)(a1 + 15));
            *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xBC) = 37;
            fn_0239477C(2, 1);
        }
        v18 = *(unsigned short *)(v7 + 12);
        if (v18 == 1)
            goto l65;
        else if (v18 == 2) {
            v52s = (unsigned short *)buf[9];
            if (v52s != 0) {
                fn_023979B0((unsigned short)(fn_02395334((unsigned int)(v52s + 3)) + (fn_02395334((unsigned int)((char *)v52s + 7)) << 8)), 0);
                *(unsigned short *)0x380FFF0 = (unsigned short)(fn_02395334((unsigned int)(v52s + 4)) + (fn_02395334((unsigned int)((char *)v52s + 9)) << 8));
                *(unsigned short *)(v7 + 148) = (unsigned short)(fn_02395334((unsigned int)((char *)v52s + 1)) - 8);
                if (*(unsigned short *)(v7 + 148) != 0) {
                    if ((((unsigned int)v52s) & 1) != 0) {
                        fn_02382C98((char *)v52s + 9, *(void **)(v7 + 144), (unsigned int)(*(unsigned short *)(v7 + 148) + 2));
                        *(unsigned short *)(v7 + 150) = 1;
                    } else {
                        fn_02382C98(v52s + 5, *(void **)(v7 + 144), (unsigned int)(*(unsigned short *)(v7 + 148) + 1));
                        *(unsigned short *)(v7 + 150) = 0;
                    }
                }
            }
        } else if (v18 != 3)
            goto l65;
        *(unsigned short *)(v7 + 116) = 0;
        {
            char *v25 = *(char **)((char *)*(char **)0x0380FFF4 + 0x2E0) + 28 * aid;
            *(unsigned short *)(v25 + 24) = *(unsigned short *)(v25 + 26);
        }
        t = *(unsigned long long *)(a1 + 22);
        {
            char *w300 = (char *)*(char **)0x0380FFF4 + 0x300;
            v26 = (unsigned int)(*(unsigned short *)(w300 + 0x6A) << 10);
        }
        v27 = fn_0238D38C(t, v26);
        v36 = (v27 + 1) * v26;
        *(unsigned short *)0x48080F6 = ((unsigned short *)&v36)[3];
        *(unsigned short *)0x48080F4 = ((unsigned short *)&v36)[2];
        *(unsigned short *)0x48080F2 = ((unsigned short *)&v36)[1];
        *(unsigned short *)0x48080F0 = ((unsigned short *)&v36)[0] | 1;
        if ((((unsigned int)*(unsigned short *)(v7 + 14) << 25) >> 31) == 1) {
            unsigned short *p40 = (unsigned short *)&v40;
            unsigned short *p41 = (unsigned short *)&v41;
            v37 = v36 - v26;
            v28 = fn_02382AC4();
            p40[0] = *(volatile unsigned short *)0x48080F8;
            p40[1] = *(volatile unsigned short *)0x48080FA;
            p40[2] = *(volatile unsigned short *)0x48080FC;
            p40[3] = *(volatile unsigned short *)0x48080FE;
            p41[0] = *(volatile unsigned short *)0x48080F8;
            p41[1] = *(volatile unsigned short *)0x48080FA;
            p41[2] = *(volatile unsigned short *)0x48080FC;
            p41[3] = *(volatile unsigned short *)0x48080FE;
            fn_02382AD8(v28);
            if ((unsigned short)v40 >= (unsigned short)v41)
                v29 = (unsigned int)((v41 - v37) >> 10);
            else
                v29 = (unsigned int)((v40 - v37) >> 10);
            v30 = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300);
            if (v29 >= v30)
                *(unsigned short *)0x4808134 = 0;
            else
                *(unsigned short *)0x4808134 = (unsigned short)(v30 - v29);
        }
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x10) == 64 && (char *)buf[8] != 0
            && (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x16) & 1) == 1) {
            unsigned int v31 = fn_02395334((unsigned int)((char *)buf[8] + 2));
            if (*(unsigned short *)(v7 + 106) != v31)
                *(unsigned short *)(v7 + 106) = (unsigned short)v31;
            *(unsigned short *)(v7 + 130) = 0;
            if (v31 == 0 && (fn_02395334((unsigned int)((char *)buf[8] + 4)) & 1) != 0)
                *(unsigned short *)(v7 + 130) |= 1;
            v32 = fn_02395334((unsigned int)((char *)buf[8] + 4)) & 0xFE;
            v33 = 8 * (fn_02395334((unsigned int)((char *)buf[8] + 1)) + v32 - 3);
            v34 = *(unsigned short *)(v7 + 94);
            if (8 * v32 <= v34 && v34 <= v33
                && ((1 << ((v34 - 8 * v32) & 7)) & fn_02395334((unsigned int)((char *)buf[8] + ((v34 - 8 * v32) >> 3) + 5))) != 0) {
                *(unsigned short *)(v7 + 130) |= 2;
                fn_023A97D8();
            }
            if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x100 + 0x60) == 0
                && *(unsigned short *)(*(char **)0x0380FFF4 + 0x100 + 0x6C) == 0
                && *(unsigned short *)(v7 + 130) == 0)
                fn_02396EFC(1);
        }
l65:
        if ((*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xFE) & 0x40) != 0)
            fn_023A535C((int)a1);
tail:
        if (buf[6] != 0)
            fn_023AE1DC(((unsigned short *)buf)[9], (int)a1);
    }
}
