struct E {
    int f0;
    int f4;
    int f8;
};

struct S {
    int f0;
    unsigned char *f1;
    int f2;
    struct E *f3;
    struct E *f4;
};

extern int func_02071844(struct S *p, int v);
extern struct E *func_02072ed0(struct E *base, int count, unsigned int key);
extern unsigned char *func_0207180c(unsigned char *p);
extern unsigned char *func_02071a50(unsigned char *p, int *out);

void func_02072dac(int r4, struct S *sb)
{
    struct E *e;
    unsigned char *p;
    int a, b, c;
    unsigned int end;
    unsigned int acc;
    unsigned int tmp;

    sb->f1 = 0;
    sb->f2 = 0;
    if (!func_02071844(sb, r4))
        return;
    e = func_02072ed0(sb->f3, sb->f4 - sb->f3, (unsigned int)r4);
    if (!e)
        return;
    if (e->f4 & 1)
        sb->f1 = (unsigned char *)&e->f8;
    else
        sb->f1 = (unsigned char *)e->f8;
    sb->f0 = e->f0;
    end = (unsigned int)(r4 - e->f0);
    p = func_0207180c(sb->f1);
    acc = 0;
    do {
        p = func_02071a50(p, &a);
        if (a == 0)
            return;
        p = func_02071a50(p, &b);
        p = func_02071a50(p, &c);
        tmp = acc + (unsigned int)a;
        if (end < tmp)
            return;
        acc = tmp + (unsigned int)b;
    } while (end > acc);
    sb->f2 = (int)sb->f1 + c;
}
