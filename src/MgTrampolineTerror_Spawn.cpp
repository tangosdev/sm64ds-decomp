//cpp
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern int _ZTV14dScMgD3DBase_c[];
extern int _ZTV18dScMgTrampoline2_c[];
extern void func_ov006_020ca604();
extern void _ZN6Player29TryExitCharacterDoorWithIntroEv();
extern void func_ov006_020d1008();
extern void func_ov006_020d100c();
extern void func_ov006_021227c8();
extern void func_ov006_021248a8();
extern void func_ov006_020eed64();
extern void func_ov006_020efc08();
extern void func_ov006_02122c68();
extern void func_ov006_02122c90();
extern void func_ov006_02120938();
extern void func_ov006_021225a8();
}

extern "C" void *MgTrampolineTerror_Spawn(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x7bac);
    if (p) {
        char *it;
        _ZN11dScMgBase_cC2Ev(p);
        *(int *)p = (int)_ZTV14dScMgD3DBase_c;
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
        *(int *)p = (int)_ZTV18dScMgTrampoline2_c;
        func_020733a8(p + 0x500c, 5, 0xdc,
                      (void *)_ZN6Player29TryExitCharacterDoorWithIntroEv,
                      (void *)func_ov006_020ca604);
        func_020733a8(p + 0x5458, 3, 0x32c,
                      (void *)func_ov006_020d100c,
                      (void *)func_ov006_020d1008);
        func_020733a8(p + 0x5ddc, 0xa, 0x1d0,
                      (void *)func_ov006_021248a8,
                      (void *)func_ov006_021227c8);
        func_020733a8(p + 0x6ffc, 0xa, 0x24,
                      (void *)func_ov006_020efc08,
                      (void *)func_ov006_020eed64);
        func_020733a8(p + 0x7164, 0x14, 0x78,
                      (void *)func_ov006_02122c90,
                      (void *)func_ov006_02122c68);
        func_020733a8(p + 0x7ad0, 5, 0x24,
                      (void *)func_ov006_021225a8,
                      (void *)func_ov006_02120938);
    }
    return p;
}
