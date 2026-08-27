// @symbol _ZN8Particle7Texture12AllocPalVramEjb
/* recovered: named members + shared header */
#include "Particle__Texture.h"
extern unsigned int data_0209ee84;
extern unsigned int data_0209ee8c;

unsigned int _ZN8Particle7Texture12AllocPalVramEjb(struct Particle__Texture *self, int b) {
  if (b) {
    unsigned int a = ((unsigned int)&self->unk_007) & ~7;
    unsigned int old = data_0209ee84;
    data_0209ee84 = old + a;
    return old;
  } else {
    unsigned int a = ((unsigned int)&self->unk_00f) & ~0xf;
    unsigned int v = data_0209ee8c - a;
    data_0209ee8c = v;
    return v;
  }
}
