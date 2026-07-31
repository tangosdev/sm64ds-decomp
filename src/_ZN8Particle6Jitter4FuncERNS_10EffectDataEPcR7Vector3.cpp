//cpp
// @symbol _ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle__Jitter.h"
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
extern "C" void _ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3(struct Particle__Jitter *self, char* p, int* v) {
    u32 s;
    int r;
    int amp;
    if ((int)*(u16*)(p + 0x2e) % (int)self->unk_006 != 0)
        return;
    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = self->unk_000;
    r = s >> 23;
    v[0] += (amp * r - (amp << 8)) >> 8;
    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = self->unk_002;
    r = s >> 23;
    *(int*)(((long long)(int)(v + 1))) += (amp * r - (amp << 8)) >> 8;
    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = self->unk_004;
    r = s >> 23;
    *(int*)(((long long)(int)(v + 2))) += (amp * r - (amp << 8)) >> 8;
}
