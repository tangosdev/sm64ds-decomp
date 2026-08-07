//cpp
extern "C" {
extern void *data_ov006_0213ccfc;
extern void *_ZTV17MgBounceAndPounce;
extern void __destroy_arr(void *, int, int, void *);
void func_ov006_020c6f3c(void);
void func_ov006_020c893c(void);
void func_ov006_020eed64(void);
void _ZN5ModelD1Ev(void *);
void _ZN8Particle10SysTrackerD1Ev(void *);
void _ZN11dScMgBase_cD2Ev(void *c);
void *func_ov006_020eebe8(char *c)
{
    *(void **)c = &data_ov006_0213ccfc;
    _ZN5ModelD1Ev(c + 0x5a14);
    __destroy_arr(c + 0x57d4, 0x10, 0x24, (void *)func_ov006_020eed64);
    __destroy_arr(c + 0x5234, 6, 0xf0, (void *)func_ov006_020c6f3c);
    __destroy_arr(c + 0x500c, 3, 0xb8, (void *)func_ov006_020c893c);
    *(void **)c = &_ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
}
