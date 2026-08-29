extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern int func_ov006_020c221c(char *t);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMgSlot3_c[];
int *func_ov006_0210c120(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x5044);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);
        p[0] = (int)_ZTV12dScMgSlot3_c;
        func_ov006_020c221c((char *)p + 0x4f38);
    }
    return p;
}
