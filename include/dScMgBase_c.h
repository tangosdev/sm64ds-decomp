/* Base class for every minigame scene -- 15 direct RTTI children, 32
 * descendants. Field names and widths were recovered from matched bodies;
 * see notes/minigame-provenance.md for the evidence behind each one, and for
 * why the destructor is declared here but defined out of line. */
#ifndef DSCMGBASE_C_H
#define DSCMGBASE_C_H
#include "dScene_c.h"
#include "dGraph_c.h"
#include "dMgState_c.h"
#include "dMgPsOpt_c.h"

extern "C" void *data_ov004_020beb68;

struct dScMgBase_c : dScene_c {

    class graphCallback_c;
    dScMgBase_c();
    /* Declared first (key function), and deliberately NOT defined inline:
       descendants call _ZN11dScMgBase_cD2Ev as a real `bl`, and an inline
       body makes every one of them miss. Defined in
       src/_ZN11dScMgBase_cD1Ev.cpp and .../_D0Ev.cpp.
       MEASURED -- do not move the body up here. */
    virtual ~dScMgBase_c();

    /* Own copy, deliberately: mwcc only inlines a D0 route through the class
       itself or its immediate base, so descendants cannot reach dScene_c's.
       Removing this breaks D0 for all 32 of them. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides of dScene_c's own virtuals, same signature, in _ZTV order.
           1, 2, 5, 7, 10 re-override slots dScene_c already gave a body;
           6, 9, 12 are the first override below fBase_c's own default. --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual s32  Behavior();                           /* slot  6 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual s32  Render();                             /* slot  9 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void OnPendingDestroy();                   /* slot 12 */

    /* --- slots 18-35: eighteen further virtuals new at this class. --------

       These are NOT dActor_c's slots. dActor_c hangs off dBase_c directly;
       this class reaches dBase_c through dScene_c. The two are parallel
       extensions that happen to share indices, and thirteen of the names
       agree, so include/dActor_c.h is a useful NAMING hint -- never a
       signature authority. Where the two disagree, the ROM wins.

       Declared one slot at a time, lowest first. mwcc emits a vtable only as
       long as the slots it has been told about, so after declaring 18..k every
       descendant emits k+1 slots -- a byte-exact PREFIX of the cartridge table,
       never a disagreement. Declaring all eighteen at once would instead write
       the BASE body into every slot a descendant has not yet declared an
       override for, turning PARTIAL into DIFFERS across the whole family.

       Slot 18 -- MEASURED, not inferred:
         arity: 13 of the 24 independently decompiled descendant overrides read
           r1 and branch on it (dScMgTeresa_c takes an entirely different path
           when it is 0). Twelve unrelated classes do not read a garbage
           register by coincidence. The base's own body ignores it, but a base
           stub proves nothing either way -- an unused argument is simply never
           read -- so only an override that reads a parameter is evidence, and
           that evidence is a LOWER bound on the arity.
         return type: int. dScMgCoin_c::OnYoshiTryEat is a real member
           definition ending `return 0;`, so declaring void would have changed
           its bytes. The 24 free-function bodies are all written void, but
           the return type is not mangled, so they are unaffected. The base's
           own ROM body is a lone `bx lr` and sets nothing.
         name: from dActor_c.h:131, corroborated by
           config/arm9/overlays/ov006/symbols.txt, which already named
           dScMgCoin_c's slot-18 override `_ZN11dScMgCoin_c13OnYoshiTryEat*`
           -- recovered before the vtable walk that placed it at 18. Unlike
           slots 19-30, this slot carries NO `recovered name:` comment on
           either side; the name is inherited, not independently proven here.
           Only the signature is measured. dActor_c.h declares it with no
           parameter, which the measurement above contradicts. */
    virtual int  OnYoshiTryEat(int arg);               /* slot 18 */

