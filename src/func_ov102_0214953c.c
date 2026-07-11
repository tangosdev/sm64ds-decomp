typedef struct Vector3 { int x, y, z; } Vector3;
extern void func_ov102_02149684(int* dst, void* src);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vector3* pos, void* r, int e, int f);
extern int RandomIntInternal(int* seed);
extern int data_0209f318;
extern int data_0209e650;

void func_ov102_0214953c(char* c, int p1, int p2)
{
    struct { Vector3 pos; int vel[3]; } f;
    int rnd;
    char* o;
    char* g;
    func_ov102_02149684((int*)&f.pos, c);
    o = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x10d, (unsigned int)(p2 | (p1 << 8)), &f.pos, 0, *(signed char*)(c + 0xcc), -1);
    if (o == 0) return;
    g = *(char**)&data_0209f318;
    f.vel[0] = 0;
    f.vel[1] = 0x11000;
    f.vel[2] = 0;
    rnd = RandomIntInternal(&data_0209e650);
    {
        int vsum = rnd + (*(short*)(g + 0x17c) + 0x8000);
        *(short*)(o + 0x92) = 0;
        *(short*)(o + 0x94) = (short)vsum;
        *(short*)(o + 0x96) = 0;
        *(int*)(o + 0x98) = 0x3320;
        *(int*)(o + 0xa4) = f.vel[0];
        *(int*)(o + 0xa8) = f.vel[1];
        *(int*)(o + 0xac) = f.vel[2];
    }
}
