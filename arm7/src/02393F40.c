#pragma scheduling off
#pragma opt_propagation off

extern void *fn_0238E688(void);
extern void fn_02393E94(void);
extern void fn_02389D60(int code);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_02393F40(int *a1) {
    char *base = (char *)fn_0238E688();
    int saved = a1[1];
    char *hi = base + 0x1000;
    *(int *)(hi + 0x610) = saved;
    int *obj = *(int **)(hi + 0x610);
    obj[0] = a1[2];
    obj[2] = a1[3];
    fn_02393E94();
    fn_02389D60(0xF);
    *(unsigned short *)obj[0] = 1;
    unsigned short *msg = fn_0238DE4C();
    msg[0] = 3;
    msg[1] = 0;
    fn_0238DEF4(msg);
}
