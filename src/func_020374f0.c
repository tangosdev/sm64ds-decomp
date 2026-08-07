extern void _ZN10ClsnResultD2Ev(void *);
extern void func_020354d0(void *);
extern void _ZN6Memory16operator_delete2EPv(void *);
extern int data_02099264[];
extern int data_02099274[];
int *func_020374f0(int *t)
{
    t[0] = (int)data_02099264;
    *(int *)((char *)t + 0x10) = (int)data_02099274;
    _ZN10ClsnResultD2Ev((char *)t + 0x10);
    func_020354d0(t);
    _ZN6Memory16operator_delete2EPv(t);
    return t;
}
