extern void func_0203ac50(void *);
extern void func_ov002_020feab8(void *);
extern void _ZN10ClsnResultD2Ev(void *);
extern void func_020354d0(void *);
extern void _ZN6Memory16operator_delete2EPv(void *);
extern int data_020992a4[];
extern int data_020992b4[];
int *func_02037710(int *t)
{
    t[0] = (int)data_020992a4;
    *(int *)((char *)t + 0x10) = (int)data_020992b4;
    func_0203ac50((char *)t + 0x64);
    func_ov002_020feab8((char *)t + 0x38);
    _ZN10ClsnResultD2Ev((char *)t + 0x10);
    func_020354d0(t);
    _ZN6Memory16operator_delete2EPv(t);
    return t;
}
