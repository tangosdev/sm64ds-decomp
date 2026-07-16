//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern int _ZTV17MgBounceAndPounce[];
extern int data_ov006_0213cbe4[];
extern void func_ov006_020c893c();
extern void func_ov006_020c8a04();
extern void func_ov006_020c6f70();
extern void _ZN6Player18St_LevelEnter_MainEv();
}

extern "C" void *MgBounceAndPounce_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x5834);
    if (p) {
        char *it;
        func_ov004_020b2adc(p);
        *(int *)p = (int)_ZTV17MgBounceAndPounce;
        *(short *)(p + 0x4664) = 0;
        it = p + 0x466c;
        {
            char *end = p;
            end += 0x47e4;
            do {
                it += 0xbc;
            } while (it != end);
        }
        _ZN8Particle10SysTrackerC1Ev(p + 0x47e4);
        *(int *)p = (int)data_ov006_0213cbe4;
        _ZN5ModelC1Ev(p + 0x501c);
        func_020733a8(p + 0x506c, 3, 0xb8,
                      (void *)func_ov006_020c8a04,
                      (void *)func_ov006_020c893c);
        func_020733a8(p + 0x5294, 6, 0xf0,
                      (void *)func_ov006_020c6f70,
                      (void *)_ZN6Player18St_LevelEnter_MainEv);
    }
    return p;
}
