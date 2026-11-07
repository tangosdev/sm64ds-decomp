/* storage 0x0239341C runtime 0x027C5394 size 0x140 (listed 0x134 + pool) main ARM.
 * (AR28 base: full reconstruction) */

extern void *fn_0238E688(void);
extern void fn_023817E0(void *a1);
extern void fn_0238176C(void *a1);

int fn_0239341C(int a1, int a2, int a3, int a4, unsigned short a5, int a6, int a7) {
    unsigned short *v11;
    unsigned short *v12;
    unsigned short *v13;
    unsigned short *p199;
    unsigned short *v18;
    int v15;
    int v16;
    int v20;
    {
        int *p = (int *)((char *)fn_0238E688() + 0x1000);
        v11 = *(unsigned short **)(p[0x184]);
    }
    v12 = v11 + 378;
    v13 = (unsigned short *)((char *)v11 + 0x908 + a1 * 4);
    if (a5 == 0)
        return 6;
    v15 = ((a2 & 8) != 0) ? 2 : 0;
    p199 = v11 + 0x80;
    if (a5 + v15 > p199[0x47])
        return 6;
    fn_023817E0((char *)v11 + 0x918);
    v16 = *(v11 + 0x47A);
    if (v16 == 0xFFFF) {
        fn_0238176C((char *)v11 + 0x918);
        return 10;
    }
    v18 = (unsigned short *)((char *)v12 + v16 * 24);
    v11[0x400 + 0x7A] = *v18;
    if (v11[0x47B] == v16)
        v11[0x47B] = 0xFFFF;
    v18[1] = a2;
    v18[2] = a3;
    *(unsigned *)&v18[6] = a4;
    v18[3] = a5;
    *(unsigned *)&v18[8] = a6;
    *(unsigned *)&v18[10] = a7;
    *v18 = 0xFFFF;
    v18[4] = 0xFFFF;
    v18[5] = v11[76];
    v20 = v13[1];
    if (v20 == 0xFFFF)
        *v13 = v16;
    else
        *(unsigned short *)((char *)v12 + v20 * 24) = v16;
    v13[1] = v16;
    fn_0238176C((char *)v11 + 0x918);
    return 2;
}
