#ifndef DMGJUMP3DMARIO_C_H
#define DMGJUMP3DMARIO_C_H

#include "ModelAnim.h"

/* Both class names and the inheritance edge are carried by ov006 RTTI:
 *
 *   dMgJump3DMario_c : dMg3DHeyhoObjAdapter_c
 *
 * The base constructor fixes its 0x4c-byte extent: the derived constructor
 * calls it, installs _ZTV16dMgJump3DMario_c, then constructs ModelAnim at
 * +0x4c. The array helpers in both jump minigames fix sizeof the derived
 * object at 0xb8.
 *
 * The three virtual names remain address-based because RTTI and the vtable
 * prove their ownership and order, but not their original source spelling.
 */
struct dMg3DHeyhoObjAdapter_c {
    dMg3DHeyhoObjAdapter_c();
    ~dMg3DHeyhoObjAdapter_c() {}

    virtual void *Unk_020c76d8() = 0; /* slot 0 */
    virtual void *Unk_020c76d0() = 0; /* slot 1 */
    virtual int Unk_020c762c() = 0;   /* slot 2 */

    u8 unk_004[0x0c]; /* 0x04 */
    s16 unk_010;      /* 0x10 */
    u8 unk_012[0x3a]; /* 0x12 */
};

/* THE FIFTEEN NON-VIRTUAL NAMES BELOW ARE COINED, and this comment is their
 * derivation.  For each of them the cartridge proves the address, the size,
 * that the first argument is the object, and -- for the nine reached through a
 * pointer-to-member -- which 8-byte record in ov006 .data at
 * 0x0213b020..0x0213b098 installs it and which member writes that record.  It
 * proves NO spelling: none of the fifteen carried a name anywhere in the ROM,
 * and none was declared in this header before 2026-09-06.  Every name below is
 * a summary of what that member's own bytes do, in the shape ov006's
 * dScMgCurling2_c uses.
 *
 * `State*` is a handler the object dispatches to through its +0x3c
 * pointer-to-member; `Enter*` is a member whose measured effect includes
 * writing the record that installs the matching handler:
 *
 *   0x020c78ec StateDamp     <- record 0x0213b030, written by EnterDamp
 *   0x020c79a8 EnterDamp
 *   0x020c7a30 StateHold     <- record 0x0213b028, written by EnterHold
 *   0x020c7ba4 EnterHold
 *   0x020c7c68 StateMove     <- record 0x0213b020, written by EnterMove
 *   0x020c802c EnterMove
 *   0x020c8048 StateFallOut  <- record 0x0213b090, written by func_ov006_020c8084
 *   0x020c814c StateRiseOut  <- record 0x0213b080, written by func_ov006_020c81e0
 *   0x020c8270 EnterHit
 *   0x020c833c StateBounce   <- records 0x0213b060 (EnterBounce) and
 *                                0x0213b078 (EnterHit)
 *   0x020c85a0 EnterBounce
 *   0x020c85bc StateWindUp   <- record 0x0213b050, written by func_ov006_020c862c
 *   0x020c864c StateIdle     <- records 0x0213b040, 0x0213b048 and 0x0213b058
 *   0x020c8680 StateRespawn  <- records 0x0213b038 (EnterRespawn) and 0x0213b088
 *   0x020c8768 EnterRespawn
 *
 * The three virtual names stay address-derived.  The cartridge gives their
 * ownership and their slot order and nothing else, and respelling them would
 * move three symbols that six committed sources already name.
 *
 * Eight further members of the same run keep their `func_ov006_*` names on
 * purpose: they are called by a plain `bl` from seven `.c` shards below the
 * run, and a C shard cannot name a C++ member.  See
 * src/actors/dMgJump3DMario_c.cpp.
 */
struct dMgJump3DMario_c : dMg3DHeyhoObjAdapter_c {
    dMgJump3DMario_c();
    ~dMgJump3DMario_c();

    virtual void *Unk_020c76d8(); /* slot 0 */
    virtual void *Unk_020c76d0(); /* slot 1 */
    virtual int Unk_020c762c();   /* slot 2 */

    void StateDamp();    /* 0x020c78ec */
    void EnterDamp();    /* 0x020c79a8 */
    void StateHold();    /* 0x020c7a30 */
    void EnterHold();    /* 0x020c7ba4 */
    void StateMove();    /* 0x020c7c68 */
    void EnterMove();    /* 0x020c802c */
    void StateFallOut(); /* 0x020c8048 */
    void StateRiseOut(); /* 0x020c814c */
    void EnterHit();     /* 0x020c8270 */
    void StateBounce();  /* 0x020c833c */
    void EnterBounce();  /* 0x020c85a0 */
    void StateWindUp();  /* 0x020c85bc */
    void StateIdle();    /* 0x020c864c */
    void StateRespawn(); /* 0x020c8680 */
    void EnterRespawn(); /* 0x020c8768 */

    ModelAnim mModelAnim; /* 0x4c */
    u8 unk_0b0[0x08];     /* 0xb0 */
};

typedef char dMg3DHeyhoObjAdapter_c_size_must_be_0x4c[
    sizeof(dMg3DHeyhoObjAdapter_c) == 0x4c ? 1 : -1];
typedef char dMgJump3DMario_c_size_must_be_0xb8[
    sizeof(dMgJump3DMario_c) == 0xb8 ? 1 : -1];

#endif
