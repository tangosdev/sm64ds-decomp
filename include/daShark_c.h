#ifndef DASHARK_C_H
#define DASHARK_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * THE CLASS NAME IS READ OUT OF THE CARTRIDGE, NOT COINED. `_ZTV9daShark_c`
 * sits at 0x021343ec; the word before it, at 0x021343e8, is the type_info
 * pointer and resolves to `_ZTI9daShark_c` at 0x021343bc. That record's name
 * pointer reaches 0x021343b0, where the overlay image literally spells
 * `9daShark_c`. This header used to call the class Shark, which was a
 * readable invention; the cartridge had a name for it all along.
 *
 * The factory and profile spellings below are NOT read out of the cartridge.
 * They are reconstructed source-style names -- evidence-bounded proposals,
 * not recovered SM64DS symbols.
 *
 * daShark_c_classInit at 0x02133ca0 (historical alias Shark_Spawn) allocates
 * 0x3a0 and installs this class's cartridge vtable. It backs the SHARK
 * registry profile, whose descriptor at 0x021343c8 is reconstructed as
 * g_profile_SHARK.
 */

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct daShark_c : dEnemyBase_c {
    /* A state record is two of these: enter, then per-frame. */
    typedef int (daShark_c::*StateFn)();

    dCcAcPos_c mdCcAcPos_c;   /* 0x110 */
    dBgCh_Actr mWithMeshClsn; /* 0x150 */
    dExtAnmModel_c mAnmModel; /* 0x30c */
    /* The state pointer. SetState sets it (InitResources passes
       data_ov090_021345cc) and Behavior calls the pointer-to-member-function
       at +8 through it. */
    StateFn *mState;     /* 0x370 */
    Vector3 mClsnOffset; /* 0x374 */
    u8 pad_380[0x4];
    /* Which of the three hurt-cylinder offsets is live: 0 centred, 1 at
       +0x8c000 z, 2 at -0x8c000 z. CheckPlayerHit steps it 0, 1, 2, 0
       each frame. */
    s32 mSwingPhase;    /* 0x384 */
    s32 mPathID;        /* 0x388 */
    s32 mPathNodeCount; /* 0x38c */
    s32 mPathNodeIdx;   /* 0x390 */

    /* --- vtable --- */
    virtual ~daShark_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    void CheckPlayerHit(); /* ov090 0x02133710 */
    /* The two state routines are static: the state descriptors hold plain
       function addresses. Behavior still calls them with the shark in r0. */
    static int StateSwim(daShark_c *);     /* ov090 0x02133830 */
    static int InitStateSwim(daShark_c *); /* ov090 0x0213387c */
    int SetState(StateFn *);               /* ov090 0x021338b4 */
    void UpdateModelMatrix();              /* ov090 0x02133904 */

    /* Tail padding. The field span stops short of the real size:
       daShark_c_classInit calls fBase_c::operator new(0x3a0), read off the
       retail instruction. A span is only a LOWER BOUND. */
    u8 pad_394[0xc]; /* 0x394, to the ROM's 0x3a0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daShark_c_size_must_be_0x3a0[sizeof(daShark_c) == 0x3a0 ? 1 : -1];
#endif

#endif /* DASHARK_C_H */
