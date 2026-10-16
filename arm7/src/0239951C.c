#pragma exceptions off

/* storage 0x0239951C runtime 0x027CB494 size 0x2EC main ARM.
 * Pools: =0x380FFF4, =0x4808010, =0x380FFF0, =0x4808134, =0x48080B6,
 * =0xFFFF, =0x4808038, =0x4808118, =0x4808048, =0x494, =0x48080AE.
 * Three-state dispatch; cases 2 and 3 share the tail update block. */

extern void fn_02395354(int, int);
extern void fn_023986F4(void);
extern void fn_0239FA98(unsigned int);
extern void fn_0239477C(int, int);

struct S300 {
    unsigned short f0;
};

struct S400 {
    unsigned char pad00[0xEA];
    unsigned short fEA;
    unsigned short fEC;
    unsigned short fEE;
};

struct S470 {
    unsigned int f0;
};

struct Ent {
    unsigned char pad00[0x8];
    unsigned short f8;
    unsigned char pad0A[2];
    unsigned short fC;
    unsigned short fE_0:1, :1, fE_2:1, fE_3:1, :12;
    unsigned char pad10[0x54];
    unsigned short f64;
    unsigned short f66;
    unsigned short f68;
    unsigned short f6A;
    unsigned char pad6C[0x14];
    unsigned short f80;
    unsigned char pad82[0x8];
    unsigned short f8A;
    unsigned char pad8C[0x10];
    int f9C;
};

struct Base {
    unsigned char pad00[0x300];
    struct S300 s300;
    unsigned char pad302[0x162];
    int f464;
    unsigned char pad468[8];
    struct S470 s470;
};

void fn_0239951C(void) {
    int *v0;
    struct Ent *v1;
    int v2;
    int v3;
    int v4;
    int v9;
    unsigned short v5;
    int *v6;
    int v7;
    unsigned int i;
    unsigned int v11;
    int v12;

    v0 = (int *)0x380FFF4;
    v1 = (struct Ent *)(*v0 + 0x308);
    v2 = *v0 + 0x3E4;
    *(unsigned short *)0x4808010 = 0x4000;
    v3 = v1->fC;
    switch (v3) {
    case 1:
        v9 = ((struct Base *)*v0)->f464 + 0x24 + v1->f8A;
        v4 = v9 + 8;
        v5 = *(unsigned short *)0x380FFF0;
        fn_02395354(v4, v5 & 0xFF);
        fn_02395354(v4 + 1, ((unsigned int)v5 >> 8) & 0xFF);
        if (v1->fE_0 == 1)
            *(unsigned short *)0x4808134 = ((struct Base *)*(int *)0x380FFF4)->s300.f0 + *(unsigned short *)0x4808134 + 1;
        char *tp;
        v6 = (int *)0x380FFF4;
        tp = (char *)*v6;
        tp += 0x400;
        ((struct S400 *)tp)->fEC = ~((struct S400 *)tp)->fEA | ((struct S400 *)tp)->fEE;
        if ((*(unsigned short *)0x48080B6 & 0x18) != 0 || (*(unsigned short *)0x48080B6 & 6) == 2) {
            ((struct Base *)*(int *)0x380FFF4)->s470.f0 &= ~2;
            fn_023986F4();
        } else {
            ((struct Base *)*v6)->s470.f0 |= 2;
        }
        break;
    case 2:
        if (v1->fE_2 != 0)
            *(unsigned short *)0x4808134 = ((struct Base *)*v0)->s300.f0 + *(unsigned short *)0x4808134 + 1;
        else
            *(unsigned short *)0x4808134 = 0xFFFF;
    case 3:
        if (v1->f8 != 0x40) {
            v7 = 1;
        } else {
            v7 = 0;
            if (v1->f66 == 1)
                v7 = 1;
            if (v1->fE_3 != 0 && (v1->f6A == 1 || (v1->f6A == 0 && v1->f68 == 1)))
                v7 = 1;
        }
        if (v7 != 0)
            *(unsigned short *)0x4808038 |= 1;
        else
            *(unsigned short *)0x4808038 &= ~1;
        if (*(unsigned short *)0x4808118 > 0xA)
            *(unsigned short *)0x4808048 = 0;
        if (--v1->f66 == 0)
            v1->f66 = v1->f64;
        v9 = v1->f6A--;
        if (v9 == 0)
            v1->f6A = v1->f68 - 1;
        for (i = 0; i < 2; ++i) {
            v11 = v2 + 20 * i;
            if (*(unsigned short *)(v2 + 20 * i) != 0) {
                v12 = *(int *)(v11 + 0xC);
                if (*(unsigned short *)(v12 + 8) == 0 &&
                    (unsigned short)(v1->f9C - *(unsigned short *)(v12 + 4)) > (unsigned int)v1->f80) {
                    fn_0239FA98(i);
                    **(unsigned short **)(v11 + 8) = 2;
                    fn_0239477C(0, 14);
                    ++*(unsigned short *)(*(int *)0x380FFF4 + 0x494);
                }
            }
        }
        *(unsigned short *)0x48080AE = 13;
        break;
    }
}
