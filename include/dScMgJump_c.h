/* class dScMgJump_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgJump_c : dScMgD3DBase_c, single edge, offset 0 (build/rtti.json).
 * Its own vtable is ov006:0x0213cbe4.
 *
 * SIZE 0x5834, the allocation dScMgJump_c_classInit (`return new
 * dScMgJump_c;`) makes. THAT FACTORY IS THIS CLASS'S, despite its
 * name: it writes dScMgD3DBase_c's vtable, then THIS class's, and
 * everything it builds after that derived vptr store belongs here. The
 * English name is kept because it is already attributed, the same
 * convention as dScMgCard_c_classInit for dScMgCard_c -- but note that until
 * the base was renamed, mModel and both arrays below were declared on
 * dScMgD3DBase_c itself and 0x5834 was asserted as the BASE's size. They
 * are this class's.
 *
 * THE LAST MEMBER CLOSES EXACTLY ON THE ALLOCATION LITERAL:
 * 0x5294 + 6 * 0xf0 = 0x5834. Nothing is left over.
 *
 * mModel IS A TYPED dExtSimpleModel_c, and here that works. The factory constructs it
 * (dExtSimpleModel_c's C1 at p + 0x501c), the destructor destroys it, and
 * sizeof(dExtSimpleModel_c) == 0x50 closes exactly on mPlayers at 0x506c. Because mModel
 * is declared BEFORE the two arrays, reverse-declaration order destroys
 * mHeyhos, mPlayers, then mModel -- exactly the ROM's order, so the compiler
 * emits all three and the destructor body is empty. All three members are
 * typed, so the factory's member construction is compiler-written too. Contrast
 * include/dScMgJump2_c.h, whose ROM destructor destroys its dExtSimpleModel_c FIRST and
 * therefore cannot use a typed member at all. Same class, same member, two
 * different answers, decided by the ROM's own ordering.
 *
 * 0x5004 IS A PAIR OF s32, not padding -- the {fn, 0} state pair the ten
 * state functions below assign the next data_ov006_0213cb* row into. That
 * write, and the s16 countdown at 0x5014 in the same functions, are what
 * bound dScMgD3DBase_c at 0x5004 from above. Both live in
 * src/minigames/d_s_mg_jump_methods.cpp now.
 *
 * THE DESTRUCTOR IS DEFINED IN THE CLASS BODY, for the emission-order reason
 * spelled out at the definition below. No separate operator delete is needed:
 * dScMgD3DBase_c, the immediate base, provides one.
 *
 * SM64DS RTTI names the implementation dScMgJump_c. The reconstructed factory
 * dScMgJump_c_classInit (historical alias MgBounceAndPounce_Spawn) installs this class's
 * cartridge vtable for the MG_JUMP registry profile.
 */
#ifndef DSCMGJUMP_C_H
#define DSCMGJUMP_C_H
#include "dScMgD3DBase_c.h"
#include "dMgJump3DMario_c.h"
#include "dExtSimpleModel_c.h"
#include "Heyho.h"

struct dScMgJump_c : dScMgD3DBase_c {
    /* DEFINED IN THE CLASS BODY, DELIBERATELY -- not a style choice.
       D1 sits at 0x020edec0, BELOW D0 at 0x020edf54. An out-of-line member
       definition makes mwccarm 2004/b56 emit the destructor group as
       D0-then-D1 plus a homeless D2, and the single translation unit that
       covers the .text run 0x020edec0..0x020ee994 cannot then be linked: the
       functions are not emitted in ROM address order. Defining it here emits
       D1 before D0, emits no D2 at all -- and no D2 exists anywhere in the
       cartridge -- and costs nothing else. It is still the same body, and it
       is still not inlined into callers, because a virtual destructor is
       always reached through the vtable. Nothing derives from this class, so
       no descendant's own destructor is affected either way.
       It does give up the key function: with this inline, the first virtual
       DECLARED but not defined here is InitResources below, which lives in
       src/minigames/d_s_mg_jump_methods.cpp, so _ZTV11dScMgJump_c is emitted by that one
       translation unit and by no other. */
    virtual ~dScMgJump_c() {}

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgD3DBase_c's. Spelled WITHOUT the `virtual` keyword,
       the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h spell
       theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so each reuses an existing slot and adds no
       field, and the 0x5834 assert below still holds. */
    s32 InitResources();    /* slot  0 -- src/minigames/d_s_mg_jump_methods.cpp, the key function */
    virtual void StartRound(int arg);               /* slot 18 */
    virtual int  CanEndState(int mode);              /* slot 19 */
    s32 CleanupResources(); /* slot  3 -- src/minigames/d_s_mg_jump_methods.cpp */
    s32 Behavior();         /* slot  6 -- src/minigames/d_s_mg_jump_methods.cpp */
    s32 Render();           /* slot  9 -- src/minigames/d_s_mg_jump_methods.cpp */

    /* The minigame's state functions. Unnamed in the cartridge; stored as
       {fn, 0} pairs in the pmf_dScMgJump_c_State_Result table rows, dispatched through
       mState by Behavior. The mangled spellings are what the table relocs
       resolve by. */
    void State_Idle();
    void State_Final();
    void BeginFinal();
    void State_Result();
    void State_End();
    void BeginEnd();
    void State_Play();
    void BeginPlay();
    void State_Intro();
    void BeginIntro();

    /* 0x5004 IS A PAIR OF s32, not padding: State_Final and the other
       state functions assign the next table row into it whole, and that write
       plus the s16 at 0x5014 are what bound dScMgD3DBase_c at 0x5004 from
       above. The sole member must stay an ARRAY: with two named scalars
       mwccarm scalarises the copy member-by-member and the assigners come
       back 2 words short of the ROM's schedule. */
    struct StatePmf { s32 w[2]; };
    StatePmf mState;      /* 0x5004 -- current state pair */
    s32   mAccum;          /* 0x500c -- gains mAccumStep >> 12 a tick */
    s32   mAccumStep;      /* 0x5010 -- Fix12 */
    s16   mStateTimer;     /* 0x5014 -- countdown to the state change */
    u8    pad_5016[0x6];   /* 0x5016 */
    dExtSimpleModel_c mModel;     /* 0x501c -- 0x50, destroyed last, see banner */
    dMgJump3DMario_c mPlayers[3]; /* 0x506c -- RTTI-proven element type */
    Heyho mHeyhos[6];             /* 0x5294 -- 6 * 0xf0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgJump_c_size_must_be_0x5834[sizeof(dScMgJump_c) == 0x5834 ? 1 : -1];
#endif

#endif
