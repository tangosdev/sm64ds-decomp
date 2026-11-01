#pragma exceptions off

/* storage 0x02399C24 runtime 0x027CBB9C size 0x3C0 main ARM.
 * Pools: =0x380FFF4, =0x480805A, =0x4804000, =0x4805F60, =0xFFFF, =0xE7FF,
 * =0xE7BF, =0x480803C, =0x564, =0x568. Dispatch table walk; v4 wraps
 * around 0x4805F60 through the s3.f96 window. */

struct Sub300 { unsigned char pad00[0x16]; unsigned short :1, f16b1:1, :0; unsigned char pad18[0x7E]; unsigned short f96; };
struct Sub400 { unsigned char pad00[0x9A]; unsigned short f9A; unsigned short f9C; };
struct Sub600 { unsigned char pad00[0x1C]; unsigned short f1C; };
struct Base {
    unsigned char pad000[0x300]; struct Sub300 s3; unsigned char pad396[0x68];
    struct Sub400 s4; unsigned char pad49E[0x162]; struct Sub600 s6;
};
extern unsigned short *fn_023999D0(unsigned short *, int);
extern void fn_02394D84(int, int, int);
extern void fn_02394DF8(int, int);
extern void fn_02394FC4(int);
extern void fn_0239477C(int, int);
extern void fn_023A2F48(unsigned short *);
extern int fn_023A3A3C(unsigned short *);
extern int fn_023A3714(unsigned short *);
extern void fn_023A3848(unsigned short *);

int fn_02399C24(void) {
    volatile int arr[11];
    unsigned short *v1;
    unsigned short *v4;
    unsigned short *v8;
    int v10;
    unsigned short v5;
    int *v0;
    int c1, c2;
    int c7;
    int v2;
    int result;
    int v6;
    unsigned short *v7;
    unsigned short v9;
    unsigned int i;

#pragma opt_propagation off
    arr[0] = 0;
    v0 = (int *)0x380FFF4;
    v1 = (unsigned short *)0x480805A;
    arr[3] = 0;
    c1 = 1;
    arr[9] = 0;
    c7 = 7;
    arr[8] = 0;
    arr[6] = 0;
    c2 = 2;
    arr[7] = 6;
    arr[4] = arr[0];
    arr[5] = 9;
    arr[10] = arr[0];
    arr[2] = 8;
    arr[1] = 16;
    while (1) {
        v2 = *v1;
        result = *v0;
        if (v2 == ((struct Base *)result)->s4.f9C)
            return result;
        v4 = (unsigned short *)(0x4804000 + 2 * v2);
        if ((unsigned int)(v4 + 1) < 0x4805F60)
            v5 = v4[1];
        else
            v5 = *(unsigned short *)((char *)(v4 + 1) - ((struct Base *)result)->s3.f96);
        if (*v4 == 0xFFFF) {
            *v1 = v5;
        } else {
            if ((unsigned int)(v4 + 4) < 0x4805F60)
                v6 = v4[4];
            else
                v6 = *(unsigned short *)((char *)(v4 + 4) - ((struct Base *)result)->s3.f96);
            v7 = fn_023999D0(v4, v6);
            v8 = v7;
            *v1 = v5;
            if (v7 == 0) {
                if ((*v4 & 0xF) == 0xC)
                    fn_02394FC4(arr[1]);
                else
                    fn_02394FC4(arr[2]);
            } else {
                if ((((struct Base *)*v0)->s6.f1C & 8) != 0 && (v8[10] & 0x4000) != 0)
                    { struct Sub400 *sub = (struct Sub400 *)((char *)*v0 + 0x400); sub->f9A = arr[3]; }
                v9 = v8[4];
                v10 = c1;
                if ((v9 & 0x200) != 0) {
                    if (((unsigned)(v8[10] << 21) >> 31) == 1 || ((unsigned)(v8[21] << 28) >> 28) != 0) {
                        v10 = arr[4];
                        fn_02394D84(*v0 + 0x14C, *v0 + 0x1AC, (int)(v8 - 8));
                        fn_0239477C(c2, arr[5]);
                    }
                } else {
                    switch (v9 & 0xF) {
                    case 8:
                        if ((v8[10] & 0xF) == 8) {
                            v10 = arr[6];
                            fn_02394D84(*v0 + 0x14C, *v0 + 0x188, (int)(v8 - 8));
                            fn_0239477C(c2, arr[7]);
                        }
                        break;
                    case 1:
                        if (v8[10] == 0x80)
                            fn_023A2F48(v8);
                        break;
                    case 0:
                        if ((v8[10] & 0xF) == 0) {
                            v10 = arr[8];
                            fn_02394D84(*v0 + 0x14C, *v0 + 0x1A0, (int)(v8 - 8));
                            fn_0239477C(c1, c7);
                        }
                        break;
                    case 5:
                        if ((v8[10] & 0xE7FF) == 0xA4) {
                            v10 = arr[9];
                            fn_02394D84(*v0 + 0x14C, *v0 + 0x1A0, (int)(v8 - 8));
                            fn_0239477C(c1, c7);
                        }
                        break;
                    case 14:
                    case 15:
                        if ((v8[10] & 0xE7BF) == 0x118)
                            fn_023A3848(v8);
                        break;
                    case 12:
                        if ((v8[10] & 0xE7FF) == 0x228) {
                            if ((((struct Base *)*v0)->s3.f16b1) == 0)
                                *(unsigned short *)0x480803C = c1;
                            ++*(int *)(*v0 + 0x564);
                            v10 = fn_023A3A3C(v8);
                        }
                        break;
                    case 13:
                        if ((v8[10] & 0xE7FF) == 0x218) {
                            ++*(int *)(*v0 + 0x568);
                            v10 = fn_023A3714(v8);
                        }
                        break;
                    }
                }
                if (v10 != 0)
                    fn_02394DF8(*v0 + 0x14C, (int)(v8 - 8));
                if ((((struct Base *)*v0)->s6.f1C & 1) != 0) {
                    i = arr[10];
                    do {
                        if ((unsigned int)v4 >= 0x4805F60)
                            v4 = (unsigned short *)((char *)v4 - ((struct Base *)*v0)->s3.f96);
                        *v4++ = 0xFFFF;
                        ++i;
                    } while (i < 7);
                }
            }
        }
        ++arr[0];
    }
}
