#include "types.h"
struct System;
struct ScaleCallback {
    u32 vtable;     /* 0x00 */
    u16 unk04;      /* 0x04 */
    s16 unk06;      /* 0x06 */
    s16 unk08;      /* 0x08 */
    u8  unk0a;      /* 0x0a */
};

typedef struct ScaleCallback ScaleCallback;

/* System layout with fields at 0x4c and 0x50 */
struct ScaleSystem {
    void* node_prev;    /* 0x00 */
    void* node_next;    /* 0x04 */
    void* list1_first;  /* 0x08 */
    u32   list1_num;    /* 0x0c */
    void* list2_first;  /* 0x10 */
    u32   list2_num;    /* 0x14 */
    void* ptr18;        /* 0x18 */
    u32   pad1c[0xc];   /* 0x1c..0x48 */
    u32   unk4c;        /* 0x4c */
    u32   unk50;        /* 0x50 */
};

extern void _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(ScaleCallback* cb, struct ScaleSystem* sys);

void _ZN8Particle13ScaleCallback14SpawnParticlesERNS_6SystemE(ScaleCallback* self, struct ScaleSystem* sys)
{
    sys->unk50 = self->unk06;
    sys->unk4c = self->unk08;
    _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(self, sys);
    self->unk0a = 0x10;
}
