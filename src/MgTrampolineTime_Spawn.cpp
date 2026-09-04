//cpp
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern int _ZTV14dScMgD3DBase_c[];
extern int _ZTV17dScMgTrampoline_c[];
extern void func_ov006_020ccfc8();
extern void func_ov006_020cd12c();
extern void func_ov006_020d1008();
extern void func_ov006_020d100c();
extern void func_ov006_02120938();
extern void func_ov006_021225a8();
extern void func_ov006_02120a54(void *);
}

/* Reconstructed source-style name: SM64DS proves dScMgTrampoline_c through RTTI,
 * allocation size, vtable identity, and the MG_TRAMPOLINE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgTrampolineTime_Spawn. */
extern "C" void *dScMgTrampoline_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x5dc8);
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
        *(int *)p = (int)_ZTV17dScMgTrampoline_c;
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
