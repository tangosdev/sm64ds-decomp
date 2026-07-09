extern void func_ov002_020f6514(unsigned char* self, void* tbl, unsigned char arg);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int p2, int p3, int p4, void* p5, void* p6);
extern unsigned short data_0209b274;
extern int data_ov002_0210bc88;

struct Vector3 { int x, y, z; };

int func_ov002_020f7038(char* c, int a, int arg)
{
    if (arg == data_0209b274) {
        *(int*)(c + 0xe8) = 0;
        *(int*)(c + 0xec) = 0;
        *(int*)(c + 0xf0) = 0;
    }
    if (*(int*)(c + 0x60) >= 0x514000) {
        if (*(int*)(c + 0xf8) < 0x3c000) {
            *(int*)(((long long)(int)(c + 0xf8)) & 0xFFFFFFFFFFFFFFFFLL) += 0x5000;
        }
    } else {
        if (*(int*)(c + 0xf8) > 0x1a000) {
            *(int*)(((long long)(int)(c + 0xf8)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x2000;
        }
    }
    {
        void* obj = *(void**)(c + 0xe0);
        if (*(unsigned char*)((char*)obj + 0x82) == 0 && (*(unsigned char*)(c + 0x103) & 1)) {
            func_ov002_020f6514((unsigned char*)obj, &data_ov002_0210bc88, 1);
        }
    }
    *(int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) -=
        (int)(((long long)*(int*)(c + 0xf8) * 0x199 + 0x800) >> 12);
    {
        struct Vector3 pos;
        int z = *(int*)(c + 0x64);
        int x = *(int*)(c + 0x5c);
        int y = *(int*)(c + 0x60) + 0x50000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(int*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xe8), 0x3b, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xec), 0x3c, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xf0), 0x3d, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
    }
    return 1;
}
