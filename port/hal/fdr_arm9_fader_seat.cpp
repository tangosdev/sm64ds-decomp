// The arm9 wipe fader's own static initialiser, RUN rather than reimplemented.
//
// Run link60 Stage 4, lane FDR. This retires the blocker lane MG2 named and
// port/tools/battery.py's SCENE_BLOCKED row for scene 374 records:
//
//     FAULT c0000005 at _ZN5Scene9SetFadersEP15FaderBrightness+0x24
//     accessing 0x00000024, reached through
//     mb_binit -> func_ov004_020b0930 (dScMgBase_c slot 1) -> Scene::SetFaders
//
// reproduced on this lane's own binary before a line of it was written
// (SM64DS_SCENE=374 SM64DS_SCENE_FRAMES=300 SM64DS_SCENE_NO_RENDER=1, exit
// 139, ecx = esi = the address walk_window.map gives for _data_0209f61c).
//
// ---- WHAT THE OBJECT ACTUALLY IS, BECAUSE THE HANDOVER GOT THE CLASS WRONG --
//
// MG2's report, port/mg_fanout_costs.txt section 10 and the SCENE_BLOCKED row
// all call data_0209f61c "an arm9 FaderBrightness". IT IS A dWipe_c, and
// FaderBrightness (dFdBrightness_c) is its GRANDBASE, two levels up: the base
// in between is dFdColor_c, and dFader_c is the third level. The ROM says so
// in its own RTTI:
// reading the typeinfo chain out of the image with config/arm9/relocs.txt
// applied,
//
//     0x020926dc  __si_class_type_info  "7dWipe_c"          base 0x0208ea00
//     0x0208ea00  __si_class_type_info  "10dFdColor_c"      base 0x0208ea24
//     0x0208ea24  __si_class_type_info  "15dFdBrightness_c" base 0x0208e9e0
//     0x0208e9e0  __class_type_info     "8dFader_c"
//
//     dFader_c <- dFdBrightness_c <- dFdColor_c <- dWipe_c
//
// which is exactly the four vptr stores func_0202fc40 makes, in base-to-derived
// construction order: data_0208eafc (Fader), data_0208eacc (FaderBrightness),
// data_0208eb2c (FaderColor), and data_020926f0 last, the most-derived table
// this file owns. Scene::SetFaders' `FaderBrightness*` parameter is legitimate
// by inheritance, so nothing in the ROM's C is wrong -- but the CLASS NAME is,
// and a successor reading "FaderBrightness" would go looking for the fix in
// the 0x02017xxx fader engine, where none of these bodies live.
//
// The correction does not change MG2's conclusion. MG2 named the table by
// ADDRESS ("a host fill for data_020926f0, which is NOT _ZTV9FaderWipe") and
// that address is right.
//
// ---- WHY A STATIC INITIALISER AND NOT A HAND-ROLLED CONSTRUCTOR -------------
//
// hal/fader_wipes.cpp's gate-31 block solved the SAME SHAPE for the sibling
// object data_0209f5e8 by placement-newing a host HalFaderWipe over the
// storage. That works and it ships. This lane is explicitly NOT allowed to
// copy it, and the reason is the port's north star: the port must BE the
// decomp. So what runs here is src/__sinit_02074f80.c -- the ROM's own
// initialiser, byte-for-byte the two calls the image makes:
//
//     0x02074f80  bl func_0202fc40        r0 = 0x0209f61c
//     0x02074f9c  bl func_020731dc        r0/r1/r2 = 0x0209f61c, the dtor
//                                         0x0202fc08, the node 0x0209f610
//
// and func_0202fc40, also the ROM's, installs the vptr. Nothing here writes a
// vptr, invents a constructor, or picks a plausible table. The one thing this
// file supplies that the image cannot is the CONTENTS of data_020926f0, because
// the ROM's ten words are DS code addresses and a host cannot call them.
//
// IT RUNS AT C++ STATIC-INIT TIME, which is where the DS runs it: before main,
// exactly once, with no seam to remember to call. That is the same mechanism
// and the same reasoning hal/fader_wipes.cpp already relies on for
// data_0209f5bc and data_0209f324 ("C++ static init is ordered before main, so
// the order is guaranteed"). The fill and the sinit sit in ONE initializer so
// their order is intra-TU and cannot be reordered by the linker.
//
// It depends on no other dynamic initialiser. func_0202fc40 writes four vptr
// words and calls func_0202ed14, which is nine plain field stores;
// func_020731dc pushes a node onto data_020aa3f0, which is zero-initialised
// storage in hal/cxx_aliases.cpp. The three base tables the ctor writes on its
// way past are already hosted, by hal/scene_boot.cpp:688-704, and nothing
// dispatches through them inside the window.
//
// ---- THE TABLE: TEN SLOTS, THE ROM'S ORDER ---------------------------------
//
// TEN, and the count is measured rather than assumed. config/arm9/relocs.txt
// carries `kind:load module:main` rows at 0x020926f0 through 0x02092714 and
// stops: 0x02092718 is a literal zero and 0x0209271c relocates to 0x02086ecc,
// which is 7dView_c's typeinfo, so a DIFFERENT class's vtable begins at
// 0x02092720. The slot NAMES are the ROM fader order pinned in
// hal/fader_wipes.cpp, and they are confirmed here independently: the ROM
// bodies at 0x20 and 0x24 are three-instruction veneers onto
// FaderBrightness::SetToEnd (0x0201761c) and FaderBrightness::SetToStart
// (0x02017610), and slots 0x14/0x18 tail into IsAtStart (0x02017684) and
// IsAtEnd (0x02017670).
//
//     off   ROM body        what              tranche
//     0x00  func_0202fc08   ~dWipe_c   (D1)   FDR  (also the sinit's atexit)
//     0x04  func_0202fbc8   (D0)              FDR
//     0x08  func_0202f428   AdvanceFade       SEAT8
//     0x0c  func_0202f928   SetBackwardTime   FDR2
//     0x10  func_0202f708   SetForwardTime    FDR2
//     0x14  func_0202ee38   IsAtStart         FDR
//     0x18  func_0202eddc   IsAtEnd           FDR
//     0x1c  func_0202ed7c   IsBetweenStart..  FDR
//     0x20  func_0202ed08   SetToEnd          VENEER, see below
//     0x24  func_0202ecfc   SetToStart        FDR
//     0x28  --              overhang          TRAP
//     0x2c  --              overhang          TRAP
//
// 0x28 and 0x2c are NOT the ROM's. They are the same two-slot overhang
// hal/fader_wipes.cpp spends on HalTail28/HalTail2c, for the same reason: a
// raw-offset caller that reads past 0x24 lands on something callable and named
// instead of on the next class's vtable header. They are the documented
// overhang, not shared ground with the ROM.
//
// THE FADE MOTION CAME IN THE SECOND TRANCHE. FDR seated the fade STATE and
// left the three motion slots as named traps sized at "roughly 7 src TUs plus
// four arm9 data symbols". Run link60 lane FDR2 re-measured that with
// port/tools/closure.py and it was low both ways: the two setters alone need
// NINE TUs and NINE data symbols, because func_0202f58c reaches func_0202f290
// and its two table builders and the HBlank handler reaches its own reset.
// 0x0c and 0x10 are the ROM's bodies now; the closure and the sizing are in
// port/fader_boot_map.txt section 4.
//
// 0x08 IS SEATED NOW, run link60 Stage 5 lane SEAT8, AND THE TWO REASONS IT
// WAS REFUSED WENT DIFFERENT WAYS. Both are worth keeping, because one was
// fixed and the other was WRONG.
//
// THE CALL DEFECT WAS REAL AND IS FIXED UPSTREAM. src/func_0202f428.c's
// `type == 1` branch used to call
//
//     _ZN10FaderColor11AdvanceFadeEv()
//
// with NO ARGUMENT -- the ROM's `bl` at 0x0202f440 leaves r0 holding the
// receiver, the same trick the veneer at 0x20 relies on -- and on the host the
// receiver was gone. main PR #1539 named the receiver in
// include/decl_FaderColor.h and passed it at the call site. This lane
// propagated the two files by address and byte-verified the TU in its own tree
// at 2004/b56 with strict relocs, before the edit and after: MATCH both times.
// The ARM bytes did not move, which is what makes the propagation free.
//
// "SLOT 0x08 HAS NO CALL SITE IN THIS IMAGE" WAS TRUE OF THE BINARY AND FALSE
// OF THE GAME, and the difference cost the slot two tranches. section 9a of
// port/fader_boot_map.txt swept walk_window's own .text, which is the right
// sweep for an ABI audit and the wrong one for a reachability ruling: a ROM
// caller that is in no slice cannot appear in it. The ROM's frame phase 2,
// func_02019390, calls TWO fader advances and both dispatch vtable byte 0x08:
//
//     func_02018efc   on data_0209d4ac, the INSTALLED fader
//     func_02018ec0   on data_0209d4b0, the fader IN MOTION
//
// and _ZN5Scene9SetFadersEP15FaderBrightness -- already linked, slice_gate10 --
// ends with `data_0209d4ac = thiz`. dScMgBase_c slot 1 hands it data_0209f61c,
// so on scene 374 the ROM's own store arms this object into d4ac on the frame
// the scene spawns. The missing piece was six lines of C: src/func_02018efc.c,
// which slice_fdr.txt now carries and hal/fader_wipes.cpp's port_fader_advance
// calls, at the same point in the frame where it already stands in for phase
// 2's other half.
//
// data_0209d4b0 IS THE WRONG POINTER FOR THIS OBJECT and port/irq2_map.txt
// section 7b named it. Nothing in the image ever stores data_0209f61c there,
// and the sufficient proof is the review's: THE TWO REFERENCE SETS DO NOT
// INTERSECT. Eleven functions reference data_0209f61c across the arm9, itcm,
// dtcm and ninety overlay reloc tables (2 arm9, 6 ov004, 2 ov005, 1 ov006)
// and eleven reference data_0209d4b0, and no function is in both, so nothing
// in the image holds both addresses. This comment first said the symbol had
// "exactly two references", which was the arm9 count written as the image's;
// port/fader_boot_map.txt section 4a carries the correction, the per-module
// breakdown and the value-by-value reading of every d4b0 writer. No arming
// needed inventing.
//
// Each remaining trap is still its own named function so a run says which slot
// fired rather than that some slot did.
//
// WHY 0x20 IS A VENEER AND NOT src/func_0202ed08.c. THE src TU IS HOST-BROKEN,
// and it is broken in exactly the way lane LK4 wrote up for Heap::_Destroy.
// The ROM body is
//
//     0202ed08  ldr ip, [pc] ; bx ip ; .word 0x0201761c
//
// a bare tail-call veneer, and on ARM r0 falls through it untouched. The src
// spells that as
//
//     void func_0202ed08(void) { _ZN15FaderBrightness8SetToEndEv(); }
//
// with NO parameter and NO argument, which is byte-correct under mwccarm and
// receiver-destroying on the host: the callee would read whatever the caller
// left. So the slot forwards to the same target the ROM veneer jumps to,
// reached with the receiver, and the src TU stays out of the link. It is not a
// hand-written body: the ROM's body has no content beyond that one edge.
// Whoever rewrites the veneer TU to take a receiver owns this line too.
//
// ---- THE CALLING CONVENTION, AND THE ARGUMENT THAT WAS WRONG ---------------
//
// Every slot is a __fastcall thunk that hands the receiver to the flat-C ROM
// body as its first argument. That is the port's standing vtable-fill shape
// (hal/scene_mg.cpp's mb_* thunks, hal/lk4_solidheap_seat.cpp's sh_*), and it
// is what a __thiscall dispatch needs: MSVC puts `this` in ECX, __fastcall
// reads its first parameter from ECX.
//
// WHAT THE FIRST VERSION OF THIS FILE GOT WRONG, and it cost a crash rather
// than a comment. It argued that a thunk with two REGISTER parameters and no
// stack parameters is balanced for every caller shape, because "a cdecl caller
// cleans its own push, and a thiscall no-argument caller pushes nothing". The
// shape it did not enumerate is the one MSVC emits for a virtual with
// arguments: __thiscall is CALLEE-CLEANS, so the caller pushes and cleans
// nothing, and a callee that also cleans nothing leaves the arguments on the
// stack. Scene::BeforeBehavior does exactly that at two sites, and scene 374's
// first behaviour tick reached one of them.
//
// THE ARITY OF EVERY SLOT IS NOW THE CALL SITE'S, MEASURED. Section 9 of
// port/fader_boot_map.txt is the audit: every dispatch site in the image that
// can reach this table, the push/clean shape MSVC emitted there, and what each
// of the twelve stubs declares against it. Three shapes reach the table and
// only three:
//
//     THISCALL, no arguments        caller pushes nothing, callee cleans 0
//     THISCALL, two arguments       caller pushes 8 and cleans NOTHING,
//                                   so the callee must clean 8
//     CDECL through a struct of
//     function pointers             caller pushes the receiver and cleans 4,
//                                   so the callee must clean 0
//
// and the audit's job was to prove no slot is reached by two of them at once.
// ONE WAS: slot 0x10, by the two-argument shape from Scene::BeforeBehavior and
// by the no-argument shape from the matched IsBetweenStartAndEnd's dtor-fold
// skew. That is not a signature problem and it is fixed at the face below,
// which no longer produces the second shape.
//
// THE GARBAGE-RECEIVER HAZARD IS REAL AND IS NOT THE CRASH. The cdecl shape
// (Scene::SetFaders' two `data_0209f5bc->vt->fNN(data_0209f5bc)` sites and
// Minimap::Behavior's `vt->f[5]`, and only those three in this image) puts the
// receiver on the stack, where a __fastcall thunk does not look. HUD::Behavior
// and func_ov003_020af038 LOOK like the same shape in a listing and are not:
// their leading pushes are a callee-save and MSVC's frame-slot idiom, and what
// separates the two conventions is the `add esp,4` after the call, which
// neither of them has. The stack stays balanced -- both sides agree
// the callee cleans nothing -- so the failure mode is a wrong `this`, not a
// smashed frame. At every one of those sites in THIS image the object is also
// still in ECX when the call is made, so the thunk reads the right receiver
// anyway. That is a property of what MSVC happened to emit here, not a
// guarantee, and it is in the unproven items.

