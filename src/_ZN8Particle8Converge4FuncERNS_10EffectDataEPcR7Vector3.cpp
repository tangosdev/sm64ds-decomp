//cpp
// @symbol _ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3
/* recovered: named members + shared header */
#include "Particle__Converge.h"
extern "C" {
void _ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3(struct Particle__Converge *self, char* p, int* vec) {
  vec[0] += (int)self->unk_00c * ((self->unk_000 - *(int*)(p+0x14)) - *(int*)(p+0x20)) >> 12;
  *(int*)(((long long)(int)(vec+1))) += (int)self->unk_00c * ((self->unk_004 - *(int*)(p+0x18)) - *(int*)(p+0x24)) >> 12;
  *(int*)(((long long)(int)(vec+2))) += (int)self->unk_00c * ((self->unk_008 - *(int*)(p+0x1c)) - *(int*)(p+0x28)) >> 12;
}
}
