//cpp
struct Vec3 { int x, y, z; };
struct Vec3_16 {};
extern "C" {
extern void _ZN9Animation7AdvanceEv(char*);
extern char* _ZN5Actor13ClosestPlayerEv(char*);
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(char*, Vec3*, unsigned int, int, unsigned short, char*);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, Vec3*, Vec3_16*, int, int);
extern int _ZN9Animation8FinishedEv(char*);
extern void func_ov064_0211a6ec(char*, int);

void func_ov064_0211a39c(char* c)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (((unsigned int)(*(int*)(c + 0x12c) << 4) >> 0x10) == 0x14)
    {
        char* p = _ZN5Actor13ClosestPlayerEv(c);

        Vec3 v1;
        v1.x = *(int*)(c + 0x5c);
        v1.y = *(int*)(c + 0x60);
        v1.z = *(int*)(c + 0x64);
        v1.y = v1.y + 0xc8000;

        if (*(unsigned char*)(c + 0x173) == 0)
        {
            Vec3 v2;
            v2.x = v1.x;
            v2.y = v1.y;
            v2.z = v1.z;
            _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, &v2, *(unsigned char*)(c + 0x172), 0, 0, 0);
        }
        if (*(unsigned char*)(p + 0x706) != 0)
        {
            char* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x123, 0, &v1, 0, *(signed char*)(c + 0xcc), -1);
            *(int*)(a + 0xa4) = 0;
            *(int*)(a + 0xa8) = 0x800;
            *(int*)(a + 0xac) = 0;
        }
    }
    if (_ZN9Animation8FinishedEv(c + 0x124) != 0)
    {
        func_ov064_0211a6ec(c, 2);
    }
}
}