#include <cstdio>
#include "types.h"
#include "FaderBrightness.h"
#include "FaderColor.h"
#include "dsstate_seg.h"

extern "C" {

/* The ROM bodies this table seats, declared flat. Each src TU spells its own
   struct for the receiver; these are the ABI-compatible views. */
void *func_0202fc08(void *self);   /* 0x00  D1, and the sinit's atexit dtor */
void *func_0202fbc8(void *self);   /* 0x04  D0 */
void  func_0202f428(void *self);   /* 0x08  AdvanceFade, run link60 SEAT8 */
int   func_0202ee38(void *self);   /* 0x14  IsAtStart */
int   func_0202eddc(void *self);   /* 0x18  IsAtEnd */
int   func_0202ed7c(void *self);   /* 0x1c  IsBetweenStartAndEnd */
void  func_0202ecfc(void *self);   /* 0x24  SetToStart (veneer, receiver-correct) */

/* The two motion bodies, run link60 lane FDR2. Their arities are the ROM's and
   differ from each other; see the note above the thunks. */
int   func_0202f928(void *self, unsigned frames, unsigned c);  /* 0x0c */
int   func_0202f708(void *self, unsigned frames);              /* 0x10 */

/* The ROM's static initialiser and the storage it constructs. */
void __sinit_02074f80(void);
extern int data_0209f61c[];        /* hal/auto_bss.cpp, 0x2c by ROM span */

}  /* extern "C" */

