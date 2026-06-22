extern void *_ZN9ActorBasenwEj(unsigned);
extern void func_ov004_020b2adc(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_ov006_020c1d80(void *);
extern int data_ov006_0213e448[];
extern int data_ov006_0213d4d4[];
int *func_ov006_020f75d4(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x5410);
    if (p) {
        func_ov004_020b2adc(p);
        p[0] = (int)data_ov006_0213e448;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);
        p[0] = (int)data_ov006_0213d4d4;
        func_ov006_020c1d80((char *)p + 0x4f38);
    }
    return p;
}
