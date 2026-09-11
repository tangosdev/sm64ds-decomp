/* Cup-shuffle minigame scene; RTTI records dScMgCup_c : dScMgSingle3DBase_c.
 * The factory allocates 0x5470 bytes. Field names are reconstructed from uses.
 * The opaque component starts at 0x4f38 and writes through 0x50e0; its full
 * extent remains unresolved. The 0x19c raw declaration is not its size.
 * Raw storage and manual component/array lifetimes remain tracked in #2492.
 */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c3288(char *t); /* decl_common.h's own signature */
extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020deac4(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgCup_c : dScMgSingle3DBase_c {
    struct CupPosition {
        s32 x;
        s32 y;
    };
    virtual ~dScMgCup_c();

    /* The ROM vtable at ov006 0x0213c154 differs from the direct base in seven
       slots: 0, 6, 9, 16, 17, 18 and 20 (16/17 are the destructor pair).
       Overrides remain virtual whether the keyword is repeated or omitted;
       they reuse existing slots and add no field. The inherited slot names,
       including OnYoshiTryEat and Virtual50, are reconstructed interface names,
       not strings read from this vtable. The destructor remains the key
       function; compiler vtable/RTTI passengers are externalized to the
       canonical cartridge copies by the manifest. */
    /* Inferred class-anchored aliases: __sinit_ov006_021303d0 fixes this
       eight-entry PMF order; behavior fixes the roles, but the original
       source spellings are not present in the ROM. */
    void StateSetup();
    void StatePrepareShuffle();
    void StateShuffle();
    void StateWaitForInput();
    void StateSelect();
    void StateResult();
    void StateFinish();
    void StateIdle();
    s32 InitResources();  /* slot  0 -- src/actors/dScMgCup_c.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */
    s32 Behavior();       /* slot  6 -- src/actors/dScMgCup_c.cpp */
    s32 Render();         /* slot  9 -- src/actors/dScMgCup_c.cpp */

    u8  unk_4f38[0x19c];  /* 0x4f38 -- raw component storage prefix */
    s32 unk_50d4;         /* 0x50d4 -- component input word */
    s32 unk_50d8;         /* 0x50d8 */
    s32 unk_50dc;         /* 0x50dc */
    u8  unk_50e0[0x8];    /* 0x50e0 -- component ctor writes here; typing pending */
    u8  mArray1[0x300];   /* 0x50e8 -- 0x20 * 0x18, elem dtor func_ov006_020deac4 */
    CupPosition mCupPositions[3]; /* 0x53e8 -- live per-cup position */
    u8  unk_5400[0xc];    /* 0x5400 -- live words at +0, +4, +8; typing pending */
    s32 mOnes[3];         /* 0x540c -- per-cup scale */
    s32 mState;           /* 0x5418 -- Behavior's pointer-to-member dispatch index */
    u8  unk_541c[0x4];    /* 0x541c */
    s32 mIds[3];          /* 0x5420 -- shuffled cup indices */
    u8  unk_542c[0x8];    /* 0x542c */
    s32 unk_5434[3];      /* 0x5434 -- three parallel per-cup arrays indexed */
    s32 unk_5440[3];      /* 0x5440    together by Behavior's own loop */
    s32 unk_544c[3];      /* 0x544c */
    s32 mShuffleSound;     /* 0x5458 */
    u16 mShuffleAngle;     /* 0x545c */
    s16 mShuffleSpeed;     /* 0x545e */
    u8  unk_5460;          /* 0x5460 */
    u8  unk_5461;          /* 0x5461 */
    u8  unk_5462[3];      /* 0x5462 */
    u8  mFlags[3];        /* 0x5465 -- per-cup reveal flags */
    u8  unk_5468;         /* 0x5468 */
    u8  unk_5469[0x7];    /* 0x5469 -- live bytes through 0x546d; typing pending */
};

typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];

#endif