/* ---- the flat-C faces the seated bodies call ------------------------------
   The matched fader TUs are already in the link, but as MSVC __thiscall
   METHODS (?IsAtStart@FaderBrightness@@UAEHXZ and friends). The bodies above
   reach them by their flat Itanium names. An /alternatename between the two
   would drop the receiver -- hal/method_faces.cpp failure class 3, the one
   lane LK4 measured a fault on -- so these are real receiver-bridging faces,
   the same shape method_faces.cpp already uses for the two of this family it
   owns (_ZN15FaderBrightness14SetForwardTimeEj and
   _ZN15FaderBrightness7IsAtEndEv, which is why neither is repeated here).
   Each call is QUALIFIED so nothing re-dispatches through the host table. */
extern "C" {

int _ZN15FaderBrightness9IsAtStartEv(void *self)
{ return ((FaderBrightness *)self)->FaderBrightness::IsAtStart(); }

/* THE TWO FACES SLOT 0x08 DRAGGED IN, run link60 Stage 5 lane SEAT8, and
   port/tools/closure.py named both before a line of this was written: they
   were the whole of the seat's unresolved set.

   src/func_0202f428.c's `type == 1` branch calls FaderColor::AdvanceFade by
   its flat ROM name, and that method's matched TU
   (src/engine/fader/_ZN10FaderColor11AdvanceFadeEv.cpp) is a real C++ member,
   so MSVC emits ?AdvanceFade@FaderColor@@QAEXXZ. That body in turn calls
   Fader::AdvanceInterp by ITS flat name, and the matched TU for that one
   (slice_w1l3) is a C++ member too. Two flat names, two MSVC methods, and the
   /alternatename that would join them drops the receiver -- method_faces.cpp
   failure class 3 again -- so both are real bridging faces.

   NEITHER IS EXERCISED ON ANY PATH MEASURED HERE. type is 0 on every scene-374
   path (func_0202ec9c(data_0209f61c, 0) from dScMgBase_c slot 1, and the
   constructor leaves it 0), so the branch that reaches them is compiled and
   not entered. They are here because the link needs them, which is stated
   rather than dressed up as coverage. */
void _ZN10FaderColor11AdvanceFadeEv(void *self)
{ ((FaderColor *)self)->FaderColor::AdvanceFade(); }

void _ZN5Fader13AdvanceInterpEv(void *self)
{ ((Fader *)self)->Fader::AdvanceInterp(); }

/* THIS ONE DOES NOT DELEGATE, AND THE REASON STOPPED BEING COSMETIC.
   The matched FaderBrightness::IsBetweenStartAndEnd ends in two UNQUALIFIED
   IsAtStart()/IsAtEnd() calls, so it re-dispatches through the RECEIVER's
   table. On mwcc those are ROM slots 5 and 6, bytes 0x14 and 0x18. MSVC folds
   the two destructor slots into one, so the same two calls compile to bytes
   0x10 and 0x14 -- one slot early against a table kept in ROM byte order. Read
   out of this lane's own binary, ?IsBetweenStartAndEnd@FaderBrightness@@QAEHXZ
   is

       +0x03  mov  eax, [esi]
       +0x05  call dword ptr [eax + 0x10]      nothing pushed
       ...
       +0x10  call dword ptr [eax + 0x14]      nothing pushed

   (offsets from the function, not from the image, so a relink cannot rot
   them)

   AN EARLIER VERSION OF THIS FACE DELEGATED AND PRINTED A WARNING, on the
   reading that the skew costs a wrong answer and nothing worse. Section 9's
   audit refutes that. Slot 0x10 is ALSO reached by Scene::BeforeBehavior,
   which pushes two arguments and cleans neither, so 0x10 has to clean eight --
   and the two calls above push nothing at all. One slot, two incompatible
   caller shapes, and no C signature satisfies both: a 0x10 declared for
   BeforeBehavior smashes eight bytes off THIS caller's frame. The warning
   could not have saved it, because the frame is gone before anything reads the
   answer.

   THE FIRST FIX FOR THIS WAS TO STOP CALLING THE MATCHED BODY and compute the
   predicate here from the receiver's own 0x14 and 0x18. It worked and the gate
   REFUSED IT: linkage 5810 -> 5809. With no caller left, /OPT:REF dropped
   ?IsBetweenStartAndEnd@FaderBrightness@@QAEHXZ out of the image, and a
   matched ROM body falling out of the link is not a rounding error on this
   port -- it is the north star running backwards. The number caught a real
   regression and the fix below is what it forced.

   SO THE MATCHED BODY STILL RUNS, AND IT IS HANDED A RECEIVER IT CAN READ.
   The mismatch is a table ORDER, so what the method needs is a view of the
   object in ITS order. FdrMsvcOrderView below is that view: a two-word stack
   object whose vptr points at an MSVC-ordered table, whose 0x10 and 0x14 are
   thunks that dispatch the REAL receiver's ROM 0x14 and 0x18. The method's two
   unqualified calls then reach exactly the pair the ROM reaches, on the real
   object, and the arithmetic between them is the decomp's own.

   WHY A VIEW IS SAFE HERE, and it is a codegen claim rather than a hope. The
   matched body reads NOTHING but the vptr -- the disassembly above is the
   whole function, and the only base register it dereferences is [esi] for the
   table. Its two calls carry the receiver onward, and the thunks put the real
   one back. If a future compiler gave that body a data access, this view would
   hand it two words of stack, so the disassembly is quoted rather than
   summarised: a successor can re-read it in one glance.

   THE DIVERGENCE, written down rather than implied: on the ROM there is no
   view and no thunk pair, just one virtual dispatch. This is the host's cost
   for MSVC folding two destructor slots into one, and hal/fader_wipes.cpp pays
   the same debt by declining to delegate at all. */

/* The MSVC-ordered stand-in. Two words, stack-allocated per call, never
   escapes: the matched body is not reentrant into it and does not store it. */
struct FdrMsvcOrderView {
    void **vt;   /* 0x00, the MSVC-ordered table below */
    void *real;  /* 0x04, the ROM-ordered object the thunks dispatch */
};

/* The fill's own thunk shape. A real MSVC __thiscall virtual is
   ABI-compatible with it: receiver in ECX, EDX unread, no stack parameters,
   callee cleans nothing. Slots 0x14 and 0x18 of a ROM-ordered table are both
   that shape -- see section 9d of port/fader_boot_map.txt. */
typedef int(__fastcall *FdrPredicate)(void *, void *);

static int fdr_view_dispatch(FdrMsvcOrderView *view, unsigned rom_byte)
{
    void **vt = *(void ***)view->real;
    return ((FdrPredicate)vt[rom_byte / 4])(view->real, 0);
}

/* MSVC byte 0x10 is FaderBrightness::IsAtStart; the ROM keeps it at 0x14. */
static int __fastcall fdr_view_is_at_start(FdrMsvcOrderView *view, void *)
{ return fdr_view_dispatch(view, 0x14); }

/* MSVC byte 0x14 is FaderBrightness::IsAtEnd; the ROM keeps it at 0x18. */
static int __fastcall fdr_view_is_at_end(FdrMsvcOrderView *view, void *)
{ return fdr_view_dispatch(view, 0x18); }

/* Nothing reaches 0x00 through 0x0c: the matched body dispatches two slots and
   returns. They are filled with a named refusal rather than left null so a
   codegen change announces itself instead of jumping to zero. It is NOT
   counted in fdr_trap_hits, which counts unseated slots of the ROM's own
   table and would read as one if this ever fired. */
static int __fastcall fdr_view_unreached(void *, void *)
{
    std::fprintf(stderr, "  [fdr] the MSVC-ordered view built for "
                 "FaderBrightness::IsBetweenStartAndEnd was dispatched below "
                 "byte 0x10. That body is supposed to reach 0x10 and 0x14 and "
                 "nothing else, so its codegen has changed and the view no "
                 "longer describes it. See port/fader_boot_map.txt section "
                 "9e.\n");
    std::fflush(stderr);
    return 0;
}

static void *fdr_msvc_order_vt[6] = {
    (void *)fdr_view_unreached,       /* 0x00  dtor, folded */
    (void *)fdr_view_unreached,       /* 0x04  AdvanceFade */
    (void *)fdr_view_unreached,       /* 0x08  SetBackwardTime */
    (void *)fdr_view_unreached,       /* 0x0c  SetForwardTime */
    (void *)fdr_view_is_at_start,     /* 0x10  IsAtStart */
    (void *)fdr_view_is_at_end,       /* 0x14  IsAtEnd */
};

}  /* extern "C" -- the view's internals are C++ and not ROM names */

