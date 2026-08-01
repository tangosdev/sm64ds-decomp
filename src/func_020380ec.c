extern void func_02037ee4(void *);
extern int data_02099368[];
int *func_020380ec(int *t)
{
    t[0] = (int)data_02099368;
    func_02037ee4((char *)t + 0x4);
    return t;
}
