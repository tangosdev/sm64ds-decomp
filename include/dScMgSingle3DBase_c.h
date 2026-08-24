/* Base class for the "single 3D minigame" family -- 13 direct RTTI children
 * (card, cup, memory x2, mahjong-carlo x2, roulette, slot3, sound, BSC,
 * snowball, flower, 3DEsp). Adds a Particle::SysTracker at 0x471c. See
 * notes/minigame-provenance.md for the field evidence. */
#ifndef DSCMGSINGLE3DBASE_C_H
#define DSCMGSINGLE3DBASE_C_H
#include "dScMgBase_c.h"

namespace Particle {
struct SysTracker {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x747];
    s32 unk_750;            /* 0x750 */
    u8  mParticle1;          /* 0x754 */
    u8  pad_755[0x7];
    s32 unk_75c;            /* 0x75c */
    u8  mParticle2;          /* 0x760 */
    u8  pad_761[0x7];
    s32 unk_768;            /* 0x768 */
    u8  unk_76c;            /* 0x76c */
    u8  pad_76d[0x7];
    s32 unk_774;            /* 0x774 */
    u8  unk_778;            /* 0x778 */
    u8  pad_779[0x7];
    s32 unk_780;            /* 0x780 */
    u8  unk_784;            /* 0x784 */
    u8  pad_785[0x7];
    s32 unk_78c;            /* 0x78c */
    u8  unk_790;            /* 0x790 */
    u8  pad_791[0x7];
    s32 unk_798;            /* 0x798 */
    u8  mParticle3;          /* 0x79c */
    u8  pad_79d[0x7];
    s32 unk_7a4;            /* 0x7a4 */
    u8  mParticle4;          /* 0x7a8 */
    u8  pad_7a9[0x7];
    s32 unk_7b0;            /* 0x7b0 */
    u8  unk_7b4;            /* 0x7b4 */
    u8  pad_7b5[0xb];
    s32 unk_7c0;            /* 0x7c0 */
    u8  unk_7c4;            /* 0x7c4 */
    u8  pad_7c5[0xf];
    u8  unk_7d4;            /* 0x7d4 */
    u8  pad_7d5[0xf];
    u8  unk_7e4;            /* 0x7e4 */
    u8  pad_7e5[0xb];
    u8  unk_7f0;            /* 0x7f0 */
    u8  pad_7f1[0x3];
    u8  unk_7f4;            /* 0x7f4 */
    u8  pad_7f5[0x3];
    u8  unk_7f8;            /* 0x7f8 */
    u8  pad_7f9[0x3];
    s32 unk_7fc;            /* 0x7fc */
    u8  unk_800;            /* 0x800 */
    u8  pad_801[0x3];
    s32 unk_804;            /* 0x804 */
    u8  unk_808;            /* 0x808 */
    u8  pad_809[0x7];
    u8  unk_810;            /* 0x810 */
    u8  pad_811[0x3];
    u8  unk_814;            /* 0x814 */
    u8  pad_815[0x3];
    u8  unk_818;            /* 0x818 */
    u8  pad_819[0x3];       /* rounds 0x819 up to the 0x81c alignment boundary */

    /* Non-virtual: declaring them changes no layout. */
    s32 Initialise();
    s32 Update();

    ~SysTracker();
};

typedef char SysTracker_size_must_be_0x81c[sizeof(SysTracker) == 0x81c ? 1 : -1];

/* Called by dScMgSingle3DBase_c::BeforeRender. */
void RenderAll();
}

struct dScMgSingle3DBase_c : dScMgBase_c {
    /* Declared first (key function); overrides slots 16 (D1) and 17 (D0).
       MUST STAY DEFINED INLINE -- all 13 children inline this body, and
       _ZN19dScMgSingle3DBase_cD2Ev exists nowhere in the ROM, so an
       out-of-line definition leaves every child with an undefined external.
       MEASURED on dScMgMemory_c; do not move the body out. */
    virtual ~dScMgSingle3DBase_c() {}

    /* --- re-overrides of dScMgBase_c's virtuals, in _ZTV order. Slots 26
           and 33 are new at this class; their signatures are not
           reconstructed yet, so they stay undeclared. --- */
    virtual void AfterInitResources(u32 vfSuccess); /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                  /* slot  7 */
    virtual int  BeforeRender();                    /* slot 10 */

    /* DO NOT restyle the seven `offset 0xNN` comments below into this
     * tree's usual hex-comment form: check_header_offsets.py's DATA_SIZE
     * precompute cannot parse the namespaced mSysTracker line and stops at
     * the last field it CAN parse, which silently undercounts every derived
     * class's field offsets. MEASURED -- see notes/minigame-provenance.md. */
    u8  pad_4660[0xa0];
    s32 unk_4700; /* offset 0x4700 */
    s32 unk_4704; /* offset 0x4704 */
    s32 unk_4708; /* offset 0x4708 */
    s32 unk_470c; /* offset 0x470c */
    s32 unk_4710; /* offset 0x4710 */
    s32 unk_4714; /* offset 0x4714 */
    s16 unk_4718; /* offset 0x4718 */
    u8  pad_471a[0x2];
    Particle::SysTracker mSysTracker; /* 0x471c */
};

/* A floor, not a claim the object ends here: 0x471c + sizeof(SysTracker).
   See notes/minigame-provenance.md. */
typedef char dScMgSingle3DBase_c_size_must_be_0x4f38[sizeof(dScMgSingle3DBase_c) == 0x4f38 ? 1 : -1];

#endif
