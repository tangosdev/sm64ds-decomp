extern void func_0207328c(void *p, int a, int b, void *fn);
extern void _ZN8Particle10SysTrackerD1Ev(void *p);
extern void func_ov004_020b29c0(void *p);
extern void func_ov006_02120938(void);
extern void func_ov006_02122c68(void);
extern void func_ov006_020eed64(void);
extern void func_ov006_021227c8(void);
extern void func_ov006_020d1008(void);
extern void func_ov006_020ca604(void);
extern void *data_ov006_0213fc7c[];
extern void *data_ov006_0213c62c[];

void *func_ov006_021225ac(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(void **)c = data_ov006_0213fc7c;
    func_0207328c(c + 0x7ad0, 5, 0x24, (void *)func_ov006_02120938);
    func_0207328c(c + 0x7164, 0x14, 0x78, (void *)func_ov006_02122c68);
    func_0207328c(c + 0x6ffc, 0xa, 0x24, (void *)func_ov006_020eed64);
    func_0207328c(c + 0x5ddc, 0xa, 0x1d0, (void *)func_ov006_021227c8);
    func_0207328c(c + 0x5458, 3, 0x32c, (void *)func_ov006_020d1008);
    func_0207328c(c + 0x500c, 5, 0xdc, (void *)func_ov006_020ca604);
    *(void **)c = data_ov006_0213c62c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    return c;
}
