//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };
struct Vec3_16f { s16 x, y, z; };

extern "C" {
    void* _ZN5Actor7FindEggER12CylinderClsn(void* self, void* c);
    void* _ZN5Actor18FindExplosionActorER12CylinderClsn(void* self, void* c);
    void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* v);
    void func_ov071_02121634(void* self, int a);
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void* v, void* cb);
    void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const void* v);
    void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const void* v, u32 a, int b, u32 c, u32 d, u32 e);
}

extern "C" void func_ov071_02120b14(void* self)
{
    u8* c = (u8*)self;

    void* egg = _ZN5Actor7FindEggER12CylinderClsn(self, (void*)(c+0x174));
    if (egg != 0) {
        int isEgg9 = (int)(*(u16*)((u8*)egg+0xc) == 9);
        if (isEgg9) goto playSound;
    }

    if (_ZN5Actor18FindExplosionActorER12CylinderClsn(self, (void*)(c+0x174)) == 0) goto idCheck;

playSound:
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void*)(c+0x74));
    func_ov071_02121634(self, 2);
    return;

idCheck:

    if (*(u32*)(c+0x198) == 0) return;

    u8* f = (u8*)_ZN5Actor10FindWithIDEj(*(u32*)(c+0x198));
    if (f == 0) return;

    int isbf = (int)(*(u16*)(f+0xc) == 0xbf);
    if (!isbf) return;

    if (*(s32*)(c+0x194) & 0x40000) {
        void* p1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(u32*)(c+0x204), 0x13a, *(s32*)(c+0x5c), *(s32*)(c+0x60), *(s32*)(c+0x64), 0, 0);
        *(void**)(c+0x204) = p1;

        void* p2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(*(u32*)(c+0x208), 0x13b, *(s32*)(c+0x5c), *(s32*)(c+0x60), *(s32*)(c+0x64), 0);
        *(void**)(c+0x208) = p2;

        func_ov071_02121634(self, 2);
        return;
    }

    struct Vec3 hv;
    hv.x = *(s32*)(c+0x5c);
    hv.y = *(s32*)(c+0x60);
    hv.z = *(s32*)(c+0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f, &hv, 2, 0xc000, 1, 0, 1);
}
