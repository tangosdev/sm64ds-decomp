extern void *_ZN9ActorBasenwEj(unsigned);
extern void func_ov004_020b2adc(void *);
extern int VT0[];
int *MgBobOmbSquad_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(23608);
    if (p) {
        func_ov004_020b2adc(p);
        p[0] = (int)VT0;
    }
    return p;
}
