//cpp
extern "C" {
extern void *data_ov006_0213ccfc;
extern void *data_ov006_0213c62c;
extern void func_0207328c(void *, int, int, void *);
void _ZN6Player18St_LevelEnter_MainEv(void);
void func_ov006_020c893c(void);
void func_ov006_020eed64(void);
void _ZN5ModelD1Ev(void *);
void _ZN8Particle10SysTrackerD1Ev(void *);
void func_ov004_020b29c0(void *c);
void *func_ov006_020eebe8(char *c)
{
    *(void **)c = &data_ov006_0213ccfc;
    _ZN5ModelD1Ev(c + 0x5a14);
    func_0207328c(c + 0x57d4, 0x10, 0x24, (void *)func_ov006_020eed64);
    func_0207328c(c + 0x5234, 6, 0xf0, (void *)_ZN6Player18St_LevelEnter_MainEv);
    func_0207328c(c + 0x500c, 3, 0xb8, (void *)func_ov006_020c893c);
    *(void **)c = &data_ov006_0213c62c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    return c;
}
}