extern "C" int _ZN15FaderBrightness20IsBetweenStartAndEndEv(void *self)
{
    FdrMsvcOrderView view;
    view.vt = fdr_msvc_order_vt;
    view.real = self;
    return ((FaderBrightness *)(void *)&view)->
        FaderBrightness::IsBetweenStartAndEnd();
}

extern "C" {

/* func_0202ecfc's target, spelled by address in its src because the veneer has
   no symbol name of its own in that TU. 0x02017610 IS
   _ZN15FaderBrightness10SetToStartEv (config/arm9/symbols.txt), whose matched
   TU rides slice_gate1. */
void func_02017610(void *self)
{ ((FaderBrightness *)self)->FaderBrightness::SetToStart(); }

/* THE BASE-CLASS BRANCH OF SLOT 0x0c, run link60 lane FDR2.
   func_0202f928's `type == 1` case tail-calls the ROM's 0x020176d8, which
   config/arm9/symbols.txt names _ZN15FaderBrightness15SetBackwardTimeEj. Its
   matched TU is src/engine/fader/, so MSVC emits it as
   ?SetBackwardTime@FaderBrightness@@UAEHI@Z -- a __thiscall method -- and the
   caller declares the flat C name. hal/method_faces.cpp owns the SetForwardTime
   half of this exact pair and rules it a face and not an /alternatename for the
   reason that applies here unchanged: an alias would run the method with a
   garbage `this` out of ECX.

   THREE PARAMETERS, and the third is the one the ROM never reads. On ARM r2 is
   live across the `bl` at 0x0202f940 whether the callee wants it or not, so
   src/func_0202f928.c forwards it and its own comment explains that keeping it
   is what makes the register allocation match. Both sides are cdecl here and
   the caller cleans its own arguments, so the extra word costs nothing and is
   carried rather than silently dropped. */
int _ZN15FaderBrightness15SetBackwardTimeEj(void *self, u32 frames, u32)
{ return ((FaderBrightness *)self)->FaderBrightness::SetBackwardTime(frames); }

}  /* extern "C" */

