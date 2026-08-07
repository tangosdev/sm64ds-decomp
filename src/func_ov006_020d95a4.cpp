//cpp
extern "C" {
extern void *data_ov006_0213bdb4;
extern void *data_ov006_0213e448;
extern void __destroy_arr(void *, int, int, void *);
int func_ov006_020c1c64(char *t);
void func_ov006_020d96f0(void);
void func_ov006_020d96e0(void);
void _ZN8Particle10SysTrackerD1Ev(void *);
void _ZN11dScMgBase_cD2Ev(void *c);
void *func_ov006_020d95a4(char *c)
{
    *(void **)c = &data_ov006_0213bdb4;
    __destroy_arr(c + 0x5298, 5, 0x30, (void *)func_ov006_020d96f0);
    __destroy_arr(c + 0x51a8, 5, 0x30, (void *)func_ov006_020d96e0);
    func_ov006_020c1c64(c + 0x4f38);
    *(void **)c = &data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
}
