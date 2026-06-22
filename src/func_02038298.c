extern int func_02037f94(void *c);
extern int func_02037f54(void *p);
extern int func_020393ac(void *p);
extern int func_020393b4(void *p);
extern int func_02037f64(void *p, int a, int b, void *c);
extern void *data_020a0c80[];

int func_02038298(void *arg)
{
    int idx; void *e; int a, b;
    if (func_02037f94(arg) == 0) return 0;
    idx = func_02037f54(arg);
    if (idx < 0) goto r;
    if (idx < 0x18) goto work;
r:  return 0;
work:
    e = data_020a0c80[idx];
    if (e == 0) return 0;
    a = func_020393ac(e);
    b = func_020393b4(e);
    return func_02037f64(arg, a, b, e);
}
