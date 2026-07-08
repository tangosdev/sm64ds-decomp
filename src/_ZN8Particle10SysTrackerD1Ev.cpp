//cpp
extern "C" void func_02021b98(void *base);
extern "C" void func_0203cbc0(void *p);
extern "C" void _ZN6Memory10DeallocateEPv(void *p);
extern void *data_0209ee80;
extern char PARTICLE_ROM_EMBEDDED_FILE[];
extern void *PARTICLE_SYS_TRACKER;

extern "C" void *_ZN8Particle10SysTrackerD1Ev(char *thiz)
{
    if (data_0209ee80 != 0) {
        func_02021b98(thiz + 8);
        func_0203cbc0(data_0209ee80);
        data_0209ee80 = 0;
    }
    if (*(void **)thiz != (void *)PARTICLE_ROM_EMBEDDED_FILE) {
        _ZN6Memory10DeallocateEPv(*(void **)thiz);
    }
    PARTICLE_SYS_TRACKER = 0;
    return thiz;
}
