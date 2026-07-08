//cpp
struct Vec3 { int x, y, z; };
struct Vec3_16 { short x, y, z; };
extern "C" {
extern unsigned int RandomIntInternal(void* g);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const Vec3* pos, const Vec3_16* rot, int e, int f);
extern void func_ov009_02111224(void* c, int r1);
extern void* RNG_STATE;
}

extern "C" void func_ov009_021116ec(char* c)
{
    if (*(unsigned char*)(c + 0x180)) {
        int n = *(int*)(c + 8) & 0xf;
        if (n > 1) {
            Vec3_16 rot = *(Vec3_16*)(c + 0x92);
            for (int i = 0; i < n - 1; i++) {
                Vec3 pos;
                pos.x = *(int*)(c + 0x5c) + (int)((RandomIntInternal(&RNG_STATE) % 400) - 0xa0) * 4096;
                pos.y = *(int*)(c + 0x60);
                pos.z = *(int*)(c + 0x64) + (int)((RandomIntInternal(&RNG_STATE) % 400) - 0xa0) * 4096;
                rot.y = (short)(rot.y + (short)(RandomIntInternal(&RNG_STATE) >> 16));
                void* a = (void*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x157, 0, &pos, &rot, (int)*(signed char*)(c + 0xcc), -1);
                if (a) func_ov009_02111224(a, *(int*)(c + 4));
            }
        }
        *(int*)(c + 0x160) = -0x14000;
        *(int*)(c + 0x168) = 0xff06a000;
    }
    *(int*)(c + 0x17c) = 1;
}
