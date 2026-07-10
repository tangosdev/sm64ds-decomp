//cpp
extern "C" {
extern void *data_ov006_0213cbe4;
extern void *_ZTV17MgBounceAndPounce;
extern void func_0207328c(void *, int, int, void *);
void _ZN6Player18St_LevelEnter_MainEv(void);
void func_ov006_020c893c(void);
void _ZN5ModelD1Ev(void *);
void _ZN8Particle10SysTrackerD1Ev(void *);
void func_ov004_020b29c0(void *c);
void *func_ov006_020edec0(char *c)
{
    *(void **)c = &data_ov006_0213cbe4;
    func_0207328c(c + 0x5294, 6, 0xf0, (void *)_ZN6Player18St_LevelEnter_MainEv);
    func_0207328c(c + 0x506c, 3, 0xb8, (void *)func_ov006_020c893c);
    _ZN5ModelD1Ev(c + 0x501c);
    *(void **)c = &_ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    return c;
}
}
