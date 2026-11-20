extern int *fn_0238E688(void);
extern int fn_02381600(void *, int *, int);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);
extern int unk_27D80C8[];

void fn_0238EB74(void) {
    int *o = fn_0238E688();
    unsigned short **pp = *(unsigned short ***)((char *)o + 0x1610);
    int v;
    if ((*pp)[0x6D] == 1)
        return;
    unk_27D80C8[0] = 0x24;
    v = **pp;
    if (v == 9 || v == 7)
        unk_27D80C8[1] = 0x7FFE;
    else if (v == 0xA || v == 8)
        unk_27D80C8[1] = 1;
    o = fn_0238E688();
    if (fn_02381600((char *)o + 0x15C, unk_27D80C8, 0) != 0)
        return;
    {
        short *msg = fn_0238DE4C();
        msg[0] = 0x80;
        msg[1] = 8;
        msg[2] = 0x16;
        msg[3] = 0x24;
        fn_0238DEF4(msg);
    }
}
