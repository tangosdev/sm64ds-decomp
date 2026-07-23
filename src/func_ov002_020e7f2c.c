typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, s32 x, s32 y, s32 z, void* rot, void* cb);

void func_ov002_020e7f2c(char* c)
{
    volatile s32 x, y, zvar;
    s32 z, yraw;

    if (*(u16 *)((char*)c + 0x400 + 0x94) == 0)
        return;
    (*(u16 *)(((long long)(int)(c + 0x494)) & 0xFFFFFFFFFFFFFFFFLL))--;
    if (*(u16 *)((char*)c + 0x400 + 0x94) == 0)
        *(u32 *)(c + 0x4bc) = 0;
    x = *(s32 *)(c + 0x5c);
    yraw = *(s32 *)(c + 0x60);
    y = yraw;
    {
        s32 zraw = *(s32 *)(c + 0x64);
        s32 yadj = yraw + 0xd000;
        z = zraw;
        zvar = zraw;
        y = yadj;
    }
    *(u32 *)(c + 0x4bc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile u32 *)(c + 0x4bc), 0x114, x, y, z, 0, 0);
}
