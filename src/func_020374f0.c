extern void _ZN5dBgPiD2Ev(void *);
extern void func_020354d0(void *);
extern void _ZN6Memory16operator_delete2EPv(void *);
extern int _ZTV9dBgCh_Gnd[];
extern int data_02099274[];
int *func_020374f0(int *t)
{
    t[0] = (int)_ZTV9dBgCh_Gnd;
    *(int *)((char *)t + 0x10) = (int)data_02099274;
    _ZN5dBgPiD2Ev((char *)t + 0x10);
    func_020354d0(t);
    _ZN6Memory16operator_delete2EPv(t);
    return t;
}
