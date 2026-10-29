#ifndef TRMP2SPINNER_H
#define TRMP2SPINNER_H

#include "types.h"
#include "dExtSimpleModel_c.h"

/* Trmp2Spinner, one of the 0x14 stars dScMgTrampoline2_c::mSpinners holds
 * (0x78 bytes; the name is coined, no RTTI names it). The constructor and
 * destructor are the class's own, out of line in
 * src/minigames/d_s_mg_trampoline2.cpp (ROM 0x02122c90 and 0x02122c68): the
 * constructor builds mModel and clears mLifeTimer, the destructor releases the
 * shared star model file before mModel is destroyed. Trmp2Spinner_Init fills
 * the rest. The pool helpers there are free functions over a char* record.
 *
 * mModel is a member, not a base: neither the constructor nor the destructor
 * stores a vptr of its own. */
struct Trmp2Spinner {
    Trmp2Spinner();
    ~Trmp2Spinner();

    dExtSimpleModel_c mModel; /* 0x00 */
    Vector3 mScale; /* 0x50 -- Render's scale: 0x4000 plus 0xa00 per Init arg, capped at 0x8000 */
    Vector3 mPos;   /* 0x5c -- UpdateMatrix's translation */
    Vector3 mVel;   /* 0x68 -- added to mPos every frame */
    s16 mAngleZ;    /* 0x74 -- UpdateMatrix's Z rotation; +0x400 a frame */
    s16 mLifeTimer; /* 0x76 -- 0xf0 at Init, counts down to 0; the pool skips
                       a 0 slot and reuses it */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Trmp2Spinner_size_must_be_0x78[sizeof(Trmp2Spinner) == 0x78 ? 1 : -1];
#endif

#endif
