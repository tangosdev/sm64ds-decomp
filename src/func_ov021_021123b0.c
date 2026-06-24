typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };

extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_02012694(int a, void *p);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void _ZN5Actor14TriplePoofDustEv(void *a);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void func_ov021_02112294(void *c);
extern void _ZN9ActorBase18MarkForDestructionEv(void *a);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void *pos);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, struct Vec3 *pos, u32 a, int b, u32 d, u32 e, u32 f);

void func_ov021_021123b0(char *c)
{
    char *a;
    u32 fl;
    u32 id = *(u32*)(c + 0x1dc);

    if (id == 0) return;
    a = (char*)_ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    fl = *(u32*)(c + 0x1d8);
    if ((fl & 0x10) != 0) {
        func_02012694(0x17a, c + 0x74);
        _ZN6Player16IncMegaKillCountEv(a);
        _ZN5Actor14TriplePoofDustEv(c);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
        func_ov021_02112294(c);
        _ZN9ActorBase18MarkForDestructionEv(c);
        return;
    }

    {
        int b = (int)(*(u16*)(a + 0xc) == 0xbf);
        if (b == 0) return;
    }
    if (*(u8*)(a + 0x6fb) != 0) return;
    if ((fl & 0x3c0) != 0) {
        if (*(int*)(a + 8) == 2) {
            func_02012694(0x17a, c + 0x74);
            _ZN9ActorBase18MarkForDestructionEv(c);
            _ZN5Actor14TriplePoofDustEv(c);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
            func_ov021_02112294(c);
            return;
        }
        _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, c + 0x74);
        return;
    }

    if (*(u8*)(c + 0x3be) == 4) return;
    {
        struct Vec3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 3, 0xc000, 1, 0, 1);
    }
}