    /* Slot 19 -- MEASURED, and dActor_c.h is wrong here too:
         arity: two of the eleven descendant overrides read r1, and both
           COMPARE it against small integer constants rather than
           dereferencing it -- dScMgJump_c does `if (sel == 0)`,
           dScMgBSC_c does `if (mode == 4) ... else if (mode == 5)`.
           Comparing a reference against 4 and 5 is meaningless, so the
           parameter is an int, not the `Player &` dActor_c.h:132 declares.
           Dereference-versus-compare is the discriminator whenever a word
           in r1 could be either: both occupy one register, so codegen
           alone cannot separate `Ei` from `ER6Player`.
         return type: int, and this one needs no argument -- the ov004 base
           body ends `return 1;` and all eleven overrides return a value.
         name: unlike slot 18, independently recovered. dScMgJump_c and
           dScMgBSC_c each carry a `recovered name: <class>_OnTurnIntoEgg`
           comment in their own legacy source, so the name here does not
           rest on dActor_c.h at all. */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    /* Slot 20 -- and this one has no name.  `Virtual50` is the placeholder
       include/dActor_c.h:133 already uses, spelled from the byte offset
       (slot 20 x 4 = 0x50).  All five bodies carry a
       `recovered name: <class>_Virtual50` comment, but every one of them is
       that same coined placeholder rather than a name read out of anything,
       so five of them are not five pieces of evidence.  Naming it would be
       inventing, so it keeps the placeholder until something real turns up.
         arity: no explicit parameters.  dScMg3DEsp_c and dScMgTeresa_c take
           nothing and tail-call `FreeGfxSlotsById(8)`; dScMgCup_c and
           dScMgSound_c read only `this`, passing `this + 0x4f38` on.  No
           override touches a second argument register.
         return type: NOT determined here, and said plainly rather than
           implied.  The ov004 base body is a bare `bx lr`, and all four
           overrides are single tail calls -- both emit identical code under
           `int` and under `void`, so the dereference-versus-compare trick that
           settled slot 19 has nothing to bite on.  This takes
           dActor_c.h:133's `int` as a hint that has held five times out of
           six; the count, and why it is a count and not an authority, is
           worked out under slot 21 below.  If a later override with an early
           return shows otherwise, that override is the evidence and this
           changes. */
    virtual int  Virtual50();                          /* slot 20 */
/* Slot 21 -- OnGroundPounded.  The name comes from all five bodies' own
   `recovered name: <class>_OnGroundPounded` comments AND from
   include/dActor_c.h:138, which is a different hierarchy that shares slot
   indices; two sources, but the second has already been measured wrong twice
   in this campaign (slot 18's arity, slot 19's parameter type), so treat it as
   a hint that agrees rather than as a second measurement.
     arity: no explicit parameters.  The ov004 base body is empty and none of
       the four overrides reads a second argument register -- dScMgBSC_c and
       dScMgCard_c both branch on `this->mHudScore` alone, and the two Memory
       classes tail-call on one field of `this`.  dActor_c.h:138 spells a
       `dActor_c &` here; nothing in these five bodies would emit differently
       with or without it, so it is not carried.
     return type: NOT determined by these five bodies.  All four overrides
       converge on a single tail call, and a tail call emits the same `b` under
       `int` and under `void`, so the discriminator that settled nothing at
       slot 20 finds nothing here either.  This takes `void` from
       dActor_c.h:138, whose comment records that slots 21, 24 and 27 were
       MEASURED to return void via an override with early returns -- in that
       hierarchy.
       How much that is worth is a count, not a rule, and an earlier draft of
       this comment got the count wrong.  It said dActor_c.h had been right on
       every return type this campaign checked.  It has not.  Of the
       eight dScMgBase_c slots with a body of their own that pins a return
       type, dActor_c.h's type matches seven and differs on one:
           18  int  / int   agree   sets r0 on a constant-return path
           19  int  / int   agree   sets r0 on a constant-return path
           22  int  / int   agree   OnAttacked1's body is `return 1;`
           23  int  / int   agree   OnAttacked2's body is `return 1;`
           24  int  / void  DIFFER  OnKicked's body ends `return 1;`
           25  int  / int   agree   OnPushed returns `mMenuOpen == 0`
           26  int  / int   agree   three bodies, three constants: 0, 1, 2
           27  void / void  agree   MEASURED here: bare return, sets no r0
       Slot 24 is the one that matters, because dActor_c.h names 24 as one of
       its three MEASURED voids -- and it is right about its own hierarchy;
       Stump and BigBrickBlock proved it there.  dScMgBase_c's slot-24 body
       sets r0 to 1 and reproduces byte-exact, so this hierarchy returns `int`
       at that same index.  Both measurements stand.  The two hierarchies
       simply do not hold the same function at slot 24: they are parallel
       branches that each began adding virtuals at 18 -- dActor_c off dBase_c
       directly, this class off dBase_c through dScene_c -- and the
       `recovered name:` comments that make the slots look paired were assigned
       BY that index, so they cannot also be evidence for it.
       So dActor_c.h transfers no better on return types than on parameter
       lists; it is seven-for-eight rather than wrong-every-time, which is why
       this line still follows it where the arity line above does not.
       Seven-for-eight is the whole case for `void` here and is offered as a hint,
       not a measurement.  Flipping all four overrides between `int` and `void`
       was tried and moves no ROM byte, so nothing in the cartridge rides on
       the choice -- but a later override with an early return would settle it,
       and that override would outrank this count.
       Slot 27 is now that case, for its own index only.  dScMgBase_c's own
       body there takes an early `popne {r4,lr}; bxne lr` with nothing setting
       r0 on either path out, so 27's `void` is MEASURED and the row above says
       so rather than `agree`.  It settles 27 and nothing else; the other seven
       rows are still the count.
       Slot 28 is the opposite case and the first of its kind: NO body pins it
       at all.  Neither dScMgBase_c's nor dScMgSlot1_c's sets r0 deliberately,
       and all three callers that dispatch through vtable+0x70 throw the result
       away -- two tail-call it out without reading it, the third overwrites r0
       on the next instruction.  It gets no row above, and its `int` rests on
       this count and on nothing else. */
    virtual void OnGroundPounded();                    /* slot 21 */
/* Slot 22 -- OnAttacked1.  Name from the ov004 body's own
   `recovered name: dScMgBase_c_OnAttacked1` comment, agreeing with
   include/dActor_c.h:139 on the parallel branch.
     overrides: NONE.  Every one of the 32 descendants inherits this slot, so
       this declaration widens 30 emitted tables by a word and adds no other
       file.  A slot with no overrides is the one case where the widened-table
       COUNT is the entire check: nothing else would notice if the base
       declaration failed to take.
     arity: no explicit parameters.  There is no override to read a second
       argument register, so the only evidence is the base body, which reads
       none.  dActor_c.h:139 spells a `dActor_c &`; it has been wrong on every
       parameter list this campaign has measured, and an unread parameter would
       leave no trace either way, so it is not carried.
     return type: int, MEASURED.  The body is `return 1;` -- it sets r0, which
       a `void` function would not, so this one is not a coin-flip the way slot
       21's tail calls were. */
    virtual int  OnAttacked1();                        /* slot 22 */
    /* Slot 23 -- OnAttacked2.  Name from the ov004 body and from all
       three overrides' own `recovered name: <class>_OnAttacked2` comments,
       agreeing with include/dActor_c.h:140.
         arity: no explicit parameters, MEASURED.  The base body reads no
           argument register at all, and none of the three overrides touches
           a second one.  dActor_c.h:140 spells a `dActor_c &`; it has been
           wrong on every parameter list this campaign has measured, so it
           is not carried.  The two flat-C callers do not even agree on a
           prototype -- dScMgSnowball_c hands the base a `void *`,
           dScMgTrampoline2_c calls it with nothing -- which is only
           possible because the callee ignores both.  That is corroboration,
           not the measurement.
         return type: int, MEASURED twice.  The base body is `return 1;`,
           and dScMgTrampoline2_c's override has four early `return 0;`
           paths and a final `return 1;` -- exactly the early-return shape
           include/dActor_c.h names as the only thing that can separate
           `int` from `void`, and here it comes down on int. */
    virtual int  OnAttacked2();                        /* slot 23 */
    /* Slot 24 -- OnKicked.  Name from all five bodies' own
       `recovered name: <class>_OnKicked` comments; include/dActor_c.h:141
       spells the same name at the same index on the parallel branch.
         arity: no explicit parameters, MEASURED.  The base body reads only
           `this`, and so does every override.  No body touches a second
           argument register and every call in the chain passes one pointer
           and nothing else.  dActor_c.h:141 spells a `dActor_c &other`; it
           has been wrong on every parameter list this campaign has
           measured, so it is not carried.
         return type: int, MEASURED, and this is the slot the table under
           slot 21 flagged in advance as the one where dActor_c.h's `void`
           and this hierarchy genuinely disagree.  The base body ends
           `return 1;`.  dScMgD3DBase_c, dScMgTrampoline_c and
           dScMgTrampoline2_c each guard on the result of the call to their
           own base and `return 0` early -- the early-return shape that is
           the only thing separating `int` from `void` -- and
           dScMgSnowball_c returns `<base call> != 0`, which cannot be
           written at all against a void callee.  Four independent
           witnesses; the cartridge comes down on int.  dActor_c.h's
           measurement of `void` at index 24 stands for ITS branch.  The two
           hierarchies simply do not hold the same function here.
         overrides: SIX tables, FOUR declarations.  dScMgJump_c and
           dScMgJump2_c point at dScMgD3DBase_c's own body (0x020e6e78) and
           override nothing themselves, so declaring it on their shared base
           is what reproduces their slots; a declaration on either child
           would invent an override the cartridge does not have.  The bodies
           spell the chain out themselves -- Trampoline and Trampoline2 both
           call 0x020e6e78, which calls the base's 0x020ae140.
         NAME CORRECTION: 0x020e6e78 carried `recovered name:
           dScMgJump2_c_OnKicked`.  It is dScMgD3DBase_c's.  A body that
           appears in a class's table AND in both its children's is supplied
           by that class; the recovered name was assigned from one child's
           table without looking at the other two. */
    virtual int  OnKicked();                           /* slot 24 */
    /* Slot 25 -- OnPushed.  Name from all six bodies' own
       `recovered name: <class>_OnPushed` comments; include/dActor_c.h:142
       spells the same name at the same index on the parallel branch.
         arity: no explicit parameters, MEASURED.  The base body reads one
           field of `this` and nothing else, and all five overrides are a
           single call passing that same one pointer through.  No body
           touches a second argument register.  dActor_c.h:142 spells a
           `dActor_c &other`; it has been wrong on every parameter list this
           campaign has measured, so it is not carried.
         return type: int, MEASURED, and not a coin flip: the base body is
           `return mMenuOpen == 0;`, which computes a value into r0 -- a
           `void` function would not.  Every one of the five overrides then
           writes `<base call> != 0`, which cannot be written at all against
           a void callee.  Six witnesses.  dActor_c.h:142 says `int` too,
           which is the first slot in this campaign where it agrees on a
           return type it had not already been credited with; the table
           under slot 21 is updated to match.  (Slot 26 lands in that
           table too, taking it to seven-for-eight.)
         overrides: SEVEN tables, FIVE declarations.  dScMgJump_c and
           dScMgJump2_c point at dScMgD3DBase_c's own body (0x020e6e54) and
           override nothing themselves, exactly as at slot 24, so the
           declaration goes on their shared base and neither child gets one.
           dScMgSmartball_c is new here -- it inherits 22, 23 and 24 and
           overrides 25 -- which is why the override set is read off the ROM
           for every slot rather than carried forward from the last one.
         NAME CORRECTION, the second on this class: 0x020e6e54 carried
           `recovered name: dScMgJump2_c_OnPushed`.  It is
           dScMgD3DBase_c's, on the same argument that settled 0x020e6e78 at
           slot 24 -- word 25 of _ZTV14dScMgD3DBase_c, _ZTV11dScMgJump_c and
           _ZTV12dScMgJump2_c all point here, and a body in a class's table
           AND in both its children's is supplied by that class.  Both
           misattributions name dScMgJump2_c, which is what a recovery pass
           reading tables in name order would produce. */
    virtual int  OnPushed();                           /* slot 25 */
    /* Slot 26 -- OnHitByCannonBlastedChar.  Name from the ov004 body's own
       `recovered name: dScMgBase_c_OnHitByCannonBlastedChar` comment, agreeing
       with include/dActor_c.h:143 on the parallel branch.
         return type: int, MEASURED THREE TIMES, and this is the strongest
           return-type evidence the campaign has had.  The three bodies are
           three DIFFERENT constants -- ov004:0x020b04e0 is `mov r0,#0; bx lr`,
           dScMgSingle3DBase_c's ov006:0x0210a600 is `mov r0,#1`, and
           dScMgD3DBase_c's ov006:0x020e6e4c is `mov r0,#2`.  A void function
           cannot return three different values, and the callers have to be
           able to tell them apart.  dActor_c.h:143 says `int` as well.
         arity: no explicit parameters, MEASURED.  All three bodies are two
           instructions and read NO argument register at all -- not even
           `this`.  dActor_c.h:143 spells a `dActor_c &other`; it has been
           wrong on every parameter list this campaign has measured, so it is
           not carried.
         overrides: NINETEEN tables, TWO declarations -- the widest gap in the
           campaign, and the one slot where the table count would have been
           badly misleading on its own.  The RTTI graph is what closes it:
           dScMgSingle3DBase_c is the shared base of thirteen of those tables
           and dScMgD3DBase_c of four more (build/rtti.json, every edge at
           offset 0), so seventeen classes point at an ancestor's body and
           declare nothing themselves.  Declaring the slot on any of the
           seventeen would invent an override the cartridge does not have.
         NAME CORRECTION, the third on dScMgD3DBase_c: 0x020e6e4c carried
           `recovered name: dScMgTrampoline2_c_OnHitByCannonBlastedChar`.
           Five tables reference it -- dScMgD3DBase_c's and all four of its
           children's -- so it is dScMgD3DBase_c's.  This one also disposes of
           a guess made when reviewing slot 25: that the two earlier
           misattributions both landing on dScMgJump2_c meant a recovery pass
           reading tables in name order.  This one names dScMgTrampoline2_c,
           so whatever the pass did, it was not that.
         The matching error one level down was already found and corrected in
           src/actors/dScMgSingle3DBase_c.cpp: 0x0210a600 had been recovered
           as dScMgFlower_c's.  Three of the same mistake on this one slot is
           what a per-table name assignment produces whenever a body is shared,
           and it is why every slot in this campaign reconciles DISTINCT
           ADDRESSES against declarations and the table count separately. */
    virtual int  OnHitByCannonBlastedChar();           /* slot 26 */
    /* Slot 27 -- OnHitByMegaChar.  Name from the ov004 body's own
       `recovered name: dScMgBase_c_OnHitByMegaChar` comment, agreeing with
       include/dActor_c.h:144 on the parallel branch and with dScMgSlot1_c's
       independently recovered override.
         return type: void, MEASURED, and this is the one slot where the table
           above records dActor_c.h's `void` as a measurement here rather than
           a hint.  ov004:0x020af27c takes an early exit at 0x020af290 --
           `popne {r4,lr}; bxne lr` -- with r0 still holding the field it has
           just loaded and tested, and nothing sets a return value on either
           path out.
         arity: no explicit parameters, MEASURED TWICE, and it matters more
           here than at any other slot.  Both real bodies open `mov r4, r0` and
           then WRITE r1 before ever reading it -- the base fills the menu-item
           coordinates through r1 from 0x020af2a0 onwards, and dScMgSlot1_c's
           override at ov006:0x0210c4dc zeroes r0/r1 for SetSubBg1Offset.  No
           second argument register is live on entry to either.  dActor_c.h:144
           spells `Player &player` and include/dScMgSlot1_c.h had copied it.
         overrides: SIX tables, TWO declarations.  dScMgD3DBase_c's own body at
           ov006:0x020e6d98 backs its table and all four of its children's;
           dScMgSlot1_c has its own at ov006:0x0210c4dc.
         THE RECONCILIATION, and the reason this slot is not the shape of the
           twenty-six before it.  dScMgSlot1_c already carried this member,
           declared as a NEW virtual with dActor_c.h's parameter list and
           landing on index 27 by arithmetic because the base stopped at 26.
           Declaring it here without touching that header would have made the
           two DIFFERENT functions: this one would take 27, dScMgSlot1_c's
           would become a new slot at 28, OnHitFromUnderneath would be pushed
           to 29, and _ZTV12dScMgSlot1_c would go straight back to DIFFERS --
           with rombuild green throughout.  Both declarations change in this
           one commit, and romdata_check is what says they were reconciled.
         NAME CORRECTION, the fourth on dScMgD3DBase_c and the fifth in this
           campaign: 0x020e6d98 carried
           `recovered name: dScMgTrampoline2_c_OnHitByMegaChar`.  Five tables
           reference it -- dScMgD3DBase_c's and all four of its children's --
           so it is dScMgD3DBase_c's.  Its twelve bytes are a long-branch
           veneer into this class's own body, which is what a forwarding
           override compiles to from ov006; the vtable word is NOT a linker
           artifact, because the twenty-six tables that do not override this
           slot hold 0x020af27c directly. */
    virtual void OnHitByMegaChar();                    /* slot 27 */
    /* Slot 28 -- OnHitFromUnderneath.  Name from the ov004 body's own
       `recovered name: dScMgBase_c_OnHitFromUnderneath` comment, agreeing with
       include/dActor_c.h:145 on the parallel branch and with dScMgSlot1_c's
       independently recovered override.
         return type: void, and slot 28 is the first in this campaign whose
           return type NO body pins, which is why it gets no row in the table
           above: that table is the eight slots whose own bodies DO pin one.
           void is chosen because nothing contradicts it and because it is the
           only spelling both definitions can honour without inventing a result
           neither body computes.  ov004:0x020af04c leaves r0 holding whatever it last tested or
           last called: the early exit at
           `cmp r0,#0; popeq {r4,lr}; bxeq lr` returns the zero it has just
           compared, and the fall-through returns whatever Enable3dEngines
           left.  Neither is a deliberate result.  Nor does any caller settle
           it.  ov004 and ov006 hold exactly three
           `ldr rN,[rM,#0x70]; blx rN` dispatch sites: 0x020aedec and
           0x020aee90 inside func_ov004_020aeb24, and 0x020b06cc inside
           dScMgBase_c::BeforeBehavior.  The first two are tail calls --
           `blx r1; add sp,sp,#0x10; pop {r4,lr}; bx lr` -- which hand r0 back
           to their own caller without ever reading it, and the third branches
           to a shared epilogue that overwrites r0 with `add r0,r4,#0x4000`
           before anything can use it.  Three sites, three discarded results.
           include/dActor_c.h:145 spells `int`, and its RETURN types have held
           up where its parameter lists have not -- but a hint from a parallel
           hierarchy is not a reason to declare a type no body here produces.
           `void` compiles to the same bytes, and unlike `int` it is a type
           both definitions can actually satisfy: dScMgSlot1_c's override
           computes no result and would otherwise fall off the end of a
           non-void function, which is undefined behaviour whatever the
           cartridge happens to leave in r0.  If a caller is ever found
           consuming this slot, that measurement -- not dActor_c.h -- is what
           should change it back.
         arity: no explicit parameters, MEASURED ONCE rather than twice.  The
           base body opens `mov r4, r0` and then writes r1 with
           `add r1, r4, #0x4000` before ever reading it, and reads no other
           argument register anywhere.  dScMgSlot1_c's override at
           ov006:0x0210c4b8 cannot corroborate it the way its slot-27 override
           did: it calls this body as its very first act, so a second argument
           would pass through r1 untouched and leave no trace either way.
           dActor_c.h:145 spells `dActor_c &other` and include/dScMgSlot1_c.h
           had copied it.
         overrides: SIX tables, TWO declarations -- the same shape as slot 27.
           dScMgD3DBase_c's own body at ov006:0x020e6d8c backs its table and
           all four of its children's; dScMgSlot1_c has its own at
           ov006:0x0210c4b8.
         THE RECONCILIATION, for the second and last time in this class.
           dScMgSlot1_c already carried this member, declared as a NEW virtual
           with dActor_c.h's parameter list and landing on index 28 by
           arithmetic because the base stopped at 27.  Declaring it here
           without touching that header would have made the two DIFFERENT
           functions: this one takes 28, dScMgSlot1_c's becomes a new slot at
           29, and _ZTV12dScMgSlot1_c goes straight back to DIFFERS -- with
           rombuild green throughout.  Both declarations change in this one
           commit.  dScMgSlot1_c now declares nothing mwcc has to number for
           itself, so this is the last time the trap is loaded THERE; it can
           still be loaded anywhere else a descendant declared a base slot
           early, which is the one thing to check before each of slots 29-35.
         NAME CORRECTION, the fifth on dScMgD3DBase_c and the sixth in this
           campaign: 0x020e6d8c carried
           `recovered name: dScMgTrampoline2_c_OnHitFromUnderneath`.  Five
           tables reference it -- dScMgD3DBase_c's and all four of its
           children's -- so it is dScMgD3DBase_c's.  Its twelve bytes are a
           long-branch veneer into this class's own body, `ldr ip, [pc];
           bx ip; .word 0x020af04c`, which is what a forwarding override
           compiles to from ov006; the vtable word is NOT a linker artifact,
           because the twenty-six tables that do not override this slot hold
           0x020af04c directly. */
    virtual void OnHitFromUnderneath();                /* slot 28 */

