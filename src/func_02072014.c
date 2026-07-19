extern unsigned char func_02072d90(void *p);
extern void func_020731fc(void);
extern unsigned char func_020729f4(void *p);
extern unsigned char *func_02071af8(unsigned char *p, int *out);

struct Arr21 {
    int w[21];
};

struct Ctx {
    int f0, f1, f2, f3, f4, f5, f6;
    struct Arr21 arr;
};

struct Local {
    int a[6];
    struct Ctx cpart;
};

int func_02072014(int *c, int *arg)
{
    struct Local l;
    unsigned char sel;
    int tmp;
    int base;
    int bt;

    l.a[0] = arg[0];
    l.a[1] = arg[1];
    l.a[2] = arg[2];
    l.a[3] = arg[3];
    l.a[4] = arg[4];
    l.a[5] = arg[5];

    l.cpart.f0 = ((struct Ctx *)c)->f0;
    l.cpart.f1 = ((struct Ctx *)c)->f1;
    l.cpart.f2 = ((struct Ctx *)c)->f2;
    l.cpart.f3 = ((struct Ctx *)c)->f3;
    l.cpart.f4 = ((struct Ctx *)c)->f4;
    l.cpart.f5 = ((struct Ctx *)c)->f5;
    l.cpart.f6 = ((struct Ctx *)c)->f6;
    l.cpart.arr = ((struct Ctx *)c)->arr;

    sel = func_02072d90(&l);
    for (;;) {
        switch (sel) {
        case 1:
        case 14:
        default:
            func_020731fc();
            goto done;
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 15:
        case 16:
        case 17:
        case 18:
            sel = func_020729f4(&l);
            break;
        case 13:
            goto done;
        }
    }
done:
    func_02071af8((unsigned char *)(l.a[2] + 1), &tmp);
    base = l.cpart.f6;
    bt = base + tmp;
    c[0] = *(int *)(bt + 4);
    c[1] = *(int *)bt;
    c[2] = 0;
    c[3] = bt;
    return bt;
}
