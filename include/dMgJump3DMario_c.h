#ifndef DMGJUMP3DMARIO_C_H
#define DMGJUMP3DMARIO_C_H

#include "ModelAnim.h"

/* dMgJump3DMario_c -- one of the three Marios in Bounce and Pounce (MG_JUMP)
 * and its sequel (MG_JUMP2), ov006.
 *
 * Both class names and the inheritance edge are carried by ov006 RTTI:
 * dMgJump3DMario_c : dMg3DHeyhoObjAdapter_c. The base is abstract (all three
 * vtable slots null) and this class is its only derived class. The destructor
 * is not virtual, so the key function is the first declared virtual,
 * Unk_020c76d8, and src/actors/dMgJump3DMario_c.cpp emits the vtable.
 *
 * There is no factory: the owning scenes hold `dMgJump3DMario_c mPlayers[3]`
 * (dScMgJump_c +0x506c, dScMgJump2_c +0x500c) and array-construct it. The base
 * ctor fixes the 0x4c split (it builds ModelAnim at +0x4c next); the array
 * helpers fix sizeof at 0xb8.
 *
 * Field names are proven by this TU's own uses; the virtual names stay
 * address-derived (RTTI proves ownership and slot order, not spelling).
 */

/* Three 20.12 words. Plain on purpose: types.h's Vector3 declares an (empty)
 * destructor, which would make C1/D1 emit _ZN7Vector3D1Ev calls the ROM
 * structors do not have. */
struct Jump3DVec { s32 x, y, z; };

struct dMgJump3DMario_c;
/* The state handler at +0x3c. Formed over the INCOMPLETE class on purpose:
 * mwccarm picks the pointer-to-member representation from the completeness at
 * the typedef, and incomplete is the ROM's 8-byte {code, adjustment} form --
 * the fifteen records at ov006 0x0213b020..0x0213b098. */
typedef void (dMgJump3DMario_c::*Jump3DState)();

struct dMg3DHeyhoObjAdapter_c {
    dMg3DHeyhoObjAdapter_c();
    ~dMg3DHeyhoObjAdapter_c() {}

    virtual void *Unk_020c76d8() = 0; /* slot 0 -- returns &mPos */
    virtual void *Unk_020c76d0() = 0; /* slot 1 -- returns &mVel */
    virtual int Unk_020c762c() = 0;   /* slot 2 -- 0 while idle-ish, 1 once live */

    Jump3DVec mAnchor;      /* 0x04 -- the point EnterHold measures mPos against; written by the owning scene, never in this TU */
    u16 mCommand;           /* 0x10 -- the scene's command: 1 = hit, 2 = hold; consumed (cleared) by the State* handlers. The one field the base ctor touches */
    u8 pad_012[2];          /* 0x12 */
    Jump3DVec mPos;         /* 0x14 -- position (20.12); advanced by mVel every update */
    Jump3DVec mVel;         /* 0x20 -- velocity (20.12) */
    u8 pad_02c[2];          /* 0x2c */
    s16 mAngleY;            /* 0x2e -- facing angle; eased toward the direction of travel */
    u8 pad_030[2];          /* 0x30 */
    s16 mTimer;             /* 0x32 -- state countdown */
    u8 pad_034;             /* 0x34 */
    u8 mVisible;            /* 0x35 -- render gate */
    s16 mScreenX;           /* 0x36 -- projected screen x */
    s16 mScreenY;           /* 0x38 -- projected screen y */
    u8 pad_03a[2];          /* 0x3a */
    Jump3DState mState;     /* 0x3c -- the state handler, installed from the ROM records */
    int mAnimIdx;           /* 0x44 -- selects the anim-file slot (0/1/2) */
    u8 pad_048[4];          /* 0x48 */
};

/* The fifteen non-virtual names below are COINED: the cartridge proves each
 * member's address, size, and -- for the nine reached through mState -- which
 * 8-byte record installs it and which member writes that record, but no
 * spelling. `State*` is a handler the object dispatches to through mState;
 * `Enter*` is a member whose measured effect includes installing one:
 *
 *   StateDamp     <- 0x0213b030, written by EnterDamp
 *   StateHold     <- 0x0213b028, written by EnterHold
 *   StateMove     <- 0x0213b020, written by EnterMove
 *   StateFallOut  <- 0x0213b090, written by func_ov006_020c8084
 *   StateRiseOut  <- 0x0213b080, written by func_ov006_020c81e0
 *   StateBounce   <- 0x0213b060 (EnterBounce) and 0x0213b078 (EnterHit)
 *   StateWindUp   <- 0x0213b050, written by func_ov006_020c862c
 *   StateIdle     <- 0x0213b040, 0x0213b048 and 0x0213b058
 *   StateRespawn  <- 0x0213b038 (EnterRespawn) and 0x0213b088
 *
 * Eight members of the same run keep their `func_ov006_*` names. That is a
 * migration dependency, not a limit on what can be spelled: each is called by
 * a plain `bl` from a still-unpromoted `.c` shard below the run, and while a C
 * shard cannot name a C++ member by its source spelling, it can declare the
 * mangled linker symbol, as this tree does elsewhere. Converting them means
 * rewriting those call sites and their TU configuration, which this PR does
 * not reserve. The `func_ov006_*` names are inferred labels and carry no claim
 * about the original spellings. See src/actors/dMgJump3DMario_c.cpp.
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
    u8 unk_0b0[0x08];     /* 0xb0 -- untouched by every member; proven only by sizeof */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMg3DHeyhoObjAdapter_c_size_must_be_0x4c[
    sizeof(dMg3DHeyhoObjAdapter_c) == 0x4c ? 1 : -1];
typedef char dMgJump3DMario_c_size_must_be_0xb8[
    sizeof(dMgJump3DMario_c) == 0xb8 ? 1 : -1];
#endif

#endif
