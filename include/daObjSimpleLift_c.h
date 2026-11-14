#ifndef DAOBJSIMPLELIFT_C_H
#define DAOBJSIMPLELIFT_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* A dBgActor_c: model at 0xd4, moving mesh collider at 0x124, clsn matrix
 * at 0x2ec, all inherited. Layout evidence: notes/platform-provenance.md.
 *
 * SM64DS proves this class as daObjSimpleLift_c through RTTI, allocation
 * size and vtable identity: the vtable at ov091:0x02135080 carries
 * _ZTI17daObjSimpleLift_c (0x02134f40) in its typeinfo word, and that
 * record's type-name string at 0x02134f4c reads "17daObjSimpleLift_c".
 * The factory and profile spellings below are reconstructed source-style
 * names -- evidence-bounded proposals, not recovered SM64DS symbols.
 *
 * The seven daObjSimpleLift_c_classInit_* factories (0x021327e8..0x02132938,
 * one per registry profile) each allocate 0x330 and install this class's
 * cartridge vtable; they are `new daObjSimpleLift_c()` and live in
 * src/game/actors/d_a_obj_simple_lift.cpp alongside the rest of the class. */

#include "dBgActor_c.h"

struct daObjSimpleLift_c : dBgActor_c {
    u8 mPauseTimer;                       /* 0x31e */
    u8  pad_31f[0x1];
    u16 mMoveTimer;                    /* 0x320 */
    u8 mVariant;                       /* 0x322 */

    /* --- vtable --- */
    virtual ~daObjSimpleLift_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Where it started; InitResources copies the actor's position in here.
       MEASURED SIZE, do not shrink to the field span: the two factories call
       fBase_c::operator new(0x330), read off the retail instruction, and a span
       is only a lower bound. These three close it exactly. */
    s32 mBasePosX;                    /* 0x324 */
    s32 mBasePosY;                    /* 0x328 */
    s32 mBasePosZ;                    /* 0x32c */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSimpleLift_c_size_must_be_0x330[sizeof(daObjSimpleLift_c) == 0x330 ? 1 : -1];
#endif

#endif /* DAOBJSIMPLELIFT_C_H */
