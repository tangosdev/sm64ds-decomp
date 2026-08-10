#ifndef STUMP_H
#define STUMP_H

#include "types.h"
#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

/* Derives from Enemy: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct Stump : Enemy {
    MovingCylinderClsn mMovingCylinderClsn;/* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x144 */
    ModelAnim mModelAnim;             /* 0x300 */
    u8  pad_364[0x10];
    s32 mVariant;                     /* 0x374 */

    /* --- vtable --- */
    virtual ~Stump();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Stump_size_must_be_0x378[sizeof(Stump) == 0x378 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct Stump {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x94];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5StumpD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x144 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5StumpD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x144 */
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x5b];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x14];
    s32 mVariant;            /* 0x374 */
};

#endif /* __cplusplus */

#endif /* STUMP_H */
