// NONMATCHING: closest C attempt, not a byte match (storage 0x02388798 runtime 0x03800630 linked 0x224 wram.).
// @ 0x02388798 size 0x208 (runtime 0x03800630, wram). Residue class: see the notes below.
// Tried by AH21; not enrolled, so the build keeps the original bytes for this range.
/* storage 0x02388798 runtime 0x03800630 linked 0x224 wram.
 * Case 1 passes fn_02388720 through a function-pointer parameter so the
 * address homes in r8 and the request stays in r7. */
extern unsigned short word_3809424[];
extern int unk_3809450[];
extern int fn_02382AC4(void);
extern int fn_023883E0(int a1);
extern void fn_023883C8(int a1);
extern void fn_02382AD8(int a1);
extern void fn_02388CE8(unsigned short *a1, int a2, int a3);
extern int fn_023883FC(int a1, int a2);
extern int fn_02388384(int a1);
extern long long fn_0238D7A0(int a1, int a2);
extern int fn_023827AC(int *a1, int a2, int a3, int (*a4)(), int a5);
extern int fn_02382568(int a1);
extern int fn_02388720();

void fn_02388798(int *req) {
    int zero;
    unsigned int i;
    int stride;
    int base;
    unsigned int t = (unsigned int)req[1];
    if (t <= 2) {
        if (t == 0)
            goto case0;
        if (t == 1)
            goto case1;
        if (t == 2)
            goto case2;
        return;
    }
    if (t != 16)
        return;
    {
        int st = ((int *)word_3809424)[8];
        if (st != 2)
            return;
    }
case0: {
    int v3 = fn_02382AC4();
    if (fn_023883E0(0) == 0) {
        fn_02382AD8(v3);
        fn_023883FC((unsigned short)req[1], 4);
        return;
    } else {
        unsigned short buf[2];
        fn_023883C8(0);
        fn_02382AD8(v3);
        fn_02388CE8(buf, ((int *)word_3809424)[10], ((int *)word_3809424)[9]);
        *(unsigned short *)0x27FFFAA = buf[0];
        *(unsigned short *)0x27FFFAC = buf[1];
        if (req[1] == 0) {
            fn_023883FC((unsigned short)req[1], 0);
        } else {
            fn_023883FC((unsigned short)req[1], (unsigned short)(req[2] & 0xFF));
        }
        fn_02388384(0);
        return;
    }
}
case1: {
    if (((int *)word_3809424)[8] == 1) {
        base = (int)word_3809424;
        i = 0;
        { int *stridep = (int *)0x107; stride = (int)stridep; }
        zero = (int)i;
        while (i < (unsigned int)req[2]) {
            long long qr1 = fn_0238D7A0((int)(i * stride), req[2]);
            int quot = (int)qr1;
            long long qr2 = fn_0238D7A0(req[3] + quot, stride);
            int rem = (int)(qr2 >> 32);
            short *slotp = (short *)(base + (int)i * 2);
            slotp[86] = (short)rem;
            fn_023827AC(&unk_3809450[8 * (int)i], (int)slotp[86], zero, fn_02388720, (int)i);
            i++;
        }
        fn_023883FC((unsigned short)req[1], 0);
        ((int *)word_3809424)[8] = 2;
        goto exitend;
    } else {
        fn_023883FC((unsigned short)t, 3);
        return;
    }
}
case2: {
    int st;
    st = ((int *)word_3809424)[8];
    if (st == 3) {
        fn_02382568(0x54505641);
        fn_023883FC((unsigned short)req[1], 0);
        ((int *)word_3809424)[8] = 0;
        goto exitend;
    } else {
        fn_023883FC((unsigned short)t, 3);
        return;
    }
}
exitend:;
}
