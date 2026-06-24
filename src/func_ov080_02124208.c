extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* self, void* clsn, void* player);
extern void _ZN6Player6BounceE5Fix12IiE(void* player, int v);
extern void _ZN6Player16IncMegaKillCountEv(void* player);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* player, void* pos, unsigned int a, int b, unsigned int d, unsigned int e, unsigned int f);
extern void func_ov080_02124088(void* self);
extern void func_02012694(int a, void* p);

struct Vec3 { int x, y, z; };

void func_ov080_02124208(char* c)
{
    char* p;
    unsigned int id = *(unsigned int*)(c + 0x15c);

    if (id == 0)
        return;

    p = (char*)_ZN5Actor10FindWithIDEj(id);
    if (p == 0)
        return;

    if ((*(int*)(c + 0x158) & 0x66fe0) != 0) {
        func_ov080_02124088(c);
        return;
    }

    if ((*(int*)(c + 0x158) & 0x400000) == 0)
        return;

    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x138, p)) {
        func_ov080_02124088(c);
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        return;
    }

    if (*(unsigned char*)(p + 0x6fb) != 0)
        return;

    if (*(unsigned char*)(p + 0x6f9) != 0) {
        func_ov080_02124088(c);
        return;
    }

    if ((*(int*)(c + 0x158) & 0x10) != 0) {
        func_02012694(0x1d, c + 0x74);
        _ZN6Player16IncMegaKillCountEv(p);
        func_ov080_02124088(c);
        return;
    }

    {
        struct Vec3 pos;
        pos.x = *(int*)(c + 0x5c);
        pos.y = *(int*)(c + 0x60);
        pos.z = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &pos, 2, 0xc000, 1, 0, 1);
    }
}
