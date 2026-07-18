typedef unsigned int u32;

struct Thing {
    int *field0;
    int field4;
    int *field8;
    int fieldC;
    void *field10;
};

extern void *func_020c3df4(int heap, int size);
extern void func_020c78b0(void);
extern int func_020c844c(int a, void *b);
extern int func_020c80a4(void);

struct Thing *func_ov007_020c798c(void *p0, int p1, void *p2, int p3)
{
    int i;
    struct Thing *t;

    t = (struct Thing *)func_020c3df4(0, 0x14);
    func_020c78b0();
    t->field10 = p0;
    t->field4 = p1;
    t->fieldC = p3;
    t->field0 = (int *)func_020c3df4(0, t->field4 * 4);
    if (t->fieldC & 1) {
        for (i = 0; i < t->field4; i++)
            t->field0[i] = func_020c844c(0, p2);
    }
    if (p1 > 1) {
        int n2;
        if (t->field4 <= 1)
            n2 = 0;
        else
            n2 = t->field4 - ((t->fieldC & 2) ? 0 : 1);
        t->field8 = (int *)func_020c3df4(0, n2 * 4);
        for (i = 0; i < n2; i++)
            t->field8[i] = func_020c80a4();
    }
    return t;
}
