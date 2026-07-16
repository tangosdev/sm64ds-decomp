typedef struct Vector3 { int x, y, z; } Vector3;

extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int kind, unsigned int b, Vector3 *pos, void *vtab, int e, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int kind, int x, int y, int z);
extern void func_02012694(unsigned int id, const Vector3 *v);

void func_ov060_021135fc(char *c)
{
    volatile Vector3 base;
    int px, py, pz;
    px = *(int*)(c+0x5c);
    base.x = px;
    py = *(int*)(c+0x60);
    base.y = py;
    pz = *(int*)(c+0x64);
    base.z = pz;
    py = py + 0x32000;
    base.y = py;

    *(int*)(int)(((long long)(int)(c+0x378)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;

    if (*(unsigned char*)(c+0x414) == 2) {
        Vector3 pos;
        pos.x = *(int*)(c+0x5c);
        pos.y = *(int*)(c+0x60);
        pos.z = *(int*)(c+0x64);
        pos.y = pos.y + 0xa0000;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11b, 7, &pos, 0, *(signed char*)(c+0xcc), -1);
    } else {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xad, base.x, base.y, base.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xae, base.x, base.y, base.z);
        void *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11a, *(unsigned char*)(c+0x414), (Vector3*)(c+0x5c), 0, *(signed char*)(c+0xcc), -1);
        *(short*)((char*)spawned + 0x440) = *(short*)(c + 0x402);
        func_02012694(0xbb, (Vector3*)(c+0x74));
    }
}
