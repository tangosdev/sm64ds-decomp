typedef unsigned int u32;
typedef int s32;
typedef int Fix12i;

extern u32 func_02022c80(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void* dir);
extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* v);
extern char* data_0209f32c;

void func_ov002_020ce9c8(char* self)
{
    int v[3];
    int effectID;

    *(volatile int*)&v[0] = *(int*)(self + 0x5c);
    *(volatile int*)&v[1] = *(int*)(self + 0x60);
    *(volatile int*)&v[2] = *(int*)(self + 0x64);
    v[1] = (int)(data_0209f32c + 0x4b000);
    effectID = 0xce;
    if (*(unsigned char*)(self + 0x703))
        effectID = 0x54;
    func_02022c80(0, effectID, v[0], v[1], v[2], 0);

    v[1] = (int)(data_0209f32c + 0x3000);
    effectID = 0xcf;
    if (*(unsigned char*)(self + 0x703))
        effectID = 0x55;
    func_02022cbc(0, effectID, v[0], v[1], v[2], 0);

    if (*(unsigned char*)(self + 0x703))
        *(int*)(self + 0x628) = func_02022d00(*(int*)(self + 0x628), 0x56, *(int*)(self + 0x5c), (int)data_0209f32c, *(int*)(self + 0x64), 0);
    else
        _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(*(int*)(self + 0x5c), (int)data_0209f32c, *(int*)(self + 0x64));

    if (*(unsigned char*)(self + 0x703))
        _ZN5Sound9PlayBank0EjRK7Vector3(0xd4, self + 0x74);
    else
        _ZN5Sound9PlayBank0EjRK7Vector3(0x16, self + 0x74);
}
