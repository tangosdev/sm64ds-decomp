extern int *fn_0238E688(void);
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern void fn_0238219C(void *);
extern int fn_02393128(int, unsigned short);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);
extern void fn_0238E57C(void *, void *);
extern int fn_02381600(void *, int *, int);
extern volatile unsigned short half_48080F8;
extern volatile unsigned short half_48080FA;
extern int unk_27D80AC[];

#pragma opt_propagation off

void fn_0238ED04(char *a1) {
    char *sb;
    unsigned char *s;
    int flag;
    int t;
    int r5;
    int av2;
    int ok;
    short *msg;

    sb = a1 + 0x10;
    s = **(unsigned char ***)((char *)fn_0238E688() + 0x1610);
    t = 0;
    if (*(int *)(s + 0x10) == 0)
        return;
    if (*(unsigned short *)(a1 + 0xE) == 0) {
        flag = 1;
        volatile unsigned short *p1 = &half_48080F8;
        unsigned short h1 = *p1;
        unsigned short h3 = half_48080FA;
        unsigned short h2 = *p1;
        unsigned int w;
        int d;
        if (h1 > h2)
            h3 = half_48080FA;
        w = (unsigned short)((h3 << 12) | (h2 >> 4));
        d = (short)(w - *(unsigned short *)(s + 0x56));
        if (d <= 0)
            return;
    } else {
        flag = t;
    }
    av2 = fn_02382AC4();
    if (*(unsigned short *)(s + 0x84) == 0) {
        fn_02382AD8(av2);
        return;
    }
    *(unsigned short *)(s + 0x84) = 0;
    r5 = *(unsigned short *)(s + 0x90);
    fn_0238219C(s + 0x58);
    fn_02382AD8(av2);
    ok = 1;
    if (*(unsigned short *)(s + 0x8C) != 0 &&
        (flag != 0 ||
         !(*(unsigned short *)(sb + 0x2E) & (1 << *(unsigned short *)(s + 0x184)))))
        ok = 0;
    if (r5 != 0 && (flag != 0 || ok != 0))
        *(unsigned short *)(s + 0xBA) = 4;
    if (*(unsigned short *)(s + 0x8C) != 0)
        *(unsigned short *)(s + 0x8C) = 0;
    if (*(unsigned short *)(s + 0x8E) != 0) {
        *(unsigned short *)(s + 0x8E) = 0;
        t = fn_02393128(flag, (unsigned short)(ok != 0));
    }
    a1 = (char *)fn_0238DE4C();
    ((short *)a1)[0] = 0xE;
    if (r5 == 0)
        ((short *)a1)[1] = 0xD;
    else if (flag != 0)
        ((short *)a1)[1] = 9;
    else if (*(unsigned short *)(sb + 0x2E) & (1 << *(unsigned short *)(s + 0x184)))
        ((short *)a1)[1] = 0xF;
    else
        ((short *)a1)[1] = 0;
    ((short *)a1)[2] = 0xD;
    *(int *)(a1 + 8) = 0;
    if (flag == 0) {
        ((short *)a1)[6] = *(unsigned short *)(sb + 0xC);
        ((short *)a1)[7] = *(unsigned short *)(sb + 0xE);
        fn_0238E57C(a1 + 0x10, sb + 0x18);
        fn_0238E57C(a1 + 0x16, sb + 0x1E);
        ((short *)a1)[0xE] = *(unsigned short *)(sb + 0x2A);
        ((short *)a1)[0xF] = *(unsigned short *)(sb + 0x2C);
        ((short *)a1)[0x10] = *(unsigned short *)(sb + 0x2E);
    }
    fn_0238DEF4((short *)a1);
    if (r5 == 0)
        return;
    if (t == 1 || *(unsigned short *)(s + 0x38) == 0) {
        a1 = (char *)fn_0238E688();
        unk_27D80AC[0] = 0x26;
        *(unsigned short *)(s + 0x38) = 0;
        *(unsigned short *)(s + 0x3A) = 0;
        *(unsigned short *)(s + 0x88) = 0;
        if (fn_02381600(a1 + 0x15C, unk_27D80AC, 0) != 0)
            return;
        if (*(int *)(a1 + 0x1610) == 0)
            return;
        a1 = (char *)fn_0238DE4C();
        ((short *)a1)[0] = 0x80;
        ((short *)a1)[1] = 8;
        ((short *)a1)[2] = 0x16;
        ((short *)a1)[3] = 0x26;
        fn_0238DEF4((short *)a1);
    } else {
        *(unsigned short *)(s + 0x38) = 0;
        *(unsigned short *)(s + 0x3A) = 1;
        *(unsigned short *)(s + 0x88) = 0;
    }
}
