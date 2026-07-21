struct E;
extern int func_02071844(void **p, int v);
extern struct E *func_02072ed0(struct E *base, int count, unsigned int key);
extern unsigned char *func_0207180c(unsigned char *p);
extern unsigned char *func_02071a50(unsigned char *p, int *out);

struct S {
    unsigned char *f0;
    unsigned char *f1;
    int f2;
    int f3;
    int f4;
};

void func_02072dac(int r4, struct S *sb)
{
    struct E *e;
    unsigned char *p;
    int a, b, c;
    unsigned int end;
    unsigned int acc, t;

    sb->f1 = 0;
    sb->f2 = 0;
    if (!func_02071844((void **)sb, r4))
        return;
    e = func_02072ed0((struct E *)sb->f3, (sb->f4 - sb->f3) / 12, (unsigned int)r4);
    if (!e)
        return;
    if (*((int *)e + 1) & 1)
        sb->f1 = (unsigned char *)e + 8;
    else
        sb->f1 = (unsigned char *)*((int *)e + 2);
    sb->f0 = (unsigned char *)*(int *)e;
    end = (unsigned int)(r4 - *(int *)e);
    p = func_0207180c(sb->f1);
    acc = 0;
    do {
        p = func_02071a50(p, &a);
        if (a == 0)
            return;
        p = func_02071a50(p, &b);
        p = func_02071a50(p, &c);
        t = acc + (unsigned)a;
        if (end < t)
            return;
        acc = t + (unsigned)b;
    } while (end > acc);
    sb->f2 = (int)sb->f1 + c;
}
