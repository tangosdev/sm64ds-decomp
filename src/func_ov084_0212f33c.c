//cpp
struct Vec3 { int x, y, z; };
extern "C" {
extern void func_02012694(unsigned int a, void *b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, const void *f, void *g);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned int a, unsigned int b, const struct Vec3 *c, const void *d, int e, int f);
void func_ov084_0212f33c(void *self);
}

void func_ov084_0212f33c(void *self)
{
    char *c = (char*)self;
    int r4;
    struct Vec3 v;

    *(unsigned char*)(c + 0x45c) = 0;
    r4 = *(int*)(c + 0x80);
    if (*(unsigned short*)(c + 0x100) == 0) {
        func_02012694(0x11f, c + 0x74);
        r4 = 0x1000;
    }

    *(void**)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x470), 0xfc, *(int*)(c + 0x5c),
        *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    *(void**)(c + 0x474) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x474), 0xfd, *(int*)(c + 0x5c),
        *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);

    if (r4 > 0) {
        r4 = r4 - 0xa3;
    } else {
        r4 = 0;
        if (*(unsigned char*)(c + 0x108) != 0) {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            v = (struct Vec3){*(int*)(c + 0x5c), y + 0x78000, z};
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x122, r4, &v, (void*)r4, *(signed char*)(c + 0xcc), -1);
            *(unsigned char*)(c + 0x108) = r4;
        }
        *(int*)(c + 0x458) = 7;
    }
    *(int*)(c + 0x80) = r4;
    *(int*)(c + 0x84) = r4;
    *(int*)(c + 0x88) = r4;
}
