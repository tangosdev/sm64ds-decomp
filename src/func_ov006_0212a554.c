extern void* data_ov006_02140140;
extern void* data_ov006_0213e448;
extern void func_ov006_0212a650(void);
extern void func_ov006_020c3e70(void* a);
extern void __destroy_arr(void* a, int b, int c, void* d);
extern void _ZN8Particle10SysTrackerD1Ev(void* t);
extern void func_ov004_020b29c0(void* c);

void* func_ov006_0212a554(char* c)
{
    *(void**)c = &data_ov006_02140140;
    func_ov006_020c3e70(c + 0x51f8);
    __destroy_arr(c + 0x4f38, 0x16, 0x20, (void*)func_ov006_0212a650);
    *(void**)c = &data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    return c;
}
