#pragma opt_propagation off

extern void *fn_0238E688(void);
extern void fn_02382C80(unsigned short count, void *dst, int n);
extern void fn_0238244C(void *p);
extern void fn_02381874(void *p);
extern void fn_02392C58(void);

void fn_02393E94(void) {
    volatile unsigned short one;
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    char **pp = *(char ***)(hi + 0x610);
    *(int *)(*pp + 0x10) = 0;
    *(int *)(*pp + 0x18) = 0;
    *(int *)(*pp + 0x14) = 0;
    *(unsigned short *)(*pp + 0x92) = 0;
    *(unsigned short *)(*pp + 0x94) = 0;
    *(unsigned short *)(*pp + 0x96) = 0;
    *(unsigned short *)(*pp + 0x18E) = 0;
    *(unsigned short *)(*pp + 0x190) = 0;
    one = 1;
    fn_02382C80(one, *pp + 0x1F4, 0x200);
    fn_0238244C(*pp + 0x58);
    fn_02381874(*pp + 0x918);
    fn_02392C58();
}
