extern void func_02037f18(void *);
extern void func_020380c0(void *);
extern int data_02099368[];
int *func_0203819c(int *t)
{
    func_02037f18((char *)t + 0x4);
    t[0] = (int)data_02099368;
    func_020380c0(t);
    return t;
}
