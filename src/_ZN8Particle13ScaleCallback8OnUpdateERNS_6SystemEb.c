#include "types.h"
struct System;

struct ScaleCallback {
    u32 vtable;
    u16 unk04;
    u16 unk06;
    u16 unk08;
    u8 unk0a;
};

typedef struct ScaleCallback ScaleCallback;

extern int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(ScaleCallback* self, struct System* sys, int active);
extern u8 DecIfAbove0_Byte(u8* p);

int _ZN8Particle13ScaleCallback8OnUpdateERNS_6SystemEb(ScaleCallback* self, struct System* sys, int active) {
    _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(self, sys, active);
    return DecIfAbove0_Byte(&self->unk0a) != 0;
}
