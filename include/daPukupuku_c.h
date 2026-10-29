#ifndef DAPUKUPUKU_C_H
#define DAPUKUPUKU_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS proves this class as daPukupuku_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daPukupuku_c_classInit at 0x02133634 (historical alias CheepCheep_Spawn)
 * allocates 0x388 and installs this class's cartridge vtable. It backs the
 * PUKUPUKU registry profile, whose descriptor at 0x02134300 is reconstructed
 * as g_profile_PUKUPUKU.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* Incomplete on purpose: the state pointer-to-member is 8 bytes (code, delta). */
struct PukuStateC;
typedef int (PukuStateC::*PukuStatePMF)();

struct daPukupuku_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;   /* 0x110 */
    dBgCh_Actr mWithMeshClsn; /* 0x150 */
    dExtAnmModel_c mAnmModel; /* 0x30c */
    /* The current state record: enter PMF at +0, main PMF at +8.
       SetState installs it and calls enter; Behavior calls main. */
    PukuStatePMF *mState; /* 0x370 */
    Vector3 mHomePos;     /* 0x374 */

    /* --- vtable --- */
    /* Inline and first: measured (class-form skill) that mwccarm 2004/b56
       emits the retail D1-then-D0 pair in ROM order plus _ZTV/_ZTI/_ZTS
       homed in the instantiating TU, and no leaf D2. Out-of-line emits
       D0 before D1, which the production isolate refuses. */
    virtual ~daPukupuku_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
    void CheckPlayerHit();         /* ov090 0x021330c8 */
    int StatePause();              /* ov090 0x02133190 */
    int InitStatePause();          /* ov090 0x021331c4 */
    int StateSwim();               /* ov090 0x02133200 */
    int InitStateSwim();           /* ov090 0x02133290 */
    int SetState(PukuStatePMF *p); /* ov090 0x021332e8 */
    void UpdateModelMatrix();      /* ov090 0x02133338 */

    /* The field span stops short of the real size:
       daPukupuku_c_classInit calls fBase_c::operator new(0x388), read off
       the retail instruction. */
    u8 pad_380[0x4]; /* 0x380 */
    /* InitStateSwim picks it at random; StateSwim points it home when the
       fish strays and turns mPrevAngleY toward it. */
    s16 mTargetAngleY; /* 0x384 */
    u8 pad_386[0x2];   /* to the ROM's 0x388 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPukupuku_c_size_must_be_0x388[sizeof(daPukupuku_c) == 0x388 ? 1 : -1];
#endif

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/dExtShadowModel_c.h. */
struct daPukupuku_c {
    u8 pad_000[0x5c];
    s32 mPosX; /* 0x05c */
    s32 mPosY; /* 0x060 */
    s32 mPosZ; /* 0x064 */
    u8 pad_068[0x26];
    s16 mAngleY; /* 0x08e */
    u8 pad_090[0x4];
    s16 mPrevAngleY; /* 0x094 */
    u8 pad_096[0x1a];
    u32 mFlags; /* 0x0b0 */
    u8 pad_0b4[0x5c];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12daPukupuku_cD1Ev, now in src/game/actors/d_a_pukupuku.cpp] */
    dCcAcPos_c mdCcAcPos_c; /* 0x110 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12daPukupuku_cD1Ev, now in src/game/actors/d_a_pukupuku.cpp] */
    dBgCh_Actr mWithMeshClsn; /* 0x150 */
    u8 mAnmModel;             /* 0x30c */
    u8 pad_30d[0x63];
    void *mState;  /* 0x370 */
    s32 mHomePosX; /* 0x374 */
    s32 mHomePosY; /* 0x378 */
    s32 mHomePosZ; /* 0x37c */
};

#endif /* __cplusplus */

#endif /* DAPUKUPUKU_C_H */
