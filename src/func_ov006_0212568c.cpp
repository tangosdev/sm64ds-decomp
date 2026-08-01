//cpp
extern "C" {
extern void *data_ov006_0214000c;
extern void *data_ov006_0213e448;
extern void __destroy_arr(void *, int, int, void *);
void func_ov006_02125800(void);
void NullDestructor_0203d47c(void);
void _ZN5ModelD1Ev(void *);
void _ZN8Particle10SysTrackerD1Ev(void *);
void func_ov004_020b29c0(void *c);
void *func_ov006_0212568c(char *c)
{
    *(void **)c = &data_ov006_0214000c;
    __destroy_arr(c + 0xba14, 0x20, 0x24, (void *)func_ov006_02125800);
    __destroy_arr(c + 0xb5d8, 0x80, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(c + 0xacd8, 0x80, 8, (void *)NullDestructor_0203d47c);
    _ZN5ModelD1Ev(c + 0xaba4);
    *(void **)c = &data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    return c;
}
}
