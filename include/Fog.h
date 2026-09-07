#ifndef FOG_H
#define FOG_H
#include "types.h"

/* One hardware fog setup: the 32-entry density ramp the GX fog table wants,
   plus the four registers that go with it. Fog::Init fills the ramp from a
   near/far pair and writes the rest. */
struct Fog {
    u8  mDensity[0x20];     /* 0x000 -- fog table, one byte per depth slice */
    u8  mEnabled;           /* 0x020 */
    u8  mShift;             /* 0x021 */
    u16 mOffset;            /* 0x022 */
    u16 mColor;             /* 0x024 -- 15-bit BGR */
    u8  pad_026[0x2];
};

/* 0x28, from the array stride: Stage::LoadFog walks its fog array with
   `dst += 0x28` and hands each element to Fog::Init. check_header_offsets.py
   sizes a member type from this typedef -- without it, Stage.h's Fog member is
   UNPARSED and the checker goes blind for the rest of that struct. */
typedef char Fog_size_must_be_0x28[sizeof(struct Fog) == 0x28 ? 1 : -1];
typedef struct Fog Fog;

#endif
