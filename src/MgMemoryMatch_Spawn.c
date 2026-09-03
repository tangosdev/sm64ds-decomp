extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void func_ov006_020c1d80(void *);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV13dScMgMemory_c[];
int *dScMgMemory_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x5340);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);
        p[0] = (int)_ZTV13dScMgMemory_c;
        func_ov006_020c1d80((char *)p + 0x4f38);
    }
    return p;
}