    /* Slots 29-35 are added the same way: one slot per change, together with
       every descendant override of that slot. Until then they stay undeclared
       and the emitted tables stop at slot 28.

       THE OCCUPIED-SLOT TRAP IS NO LONGER LOADED IN dScMgSlot1_c -- slots 27
       and 28 were both of its early declarations and both are reconciled --
       but it is a property of the FAMILY, not of that one class.  Before
       declaring slot N, check every descendant header for a virtual that
       already lands on N by arithmetic.  Declaring the base's while a
       descendant keeps a different signature makes them two different
       functions, pushes the descendant's own down one index, and regresses its
       table to DIFFERS with rombuild still green -- only romdata_check sees
       it.  Reconcile in the same commit, and remember the rename has to reach
       config/converted-baseline.json (the ratchet has no rename detection) and
       any prose path that names the file. */

    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    s32 mTimeLimit;         /* 0x058 -- 60 or 120 from the scene ID */
    s32 mFrameCounter;      /* 0x05c -- wraps every 40 frames */
    s32 unk_060;            /* 0x060 */
    s32 unk_064;            /* 0x064 */
    u8  pad_068[0x38];
    s32 unk_0a0;            /* 0x0a0 */
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x4];
    s32 mHudScore;          /* 0x0b4 -- zeroed by BeforeInitResources; the
                                value func_ov004_020adb1c writes into the HUD
                                counter word. Leaves keep their own tallies and
                                copy the result here */
    s32 unk_0b8;            /* 0x0b8 */
    u32 unk_0bc;            /* 0x0bc */
    u16 mPromptBlinkTimer;  /* 0x0c0 -- free-runs 0..0x2f; the prompt shows
                                during the first 0x18 frames of each cycle */
    u8  unk_0c2;            /* 0x0c2 -- constructor seeds this to 1 */
    u8  mPromptEnabled;     /* 0x0c3 -- nothing is drawn while this is 0 */
    u8  mPromptBlinkCount;  /* 0x0c4 -- after 4 blink cycles the per-language
                                prompt sprite stays up every frame */
    u8  pad_0c5[0x3];
    s32 unk_0c8;            /* 0x0c8 */
    dMgState_c mStateController; /* 0x0cc -- minigame UI state controller */
    dMgPsOpt_c mTouchOptions; /* 0x0f4 -- eight polymorphic touch icons */
    u32 mSavedMainBgBits;   /* 0x21c -- func_ov004_020af094 saves data_0209d45c
                                here and func_ov004_020aeed8 restores it */
    u32 mSavedSubBgBits;    /* 0x220 -- the same pair for data_0209d454 */
    u32 mSavedScreenSwap;   /* 0x224 -- bit 15 of POWCNT1 (0x4000304), saved and
                                restored by the same two functions */
    u8  pad_228[0x4400];
    s32 mMenuOpen;          /* 0x4628 -- the three-item overlay menu is up;
                                BeforeRender skips the normal render pass and
                                draws that instead */
    s32 unk_462c;           /* 0x462c */
    s32 unk_4630;           /* 0x4630 */
    s16 mMenuItem0X;        /* 0x4634 */
    s16 mMenuItem0Y;        /* 0x4636 */
    s16 mMenuItem1X;        /* 0x4638 */
    s16 mMenuItem1Y;        /* 0x463a */
    s16 mMenuItem2X;        /* 0x463c */
    s16 mMenuItem2Y;        /* 0x463e */
    s32 mMenuCursorPhase;   /* 0x4640 -- drives the highlighted item's pulse */
    u8  pad_4644[0x2];
    s16 mMenuCursor;        /* 0x4646 -- highlighted item, -1 for none */
    s32 unk_4648;           /* 0x4648 */
    s32 unk_464c;           /* 0x464c */
    u8  pad_4650[0x4];
    s32 unk_4654;           /* 0x4654 */
    s32 unk_4658;           /* 0x4658 */
    u8  unk_465c;           /* 0x465c */
    u8  pad_465d[0x1];
    s16 mSceneKind;         /* 0x465e -- table-selected from param1's high byte */
};

/* The nested callback class the ROM's type graph records for this scene,
   deriving from dGraph_c::callback_c (include/dGraph_c.h). It is registered
   with the owning scene as its context; mScene at +0x4 is that back-pointer,
   read by all four slots. Declaring the class adds no storage to
   dScMgBase_c -- the object itself is not a member of this class.

   The slots are declared virtual, matching dGraph_c::callback_c's own
   declarations (include/dGraph_c.h): the base supplies the vptr at +0x0,
   so the derived fields fall where the ROM puts them, and strict object
   isolation discards the vtable/RTTI passengers the function range does
   not own. */
class dScMgBase_c::graphCallback_c : public dGraph_c::callback_c {
public:
    dScMgBase_c *mScene;                             /* 0x04 */

    virtual int GraphCallback0();                            /* slot 0 */
    virtual int GraphCallback1();                            /* slot 1 */
    virtual int GraphCallback2();                            /* slot 2 */
    virtual int GraphCallback3();                            /* slot 3 */
};

/* A floor, not a claim the object ends here: 0x465c is the last field any
   matched body has observed. See notes/minigame-provenance.md. */
typedef char dScMgBase_c_size_must_be_0x4660[sizeof(dScMgBase_c) == 0x4660 ? 1 : -1];

#endif
