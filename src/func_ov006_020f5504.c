extern void *_ZN9ActorBasenwEj(unsigned);
extern void func_ov004_020b2adc(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_ov006_020c1d80(void *);
extern int data_ov006_0213e448[];
extern int data_ov006_0213d1b8[];
int *func_ov006_020f5504(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x5340);
    if (p) {
        func_ov004_020b2adc(p);
        p[0] = (int)data_ov006_0213e448;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);
        p[0] = (int)data_ov006_0213d1b8;
        func_ov006_020c1d80((char *)p + 0x4f38);
    }
    return p;
}
