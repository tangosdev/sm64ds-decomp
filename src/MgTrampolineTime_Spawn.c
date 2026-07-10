//cpp
// NONMATCHING: register allocation (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" void *_ZN9ActorBasenwEj(unsigned int sz);
extern "C" void func_ov004_020b2adc(void);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *p);
extern "C" void func_020733a8(void *p, int a, int b, void *fn, void *fn2);
extern "C" void func_ov006_02120a54(char *p);
extern "C" int _ZTV17MgBounceAndPounce;
extern "C" int data_ov006_0213fb34;
extern "C" void func_ov006_020ccfc8();
extern "C" void func_ov006_020cd12c();
extern "C" void func_ov006_020d1008();
extern "C" void func_ov006_020d100c();
extern "C" void func_ov006_02120938();
extern "C" void func_ov006_021225a8();

struct Elem { char pad[0xbc]; };

extern "C" void *MgTrampolineTime_Spawn(void)
{
    char *thiz = (char *)_ZN9ActorBasenwEj(0x5dc8);
    if (thiz) {
        func_ov004_020b2adc();
        *(int**)thiz = &_ZTV17MgBounceAndPounce;
        *(unsigned short*)(thiz + 0x4664) = 0;
        Elem *p = (Elem*)(thiz + 0x466c);
        Elem *e = (Elem*)(thiz + 0x47e4);
        do {
            p++;
        } while (p != e);
        _ZN8Particle10SysTrackerC1Ev(thiz + 0x47e4);
        *(int**)thiz = &data_ov006_0213fb34;
        func_020733a8(thiz + 0x500c, 4, 0xd0, &func_ov006_020ccfc8, &func_ov006_020cd12c);
        func_020733a8(thiz + 0x534c, 3, 0x32c, &func_ov006_020d1008, &func_ov006_020d100c);
        func_020733a8(thiz + 0x5cd0, 5, 0x24, &func_ov006_02120938, &func_ov006_021225a8);
        func_ov006_02120a54(thiz + 0x5d84);
    }
    return thiz;
}
