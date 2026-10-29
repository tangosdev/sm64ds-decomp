/* class dScMgJump2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgJump2_c : dScMgD3DBase_c, single edge, offset 0 (build/rtti.json).
 * Its own vtable is ov006:0x0213ccfc.
 *
 * SIZE 0x5a78 -- AND ITS FACTORY USED TO CARRY ANOTHER CLASS'S NAME ENTIRELY.
 * The allocation lives in `dScMgJump2_c_classInit` (ov006:0x020efaf0, renamed from
 * the mangled `_ZN8PathLift17BaseInitResourcesEv`, which was neither a PathLift
 * method nor a BaseInitResources): its body is `operator new(0x5a78)`,
 * dScMgBase_c's constructor, dScMgD3DBase_c's vtable, mSysTracker, then THIS
 * class's vtable and every member below. A real `PathLift` class does exist --
 * in ov002, `_ZN8PathLiftD1Ev` and friends at 0x020ef320..0x020eff18 -- so the
 * old name was matched across an OVERLAY BOUNDARY at a near-identical address,
 * not a coined one like MgBounceAndPounce was. Renamed to the fallback
 * `<ClassName>_Spawn` convention (compare daObjEmmLog_c_classInit): no game object
 * name is known for this minigame, unlike dScMgJump_c's own factory
 * dScMgJump_c_classInit.
 *
 * Every member the factory builds is typed: mPlayers (dMgJump3DMario_c[3],
 * named by the ROM's own RTTI), mHeyhos (Heyho[6]), mPopups (Jump2Popup[0x10])
 * and mModel, in the factory's construction order. The ROM destructor tears
 * them down in exact reverse -- mModel first, then mPopups, mHeyhos and
 * mPlayers -- so the compiler writes the whole teardown and the factory is
 * `return new dScMgJump2_c;`. sizeof(dExtSimpleModel_c) == 0x50 closes mModel
 * on mTextureOffsetT at 0x5a64.
 *
 * THE DESTRUCTOR IS DEFINED IN THE CLASS BODY, for the emission-order reason
 * spelled out at the definition below. No separate operator delete is needed:
 * dScMgD3DBase_c, the immediate base, provides one.
 *
 * SM64DS RTTI names the implementation dScMgJump2_c. The reconstructed factory
 * dScMgJump2_c_classInit (historical alias dScMgJump2_c_Spawn) installs this class's
 * cartridge vtable for the MG_JUMP2 registry profile.
 */
#ifndef DSCMGJUMP2_C_H
#define DSCMGJUMP2_C_H
#include "dScMgD3DBase_c.h"
#include "dMgJump3DMario_c.h"
#include "dExtSimpleModel_c.h"
#include "Heyho.h"
#include "Jump2Popup.h"

struct dScMgJump2_c : dScMgD3DBase_c {
    /* DEFINED IN THE CLASS BODY, DELIBERATELY -- not a style choice.
       D1 sits at 0x020eebe8, BELOW D0 at 0x020eec9c. An out-of-line member
       definition makes mwccarm 2004/b56 emit the destructor group as
       D0-then-D1 plus a homeless D2, and a single translation unit covering
       the whole .text run 0x020eebe8..0x020efc0c cannot then be linked: the
       functions are not emitted in ROM address order. Defining it here emits
       D1 before D0, emits no D2 at all, and costs nothing else -- it is still
       the same body, and it is still not inlined into callers because a
       virtual destructor is always reached through the vtable.
       It does give up the key function: with this inline, the first virtual
       DECLARED but not defined here is StartRound below, which lives in
       src/minigames/d_s_mg_jump2.cpp, so _ZTV12dScMgJump2_c is still emitted
       by that one translation unit and by no other. */
    virtual ~dScMgJump2_c() {}
    virtual void StartRound(int arg);  /* slot 18 */
    virtual int CanEndState(int mode); /* slot 19 */

    /* 0x5004 -- the state callback, a pointer-to-member of this class, which
       is mwccarm's eight bytes exactly. Behavior calls through it every frame:
       `(this->**(State *)pad_5004)()`. Was "no matched access", which was only
       ever true of the files NAMED after this class -- the access was in
       func_ov006_020ef3e0 all along, and reads as this class's own now that
       the function carries its real name. Left as bytes rather than declared
       as the member pointer: naming it would fix a signature for every state
       function in the table, and none of them is recovered. */
    u8 pad_5004[0x8];             /* 0x5004 -- the state callback; see the block above */
    dMgJump3DMario_c mPlayers[3]; /* 0x500c -- RTTI-proven element type */
    Heyho mHeyhos[6];             /* 0x5234 -- 6 * 0xf0 */
    Jump2Popup mPopups[0x10];     /* 0x57d4 -- 0x10 * 0x24 */
    dExtSimpleModel_c mModel;     /* 0x5a14 -- see banner */
    s32 mTextureOffsetT;          /* 0x5a64 -- T texture-coordinate offset for material 0 */
    s32 mTextureScrollRate;       /* 0x5a68 -- texture scroll rate, applied >> 12 */
    u32 mParticleID;              /* 0x5a6c -- particle system unique ID */
    u8 pad_5a70[0x4];             /* 0x5a70 -- no matched access */
    s16 mStateTimer;              /* 0x5a74 -- the state functions' countdown */
    u16 pad_5a76;                 /* 0x5a76 */

    /* The PMF state machine. pmf_dScMgJump2_c_State_Idle..9c are the eight-byte
       records the state functions copy into pad_5004 whole; their first
       words relocate to State_Idle, ..ef5ac, ..ef480, ..ef794 and
       ..ef4ec respectively (config/arm9/overlays/ov006/relocs.txt). The
       three remaining members are the transitions that install them. */
    void State_Idle();
    void State_Result();
    void State_End();
    void BeginEnd();
    void State_Play();
    void BeginPlay();
    void State_Intro();
    void BeginIntro();

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. StartRound remains the key function; these declarations
       do not change ownership of _ZTV12dScMgJump2_c.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();    /* slot 0 -- src/_ZN12dScMgJump2_c13InitResourcesEv.cpp */
    s32 CleanupResources(); /* slot 3 -- ov006 0x020ef110 */
    s32 Behavior();         /* slot 6 -- ov006 0x020ef3e0 */
    s32 Render();           /* slot 9 -- ov006 0x020ef148 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgJump2_c_size_must_be_0x5a78[sizeof(dScMgJump2_c) == 0x5a78 ? 1 : -1];
#endif

#endif
