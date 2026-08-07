extern void* data_ov006_0213fec8;
extern void* data_ov006_0213e448;
extern void NullDestructor_0203d47c(void);
extern void __destroy_arr(void* a, int b, int c, void* d);
extern int func_ov006_020c1c64(char* t);
extern void _ZN8Particle10SysTrackerD1Ev(void* t);
extern void _ZN11dScMgBase_cD2Ev(void* c);

void* func_ov006_02124908(char* c)
{
    *(void**)c = &data_ov006_0213fec8;
    __destroy_arr(c + 0x51a8, 2, 8, (void*)NullDestructor_0203d47c);
    func_ov006_020c1c64(c + 0x4f38);
    *(void**)c = &data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    return c;
}
