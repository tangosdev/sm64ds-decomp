typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;
typedef unsigned char u8;

struct Callback;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* rot, struct Callback* cb);

int func_ov002_020f72bc(char* c)
{
    *(u32*)(c + 0x80) = 0x3000;
    *(u32*)(c + 0x84) = 0x3000;
    *(u32*)(c + 0x88) = 0x3000;
    {
        s16* p = (s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 0x400;
    }
    *(u8*)(c + 0x102) = 0x1f;

    *(u32*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xe8), 0x31, *(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64), 0, 0);
    *(u32*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xec), 0x32, *(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64), 0, 0);
    *(u32*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xf0), 0x33, *(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64), 0, 0);
    return 1;
}
