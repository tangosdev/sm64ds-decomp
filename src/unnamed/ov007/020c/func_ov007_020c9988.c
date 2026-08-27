struct Local { char b[0x84]; };
struct P { int f0; int f4; char rest[1]; };
extern void func_ov007_020c9a2c(struct Local *l, int a);
extern char *func_ov007_020c3398(char *p, unsigned short n);
char *func_ov007_020c9988(int a) {
    struct Local l;
    struct P *p;
    if (a == 0) {
        return 0;
    }
    func_ov007_020c9a2c(&l, a);
    p = *(struct P**)((char*)&l + 0x44);
    return func_ov007_020c3398((char*)p + 8, (unsigned short)(p->f0 * p->f4));
}
