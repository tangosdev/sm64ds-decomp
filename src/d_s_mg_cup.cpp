//cpp
// @symbol dScMgCup_c_classInit
extern "C" void *_ZN11dScMgBase_cC2Ev(void);
extern "C" void func_ov006_020c33dc(void *);
extern "C" void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern "C" void *_ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *);
extern void *_ZTV19dScMgSingle3DBase_c;
extern void *_ZTV10dScMgCup_c;
extern "C" void func_ov006_020deac4(void);
extern "C" void func_ov006_020e0634(void);
extern "C" void NullDestructor_0203d47c(void);
extern "C" void func_0203d738(void);

/* Reconstructed source-style name: SM64DS proves dScMgCup_c through RTTI,
 * allocation size, vtable identity, and the MG_CUP registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov006_020e0574. */
extern "C" void *dScMgCup_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x5470);
    if (p) {
        _ZN11dScMgBase_cC2Ev();
        *(void **)p = &_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = &_ZTV10dScMgCup_c;
        func_ov006_020c33dc(p + 0x4f38);
        func_020733a8(p + 0x50e8, 0x20, 0x18, (void *)func_ov006_020e0634, (void *)func_ov006_020deac4);
        func_020733a8(p + 0x53e8, 3, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    }
    return p;
}
