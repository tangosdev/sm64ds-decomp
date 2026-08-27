#include "types.h"
// @symbol _ZN8Particle25EndingStarGlitterCallback14SpawnParticlesERNS_6SystemE
/* recovered: named members + shared header */
#include "Particle__EndingStarGlitterCallback.h"
struct System;

/* EndingStarGlitterCallback fields at large offsets (around 0x300) */
struct EndingStarGlitterCallback {
    u32 vtable;
    /* ... */
    /* 0x308: a u32/ptr field */
    /* 0x30c: a s16 field */
};

/* System has a field at 0x18 (pointer) and 0x48 (s32 write of s16 value) */
struct StarGlitterSystem {
    void* node_prev;    /* 0x00 */
    void* node_next;    /* 0x04 */
    void* list1_first;  /* 0x08 */
    u32   list1_num;    /* 0x0c */
    void* list2_first;  /* 0x10 */
    u32   list2_num;    /* 0x14 */
    void* ptr18;        /* 0x18 */
    u32   pad1c[0xb];   /* 0x1c..0x44 */
    s32   unk48;        /* 0x48 */
};

extern void _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(void* cb, struct StarGlitterSystem* sys);

void _ZN8Particle25EndingStarGlitterCallback14SpawnParticlesERNS_6SystemE(struct Particle__EndingStarGlitterCallback *self, struct StarGlitterSystem* sys) {
    void** p;
    u32 ip;
    s16 val;

    p  = (void**)sys->ptr18;
    ip = *(u32*)((char*)&self->unk_308);
    ((u32*)*p)[1] = ip;

    val = *(s16*)((char*)&self->unk_30c);
    sys->unk48 = val;

    _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(((void*)self), sys);
}
