extern int *fn_0238E688(void);
extern void *fn_02381C7C(int, int, int);
extern int fn_02381600(void *, void *, int);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

void fn_0238EFCC(void) {
    int *o = fn_0238E688();
    char *p = fn_02381C7C(*(int *)((char *)o + 0x1608), *(int *)((char *)o + 0x160C), 0x40);
    *(short *)(p + 0xC) = 0x185;
    *(short *)(p + 0xE) = 0;
    if (fn_02381600((char *)o + 0x34, p, 0) != 0)
        return;
    if (*(int *)((char *)o + 0x1610) == 0)
        return;
    short *msg = fn_0238DE4C();
    msg[0] = 0x80;
    msg[1] = 8;
    msg[2] = 0x16;
    msg[3] = 0x80;
    fn_0238DEF4(msg);
}
