typedef unsigned int u32;
extern char* PARTICLE_SYS_TRACKER;
void _ZN8Particle19SetSelfDestructFlagEj(u32 idx)
{
    char* a = PARTICLE_SYS_TRACKER;
    a = *(char**)(a + 4);
    a = *(char**)(a + 0x1c);
    u32* o = *(u32**)(a + idx * 0x20);
    *o |= 0x4000;
}