/* ---- the storage ---------------------------------------------------------
   Both are hosted DS globals, so both go inside the .dsstate bracket and roll
   back with a save state.

   data_020926f0 is arm9 .data in the ROM and its ten words are relocated code
   addresses; hosted raw it would hand a host dispatch ten DS addresses. Twelve
   words here: the ROM's ten plus the two-slot overhang described in the header.

   data_0209f610 is arm9 bss and its ROM span is exactly 0xc -- the next symbol
   in config/arm9/symbols.txt is data_0209f61c. Three words, which is what
   func_020731dc writes into it: {next, dtor, object}.

   THE SEVEN BELOW ARRIVED WITH THE MOTION SLOTS, run link60 lane FDR2, and
   every size is the ROM span from config/arm9/symbols.txt rather than the
   width of the field the C reads through:

     data_0209f5f8   4     func_0202f58c's sub-engine select
     data_0209f5fc   4     func_0202f2c4 / func_0202f3a4's "table is live" flag
     data_0209f600   4     SetForwardTime's type == 2 palette, the bss twin of
                           data_020926c8
     data_0209f604   4     SetBackwardTime's, the twin of data_020926cc
     data_0209f608   4     the scanline table's WORKING index
     data_0209f60c   4     its SOURCE index
     data_0209f648   0x600 the table itself: two 0x300-byte double buffers of
                           192 scanlines x 4 bytes, which is what func_0202ee94
                           and func_0202efa0 fill and func_0202f2c4 pushes at
                           the blend registers one line at a time. The span is
                           the delta to data_0209fc48 and it agrees with the
                           stride the code indexes by.

   All seven are bss, so zero is the ROM's own starting value and nothing here
   invents one. They go inside the bracket with the rest: they are live fade
   state and a save state that rolled back the object without them would
   restore a fader pointing into a stale table. */
