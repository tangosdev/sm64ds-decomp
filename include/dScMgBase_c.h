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

ALL EIGHTEEN ARE DECLARED (2026-08-31). This class and all 32 of
       its descendants emit their full 36-slot vtables from source; none of
       them is a prefix any more.

       They were declared one slot at a time, lowest first, and that ordering
       is why it worked. mwcc emits a vtable only as long as the slots it has
       been told about, so after declaring 18..k every descendant emitted k+1
       slots -- a byte-exact PREFIX of the cartridge table, never a
       disagreement, so every intermediate commit was shippable. Declaring all
       eighteen at once would instead have written the BASE body into every
       slot a descendant had not yet declared an override for, turning PARTIAL
       into DIFFERS across the whole family. Anything ADDED here from now on
       is past the cartridge's own table and has no such safety net.

       Slot 18 -- MEASURED, not inferred:
         arity: 13 of the 24 independently decompiled descendant overrides read
           r1 and branch on it (dScMgTeresa_c takes an entirely different path
           when it is 0). Twelve unrelated classes do not read a garbage
           register by coincidence. The base's own body ignores it, but a base
           stub proves nothing either way -- an unused argument is simply never
           read -- so only an override that reads a parameter is evidence, and
           that evidence is a LOWER bound on the arity.
         return type: void, MEASURED -- and it corrects the `int` this block
           used to carry, which rested entirely on dScMgCoin_c::OnYoshiTryEat
           ending `return 0;`.  That reasoning does not survive the bytes.
           Coin's tail is

               bl <func_ov004_020adc1c>      ; result lands in r0
               add r1, r4, #0x5000           ; r0 is LIVE, so the address is r1
               str r0, [r1, #0x1d4]
               mov r0, #0                    ; r0 now free -- the constant 0
               str r0, [r1, #0x1c8]

           The address goes to r1 because r0 still held the call's result, not
           because r0 was reserved for a return value, and the trailing
           `mov r0,#0` is the constant being stored to unk_51c8.  A `return 0;`
           written after it costs nothing, so Coin is byte-identical under
           BOTH spellings and pins neither.

           Six overrides do pin it, and they pin void.  Converting the
           free-function bodies to real members forces the declaration on them,
           and as `int` mwcc reserves r0 and shifts every register at their
           exits by one -- dScMgSlot1_c 3 words of 19, dScMgMemory_c and
           dScMgMemory2_c 5 of 24 each, dScMgRoulette_c 3 of 63,
           dScMgPanel_c 4 of 68, dScMgSlot3_c 84 of 205:

               ROM   add r0, r4, #0x4000 / mov r1, #0 / str r1,[r0,#0x6b4]
               int   add r1, r4, #0x4000 / mov r2, #0 / str r2,[r1,#0x6b4]

           Spelled void all six are byte-exact and the whole 106/106 holds.
           mwcc rejects the halfway position outright -- a void override of an
           int virtual is `differs from virtual base function ... in return
           type only`, an error, not a warning -- so the base and all 31
           declarations move together.  The return type is not part of the
           Itanium mangled name, so no symbol, vtable or config entry moves.
           The base's own ROM body is a lone `bx lr` and sets nothing.
         name: from dActor_c.h:131, corroborated by
           config/arm9/overlays/ov006/symbols.txt, which already named
           dScMgCoin_c's slot-18 override `_ZN11dScMgCoin_c13OnYoshiTryEat*`
           -- recovered before the vtable walk that placed it at 18. Unlike
           slots 19-30, this slot carries NO `recovered name:` comment on
           either side; the name is inherited, not independently proven here.
           Only the signature is measured. dActor_c.h declares it with no
           parameter, which the measurement above contradicts. */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

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
         return type: int, A HINT -- and slot 28 is the first in this campaign
           whose return type NO body pins, which is why it gets no row in the
           table above: that table is the eight slots whose own bodies DO pin
           one.  ov004:0x020af04c leaves r0 holding whatever it last tested or
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
           `int` is include/dActor_c.h's, and its RETURN types have held up
           where its parameter lists have not; `void` compiles to the same
           bytes.
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
    virtual int  OnHitFromUnderneath();                /* slot 28 */
    /* Slot 29 -- OnAimedAtWithEgg.  Name from the ov004 body's own
       `recovered name: dScMgBase_c_OnAimedAtWithEgg` comment and from
       include/dActor_c.h:146 at the same index -- and this is the slot where
       that pairing stops being two witnesses.  See the caution below.
         return type: int, A HINT, the second consecutive slot that no body
           pins, so it gets no row in the table above.  ov004:0x020af094 has
           two exits and neither sets a result: the early one is
           `cmp r0,#2; addeq sp,sp,#8; popeq {r4-r8,lr}; bxeq lr`, returning
           the 2 it has just compared against slot 26's result, and the
           fall-through returns whatever MultiCopy_Int last left in r0.
           No caller settles it either, and here that is MEASURED rather than
           assumed.  Scanning arm9 and all 103 overlays for the dispatch
           pattern -- `ldr rN,[rM,#0x74]` immediately followed by `blx rN`,
           not any load at +0x74, which also matches an ordinary field read --
           finds 35 sites, of which exactly ONE lies in ov004 or ov006, the
           two modules this class and all thirty-two of its descendants live
           in.  That one is ov004:0x020ae178, inside dScMgBase_c::OnKicked
           (slot 21), and the instruction after the `blx` is
           `add r0,r4,#0x4000`: the result is dead before anything can read
           it, and OnKicked goes on to return a literal 1.  The other
           thirty-four sites are in ov025, ov071, ov077, ov079, ov081, ov091,
           ov096 and ov100 -- enemy overlays, dActor_c's branch -- which is
           the sharpest measurement yet of how little the shared index means.
         arity: no explicit parameters, MEASURED THREE TIMES, and the third
           witness is destructive rather than circumstantial.  The base opens
           `ldr r1,[r0]; mov r4,r0`, writing r1 out of `this` before any read,
           and writes r2, r3 and ip before reading them too.
           dScMgD3DBase_c's override at ov006:0x020e6d24 writes
           `mov r1,#0x100; mov r2,r1` and reads neither on entry.
           dScMgSlot3_c's at ov006:0x0210aa3c settles it: it loads r2 and ip
           from its literal pool, CLOBBERS r1 with a masked read of the sub
           display-control register, and only then tail-branches `bx ip` into
           0x020af094.  A second argument passed in r1 would reach the base as
           a display-control word, so the base cannot be reading one.
           include/dActor_c.h:146 declares no parameter here either -- the
           first time in this campaign its parameter list has agreed.
         overrides: SIX tables, TWO declarations, and NO reconciliation.  This
           is the first slot since 26 where no descendant had already declared
           the member early, so nothing has to be un-declared alongside it.
           dScMgD3DBase_c's own body at ov006:0x020e6d24 backs its table and
           all four of its children's; dScMgSlot3_c, which hangs off
           dScMgSingle3DBase_c rather than off D3DBase, has its own at
           ov006:0x0210aa3c.  dScMgSingle3DBase_c itself does not override.
         NAME CORRECTION, the sixth on dScMgD3DBase_c and the seventh in this
           campaign: 0x020e6d24 carried
           `recovered name: dScMgTrampoline2_c_OnAimedAtWithEgg`.  Five tables
           reference it, so it is dScMgD3DBase_c's.  Unlike slots 26, 27 and
           28 this one is not a twelve-byte veneer -- it is a real 0x68-byte
           body that loads a sub-screen OBJ palette and claims two VRAM banks
           before calling this class's, which is why it also had to change its
           include and the type it casts `this` to.
         SEMANTIC CAUTION, the first of its kind on this class.  Every method
           name here is transplanted from include/dActor_c.h at the same
           index.  Nothing in the cartridge carries a method name for
           dScMgBase_c -- its RTTI carries class names only -- so the
           `recovered name:` comments are themselves index transplants, not
           independent recoveries.  There was never a second witness, only the
           same witness reached twice.  Until now that cost nothing.  Here the
           ROM contradicts the transplant outright.  The sole ov004 dispatch
           site is guarded on an EDGE of mMenuOpen (+0x4628): OnKicked compares
           mMenuOpen against unk_462c and, only when they differ, calls slot 30
           if mMenuOpen is zero and THIS slot if it is not, then copies one
           into the other.  And this body does exactly what that guard implies.
           It saves POWCNT1's screen-swap bit and the two BG-enable bytes into
           mSavedScreenSwap, mSavedMainBgBits and mSavedSubBgBits; clears every
           layer- and window-enable bit in both DISPCNTs; saves 0x400 bytes of
           BG palette out of palette RAM into a buffer at +0x4228 and loads the
           menu's own palettes over it; and saves 0x2000 bytes of sub-screen
           OBJ VRAM at 0x06606000 into +0x2228 before decompressing a
           per-language image over that.  Save the screen, then draw over it:
           that is the three-item overlay menu going UP.  It is not a Yoshi
           egg.  The name is kept because it is the only one in evidence and
           because coining a replacement would put an unverifiable identifier
           on a byte-exact symbol -- but it is kept as a LABEL, not as a claim,
           and slots 18-30 should be read in that light.
         AND A WARNING FOR SLOT 30, measured here because it is cheaper to
           record now than to rediscover: include/dActor_c.h:151 declares slot
           30 returning a Vector3 BY VALUE.  That cannot be transplanted.  At
           ov004:0x020ae168 slot 30 is dispatched with r0 still holding `this`
           and r1 holding the loaded function pointer; a 12-byte return would
           put a hidden result pointer in r0 and `this` in r1 under AAPCS.
           Whatever slot 30 returns here, it is not a Vector3. */
    virtual int  OnAimedAtWithEgg();                   /* slot 29 */
    /* Slot 30 -- OnAimedAtWithEggReturnVec.  Name transplanted from
       include/dActor_c.h:151 at the same index, by way of the ov004 body's own
       `recovered name:` comment, which is the same transplant reached a second
       time rather than a second witness.  Both halves of it are wrong here and
       both are MEASURED wrong; it is kept as a label, for the reasons under
       slot 29.
         return type: void, and this is the one slot in the whole 18-35 range
           whose return type the BYTES settle rather than merely permit.  It
           was declared `int` here as a hint until the base body became a real
           member definition; that spelling is now refuted.
           ov004:0x020aeed8 sets no result on either exit -- the early one is a
           bare return after `cmp r0,#2` and the fall-through ends in a
           read-modify-write of DISPCNT.  Declaring the member `int` and
           letting control fall off the end costs no instruction, but it does
           reserve r0 as the result register, and the closing block wants four
           scratch registers.  mwcc then allocates r1-r4 where the ROM
           allocates r0-r3 -- fourteen of ninety-three words differ, with no
           other change to the source:
               ROM   ldr r0,[pc,#0x38] / mov r3,#0x4000000 / ldr r1,[r3]
               int   ldr r1,[pc,#0x38] / mov r4,#0x4000000 / ldr r2,[r4]
           Spelled `void` the function is byte-exact.  Contrast slots 31, 32,
           35 and the OnXxx slots, where `int` costs nothing because r0 is not
           contended at the exit -- there the hint stands unrefuted, which is
           NOT the same as confirmed.  decl_common.h:2321 and both overrides
           already said void.
           Neither dispatch site reads a result
           either, and that is measured, not assumed: dScMgBase_c::OnKicked
           (slot 21) is the only caller of this slot anywhere in ov004 or ov006
           -- see the whole-image `ldr rN,[rM,#0x78]` + `blx rN` scan under slot
           29 for the method -- and at ov004:0x020ae16c the very next
           instruction after the `blx`, reached through `b 0x020ae180`, is
           `add r0,r4,#0x4000`.  The result is dead one instruction later, on
           both arms of that branch.
         `ReturnVec` IS REFUTED, twice over.  include/dActor_c.h:151 declares
           slot 30 returning a Vector3 BY VALUE, and in THAT hierarchy that is
           itself a measurement (of 0x020100dc) -- so this is a genuine
           divergence between the two branches, not a defect in dActor_c.h.  In
           THIS one it cannot hold.  A 12-byte return is returned in memory
           under AAPCS: r0 would carry a hidden result pointer and `this` would
           move to r1.  At the dispatch, ov004:0x020ae168, r0 still holds
           `this` -- OnKicked's entry does `mov r4,r0` and never rewrites r0 --
           and r1 holds the loaded function pointer.  And the base's own body
           opens `ldr r1,[r0]`, reading its vptr out of r0.  Whatever this slot
           returns, it is not a Vector3, and it is not returned in memory.
           And the contrast is the whole lesson of this campaign in one pair
           of slots: over in dActor_c, 29 and 30 are a lock-on RADIUS and the
           AIM POINT derived from it, and 30 asks 29 virtually.  Here 29 and
           30 are a screen SAVE and the RESTORE that undoes it.  Same two
           indices, same shared ancestor, nothing else in common.  An index
           is not a name.
         arity: no explicit parameters, MEASURED THREE TIMES, the third
           destructively.  The base writes r1 out of `this` before reading it.
           dScMgD3DBase_c's override at ov006:0x020e6cac loads r1 from `this`
           for its first call and never reads an incoming one.
           dScMgSlot3_c's at ov006:0x0210aa10 CLOBBERS r1 with a masked read of
           the sub display-control register 0x0400000A before falling into
           0x020aeed8 -- a second argument passed in r1 would reach the base as
           a display-control word.
         overrides: SIX tables, TWO declarations, NO reconciliation -- the same
           shape as 29, and for the same reason: nothing in the family declared
           this member early.  dScMgD3DBase_c's body at ov006:0x020e6cac backs
           its own table and all four of its children's; dScMgSlot3_c, which
           hangs off dScMgSingle3DBase_c, has its own at ov006:0x0210aa10.
         NAME CORRECTION, the seventh on dScMgD3DBase_c and the eighth in this
           campaign: 0x020e6cac carried
           `recovered name: dScMgTrampoline2_c_OnAimedAtWithEggReturnVec` and
           included dScMgTrampoline2_c.h.  Five tables reference it, so it is
           dScMgD3DBase_c's.  Like 29's it is a real body, not a veneer, so the
           include and the type it casts `this` to had to change with the name.
         WHAT IT ACTUALLY DOES, and this is the finding that settles slot 29
           rather than merely cautioning about it: this body is the WORD-FOR-
           WORD MIRROR of 0x020af094.  Slot 29 saves POWCNT1's screen-swap bit
           into mSavedScreenSwap and the two BG-enable bytes into
           mSavedMainBgBits / mSavedSubBgBits, blanks both DISPCNTs, spools
           0x400 bytes of BG palette into the buffer at +0x4228 and 0x2000
           bytes of sub-screen OBJ VRAM at 0x06606000 into +0x2228, then draws
           the menu over the top.  This one restores every one of those, in
           reverse: POWCNT1 bit 15 from mSavedScreenSwap, both DISPCNT 0xe000
           fields, the palettes back out of +0x4228 (with +0x200 going to the
           sub screen), the 0x2000-byte VRAM block back to 0x06606000, and the
           two BG-enable bytes back out of mSavedMainBgBits / mSavedSubBgBits
           and into both DISPCNTs' 0x1f00 fields.  Save-then-draw at 29,
           restore at 30.  OnKicked picks between them on an EDGE: it compares
           mMenuOpen (+0x4628) against unk_462c (+0x462c), does nothing when
           they agree, calls THIS slot when mMenuOpen has fallen to zero and
           slot 29 when it has risen, then latches mMenuOpen into unk_462c.
           unk_462c is that edge latch and nothing else.  Slot 29 is the
           three-item overlay menu going up and slot 30 is it coming back down;
           neither has anything to do with a Yoshi egg or with a vector. */
    virtual void OnAimedAtWithEggReturnVec();          /* slot 30 */
    /* Slot 31 -- Virtual7C, and the name is a deliberate NON-name.  This is the
       first slot above dActor_c's table, and reaching it settles what the
       thirteen slots below it only hinted at.
         WHY THE NAME CHANGES HERE.  Slots 18-30 carry dActor_c's names because
           they sit at dActor_c's indices.  That is the whole of the argument,
           and at this slot it runs out: include/dActor_c.h stops at 30.  What
           the two tables actually share is their BASE, not their tail --
           include/fBase_c.h declares slots 0-15 plus the destructor pair at
           16/17, and dBase_c adds no virtual of its own, so both branches begin
           appending at 18:
               fBase_c -> dBase_c -> dActor_c                  (adds 18-30)
               fBase_c -> dBase_c -> dScene_c -> dScMgBase_c   (adds 18-35)
           dScMgBase_c is a SCENE, not an actor; dScene_c and dActor_c are
           siblings under dBase_c and dScene_c declares nothing above slot 11.
           Two independent extensions of the same 18-slot base land on the same
           indices for the same reason two books have a page 19.  That is the
           root cause behind every semantic contradiction recorded at slots 26,
           29 and 30, and it applies to all eighteen.
         WHERE `Kill` CAME FROM, since the sources still say it.  ov002 really
           does carry _ZN10dBgActor_c4KillEv at 0x020ee55c -- a genuine mangled
           ROM symbol, and notes/actor-vtables.md derives it independently.  But
           dBgActor_c derives from dActor_c: it is a NEPHEW of this branch, and
           Kill is the one new virtual IT appends at ITS slot 31.  The name was
           carried across a fork, not down a chain.  Nothing in the cartridge
           names this function.  Virtual7C is the spelling fBase_c already uses
           for the same situation (Virtual34, Virtual38) and the one slots 33,
           34 and 35 are recorded under in
           notes/dScMgBase_c-slots-18-35.md, so it costs no new convention.
         return type: int, A HINT, the third consecutive slot no body pins.
           ov004:0x020b2880 sets r0 to #0 on its second instruction -- as
           SetSubBg1Offset's first argument, before anything reads it -- and
           never assigns a result; whatever falls out is LoadCompressedFileAt's
           return.  The one caller discards it (below).
         arity: no explicit parameters, MEASURED at the only in-family call
           site.  Scanning arm9 and all 103 overlays for `ldr rN,[rM,#0x7c]`
           immediately followed by `blx rN` -- the dispatch pair, not any load
           at +0x7c -- finds 39 sites, of which exactly ONE is in ov004 or
           ov006: ov004:0x020b0a0c, inside dScMgBase_c::BeforeInitResources.
           It reads `mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#0x7c]; blx r1`, so r1
           is the loaded pointer and cannot also be a second argument.  The
           other thirty-eight sites are in ov002, ov015, ov018, ov027, ov064,
           ov079, ov081 and ov098 -- dBgActor_c's Kill, on the other branch.
           The base body does not even read `this`; dScMgD3DBase_c's override
           does (unk_4660, unk_0a0), which is what fixes r0 as the object.
         WHAT IT DOES.  All four bodies are one shape.  Three read-modify-writes
           on the sub engine's BG1CNT at 0x0400100a (`& 0x43`, `| X`, `& ~0x40`,
           `& ~3`) leave the register holding exactly X: priority 0, no mosaic,
           and the class's own base-block bits -- 0x10 here, 0x800 in
           dScMgD3DBase_c, 4 in both dScMgAmida_c and dScMgSmartball_c.  Then
           SetSubBg1Offset(0, 0) resets the layer's scroll, `data_0209d454 &= ~2`
           clears BG1's bit in the sub BG-enable shadow that slot 30 restores
           the sub DISPCNT from, and two LoadCompressedFileAt calls install a
           language-indexed character file at G2S::GetBG1CharPtr() and the
           shared screen map, file 0x5b, at G2S::GetBG1ScrPtr().  Each class
           differs only in X and in its own language table.  It installs this
           minigame's touch-screen background; it destroys nothing.
         WHEN IT RUNS.  BeforeInitResources (ov004:0x020b0930) calls slot 33 at
           +0x84, zeroes data_0209d460 and data_0209d458 -- the main/sub pair
           slot 30 restores the DISPCNT layer bits from -- calls THIS slot last,
           and returns a literal 1 without touching the result.  Setup, at scene
           construction.
         overrides: SEVEN tables, THREE declarations, no reconciliation.
           dScMgD3DBase_c's body at ov006:0x020e72c0 backs its table and all
           four children's; dScMgAmida_c (ov006:0x020d11a0) and
           dScMgSmartball_c (ov006:0x02118ae4) each have their own.
         NAME CORRECTION, the ninth in this campaign and the eighth on
           dScMgD3DBase_c: 0x020e72c0 carried
           `recovered name: dScMgTrampoline2_c_Kill` and included
           dScMgTrampoline2_c.h.  Five tables reference that address, so it is
           dScMgD3DBase_c's.  Fixed here along with the type it casts `this` to. */
    virtual int  Virtual7C();                          /* slot 31 */
    /* Slot 32 -- Virtual80, and it is slot 31 again with the other display
       engine.  Reading the two bodies side by side is the fastest way to see
       that neither name the recovery pass gave them was ever a measurement.
         WHY THE NAME CHANGES, and it is a WORSE case than 31.  `AfterClsn` is a
           genuine ROM name -- include/PathLift.h:58 declares it and
           _ZN16dPathLiftActor_c9AfterClsnEi is a real mangled symbol -- but
           dPathLiftActor_c derives from dBgActor_c, which derives from
           dActor_c.  That is TWO forks off this branch, not one:
               fBase_c -> dBase_c -> dActor_c -> dBgActor_c -> dPathLiftActor_c
               fBase_c -> dBase_c -> dScene_c -> dScMgBase_c
           The shared ancestry stops at dBase_c, which adds no virtual, so the
           only thing the two indices have in common is fBase_c's 18-slot base.
           dPathLiftActor_c's AfterClsn also takes an `int`; this slot takes
           nothing.  Same borrowed-label defect settled at slot 31, one fork
           further out.  Virtual80 is the offset spelling, as at 31 and as
           slots 33-35 are recorded in notes/dScMgBase_c-slots-18-35.md.
         WHAT IT DOES -- the MAIN screen, where 31 did the sub.  Line for line
           the same body against the other engine: three read-modify-writes on
           the MAIN BG1CNT at 0x0400000a (`& ~3`, `(& 0x43) | 0x1000`, `& ~0x40`)
           leave it holding exactly 0x1000, SetBg1Offset(0, 0) resets the
           layer's scroll, `data_0209d45c &= ~2` clears BG1's bit in the MAIN
           BG-enable shadow that slot 30 restores the main DISPCNT from, and two
           LoadCompressedFileAt calls install a language-indexed character file
           and the shared screen map -- file 0x67 here, 0x5b there.  Slot 31
           went through G2S::GetBG1CharPtr and G2S::GetBG1ScrPtr; this one goes
           through func_02054ea8 and _ZN2G212GetBG1ScrPtrEv (ov004:0x020b2848
           and 0x020b285c).  So the ROM itself names the pair G2S/G2.  It builds
           this minigame's TOP-screen background; it has nothing to do with
           collision.
         WHEN IT RUNS, and this is the other half of the symmetry.  Slot 31 is
           called last by dScMgBase_c::BeforeInitResources; slot 32 is called
           FIRST by dScMgBase_c::AfterInitResources(u32) at ov004:0x020b0900,
           before the scene goes live.  Before/after, sub/main.
         arity: no explicit parameters, MEASURED at that one call site.  The
           whole-image scan for the dispatch pair at +0x80 (`ldr rN,[rM,#0x80]`
           with Rn != pc, followed within three instructions by `blx rN`) finds
           exactly THREE sites: ov004:0x020b0900, and ov002:0x020effa4 plus
           ov064:0x02116e58, which are dPathLiftActor_c::AfterClsn on the other
           branch.  At the ov004 site the sequence is
           `ldr r2,[r0]; mov r5,r0; ldr r2,[r2,#0x80]; mov r4,r1; blx r2` --
           r0 is `this`, and r1 is AfterInitResources's own `vfSuccess`, saved
           into r4 BECAUSE the call clobbers it and handed to
           0x0203188c afterwards.  A callee that consumed r1 would not need it
           parked first.
         return type: void, MEASURED -- and it corrects the `int` this block
           used to carry, which it called "A HINT ... no body pins".  A body
           pins it now.  dScMgSlot3_c's override was converted to a real member
           definition, and as `int` mwcc reserves r0 for the result: its closing
           read-modify-write on BG1CNT then allocates r1-r3 where the ROM
           allocates r0-r2, six of forty-two words differing with no other
           source change.

               ROM   ldr r2,[pc,#0x18] / ldr r0,[pc,#0x20] / ldrh r1,[r2]
               int   ldr r3,[pc,#0x18] / ldr r1,[pc,#0x20] / ldrh r2,[r3]

           Spelled void both bodies are byte-exact.  The base's own body was
           byte-exact either way -- its last statement is the
           LoadCompressedFileAt call, so r0 is already the call's result and
           nothing contends for it -- which is exactly why the hint survived
           unrefuted for so long.  Unrefuted is not confirmed.  The return type
           is not part of the Itanium mangled name, so nothing moved: no
           symbol, no vtable, no config entry.  Same adjudication as slot 30.
         overrides: ONE table, one declaration -- dScMgSlot3_c
           (ov006:0x0210aa60), which repeats the base body verbatim and then
           writes BG1CNT once more, to 0x1118 instead of the base's 0x1000:
           same layer, this minigame's own character and screen base blocks.
           No reconciliation, no misattribution: the smallest slot in the
           campaign after 22's zero. */
    virtual void Virtual80();                          /* slot 32 */
    /* Slot 33 -- Virtual84, the display bring-up, and the quietest slot in this
       campaign: it is the first one with nothing to correct.
         WHY THE NAME.  Neither override body carries a `recovered name:` line
           and neither does the base, so unlike slots 26, 29, 30, 31 and 32 there
           is no borrowed dActor_c label here to retire -- there was never one to
           borrow, because dActor_c's table ends at slot 30 and dScMgBase_c is on
           the sibling branch anyway (fBase_c -> dBase_c -> dScene_c ->
           dScMgBase_c; see the slot-31 block above).  Nothing in the cartridge
           names this method -- RTTI carries class names only -- so it takes the
           offset spelling fBase_c already uses for Virtual34/Virtual38, which is
           also how notes/dScMgBase_c-slots-18-35.md has recorded it all along.
         return type: void, and here that is the reading of the bodies rather
           than a default.  All three fall off the end without setting a result,
           the one call site discards whatever is in r0, and -- unlike slots
           28-32 -- one of the three is about to become a real member definition
           (see below), where a declared `int` with no return statement would be
           a lie the compiler has to paper over.  notes/dScMgBase_c-slots-18-35.md
           records the signature as void(char *obj) and this agrees with it.
         arity: no explicit parameters, MEASURED.  Scanning arm9 and all 103
           overlays for the dispatch pair -- `ldr rD,[rN,#0x84]` with rN != 15,
           followed within three instructions by `blx rD` -- finds TWO sites in
           the whole image, of which exactly one is in ov004 or ov006:
           ov004:0x020b09d0, inside dScMgBase_c::BeforeInitResources.  It reads
           `mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#0x84]; blx r1`, so r1 holds the
           loaded pointer and cannot also be a second argument, and r0 is `this`.
           The scan was validated against a known answer first: the same run at
           +0x80 reproduces slot 32's site (ov004:0x020b0900) and nothing else in
           the family.  The other +0x84 hit is in ov064, outside this hierarchy.
         `this` IS carried, and for once the base body proves it on its own.
           Slots 26-32 all had base bodies that never touched the object; this
           one writes obj[0x68] and obj[0x6c] on its second and third statements
           and then publishes obj itself into the scene registry.
         WHAT IT DOES.  This is the engine bring-up, and it runs BEFORE anything
           else the family does.  Both engines get a graphics mode, VRAM banks
           are assigned, both BG-enable shadows are initialised to 0x10 --
           data_0209d45c for main and data_0209d454 for sub, the same pair slot
           30 restores the DISPCNTs from and slots 31 and 32 clear BG1 out of --
           a language-indexed compressed character file is decompressed into BG
           character VRAM, OBJ palette file 0xc3 is loaded into both engines, and
           the scene object is published into the global registry at
           data_ov004_020beb74[1] / data_0209d4a8.  The three bodies differ
           exactly where the classes differ: the base is 2D (GX mode (1,0,0),
           GXS mode 0, BG bank 3), dScMgD3DBase_c is 3D (DisableAllBanks first,
           texture and texture-palette banks, GX mode (1,0,1), GXS mode 5, BG
           bank 2, InitialiseVramGlobals, and it keeps the decompressed file
           pointers in globals), and dScMgSingle3DBase_c is the base's 2D
           sequence with the 3D banks folded in.
         WHEN IT RUNS.  BeforeInitResources (ov004:0x020b0930) dispatches this
           slot at 0x020b09d0, near the top, and dispatches slot 31 at
           0x020b0a0c, last.  With slot 32 called first out of
           AfterInitResources, the whole sequence reads: bring the engines up
           (33), dress the sub screen (31), then dress the main screen (32).
         overrides: NINETEEN tables, TWO declarations -- structurally slot 26
           over again, and the same two classes own the bodies.  Thirteen
           children of dScMgSingle3DBase_c and four of dScMgD3DBase_c point at an
           ancestor's body and declare nothing; declaring the slot on any of
           those seventeen would invent an override the cartridge does not have.
         FIRST OVERRIDE INSIDE A PROMOTED TU.  dScMgSingle3DBase_c's body is not
           a one-function file -- it lives inside the intact-object unit
           src/actors/dScMgSingle3DBase_c.cpp (promoted in #2064), where it has
           been waiting on this declaration by name since then: the slot-26
           member in that file ends "Slot 33 below is still waiting on the same
           declaration."  It was the last vtable slot in that unit still spelled
           as a mangled free function (0x0210a534, the only other one left, is in
           no vtable at all), so this turns it into a real member definition and
           takes the class from a byte-exact 33-slot vtable prefix to 34. */
    virtual void Virtual84();                          /* slot 33 */
    /* Slot 34 -- Virtual88, the first slot in this family that takes arguments,
       and the first whose job is legible from the body without any naming help.
       IT IS THE BRUSH.  ov004:0x020ae3b4 walks a size x size square centred on
         (cx, cy), and for each cell inside it computes the address of one
         4-bit pixel in BG character VRAM -- `(x/8 + (y/8)*32)*32 + (y&7)*4`,
         the standard DS 4bpp char layout -- reads the containing word, splices
         `colour` into the nibble at `(x&7)*4`, and writes it back.  Sixteen
         colours, one palette index per pixel.  It clips on all four sides.
       THE TWO FIELDS SLOT 33 INITIALISES ARE THE ONES IT READS.  Slot 33
         (Virtual84, engine bring-up) sets `obj+0x68` to 0 and `obj+0x6c` to -1.
         Here `obj+0x6c` selects WHICH background layer to draw into -- 0..3
         index G2S::GetBG0CharPtr through GetBG3CharPtr, anything else returns
         without drawing, which is what -1 buys -- and `obj+0x68` gates the
         wrapped region above the touch screen, where the main engine's
         G2::GetBG*CharPtr are used instead and y is folded by
         `+ data_ov004_020beb6c + 0xc0`.  So slot 33 leaves the brush disabled
         and a minigame arms it by picking a layer.  Two slots, one mechanism.
       WHO CALLS IT: ov004:0x020ae5c4, a line rasteriser sitting immediately
         after the brush in the image, which dispatches through +0x88 at seven
         separate sites as it steps along a segment.  That is the whole of the
         in-family call graph for this slot -- every one of the seven is inside
         that one function.  A line-drawing minigame is exactly the set that
         overrides it (dScMgAmida_c is the ghost-leg/stylus one).
       arity: FOUR explicit parameters, MEASURED and unanimous.  Every one of
         the seven sites sets up r1, r2, r3 AND one stack word before the call
         -- e.g. at 0x020ae690, `mov r1,sb; mov r2,r8` with r3 loaded from
         [sp,#0x3c] and [sp] already holding r7 -- so the callee takes this plus
         four.  Image-wide the +0x88 dispatch pair appears 14 times, 7 in
         ov004/ov006 and 7 in ov064; the scanner is the one validated at slot 33
         by reproducing slot 32's site at +0x80.
       return type: void, and here the ROM says so rather than merely permitting
         it.  Two of the seven sites (0x020ae7e4, 0x020ae83c) overwrite r0 on
         the very next instruction with `ldr r0,[sp,#0xc]`; the rest tail-return
         without reading it.  All five bodies fall off the end without setting a
         result.
       NOTHING TO CORRECT, as at slot 33: no `recovered name:` line exists on
         the base body or on any of the four overrides, so no borrowed label is
         being retired.  Virtual88 is this tree's own no-name spelling after the
         +0x88 vtable offset.
       overrides: FOUR tables, FOUR declarations, nothing inherited and nothing
         shared -- dScMgAmida_c (ov006:0x020d14c0), dScMgTeresa_c (0x021200dc),
         dScMgTrampoline_c (0x02120da8) and dScMgTrampoline2_c (0x02122cb0) each
         have a body of their own.  dScMgD3DBase_c does NOT override this slot,
         so its two trampoline children declare it themselves rather than
         inheriting a shared body the way they do at 26-31 and 33.
       dScMgAmida_c's body decompiles with only THREE explicit parameters.  That
         is not a contradiction: the fourth arrives on the stack and that body
         never reads it, so the reconstruction had nothing to name.  The slot's
         signature is fixed by the call sites, which are unanimous, and an
         ignored stack argument costs the callee nothing. */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */
    /* Slot 35 -- Virtual8C, the LAST slot of dScMgBase_c's own eighteen and the
       end of the 18-35 range.  With this declared the class emits its full
       36-slot vtable from source, and dScMgAmida_c's Unk36 finally lands on 36
       where the ROM puts it.
       WHAT IT IS: a predicate on the scene's own spawn parameter.  The base
         body (ov004:0x020ad660, twenty bytes) is `return (param1 & 0xff) != 0`
         -- fBase_c::param1 at +0x08, the word every fBase_c is constructed
         with.  dScMgAmida_c's override (ov006:0x020d1170, twenty-four bytes)
         asks the narrower question, `== 1`.  Nothing else in the family
         overrides it, so thirty-one of the thirty-two tables carry the base's
         answer.
       WHO CALLS IT, and this is the strongest call-site evidence in the whole
         campaign: THIRTEEN dispatch sites, spread across FOUR different leaf
         classes' code regions -- dScMgCoin_c (2), dScMgPanel_c (4),
         dScMgSound_c (3) and dScMgSnowball_c (4).  Each is the same shape,
         `mov r0,<this>; ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1`, a class asking
         the question of ITSELF.  They use the answer to pick between two
         variants of the same minigame: two asset tables at ov006:0x02105488,
         a different field path at 0x0211b9e0, a whole block skipped at
         0x02126f58.
       arity: no explicit parameters, MEASURED.  r1 is the loaded function
         pointer at every one of the thirteen sites and so cannot also be an
         argument, and r2/r3 are never set up for the call.  `this` only.
       return type: int, and this is the first slot in the campaign where the
         ROM does not merely permit a return value but is SEEN TO CONSUME ONE.
         All 13 sites follow the call with `cmp r0, #0` and branch on the
         result.  Both bodies compute a comparison and return it, so the value
         is 0 or 1; `int` is the spelling slots 29-32 already use for the same
         shape.
       NOTHING TO CORRECT, as at slots 33 and 34: no `recovered name:` line on
         the base body or on the one override.  Virtual8C is this tree's own
         no-name spelling after the +0x8c vtable offset.
       the rename is SCOPED.  0x020ad660 is an overlay load base, so ov000,
         ov002, ov003, ov004 and ov007 each have a different, unrelated symbol
         at that address -- ov003's was dScTitle_c's D1.  Only
         func_ov004_020ad660 was this one, so the rename was keyed on that
         module-qualified symbol; keying it on the address would have hit
         five files in four unrelated overlays and every byte gate would
         still have passed. */
    virtual int  Virtual8C();                          /* slot 35 */

    /* THAT IS ALL EIGHTEEN.  Slots 18-35 are declared, each in its own
       commit together with every descendant override of that slot, and this
       class emits its full 36-slot vtable from source.  Nothing in the family
       is a prefix any more.

       Slots 18-30 carry names borrowed from dActor_c by INDEX, which is a
       coincidence and not evidence: dScMgBase_c is a scene
       (fBase_c -> dBase_c -> dScene_c -> dScMgBase_c), a sibling branch that
       happens to start appending its own virtuals at the same index dActor_c
       does.  Slot 31 is the first one above dActor_c's table and is what
       proved it.  Retiring those thirteen borrowed names is a separate
       question from this range being declared, and is deliberately left open.

       THE OCCUPIED-SLOT TRAP IS NO LONGER LOADED IN dScMgSlot1_c -- slots 27
       and 28 were both of its early declarations and both are reconciled --
       but it is a property of the FAMILY, not of that one class.  Before
       adding any FURTHER virtual to this class, check every descendant
       header for a virtual that already lands on the new index by
       arithmetic.  Declaring the base's while a
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
    u32 mSavedMainBgBits;   /* 0x21c -- OnAimedAtWithEgg saves data_0209d45c
                                here and OnAimedAtWithEggReturnVec restores it */
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
