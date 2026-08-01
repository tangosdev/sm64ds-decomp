#include "types.h"
struct Actor;

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player16IncMegaKillCountEv(struct Actor* p);
extern void func_02012694(u32 id, void* pos);
extern void func_020105cc(struct Actor* thiz, u32 flags);
extern void func_ov084_0212ebb4(void* c);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(struct Actor* self, void* clsn, struct Actor* player);
extern void _ZN6Player6BounceE5Fix12IiE(struct Actor* p, int fix);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(struct Actor* self, const void* pos, u32 a, int fix, u32 b, u32 c, u32 d);
extern int data_ov084_02130e0c[];
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* anim, int a, int fix, u32 flags);
extern void _ZN9Animation8SetFlagsEi(void* thiz, int flags);

int func_ov084_0212ef00(char* self)
{
    struct Actor* actor;
    u32 flags;
    u32 raw;
    u16 type;
    u32 id;

    id = *(u32*)(self + 0x3a4);
    if (id == 0) goto second;

    actor = _ZN5Actor10FindWithIDEj(id);
    if (actor == 0) goto second;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkA2;
    }

    raw = *(u32*)(self + 0x3a0);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpA;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc((struct Actor*)self, flags);
    func_ov084_0212ebb4(self);
    goto successA;

jumpA:
    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player((struct Actor*)self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(self);
        goto successA;
    }

    if (*(u32*)(self + 0x3a0) & 0x40000) {
        func_ov084_0212ebb4(self);
        goto successA;
    }

    {
        int v[3];
        v[0] = *(int*)(self + 0x5c);
        v[1] = *(int*)(self + 0x60);
        v[2] = *(int*)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, v, 3, 0xc000, 1, 0, 1);
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, (void*)data_ov084_02130e0c[1], 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x160, 0x40000000);
    *(int*)(self + 0x458) = 3;
    func_02012694(0x175, self + 0x74);
    return 0;

successA:
    return 1;

checkA2:
    if (!(*(u32*)(self + 0x3a0) & 0x2000)) goto second;
    {
        int u = (int)(type == 9);
        if (u == 0) goto second;
    }
    func_ov084_0212ebb4(self);
    return 1;

second:
    id = *(u32*)(self + 0x3d8);
    if (id == 0) goto fail;

    actor = _ZN5Actor10FindWithIDEj(id);
    if (actor == 0) goto fail;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xbf);
        if (t == 0) goto checkB2;
    }

    raw = *(u32*)(self + 0x3d4);
    flags = raw & 0x26ff0;
    if (flags == 0) goto jumpB;

    if (flags & 0x10) {
        _ZN6Player16IncMegaKillCountEv(actor);
        func_02012694(0x1d, self + 0x74);
    }
    func_020105cc((struct Actor*)self, flags);
    func_ov084_0212ebb4(self);
    return 1;

jumpB:
    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player((struct Actor*)self, self + 0x380, actor) != 0) {
        _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
        func_ov084_0212ebb4(self);
        return 1;
    }

    if (*(u32*)(self + 0x3d4) & 0x40000) {
        func_ov084_0212ebb4(self);
        return 1;
    }

    goto fail;

checkB2:
    if (!(*(u32*)(self + 0x3d4) & 0x2000)) goto fail;
    {
        int u = (int)(type == 9);
        if (u == 0) goto fail;
    }
    func_ov084_0212ebb4(self);
    return 1;

fail:
    return 0;
}
