//cpp
extern "C" {
extern void *data_ov006_0213c154;
extern void *data_ov006_0213e448;
extern void __destroy_arr(void *, int, int, void *);
int func_ov006_020c3288(char *t);
void NullDestructor_0203d47c(void);
void func_ov006_020deac4(void);
void _ZN8Particle10SysTrackerD1Ev(void *);
void func_ov004_020b29c0(void *c);
void *func_ov006_020de988(char *c)
{
    *(void **)c = &data_ov006_0213c154;
    __destroy_arr(c + 0x53e8, 3, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(c + 0x50e8, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288(c + 0x4f38);
    *(void **)c = &data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    return c;
}
}
