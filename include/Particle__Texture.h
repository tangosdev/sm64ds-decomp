/* Particle::Texture's two VRAM allocation helpers are static. Their first
 * argument is a byte count, not `this`: both functions consume it in r0, and
 * the bool named by the trailing `b` in each mangled name arrives in r1.
 * The old generated header mistook size + 7 / size + 15 alignment arithmetic
 * for fields at offsets 0x07 / 0x0f and invented an object layout.
 *
 * The exact `Particle::Texture` source spelling is inherited from the enrolled
 * symbols rather than an RTTI record; the static ABI and absence of an object
 * are independently proven by both function bodies. */
#ifndef PARTICLE__TEXTURE_H
#define PARTICLE__TEXTURE_H
#include "types.h"

namespace Particle {

struct Texture {
    static u32 AllocPalVram(u32 size, bool fromLowAddress);
    static u32 AllocTexVram(u32 size, bool isTexel4x4);
};

}

#endif
