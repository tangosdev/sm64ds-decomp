#ifndef MONTYMOLEROCK_H
#define MONTYMOLEROCK_H

#include "types.h"

/* Derives from Enemy: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "Model.h"
#include "MovingCylinderClsn.h"
#include "WithMeshClsn.h"

struct MontyMoleRock : Enemy {
    Model mModel;                     /* 0x110 */
    MovingCylinderClsn mMovingCylinderClsn;/* 0x160 */
    WithMeshClsn mWithMeshClsn;       /* 0x194 */
    u8 unk_350;                       /* 0x350 */

    /* --- vtable --- */
    virtual ~MontyMoleRock();

    virtual s32 Behavior();
    virtual s32 InitResources();
    virtual s32 Render();
};

typedef char MontyMoleRock_size_must_be_0x354[sizeof(MontyMoleRock) == 0x354 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct MontyMoleRock {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x6c];
    /* Model member, named by _ZN5ModelD1Ev at +0x110 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x110 */
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x160 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MontyMoleRockD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x160 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x194 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13MontyMoleRockD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x194 */
    u8  unk_350;            /* 0x350 */
};

#endif /* __cplusplus */

#endif /* MONTYMOLEROCK_H */
