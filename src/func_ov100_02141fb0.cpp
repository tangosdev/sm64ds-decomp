//cpp
// @symbol func_ov100_02141fb0
/* recovered: shared common types, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;




struct Obj {
    virtual u32 v00(); virtual u32 v01(); virtual u32 v02(); virtual u32 v03();
    virtual u32 v04(); virtual u32 v05(); virtual u32 v06(); virtual u32 v07();
    virtual u32 v08(); virtual u32 v09(); virtual u32 v10(); virtual u32 v11();
    virtual u32 v12(); virtual u32 v13(); virtual u32 v14(); virtual u32 v15();
    virtual u32 v16(); virtual u32 v17(); virtual u32 v18(); virtual u32 v19();
    virtual u32 v20(); virtual u32 v21(); virtual u32 v22(); virtual u32 v23();
    virtual u32 v24(); virtual u32 v25(); virtual u32 v26(); virtual u32 v27();
    virtual u32 v28();
    virtual u32 m();
};

extern "C" void *_ZN5Actor10FindWithIDEj(u32 id);
extern "C" void func_020ada40(char *c, Vector3_16 *s, void *a, int z);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern "C" void _ZN5Actor8PoofDustEv(void *a);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void *a);
extern "C" void func_02012694(int a, void *p);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void *pos);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, Vector3 *pos, u32 a, int b, u32 d, u32 e, u32 f);

extern "C" void func_ov100_02141fb0(Obj *thiz)
{
    char *c = (char *)thiz;
    char *a;
    u32 fl;
    u32 id = *(u32*)(c + 0x398);

    if (id == 0) return;
    a = (char*)_ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    {
        int b = (*(u16*)(a + 0xc) == 0xbf);
        if (b == 0) return;
    }
    if (*(u8*)(a + 0x6fb) != 0) return;
    fl = *(u32*)(c + 0x394);
    if ((fl & 0x10) != 0) {
        Vector3_16 s;
        u32 r = thiz->m();
        *(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) += r;
        s.x = 0; s.y = 0; s.z = 0;
        func_020ada40(c, &s, a, 0);
        _ZN12WithMeshClsn15ClearGroundFlagEv(c + 0x110);
        return;
    }
    if ((fl & 0x3c0) != 0) {
        if (*(int*)(a + 8) == 2) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0xf, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            func_02012694(0x173, c + 0x74);
            return;
        }
        _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, c + 0x74);
        return;
    }
    {
        Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 2, 0xc000, 1, 0, 1);
    }
}
