extern void func_02037f18(void *);
extern void func_020380c0(void *);
extern int _ZTV5dBgPi[];
int *func_0203819c(int *t)
{
    func_02037f18((char *)t + 0x4);
    t[0] = (int)_ZTV5dBgPi;
    func_020380c0(t);
    return t;
}
