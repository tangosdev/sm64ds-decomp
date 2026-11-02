#ifndef PRIVATE_PARTICLE_RESOURCE_HEADER_H
#define PRIVATE_PARTICLE_RESOURCE_HEADER_H

#include "types.h"

/* The particle resource file's header, as far as its readers use it.
 * Particle::SysTracker::Initialise (Particle.cpp) checks the first four
 * bytes against the data_0208f668 magic to tell a raw file from an
 * LZ-compressed one. func_0204a17c (Particle_Manager.cpp) reads the
 * counts; its definition records start right after this header and the
 * texture records follow them.
 * Once func_0204a0dc has loaded the textures into VRAM, Initialise
 * reallocates the buffer down to residentSize. */
typedef struct ParticleResourceHeader {
    char magic[4]; /* 0x00 */
    u8 pad_04[4];
    u16 numDefinitions; /* 0x08 */
    u16 numTextures;    /* 0x0a */
    u8 pad_0c[0xc];
    u32 residentSize; /* 0x18 */
    u8 pad_1c[4];
} ParticleResourceHeader;

typedef char
    ParticleResourceHeader_size_must_be_0x20[sizeof(ParticleResourceHeader) == 0x20 ? 1 : -1];

#endif
