//cpp
// @symbol _ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3
/* recovered: named members + shared header */
#include "Particle__RadiusConverge.h"
extern "C" {
void _ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3(struct Particle__RadiusConverge *self, char* p, int* vec) {
  *(int*)(p+0x14) += (int)((((long long)self->unk_00c * (self->unk_000 - *(int*)(p+0x14))) + 0x800) >> 12);
  *(int*)(p+0x18) += (int)((((long long)self->unk_00c * (self->unk_004 - *(int*)(p+0x18))) + 0x800) >> 12);
  *(int*)(p+0x1c) += (int)((((long long)self->unk_00c * (self->unk_008 - *(int*)(p+0x1c))) + 0x800) >> 12);
}
}