DSSTATE_BEGIN
extern "C" {
void *data_020926f0[12];
void *data_0209f610[3];
int data_0209f5f8[1];
int data_0209f5fc[1];
int data_0209f600[1];
int data_0209f604[1];
int data_0209f608[1];
int data_0209f60c[1];
unsigned char data_0209f648[0x600];
}
DSSTATE_END

namespace {

int fdr_trap_hits;

void fdr_trap(const char *slot, const char *rom_body)
{
    ++fdr_trap_hits;
    std::fprintf(stderr, "  [fdr] dWipe_c vtable %s FIRED -- the ROM body is "
                 "%s and this tranche did not seat it. See "
                 "port/fader_boot_map.txt.\n", slot, rom_body);
    std::fflush(stderr);
}

/* THE ARITY IS THE CALL SITE'S, NOT THE ROM BODY'S, and that distinction is
   the whole of section 9. A slot's signature here has one job: leave the stack
   exactly as the caller expects to find it. On ARM the ROM's extra arguments
   ride in registers and cost nothing, so the ROM body's own arity says nothing
   about what a host caller pushed; MSVC's __thiscall is CALLEE-CLEANS, so a
   caller that pushes two arguments and cleans neither needs a callee that
   cleans eight. 0x0c and 0x10 are both reached that way and are declared for
   it. 0x08 is not reached at all in this link and keeps the no-argument shape.

   __fastcall(self, edx, a, b) IS the host spelling of __thiscall(self, a, b):
   ECX carries the receiver, EDX is unread, a and b come off the stack, and the
   `ret 8` is what balances the frame.

   0x0c AND 0x10 ARE THE ROM'S OWN BODIES. func_0202f928 takes three arguments
   and func_0202f708 two, and the difference is not a mistake in either: on ARM
   the third rides in r2 and costs nothing, which is why the ROM body could
   ignore it and why the ROM's own C for the setter that DOES forward it says
   so. The stub cleans what the caller pushed and passes on what the body
   declares; those are two different counts and both are right.

   0x08 IS THE ONE SLOT IN THIS TABLE THAT IS NOT __fastcall, run link60
   Stage 5 lane SEAT8, and the reason is section 9b's shape C rather than a
   preference. Its one caller is the ROM's own src/func_02018efc.c:

       void func_02018efc(void){
         void** o = (void**)data_0209d4ac;
         if(!o) return;
         void** vt = (void**)o[0];
         ((void(*)(void*))vt[2])(o);
       }

   a CDECL call through a struct of function pointers -- the receiver is an
   ARGUMENT there, not a `this`, so MSVC pushes it and cleans its own push.
   That makes the stack-parameter spelling correct BY THE LANGUAGE and not by
   what a particular codegen happened to leave in ECX, which is the difference
   between this slot and the three dormant-hazard sites the unproven items in
   port/fader_boot_map.txt list.

   A __fastcall STUB WOULD ALSO HAVE WORKED HERE, and saying so is the point
   rather than an admission. Read out of this lane's own walk_window.exe:

       +0x00  mov ecx, dword ptr [_data_0209d4ac]
       +0x06  test ecx, ecx / je
       +0x0a  mov eax, dword ptr [ecx]
       +0x0c  push ecx
       +0x0d  mov eax, dword ptr [eax + 8]
       +0x10  call eax
       +0x12  pop ecx

   ECX still holds the object at the call, so a stub that read it would get
   the right receiver -- by the same coincidence, and with the same absence of
   any guard, as the three sites the unproven items already name. THIS DRAFT
   PREDICTED ECX WOULD HOLD THE VTABLE and the disassembly says otherwise; the
   prediction is recorded because it is the reason the stack spelling was
   chosen, and the choice survives being wrong about the codegen, which is
   exactly what "correct by the language" buys.

   THE CLEANUP IS BALANCED FOR BOTH SHAPES EVEN SO, which is why the choice
   costs nothing: __cdecl cleans nothing, a shape C caller cleans its own four
   bytes, and a shape A caller (a real C++ virtual with no arguments) pushes
   nothing. What a shape A caller WOULD get is a receiver read off its own
   frame, so the stub checks the vptr and says so instead of running on
   garbage. There is no shape A caller today: hal/fader_wipes.cpp's
   port_fader_advance dispatches AdvanceFade on data_0209d4b0[0], and nothing
   in the image ever puts this object there. */
void __cdecl fdr_s08(void *s)
{
    if (s == 0 || *(void **)s != (void *)data_020926f0) {
        fdr_trap("slot 0x08 AdvanceFade, WRONG RECEIVER",
                 "func_0202f428 -- the caller did not pass the object on the "
                 "stack, so it is not the cdecl shape this stub is declared "
                 "for");
        return;
    }
    func_0202f428(s);
}
int __fastcall fdr_s0c(void *s, void *, int frames, int c)
{ return func_0202f928(s, (unsigned)frames, (unsigned)c); }
int __fastcall fdr_s10(void *s, void *, int frames, int)
{ return func_0202f708(s, (unsigned)frames); }
int __fastcall fdr_s28(void *, void *)
{ fdr_trap("overhang slot 0x28", "nothing, this slot is not the ROM's"); return 0; }
int __fastcall fdr_s2c(void *, void *)
{ fdr_trap("overhang slot 0x2c", "nothing, this slot is not the ROM's"); return 0; }

/* The seven seated slots. Receiver out of ECX, into the ROM body's first
   argument. */
void *__fastcall fdr_s00(void *s, void *) { return func_0202fc08(s); }
void *__fastcall fdr_s04(void *s, void *) { return func_0202fbc8(s); }
int   __fastcall fdr_s14(void *s, void *) { return func_0202ee38(s); }
int   __fastcall fdr_s18(void *s, void *) { return func_0202eddc(s); }
int   __fastcall fdr_s1c(void *s, void *) { return func_0202ed7c(s); }
/* 0x20: the target the ROM's veneer at 0x0202ed08 tail-calls. See the header
   for why src/func_0202ed08.c is not in the link. */
void  __fastcall fdr_s20(void *s, void *)
{ ((FaderBrightness *)s)->FaderBrightness::SetToEnd(); }
void  __fastcall fdr_s24(void *s, void *) { func_0202ecfc(s); }

void fdr_fill(void)
{
    data_020926f0[0] = (void *)fdr_s00;
    data_020926f0[1] = (void *)fdr_s04;
    data_020926f0[2] = (void *)fdr_s08;
    data_020926f0[3] = (void *)fdr_s0c;
    data_020926f0[4] = (void *)fdr_s10;
    data_020926f0[5] = (void *)fdr_s14;
    data_020926f0[6] = (void *)fdr_s18;
    data_020926f0[7] = (void *)fdr_s1c;
    data_020926f0[8] = (void *)fdr_s20;
    data_020926f0[9] = (void *)fdr_s24;
    data_020926f0[10] = (void *)fdr_s28;
    data_020926f0[11] = (void *)fdr_s2c;
}

/* Fill the table. The ROM's initialiser used to be called from here too; run
   link100 lane CTOR, rung C1a, moved it to where the ROM runs it -- word 12 of
   the .ctor table at 0x02086b60, walked by func_02072f94 at Entry's own point
   (0x020048a8). hal/ctor_runner.cpp publishes the table and hal/rom_main.cpp
   makes Entry's call. The hand call was deleted in the same commit: a word in
   the table plus a call from here is __sinit_02074f80 running TWICE, and it is
   not idempotent -- func_020731dc would push a SECOND destructor node for the
   same object onto data_020aa3f0.

   THE ORDER THE HEADER ABOVE ARGUES FOR IS UNCHANGED AND STILL INTRA-FILE IN
   EFFECT: this fill runs at C++ static-init time, before walk_window's main,
   and the .ctor walk runs later still, inside the boot, so data_020926f0 is
   full before func_0202fc40 installs the vptr. What used to be guaranteed by
   sharing one initializer is now guaranteed by the CRT running every static
   initializer before main. The counter in hal/ctor_runner.cpp prints the
   number of times __sinit_02074f80 ran; one is the pass. */
struct FdrArm9FaderBoot {
    FdrArm9FaderBoot()
    {
        fdr_fill();
    }
};
FdrArm9FaderBoot fdr_arm9_fader_boot;

}  /* anonymous namespace */

