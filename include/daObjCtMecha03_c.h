#ifndef DAOBJCTMECHA03_C_H
#define DAOBJCTMECHA03_C_H

#include "types.h"

/* Derives from dBgActor_c directly: RTTI _ZTI16daObjCtMecha03_c /
 * _ZTS16daObjCtMecha03_c (config/arm9/overlays/ov065/symbols.txt) give the
 * class name; the un-migrated D1 (func_ov065_02119f3c) stores this class's
 * own vtable, then dBgActor_c's -- inlined, because dBgActor_c's destructor
 * is defined in its class body -- then destroys the MovingMeshCollider at
 * 0x124 and the Model at 0xd4 before chaining to dActor_c. All three belong
 * to dBgActor_c and are inherited now; nothing below 0x31e restates them.
 *
 * 0x08e/0x090 are NOT this class's own storage: dActor_c.h already names
 * them mAngleY/mAngleZ, and dBgActor_c ends its own data at 0x31e (sizeof
 * rounds to 0x320) -- see the header comment there. The original
 * AUTO-GENERATED flat placeholder called them unk_08e/unk_090 because it
 * had no base class to inherit them from; the real class uses the
 * inherited names instead.
 *
 * SIZE IS THE ROM'S OWN LITERAL: the factory (func_ov065_0211a45c) calls
 * fBase_c::operator new(0x388), read off the retail instruction. The field
 * span only reaches 0x358; the rest is unclaimed trailing pad, same shape
 * as TTC_MovingBar's own pad_34c[0x30] tail.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"

struct daObjCtMecha03_c : dBgActor_c {
    s16 unk_31e;                      /* 0x31e */
    s16 unk_320;                      /* 0x320 */
    s16 unk_322;                      /* 0x322 */
    s16 unk_324;                      /* 0x324 -- read/written by Behavior */
    s16 unk_326;                      /* 0x326 -- read/written by Behavior */
    s16 unk_328;                      /* 0x328 -- read/written by Behavior */
    u8  pad_32a[0x6];
    ShadowModel mShadowModel;         /* 0x330 */

    /* --- vtable --- */
    virtual ~daObjCtMecha03_c();

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    /* Tail padding. The field span stops short of the real size: the
       factory calls fBase_c::operator new(0x388), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_358[0x30];      /* 0x358, to the ROM's 0x388 */
};

typedef char daObjCtMecha03_c_size_must_be_0x388[sizeof(daObjCtMecha03_c) == 0x388 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA03_C_H */
