typedef short s16;
typedef long long s64;

extern void func_ov007_020c90c0(void *c);

inline int keep(int x)
{
    return x;
}

int func_ov007_020c8f98(int *thiz)
{
    s16 t;
    int a;
    int b;
    int *slot;

    if (thiz[0] == 2) {
        thiz[0] = 3;
        goto ret;
    }
    if (thiz[0] != 1)
        goto ret;

    switch (thiz[1]) {
    case 0:
        t = *(s16 *)((char *)thiz + 0xc);
        a = thiz[6];
        b = thiz[5];
        if (t < 0x1000) {
            if (t <= 0)
                a = b;
            else
                a = (int)((((s64)t) * keep(a) + ((s64)(0x1000 - t)) * b) >> 12);
        }
        slot = (int *)thiz[4];
        *slot = a;
        func_ov007_020c90c0(thiz);
        break;
    case 1: {
        int c;
        int target;
        int st;
        int d;
        int ad;
        int sg;
        int dn;

        c = *(int *)thiz[4];
        target = thiz[6];
        st = thiz[2];
        d = target - c;
        if (d >= 0)
            ad = d;
        else
            ad = -d;
        if (ad <= st) {
            *(int *)thiz[4] = target;
            dn = 1;
        } else {
            if (d >= 0)
                sg = 1;
            else
                sg = -1;
            *(int *)thiz[4] = st * sg + c;
            dn = 0;
        }
        if (dn != 0)
            thiz[0] = 2;
        break;
    }
    default:
        break;
    }

ret:
    if (thiz[0] == 3)
        return 1;
    return 0;
}
