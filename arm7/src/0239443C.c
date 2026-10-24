/* storage 0x0239443C runtime 0x027C63B4 size 0x1F0 (listed 0x1D0 + pool) main ARM.
 * (AR28 base: full reconstruction) */

extern int fn_02382CE0(int a1, int a2, int a3);
extern int fn_02380A4C(void);
extern int fn_02394A38(int a1);
extern int fn_023A424C(int a1);
extern int fn_023A4340(int a1, int a2, int a3);
/* local extern: AR20 matched sub_3805638 whole-asm as `asm void fn_0238D7A0(void)',
 * but the ROM caller in fn_0239443C provably sets up two argument registers
 * (R0 = *(a1+48) loaded, R1 = previous call result) and consumes R0 after the
 * BL, so this TU declares it old-style with an int result. */
extern int fn_0238D7A0();
extern int fn_02397E3C(int a1, int a2);
extern int fn_023948FC(int a1);
extern int fn_0239AE00(int a1);
extern int fn_0239B224(int a1);
extern int fn_0239A434(void);
extern int fn_023957C4(void);
extern int fn_023A43D8(void *a1);
extern int fn_02395A7C(void);
extern int fn_02398358(int a1);
extern int fn_02396DD8(int a1);
extern int fn_02395A44(void);
extern int fn_02395830(void);
extern int fn_023981D0(int a1);
extern int fn_02397ED4(int a1);
extern int fn_023959E0(void);
extern int fn_0239730C(int a1);
extern int fn_02396E6C(int a1);
extern void fn_0239483C(void);
extern int fn_02381110(void *a1, void (*a2)(void), int a3, int a4, int a5, int a6);
extern int fn_02380FAC(void *a1);
extern int fn_02398500(int a1);

#define OFF (*(int *)0x380FFF4)

int fn_0239443C(int a1) {
    int v2;
    int v25;
    volatile int v26;
    int t, v9, v10, v11, v12, v13, v23;
    int *b4;
    v2 = *(int *)a1;
    OFF = v2;
    v26 = 0;
    fn_02382CE0(v26, v2, 0x678);
    t = fn_02380A4C();
    ((int *)OFF)[182] = t;
    ((int *)OFF)[180] = *(int *)(a1 + 24);
    ((int *)OFF)[181] = (int)((unsigned)*(int *)(a1 + 28) >> 1);
    b4 = (int *)OFF;
    if (b4[181] == 0)
        b4[181] = -1;
    t = fn_02394A38(a1 + 32);
    fn_023A424C(t);
    *(unsigned short *)0x4000304 |= 2;
    *(unsigned short *)0x4000206 = 48;
    ((int *)OFF)[390] = 3;
    fn_023A4340(60, 2, OFF + 0x61E);
    fn_023A4340(62, 2, OFF + 0x61C);
    ((int *)OFF)[179] = *(int *)(a1 + 16);
    ((int *)OFF)[178] = *(int *)(a1 + 20);
    t = fn_0238D7A0(*(int *)(a1 + 48), 28);
    v9 = fn_02397E3C(*(int *)(a1 + 44), t);
    v10 = fn_023948FC(v9);
    v11 = fn_0239AE00(v10);
    v12 = fn_0239B224(v11);
    v13 = fn_0239A434();
    fn_023957C4();
    if (fn_023A43D8(&v25) != 0) {
        ((unsigned short *)OFF)[0x181] |= 0x80;
    } else {
        t = fn_02395A7C();
        v9 = fn_02398358(t);
        v10 = fn_02396DD8(v9);
        v11 = fn_02395A44();
        v12 = fn_02395830();
        v13 = fn_023981D0(v12);
        t = fn_02397ED4(v13);
        v9 = fn_023959E0();
        v10 = fn_0239730C(v9);
        v11 = fn_02396E6C(v10);
    }
    fn_02381110((char *)OFF + 24, fn_0239483C, 0, *(int *)(a1 + 4), *(int *)(a1 + 8), *(int *)(a1 + 12));
    v23 = fn_02380FAC((char *)OFF + 24);
    fn_02398500(v23);
    return ((unsigned short *)OFF)[0x181];
}
