//cpp
struct Vector3 { int x, y, z; };
extern "C" {
void func_0201267c(int id, char* p);
void func_ov062_02117994(char* c, int idx);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const Vector3& pos, const void* d, int e, int f);
}

extern "C" void func_ov062_02117bf4(char* c){
    func_0201267c(0xed, c+0x74);
    *(int*)(c+0x390) = 1;
    *(int*)(c+0x38c) = 3;
    *(int*)(c+0x98) = 0x14000;
    func_ov062_02117994(c, 7);
    {
        Vector3 v;
        int yy = *(int*)(c+0x64);
        int zz = *(int*)(c+0x60) + 0x3c000;
        v.x = *(int*)(c+0x5c);
        v.y = zz;
        v.z = yy;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11d, *(unsigned int*)(c+0x394), v, 0, *(signed char*)(c+0xcc), -1);
    }
    *(short*)(c+0x3ca) = 0xa;
    func_0201267c(1, c+0x74);
}
