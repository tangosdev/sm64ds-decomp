#ifndef DAYUREI_MUCHO_C_H
#define DAYUREI_MUCHO_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN15daYurei_Mucho_cD1Ev`
 * stores this vtable, destroys four members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The four members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 dCcAc_c  0x34   -> 0x144
 *     0x144 dBgCh_Actr        0x1bc  -> 0x300
 *     0x300 ModelAnim           0x64   -> 0x364
 *     0x364 ShadowModel         0x28   -> 0x38c
 *
 * Two fields the auto-generated header never listed turned up in Behavior,
 * reachable only as raw `c + 0x...`: the state pointer at 0x3bc and the
 * angle at 0x3d8. The class is 0x3dc, not the 0x3d8 its old field span
 * implied.
 *
 * Typing mModelAnim absorbed unk_35c, which sat at +0x5c inside it -- the
 * `speed` of its Animation base (base at +0x50, speed at +0x0c). Same shape as
 * HootTheOwl's.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daYurei_Mucho_c_classInit` calls
 * `fBase_c::operator new(996)` -- 0x3e4 -- and stores `_ZTV15daYurei_Mucho_c`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3dc; the difference is trailing space no source reads.
 *
 * SM64DS RTTI names the implementation daYurei_Mucho_c. The reconstructed
 * factory daYurei_Mucho_c_classInit (historical alias
 * Snufit_Spawn) constructs it for the YUREI_MUCHO
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daYurei_Mucho_c : dEnemyBase_c {
    /* What mCurrentState points at. Four of these live in ov065's .bss at
       0x0211d650/660/670/680, 0x10 apart, the last ending exactly where the
       next class's band begins -- which is what makes 0x10 the size.

       BOTH words are pointers-to-member, and both are evidenced. The module's
       static initializer copies each object in as two 8-byte halves from the
       .data pointer-to-member records at 0x0211cb20..0x0211cb60, and the two
       hooks are invoked from different places: func_ov065_0211691c calls the
       one at +0x00 the moment a state is installed, and Behavior calls the one
       at +0x08 every frame.

       COINED: the words "Enter" and "Main". The ROM proves the offsets, the
       encoding and which caller reaches which; it preserves no name. The `int`
       return type is measured -- func_ov065_0211691c returns the +0x00 hook's
       result -- not chosen. */
    struct State {
        int (daYurei_Mucho_c::*mEnter)();      /* 0x00 */
        int (daYurei_Mucho_c::*mMain)();       /* 0x08 */
    };

    dCcAc_c mdCcAc_c; /* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x144 */
    ModelAnim mModelAnim;             /* 0x300 */
    ShadowModel mShadowModel;         /* 0x364 */
    u8  pad_38c[0x30];
    State *mCurrentState;             /* 0x3bc */
    u8  pad_3c0[0xc];
    /* InitResources copies the spawn position into these three, Behavior
       re-captures the live position into them in the Yoshi-eat branch, and
       reads mHomePosY back when steering. */
    s32 mHomePosX;                    /* 0x3cc */
    s32 mHomePosY;                    /* 0x3d0 */
    s32 mHomePosZ;                    /* 0x3d4 */
    s32 unk_3d8;                      /* 0x3d8 -- advanced by 0x200 a frame */
    u8  pad_3dc[0x8];

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. This directly included declaration makes
       mwccarm emit the cartridge's D1-then-D0 pair (0x02115ee0, 0x02115f28)
       without the otherwise homeless D2 an out-of-line definition produces.
       It is also what makes the promoted TU this class's key-function TU, so
       the _ZTV/_ZTI/_ZTS group is emitted here and externalized. */
    virtual ~daYurei_Mucho_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char daYurei_Mucho_c_size_must_be_0x3e4[sizeof(daYurei_Mucho_c) == 0x3e4 ? 1 : -1];

#endif /* DAYUREI_MUCHO_C_H */
