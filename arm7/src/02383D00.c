extern int fn_02384054(int *);
extern void fn_023840A4(int *);

unsigned int fn_02383D00(int *a1, int a2)
{
    long long w = fn_02384054(a1 + 10);
    unsigned int v5 = (unsigned int)w;
    if (w != 0) {
        int v8 = *(unsigned char *)(a1 + 10);
        if (v8 != 0) {
            if (v8 != 1) {
                if (v8 == 2)
                    w = w << 6;
            } else
                w = w * 60;
        } else
            w = w << 6;
        v5 = (unsigned int)(w >> 14);
    }
    if (a2 != 0)
        fn_023840A4(a1 + 10);
    return v5;
}
