extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int p2, int p3, int p4, void* p5, void* p6);
extern unsigned short data_0209b274;

struct Vector3 { int x, y, z; };

int func_ov002_020f71c4(char* c, int a, int arg)
{
    if (arg == 0) {
        if (arg == data_0209b274) {
            *(int*)(c + 0xf4) = 0x10000;
        }
        _Z14ApproachLinearRiii((int*)(c + 0xf4), 0x1f000, 0x3e3);
        *(unsigned char*)(c + 0x102) = *(int*)(c + 0xf4) >> 12;
    }
    {
        struct Vector3 pos;
        int z = *(int*)(c + 0x64);
        int x = *(int*)(c + 0x5c);
        int y = *(int*)(c + 0x60) + 0x50000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(int*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xe8), 0x38, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xec), 0x39, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xf0), 0x3a, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
    }
    return 1;
}
