#include "types.h"
// @symbol _ZN8Particle7Manager9AddSystemEiR7Vector3
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle__Manager.h"
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))


int _ZN8Particle7Manager9AddSystemEiR7Vector3(struct Particle__Manager *self, int idx, void* v) {
    int r4;
    char* sys;
    u32* p;
    u32 lo;
    r4 = 0;
    if (self->unk_00c != 0) {
        sys = (char*)func_0204d958((char*)&self->unk_00c);
        r4 = (int)sys;
        func_0204ae2c(sys, *(char**)((char*)&self->unk_01c) + idx * 0x20, v);
        p = (u32*)AT(sys, 0x74);
        *p = (*p & ~0x3fu) | (self->unk_02c & 0x3f);
        *p = (*p & ~0xfc0u) | ((self->unk_02e & 0x3f) << 6);
        lo = (u32)(*(u32*)(sys + 0x74) << 26);
        lo = lo >> 26;
        *p = (*p & ~0x3f000u) | (((u32)(lo) & 0x3f) << 12);
        *p = *p & 0x3ffff;
        func_0204d9a0(((char*)self) + 4, sys);
        if ((***(u32***)(sys + 0x18) << 17) >> 31) r4 = 0;
    }
    return r4;
}
