extern void _ZN5dBgPiD2Ev(void *);
extern void _ZN5dBgChD2Ev(void *);
extern void _ZN6Memory16operator_delete2EPv(void *);
extern int _ZTV9dBgCh_Gnd[];
extern int VTable_dBgPi_dBgCh_GndThunk[];
int *func_020374f0(int *t)
{
    t[0] = (int)_ZTV9dBgCh_Gnd;
    *(int *)((char *)t + 0x10) = (int)VTable_dBgPi_dBgCh_GndThunk;
    _ZN5dBgPiD2Ev((char *)t + 0x10);
    _ZN5dBgChD2Ev(t);
    _ZN6Memory16operator_delete2EPv(t);
    return t;
}
