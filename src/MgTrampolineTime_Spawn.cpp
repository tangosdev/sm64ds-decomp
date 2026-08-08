//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern int _ZTV17MgBounceAndPounce[];
extern int data_ov006_0213fb34[];
extern void func_ov006_020ccfc8();
extern void func_ov006_020cd12c();
extern void func_ov006_020d1008();
extern void func_ov006_020d100c();
extern void func_ov006_02120938();
extern void func_ov006_021225a8();
extern void func_ov006_02120a54(void *);
}

extern "C" void *MgTrampolineTime_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x5dc8);
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
        *(int *)p = (int)data_ov006_0213fb34;
        func_020733a8(p + 0x500c, 4, 0xd0,
                      (void *)func_ov006_020cd12c,
                      (void *)func_ov006_020ccfc8);
        func_020733a8(p + 0x534c, 3, 0x32c,
                      (void *)func_ov006_020d100c,
                      (void *)func_ov006_020d1008);
        func_020733a8(p + 0x5cd0, 5, 0x24,
                      (void *)func_ov006_021225a8,
                      (void *)func_ov006_02120938);
        func_ov006_02120a54(p + 0x5d84);
    }
    return p;
}