/* ARE THE MOTION SLOTS STILL TRAPS? Run link60 lane NFS added this and the
   reason is worth keeping with the traps rather than with the caller. FDR's
   first unproven item said the three slots "have never been dispatched" and
   that "any other minigame, and the level path's own fader use, may reach
   them". Scene 374 did: with the NitroSDK open-by-name seam in, dScMgCurling_c
   got past InitResources to its first behaviour tick, slot 0x0c was
   dispatched, and the frame died on the stub ABI. So the fact stopped being a
   footnote and became a battery row, and a battery row needs a PREDICATE
   somebody outside this file can ask before the run rather than a string
   somebody greps for afterwards.

   IT NOW ASKS ABOUT ONE SLOT, and the narrowing is the point rather than a
   weakening. Run link60 lane FDR2 seated 0x0c and 0x10 on the ROM's own
   bodies, so a check that still tested all three would have gone on printing
   over two slots that run -- the exact rot the predicate was built against,
   arriving from the other direction. Only 0x08 is a trap now, so only 0x08 is
   tested, and the day func_0202f428 is seatable this goes false on its own.

   THE TEST IS STILL ON THE TABLE AND NOT ON A FLAG. It compares the installed
   word against the trap's own address, so re-pointing the slot is what
   retires it. Note the two slots dropped from the test are still filled by
   fdr_s0c and fdr_s10 -- the same symbols, no longer traps -- which is why
   comparing against those names would have kept answering "unseated" forever.
   The battery row this used to key is retired; hal/scene_mg.cpp's remaining
   use of it is an advisory.

   AND IT IS RETIRED NOW, run link60 Stage 5 lane SEAT8, on the contract the
   paragraph above wrote for it. Slot 0x08 is func_0202f428, so every one of
   the ROM's ten slots is a ROM body and the predicate has nothing left to
   ask. Retired rather than left returning zero: a predicate that can only
   answer one way is a line a reader has to disprove.

   THE ROT THE LAST PARAGRAPH PREDICTED IS EXACTLY WHY IT COULD NOT SIMPLY BE
   LEFT ALONE. fdr_s08 is still the symbol in slot 0x08 -- it forwards to the
   ROM body now instead of trapping -- so `data_020926f0[2] == fdr_s08` would
   have gone on answering "unseated" over a seated slot, which is the failure
   the predicate was built to prevent, arriving a third time from a third
   direction.

   WHAT REPLACES IT IS NARROWER AND STILL LOUD. fdr_s08's own wrong-receiver
   refusal counts into fdr_trap_hits and prints, so a caller that reaches slot
   0x08 with a shape the stub is not declared for says so on stderr and shows
   up in port_fdr_fader_report's count. That is a check on the thing that can
   actually still go wrong here, rather than on a seat that is done. */

/* The proof line, callable from a harness that wants it in its own output.
   Nothing in the build depends on it being called: the seat is complete
   without it, and the object above is what makes that true. */
extern "C" void port_fdr_fader_report(void)
{
    std::printf("[fdr] dWipe_c at data_0209f61c: vptr %p (table %p), "
                "%d unseated-slot hit(s)\n",
                (void *)(size_t)data_0209f61c[0], (void *)data_020926f0,
                fdr_trap_hits);
    std::fflush(stdout);
}
