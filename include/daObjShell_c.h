#ifndef DAOBJSHELL_C_H
#define DAOBJSHELL_C_H

#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The clam of Jolly Roger Bay and the sunken ship (profile OBJ_SHELL). It sits
 * shut, snaps open on a timer once the player is within 500.0, and hurts
 * whatever its cylinder touches during the open part of that lunge.
 *
 * The cartridge RTTI names this class daObjShell_c, and that is the spelling
 * every named virtual carries. `_ZTS12daObjShell_c` at 0x0211c594 holds the
 * literal "12daObjShell_c"; `_ZTI12daObjShell_c` at 0x0211c588 is an
 * __si_class_type_info record -- one direct base, dActor_c (`_ZTI8dActor_c`,
 * arm9:0x0208e390), at offset zero. `_ZTV12daObjShell_c` at 0x0211c5c8 is that
 * table's address point: 31 slots, the same extent as dActor_c, with 0, 3, 6,
 * 9, 16 and 17 the only overrides.
 *
 * daObjShell_c_classInit constructs dActor_c, then ModelAnim and dCcAc_c at +0xd4 and
 * +0x138. D1/D0 destroy the same subobjects in reverse, independently pinning
 * both the inheritance and owned-member layout. */
struct daObjShell_c : dActor_c {
    u8 pad_0d0[0x4];         /* 0x0d0 */
    ModelAnim mModelAnim;     /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    u8 mState;                /* 0x16c: 0 shut, 1 lunging */
    u8 pad_16d[0x1];
    u16 mStateTimer;          /* 0x16e */
    u16 mShutTimer;           /* 0x170 */

    /* Inline empty body on purpose. From an inline destructor mwccarm emits D1
     * and then D0 -- the cartridge's own order at 0x0211a930 and 0x0211a968 --
     * and no leaf D2. Written out of line in the translation unit instead, the
     * same two bodies come out D0-before-D1 and the isolation step rejects the
     * object. Every instruction in both is compiler-generated: this class's
     * vptr store, then dCcAc_c at 0x138 and ModelAnim at 0x0d4 in reverse
     * construction order, then the dActor_c base; D0 additionally returns the
     * object to the actor heap. */
    virtual ~daObjShell_c() {}   /* slots 16, 17 */

    virtual int InitResources();    /* slot 0 */
    virtual int CleanupResources(); /* slot 3 */
    virtual int Behavior();         /* slot 6 */
    virtual int Render();           /* slot 9 */

    /* The factory's allocation goes through the actor heap, not the global
     * operator new: daObjShell_c_classInit's first call is fBase_c's own
     * operator new with the literal 0x174. Spelling it here as a leaf operator
     * new is what lets the factory be written as a plain `new daObjShell_c()`
     * and still emit that call. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#else

/* Flat layout for the remaining C translation units. */
struct daObjShell_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* dActor_c::mCamSpacePos -- dActor_c.h declares s32 here. The sound call in
       Behavior takes its address, which is what pins it. */
    s32 mCamSpacePosX;            /* 0x074 */
    u8  pad_078[0x5c];
    ModelAnim mModelAnim;            /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    u8  mState;            /* 0x16c */
    u8  pad_16d[0x1];
    u16 mStateTimer;            /* 0x16e */
    u16 mShutTimer;            /* 0x170 */
};

#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjShell_c_size_must_be_0x174[sizeof(struct daObjShell_c) == 0x174 ? 1 : -1];
#endif

#endif /* DAOBJSHELL_C_H */
