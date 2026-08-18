#include "types.h"
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern int _ZN6Player15IsCollectingCapEv(void* p);
extern void _ZN6Player16InitWingFeathersEb(void* p, int b);
extern void _ZN6Player16InitBalloonMarioEv(void* p);
extern void _ZN7fBase_c18MarkForDestructionEv(void* self);
extern void _ZN6Player14InitMetalWarioEv(void* p);
extern void _ZN6Player15InitVanishLuigiEv(void* p);
extern void _ZN6Player13InitFireYoshiEv(void* p);

void func_ov002_020b979c(char* self) {
    char* other;
    u32 id = *(u32*)(self + 0x1f0);
    if (id == 0) return;

    other = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (other == 0) return;

    {
        int b = (int)(*(u16*)(other + 0xc) == 0xbf);
        if (b == 0) return;
    }

    if (_ZN6Player15IsCollectingCapEv(other) != 0) return;

    {
        char* p = *(char**)(other + 0x358);
        int t = (int)(p != 0);
        if (t != 0) {
            int b = (int)(*(u16*)(p + 0xc) == 0x10b);
            if (b != 0) return;
        }
    }

    {
        u32 flags = *(u32*)(self + 0xb0);
        int t = (int)((flags & 0x20000) != 0);
        if (t != 0) {
            *(u8*)(self + 0x3ca) = 0x64;
            return;
        }
    }

    switch (*(int*)(other + 8)) {
    case 0:
        if (*(int*)(self + 8) == 1) {
            _ZN6Player16InitWingFeathersEb(other, 1);
        } else {
            _ZN6Player16InitBalloonMarioEv(other);
        }
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    case 2:
        _ZN6Player14InitMetalWarioEv(other);
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    case 1:
        _ZN6Player15InitVanishLuigiEv(other);
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    case 3:
        if (*(char**)(other + 0x360) != 0) {
            char* p = *(char**)(other + 0x360);
            if (*(u16*)(p + 0xc) == 0x10c) return;
        }
        _ZN6Player13InitFireYoshiEv(other);
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    }
}
