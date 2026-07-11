extern void func_0205c528(void *dst, void *bits, int n);

struct Out { unsigned char b0; unsigned char pad; unsigned short b2; };

int func_0205c264(int *thiz)
{
    char *r4 = (char *)thiz[0xb];    /* [0x2c] */
    struct Out out;                  /* [sp+0] */
    int two[2];                      /* [sp+4]=thiz[8], [sp+8]=thiz[9] */
    unsigned char v;
    int hi, lo;

    two[0] = thiz[2];                /* [8] -> sp+4 */
    two[1] = thiz[9];                /* [0x24] -> sp+8 */
    func_0205c528(&two[0], &out, 1);
    v = out.b0;
    lo = v & 0x7f;
    hi = (v >> 7) & 1;
    *(int *)(r4 + 0x10) = lo;
    *(int *)(r4 + 0xc) = hi;
    if (*(int *)(r4 + 0x10) == 0) {
        return 1;
    }
    if (thiz[0xc] != 0) {            /* [0x30] */
        two[1] = two[1] + *(int *)(r4 + 0x10);
    } else {
        func_0205c528(&two[0], r4 + 0x14, *(int *)(r4 + 0x10));
        *(r4 + 0x14 + *(int *)(r4 + 0x10)) = 0;
    }
    if (*(int *)(r4 + 0xc) != 0) {
        func_0205c528(&two[0], &out.b2, 2);
        *(int *)r4 = thiz[2];
        *(short *)(r4 + 4) = out.b2 & 0xfff;
        *(short *)(r4 + 6) = 0;
        *(int *)(r4 + 8) = 0;
    } else {
        unsigned short *q = (unsigned short *)(int)(((long long)(int)((char *)thiz + 0x22)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)r4 = thiz[2];
        *(int *)(r4 + 4) = *(unsigned short *)((char *)thiz + 0x22);
        *q = *q + 1;
    }
    thiz[9] = two[1];               /* [0x24] */
    return 0;
}
