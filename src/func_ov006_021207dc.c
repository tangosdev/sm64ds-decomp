extern void func_0207328c(void *p, int a, int b, void *fn);
extern void _ZN8Particle10SysTrackerD1Ev(void *p);
extern void func_ov004_020b29c0(void *p);
extern void func_ov006_02120938(void);
extern void func_ov006_020d1008(void);
extern void func_ov006_020ccfc8(void);
extern void *data_ov006_0213fb34[];
extern void *_ZTV17MgBounceAndPounce[];

void *func_ov006_021207dc(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(void **)c = data_ov006_0213fb34;
    func_0207328c(c + 0x5cd0, 5, 0x24, (void *)func_ov006_02120938);
    func_0207328c(c + 0x534c, 3, 0x32c, (void *)func_ov006_020d1008);
    func_0207328c(c + 0x500c, 4, 0xd0, (void *)func_ov006_020ccfc8);
    *(void **)c = _ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    return c;
}
