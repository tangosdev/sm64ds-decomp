//cpp
extern "C" void *func_ov004_020b2adc(void);
extern "C" void func_ov006_020c33dc(void *);
extern "C" void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern "C" void *_ZN9ActorBasenwEj(unsigned int);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *);
extern void *_ZTV19dScMgSingle3DBase_c;
extern void *data_ov006_0213c154;
extern "C" void func_ov006_020deac4(void);
extern "C" void func_ov006_020e0634(void);
extern "C" void NullDestructor_0203d47c(void);
extern "C" void func_0203d738(void);

extern "C" void *func_ov006_020e0574(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x5470);
    if (p) {
        func_ov004_020b2adc();
        *(void **)p = &_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = &data_ov006_0213c154;
        func_ov006_020c33dc(p + 0x4f38);
        func_020733a8(p + 0x50e8, 0x20, 0x18, (void *)func_ov006_020e0634, (void *)func_ov006_020deac4);
        func_020733a8(p + 0x53e8, 3, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    }
    return p;
}
