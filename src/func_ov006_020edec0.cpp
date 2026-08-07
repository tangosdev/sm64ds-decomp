//cpp
extern "C" {
extern void *data_ov006_0213cbe4;
extern void *_ZTV17MgBounceAndPounce;
extern void __destroy_arr(void *, int, int, void *);
void func_ov006_020c6f3c(void);
void func_ov006_020c893c(void);
void _ZN5ModelD1Ev(void *);
void _ZN8Particle10SysTrackerD1Ev(void *);
void _ZN11dScMgBase_cD2Ev(void *c);
void *func_ov006_020edec0(char *c)
{
    *(void **)c = &data_ov006_0213cbe4;
    __destroy_arr(c + 0x5294, 6, 0xf0, (void *)func_ov006_020c6f3c);
    __destroy_arr(c + 0x506c, 3, 0xb8, (void *)func_ov006_020c893c);
    _ZN5ModelD1Ev(c + 0x501c);
    *(void **)c = &_ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
}
