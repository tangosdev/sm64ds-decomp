//cpp
#include "types.h"
// @symbol func_ov098_02138e6c
/* recovered: shared common types */
#include "common.h"
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void m();
};

extern "C" {
void Crate_SetState(void *c, int i);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
int _ZN6Player7TryGrabER8dActor_c(void *f, void *a);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, struct Vector3 *pos, u32 a, int b, u32 d, u32 e, u32 f);
}

extern "C" void func_ov098_02138e6c(char *c)
{
    void *a;
    u32 fl;
    int b;

    if (*(u8*)(c + 0x606) != 0) return;

    fl = *(u32*)(c + 0xb0);
    {
        int b1 = (int)((fl & 0x20000) != 0);
        if (b1 != 0) {
            Crate_SetState(c, 5);
        } else {
            int b2 = (int)((fl & 0x40000) != 0);
            if (b2 != 0) {
                Crate_SetState(c, 5);
            }
        }
    }

    if (*(u32*)(c + 0x588) == 0) return;

    if ((*(u32*)(c + 0x584) & 0x40000) != 0) {
        u32 *pp = (u32 *)(((int)c + 0x57c));
        *(u8*)(c + 0x606) = 0x3c;
        *pp = *pp & ~0x8000u;
    }
    if ((*(u32*)(c + 0x584) & 0x4000) != 0) {
        Base *o = (Base*)c;
        o->m();
    }

    a = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x588));
    if (a == 0) return;
    {
        int bf = (int)(*(u16*)((char*)a + 0xc) == 0xbf);
        if (bf == 0) return;
    }

    fl = *(u32*)(c + 0x584);
    if ((fl & 0x380) != 0) {
        Base *o = (Base*)c;
        o->m();
        return;
    }
    if ((fl & 0x10) != 0) {
        Base *o = (Base*)c;
        o->m();
        return;
    }
    if ((fl & 0x1000) != 0) {
        if (_ZN6Player7TryGrabER8dActor_c(a, c) == 0) return;
        *(void**)(c + 0x5e4) = a;
        Crate_SetState(c, 1);
        return;
    }

    if (*(u32*)(c + 0x560) != 2) return;
    b = 1;
    if (*(void**)(c + 0x5e8) == a) b = 0;
    if (b == 0) return;

    {
        struct Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 1, 0xc000, 1, 0, 1);
    }
}
