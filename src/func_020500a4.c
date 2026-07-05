extern int data_020a55fc;
extern int data_020a55f8;
extern int data_020a5600;
extern void func_02058894(void *a, void *slot, int c);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);

struct Q { int f0, f4, f8, fc, f10; };   /* 0x14 bytes */
extern struct Q data_020a5718[];

struct T {
    int f0, f4;
    int f8;                /* 8 */
    int fc;                /* c */
    int f10;               /* 10 */
    int f14;               /* 14 */
    int f18;               /* 18 */
    int f1c, f20, f24, f28;
    int f2c;               /* 2c */
    void (*f30)(int, int, int, int, int);  /* 30 */
    int f34;               /* 34 */
};

void func_020500a4(struct T *thiz)
{
    int quot = thiz->f14 / thiz->f2c;
    int prod = quot * thiz->f18;
    int e6 = thiz->fc + prod;
    int e5 = thiz->f10 + prod;

    if (data_020a55fc != 0) {
        struct Q *slot = &data_020a5718[data_020a55f8];
        slot->f0 = (int)thiz;
        slot->f4 = quot;
        slot->f8 = prod;
        slot->fc = e6;
        slot->f10 = e5;
        func_02058894(&data_020a5600, slot, 0);
        data_020a55f8 = data_020a55f8 + 1;
        if (data_020a55f8 >= 8) data_020a55f8 = 0;
    } else {
        _ZN4CP1519InvalidateDataCacheEjj(e6, quot);
        _ZN4CP1519InvalidateDataCacheEjj(e5, quot);
        thiz->f30(e6, e5, quot, thiz->f8, thiz->f34);
    }
    {
        int *pf18 = (int *)((int)&thiz->f18 & 0xFFFFFFFFFFFFFFFF);
        *pf18 = *pf18 + 1;
        if (thiz->f18 >= thiz->f2c) thiz->f18 = 0;
    }
}
