#ifndef PYRAMIDSTEP_H
#define PYRAMIDSTEP_H

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct PyramidStep : dBgActor_c {
    u8  pad_31e[0x2];
    Model mModel;                     /* 0x320 */
    s16 mStateTimer;                      /* 0x370 */
    u8 mState;                       /* 0x372 */
    u8  pad_373[0x1];
    u8 unk_374;                       /* 0x374 */

    /* --- vtable --- */
    virtual ~PyramidStep();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Tail padding. The field span stops short of the real size: PyramidStep_Spawn
       calls fBase_c::operator new(0x3a4), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_378[0x2c];      /* 0x378, to the ROM's 0x3a4 */
};

typedef char PyramidStep_size_must_be_0x3a4[sizeof(PyramidStep) == 0x3a4 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct PyramidStep {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x18];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11PyramidStepD1Ev.c] */
    Model mModel1;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x320 */
    s16 mStateTimer;            /* 0x370 */
    u8  mState;            /* 0x372 */
    u8  pad_373[0x1];
    u8  unk_374;            /* 0x374 */
};

#endif /* __cplusplus */

#endif /* PYRAMIDSTEP_H */
