//cpp
// @symbol _ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3
/* recovered: named members + shared header */
#include "Particle__Acceleration.h"
extern "C" {
void _ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3(struct Particle__Acceleration *self, char* p, int* vec) {
  vec[0] += (int)self->unk_000;
  *(int*)(((long long)(int)(vec+1))) += (int)self->unk_002;
  *(int*)(((long long)(int)(vec+2))) += (int)self->unk_004;
}
}
