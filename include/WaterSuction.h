#ifndef WATERSUCTION_H
#define WATERSUCTION_H

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
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct WaterSuction : Enemy {
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;/* 0x110 */
    WithMeshClsn mWithMeshClsn;       /* 0x150 */
    u8  pad_30c[0x8];
    s32 unk_314;                      /* 0x314 */

    /* --- vtable --- */
    virtual ~WaterSuction();

    int Behavior();
    int InitResources();
};

typedef char WaterSuction_size_must_be_0x318[sizeof(WaterSuction) == 0x318 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct WaterSuction {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x80];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    u8  pad_098[0x68];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12WaterSuctionD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1c3];
    s32 unk_314;            /* 0x314 */
};

#endif /* __cplusplus */

#endif /* WATERSUCTION_H */
