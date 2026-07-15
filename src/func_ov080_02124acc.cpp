//cpp
struct Vector3 { int x, y, z; };
extern "C" {
void* func_02010304(void* a, void* b);
void func_ov080_0212513c(void* c, int n);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN6Player16IncMegaKillCountEv(void* p);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const Vector3& v, unsigned int n, int vel, short unk);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
}
extern "C" void func_ov080_02124acc(char* c);
void func_ov080_02124acc(char* c)
{
    if (*(int*)(c + 0x170) == 0) return;
    void* p = func_02010304(c, c + 0x14c);
    if (p != 0) { *(void**)(c + 0x374) = p; func_ov080_0212513c(c, 1); return; }
    void* a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x170));
    if (a == 0) return;
    int b1 = (int)(*(unsigned short*)((char*)a + 0xc) == 0xbf);
    if (b1 == 0) return;
    int b2 = (int)((*(unsigned int*)(c + 0xb0) & 0x20000) != 0);
    if (b2) { func_ov080_0212513c(c, 2); return; }
    if ((*(unsigned int*)(c + 0x16c) & 0x10) == 0) return;
    _ZN6Player16IncMegaKillCountEv(a);
    Vector3 v; Vector3 v2; Vector3 v3;
    int y0 = *(int*)(c + 0x60);
    int z = *(int*)(c + 0x64);
    int x = *(int*)(c + 0x5c);
    int y = y0 + 0x32000;
    v.x = x; v.y = y; v.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, v.x, v.y, v.z);
    ((int*)&v2)[0] = ((int*)&v)[0];
    ((int*)&v2)[1] = ((int*)&v)[1];
    ((int*)&v2)[2] = ((int*)&v)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(c, v2);
    int t = *(int*)(c + 0x60) + 0x64000;
    v3.x = v.x;
    v.y = t;
    v3.y = t;
    v3.z = v.z;
    _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v3, 5, 0xf000, 0);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
    _ZN9ActorBase18MarkForDestructionEv(c);
}
