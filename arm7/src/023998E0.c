#pragma exceptions off

/* storage 0x023998E0 runtime 0x027CB858 size 0xE4 main ARM.
 * Pools: =0x4808010, =0x4808012, =0x380FFF8.
 * Interrupt-dispatch loop: v2 = *0x4808010 & *0x4808012, bit → handler. */

extern void fn_02398A20(void);
extern void fn_02398780(void);
extern int fn_02399834(void);
extern void fn_0239951C(void);
extern int fn_02399498(void);
extern void fn_02399448(void);
extern int fn_02399484(void);
extern int fn_023992D8(void);
extern void fn_023990E4(void);
extern void fn_02398BD4(void);
extern int fn_0239940C(void);
extern void fn_02398E88(void);
extern void fn_02398ACC(int);

int fn_023998E0(void) {
    unsigned short *v0, *v1;
    unsigned int v2;
    int result;

    v0 = (unsigned short *)0x4808010;
    v1 = (unsigned short *)0x4808012;
    while (1) {
        v2 = *v0 & *v1;
        if (v2 == 0)
            break;
        if ((v2 & 0x80) != 0)
            fn_02398A20();
        if ((v2 & 0x40) != 0)
            fn_02398780();
        if ((v2 & 0x8000) != 0)
            fn_02399834();
        if ((v2 & 0x4000) != 0)
            fn_0239951C();
        if ((v2 & 0x2000) != 0)
            fn_02399498();
        if ((v2 & 0x20) != 0)
            fn_02399448();
        if ((v2 & 0x800) != 0)
            fn_02399484();
        if ((v2 & 8) != 0)
            fn_023992D8();
        if ((v2 & 4) != 0)
            fn_023990E4();
        if ((v2 & 1) != 0)
            fn_02398BD4();
        if ((v2 & 0x10) != 0)
            fn_0239940C();
        if ((v2 & 2) != 0)
            fn_02398E88();
        if ((v2 & 0x1000) != 0)
            fn_02398ACC(1);
    }
    result = *(int *)0x380FFF8 | 0x1000000;
    *(int *)0x380FFF8 = result;
    return result;
}
