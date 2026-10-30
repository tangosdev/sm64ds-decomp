// The fader-wipe array (WIPES / data_0209f324), staged for the host.
//
// Stage::InitResources builds the wipes at boot:
//
//     data_0209f324 = func_02073470(7, 0x60, 8, FaderWipe::FaderWipe,
//                                               FaderWipe::~FaderWipe);
//
// seven 0x60-byte FaderWipe objects on the stage heap, each carrying the
// ROM's _ZTV9FaderWipe. The port never boots a Stage, so that pointer was
// plain zero, and the first death took the null through
// KillPlayer -> StartExitFaderWipe -> &WIPES[4] -> a virtual call on
// address 0x180. That fault is why gate 1 excluded the wipe and why the
// tier-2 wave held the four death states out. The same null sat under
// St_Respawn_Main, which calls FUN_02029980 -> WIPES + 0x240.
//
// What is staged here is the array and its vtable, not the wipe itself.
// FaderWipe's fade is a ModelComponents::Render of a mesh that
// FaderWipe::LoadAndSetFile pulls from the stage filesystem, which the
// port does not mount, so nothing draws. The interpolator the rest of the
// engine polls IS real, and since wave 3 it is not merely mirrored: the five
// matched bodies that own it are CALLED from the slots below -- see the
// delegation note after the slot table. AdvanceFade keeps one deliberate
// divergence: an UNDRIVEN advance snaps currInterp to its target rather than
// stepping, because a fade nobody is driving must not hold a scene transition
// open for 30 frames, while the frame loop's phase 2 (port_frame_phase2) steps for real
// so the fade renders.
//
// ---- SLOT ORDER: the ROM's, not MSVC's ------------------------------------
//
// The wipe is reached two different ways in src, and the two disagree:
//
//   raw-offset view   Scene::SetFaders, FUN_02029980 and FUN_020299f4 hand-
//                     write a struct of function pointers over the vtable
//                     word and index it at ROM byte offsets.
//   class view        StartExitFaderWipe.c / StartEntranceFaderWipe.c declare
//                     a `struct FaderWipe : Fader` with nine virtuals and let
//                     the compiler pick the slot.
//
// mwcc emits TWO destructor slots (D1 at 0x00, D0 at 0x04); MSVC folds them
// into one. So a host-generated class vtable runs one slot ahead of the ROM
// from AdvanceFade onward. The table below is laid out in ROM order, which
// is the order the raw-offset callers need -- and they are the ones that
// matter, because two of them pass arguments (FUN_02029980 calls the 0x0c
// slot as w3(0x1e, 0)). Under a class-ordered table that call would land on
// a no-argument predicate and unbalance the __thiscall stack. The 0x0c and
// 0x10 stubs therefore take the two ints those call sites push.
//
//     host/ROM  name                   reached by
//     0x00      ~FaderWipe   (ROM D1)  nobody on host
//     0x04      (ROM D0)               nobody on host
//     0x08      AdvanceFade            the scene loop, once hosted
//     0x0c      SetBackwardTime        FUN_02029980  (respawn)
//     0x10      SetForwardTime         FUN_020299f4  (dead-pit, battle levels)
//     0x14      IsAtStart              Scene::SetFaders
//     0x18      IsAtEnd                Scene::SetFaders
//     0x1c      IsBetweenStartAndEnd   FUN_02029980, FUN_020299f4
//     0x20      SetToEnd               FUN_02029980, Scene::SetFaders,
//                                      AND StartExitFaderWipe's SetToStart
//     0x24      SetToStart             FUN_020299f4, Scene::SetFaders
//     0x28/2c   tail                   nobody; they exist so a raw-offset
//                                      read past 0x24 stays inside a table
//
// The one skew left is on the last line of that 0x20 row: StartExitFaderWipe
// compiles `f->SetToStart()` into class slot 8 = byte 0x20, which here is
// SetToEnd. The host result is that the exit wipe lands snapped to its END
// (currInterp = 0x1000) rather than its start, which is the outcome the port
// wants anyway -- IsAtEnd, the predicate a scene transition waits on, reads
// true immediately instead of waiting on a fade that can never render.
//
// ---- WHAT IS HOST HERE AND WHAT IS THE DECOMP -----------------------------
//
// The table below is a host CLASS, because the ROM slot order above is the
// whole point of it and MSVC will not emit that order for a class derived from
// the ROM's Fader. But the ARITHMETIC inside the slots is no longer host: the
// five interpolator bodies that carry it are matched TUs, and this file now
// CALLS them instead of restating them (slice_w1l3.txt seats the five):
//
//     Fader::AdvanceInterp            the one-frame 20.12 step
//     FaderBrightness::SetToStart     currInterp = 0
//     FaderBrightness::SetToEnd       currInterp = 0x1000
//     FaderBrightness::IsAtStart      currInterp == 0
//     FaderBrightness::IsAtEnd        currInterp == 0x1000
//
// The cast is a reinterpret, not a base conversion: Fader is vptr at 0x0 then
// currInterp 0x4 and speed 0x8 (include/Fader.h, pinned by the bytes), which is
// exactly HalFaderWipe's prefix, and every one of the five is reached QUALIFIED
// so no call re-dispatches through the host table.
//
// THREE SLOTS ARE DELIBERATELY NOT DELEGATED, and the reason is the MSVC dtor
// fold. FaderBrightness's matched SetForwardTime/SetBackwardTime end in a
// VIRTUAL IsAtEnd()/IsAtStart(), and IsBetweenStartAndEnd is two more. Under
// mwcc those resolve at ROM slots 5 and 6; MSVC folds D1/D0 into one slot, so
// on a HalFaderWipe -- which spends two slots keeping ROM byte order -- a
// FaderBrightness-slot-4 call lands on SetForwardTime and a slot-5 call on
// IsAtStart. Slot 4 is the dangerous one: it takes two ints under __thiscall,
// so a no-argument virtual landing there unbalances the stack. Those three stay
// host, spelled to match the matched bodies they cannot safely call.
#include <cstdio>
#include <cstdlib>
#include <new>
#include "dsstate_seg.h"
/* The ROM classes whose interpolator this file mirrors. Included for the
   LAYOUT and the method declarations, never to derive from. Pulls types.h,
   whose `typedef s32 Fix12i` is the same `int` this file used to typedef
   locally, so the local typedef is gone rather than shadowed. */
#include "dFdBrightness_c.h"
/* For the C-name face at the bottom of this file. FaderWipe.h's FaderWipe is
   the auto-generated NON-polymorphic view -- pad_000[4] where the vptr sits,
   no virtuals -- so including it cannot collide with HalFaderWipe below. */
#include "dFdWipe_c.h"

/* Fader::AdvanceInterp calls the 20.12 approach helper by its ROM spelling,
   func_0203ae58. That name is DEFINED only in hal/shims.cpp, the gate-1 smoke's
   target, which walk_window does not link -- so seating AdvanceInterp leaves it
   unresolved unless this file supplies it. The matched ApproachLinear(int&,
   int, int) IS linked here (src/_Z14ApproachLinearRiii.cpp, reached through the
   C face in hal/player_bridges.cpp), and the two agree on the cdecl signature;
   func_0203ae58 is declared void and ApproachLinear returns int, which a cdecl
   caller discards. alternatename rather than a definition, so that a target
   which DOES link shims.cpp keeps its own strong one and nothing collides. */
#pragma comment(linker, "/alternatename:_func_0203ae58=__Z14ApproachLinearRiii")

/* SetBlendBrightness, the matched G2x routine, is what both FaderColor and
   FaderWipe write the 2D master-blend register with. It is in slice_gate1, so
   reuse it -- the host mapping at 0x4000050/0x4000054 latches the write and the
   framebuffer compositor (walk_window.cpp) reads it back. */
extern "C" void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p,
                                                  unsigned short val, short amt);

/* cstd::fdiv by its flat name, declared exactly as the matched FaderBrightness
   setters declare it. On the three targets that compile this file the name is
   the ROM's own body (src/_ZN4cstd4fdivEii.cpp over the divider model; see
   hal/cstd_div.c's header for where it is seated and where it is not). */
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

/* The step a fade of `frames` frames takes each frame, computed the way
   FaderBrightness::SetForwardTime / SetBackwardTime compute it
   (src/engine/fader/): +-0x1000 for no frames, else
   cstd::fdiv(+-0x1000, frames << 12). fdiv ROUNDS: fdiv_result (arm9
   0x020530cc) reads the divider's 64/32 quotient back as (q + 0x80000) >> 20.
   The plain 0x1000 / frames this file used before truncates, which is not the
   same number whenever 0x1000 / frames has a fraction of a half or more: 30
   frames stepped 136 here against the cartridge's 137, so 0 -> 0x1000 took 31
   steps instead of 30. */
static Fix12i hal_fade_speed(unsigned frames, int backward)
{
    const Fix12i one = backward ? -(Fix12i)0x1000 : (Fix12i)0x1000;
    if (frames == 0)
        return one;
    return _ZN4cstd4fdivEii(one, (Fix12i)(frames << 12));
}

/* the wipe picture's ROM callees and tables (THE WIPE'S PICTURE, below) */
extern "C" {
void _ZN5ModelC1Ev(void *thiz);
void _ZN5ModelD1Ev(void *thiz);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToScale(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(void *thiz, void *mtx,
                                                      void *vec);
void _ZN9dFdWipe_c14LoadAndSetFileEt(void *thiz, unsigned short fileID);
extern unsigned short data_020755e0[], data_020755f0[], data_02075600[];
extern signed char data_0209f2f8;       /* the level being built */
}

namespace {

/* Snap vs step: the historical stub snapped the interpolator to its target so
   an invisible fade could not hold a transition open. Now the fade renders, so
   it has to STEP -- but only when the frame loop is actually driving the
   advance (port_frame_phase2). A direct SetToStart/SetToEnd still snaps. The
   flag is set for the duration of one driven advance. */
int g_hal_fader_stepping;

int hal_wipe_index(const void *self);

/* Is the word the caller handed us a wipe object, or the vtable a wrong-shape
   caller left behind? Both defined beside hal_wipe_index, under the array. */
int hal_wipe_receiver_ok(const void *self);
void hal_wipe_shape_trap(const char *slot, const void *self);

/* THE WIPE'S PICTURE (see the block under the array): does this object carry
   the wipe mesh this level's boot loaded, and the ROM's own
   FaderWipe::AdvanceFade statements with the mesh draw handed to the render
   phase. */
int hal_wipe_mesh_ready(void *self);
int hal_wipe_advance_rom(void *self);

/* Loud, but not per-frame: the first few calls say what the host is
   skipping, then it goes quiet. */
void hal_wipe_note(const char *what, const void *self)
{
    static int said;
    if (said >= 8) return;
    ++said;
    std::fprintf(stderr, "  [wipe] %s on wipe %d (host stub: the "
                 "interpolator is real, no fade renders)\n",
                 what, hal_wipe_index(self));
    if (said == 8)
        std::fprintf(stderr, "  [wipe] (further wipe calls stay quiet)\n");
}

}  /* anonymous namespace */

/* ---- THE CLASS IS NAMED, AND ITS VTABLE CARRIES THE ROM'S SYMBOL ----------
   run link60 lane SL0. HalFaderWipe used to sit in the anonymous namespace,
   which made its vtable a local symbol nothing could refer to by name. It is
   at file scope now for exactly one reason: MSVC emits its vtable as
   ??_7HalFaderWipe@@6B@, and the ROM's _ZTV9FaderWipe is aliased onto it at
   the bottom of this file.

   THAT ALIAS IS ONLY LEGITIMATE BECAUSE THE TWO TABLES AGREE, and the
   agreement was MEASURED rather than assumed. _ZTV9FaderWipe is arm9
   0x0208ea9c; read out of extracted/arm9_dec.bin at (addr - 0x02004000) and
   every word resolved against config/arm9/symbols.txt, the ROM's TEN slots
   are:

     0x00 FaderWipe::~FaderWipe (D1)        0x18 FaderBrightness::IsAtEnd
     0x04 FaderWipe D0                      0x1c FaderBrightness::
     0x08 FaderWipe::AdvanceFade                 IsBetweenStartAndEnd
     0x0c FaderBrightness::SetBackwardTime  0x20 FaderBrightness::SetToEnd
     0x10 FaderBrightness::SetForwardTime   0x24 FaderBrightness::SetToStart
     0x14 FaderBrightness::IsAtStart

   which is the slot table at the head of this file, byte for byte, 0x00
   through 0x24. The ROM order this file adopted from the raw-offset callers
   is now confirmed against the ROM record itself, and the two dtor slots the
   class spends to keep that order are what make the alias sound: MSVC folds
   D1/D0 into one slot, so a class with a single virtual destructor would run
   one slot ahead of the ROM from AdvanceFade onward.

   THE RECORDS ARE TEN SLOTS, NOT TWELVE, and reading them as twelve is the
   easy mistake this note used to make. Spanning symbol to symbol picks up two
   extra words past 0x24, which look like a zero and a pointer to
   _ZTI15dFdBrightness_c and invite being read as a trailer. They are not part of this
   table at all: they are the ITANIUM HEADER OF THE NEXT VTABLE, offset-to-top
   then typeinfo, and config/arm9/relocs.txt proves it rather than suggesting
   it. Three rows, one per fader record:

     from:0x0208ea98 kind:load to:0x0208ea18   header of _ZTV9FaderWipe
     from:0x0208eac8 kind:load to:0x0208ea24   header of data_0208eacc
     from:0x0208eb28 kind:load to:0x0208ea00   header of data_0208eb2c

   Each typeinfo word sits four bytes BELOW the symbol the config names, which
   is exactly where the Itanium ABI puts it. So _ZTV9FaderWipe runs
   0x0208ea9c..0x0208eac4, ten slots, and the word at 0x0208eac8 belongs to
   FaderBrightness.

   THE DIVERGENCE THAT REMAINS IS STILL REAL AND STILL DELIBERATE. This class
   spends two slots past 0x24 on HalTail28 and HalTail2c, where the ROM's
   object has the next record's header. Nothing in the port or in src reads a
   fader vtable past 0x24, so it is unobservable; it is kept because a
   raw-offset read that overruns lands on something callable instead of on a
   typeinfo pointer. Recorded as a divergence, not dressed up as a trailer.

   AND Fader'S OWN TABLE IS MOSTLY NULL. data_0208eafc carries Fader::~Fader
   at 0x00 and its D0 at 0x04 and then eight zero slots, 7, 8 and 9 included.
   Fader is abstract for everything below the destructor pair; the concrete
   behaviour arrives at FaderBrightness. That is why FaderWipe's ctor writes
   this table only in passing.

   include/Fader.h AND include/FaderBrightness.h ARE WRONG, and this is the
   one correction here that costs someone else work. Both declare
   IsBetweenStartAndEnd, SetToEnd and SetToStart as NON-virtual members. The
   ROM has all three in the vtable, at slots 7, 8 and 9, in FaderBrightness,
   FaderColor and FaderWipe alike. That error is the source of the one skew
   this file documents at its 0x20 row. NOT FIXED HERE ON PURPOSE: include/ is
   byte gate input, mwccarm already emits two destructor slots, and adding a
   virtual to Fader shifts every slot and breaks the matched fader TUs. It is
   a decomp-side correction and it needs the byte gate, not this file.

   ONE CLAIM RETRACTED. Commit 7a1e6b17f's message reads data_0208eb2c's
   naming as another instance of the Actor/ActorBase D1/D2 swap, on the
   grounds that src/_ZN10FaderColorD2Ev.cpp calls it "vtable for Color". It is not.
   FaderColor's destructors are named correctly in the config -- slots 0 and 1
   of data_0208eb2c are _ZN10FaderColorD1Ev and _ZN10FaderColorD0Ev, exactly
   as the config has them -- and Color at 0x02017574 is its own base class,
   not a misnamed FaderColor D2. The commit message stands as history; this
   note carries the truth. */

/* THE ROM'S OWN DELETING DESTRUCTOR, for slot 0x04 below.  Run link100 wave 7,
   lane SEAT2, census batch B2.  src/engine/fader/_ZN9FaderWipeD0Ev.c is the
   matched body at arm9 0x02017418, and the cartridge's own table holds it at
   _ZTV9FaderWipe + 0x04 -- read out of extracted/arm9_dec.bin at 0x0208eaa0
   and confirmed by config/arm9/relocs.txt's `from:0x0208eaa0 kind:load
   to:0x02017418`.  That word is THIS class's slot 0x04, because the alias at
   the bottom of this file makes ??_7HalFaderWipe@@6B@ the ROM's table. */
extern "C" void *_ZN9dFdWipe_cD0Ev(void *thiz);

struct HalFaderWipe {
    Fix12i currInterp;
    Fix12i speed;
    unsigned short color;
    unsigned short unk0e;
    unsigned char model[0x50];

    HalFaderWipe() : currInterp(0), speed(0), color(0), unk0e(0)
    {
        for (int i = 0; i < 0x50; ++i) model[i] = 0;
    }

    virtual ~HalFaderWipe() {}                       /* 0x00  ROM D1 */
    /* 0x04  ROM D0.  WAS AN EMPTY STUB and the slot table above still says
       what it says: "nobody on host".  It is the ROM's body now (lane SEAT2),
       and that changes nothing about what runs, because no dispatch site in
       the port or in src reaches a wipe's second destructor word -- the seven
       wipes are Stage::InitResources' pool objects and hal/fader_wipes.cpp's
       stand-ins for them are static, so nothing deletes one.  What it changes
       is that the word is the cartridge's word.  The trace line is here
       because a seat nothing enters and a seat that frees a static object are
       opposite findings and both are silent otherwise. */
    virtual void DtorDeleting()                      /* 0x04  ROM D0 */
    {
        hal_wipe_note("DtorDeleting (ROM D0, the matched body)", this);
        _ZN9dFdWipe_cD0Ev(this);
    }
    virtual int AdvanceFade()                        /* 0x08 */
    {
        /* Driven advance (the frame loop's port_frame_phase2) STEPS the
           interpolator one frame and writes the 2D master-blend register the
           way FaderColor::AdvanceFade does, so the fade renders. Any other
           caller keeps the old snap: a fade nobody is driving must not stall a
           transition. */
        if (!g_hal_fader_stepping) {
            hal_wipe_note("AdvanceFade", this);
            currInterp = speed >= 0 ? 0x1000 : 0;
            return 1;
        }
        /* ONE OF THE SEVEN WIPES WITH ITS MESH: FaderWipe::AdvanceFade's own
           statements, the iris the cartridge draws (the block under the array).
           The colour fader placement-new'd at data_0209f5e8 shares this class
           and keeps the FaderColor arm below, as does a wipe whose mesh could
           not be loaded. */
        if (hal_wipe_mesh_ready(this))
            return hal_wipe_advance_rom(this);
        Fix12i old = currInterp;
        Fix12i target = speed >= 0 ? 0x1000 : 0;
        /* the step itself is the matched body: it picks target and |step| from
           the sign of speed exactly as the three lines here used to */
        ((dFader_c *)(void *)this)->dFader_c::AdvanceInterp();
        if (currInterp == old)
            return currInterp == target;
        /* FaderColor::AdvanceFade, verbatim on the numbers: color != 0 means a
           WHITE fade (brightness increase), color == 0 a BLACK fade (decrease).
           r is the 5-bit EVY coefficient, 0..0x10. Both 2D engines
           (0x4000050 main, 0x4001050 sub) get it so the whole panel fades. */
        int m = color ? 0x10 : -0x10;
        int r = (currInterp * m) >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts(
                (volatile unsigned short *)0x4000050, 0x3f, (short)r);
            _ZN3G2x18SetBlendBrightnessEPVtts(
                (volatile unsigned short *)0x4001050, 0x3f, (short)r);
        } else {
            *(volatile unsigned short *)0x4000050 = 0;
            *(volatile unsigned short *)0x4001050 = 0;
        }
        return currInterp == target;
    }
    /* 0x0c / 0x10 -- NOT delegated: the matched bodies end in a virtual
       IsAtStart()/IsAtEnd() that would land one slot off here. See the dtor-fold
       note in the header. The arithmetic IS the matched bodies' own:
       hal_fade_speed above makes their cstd::fdiv call, rounding included.
       This comment used to say fdiv(+-0x1000, frames << 12) equals
       +-0x1000 / frames; it does not, and the difference was one frame on
       every 30-frame wipe (the arrival fade after a star took 31 frames at
       -136 per frame here, and takes 30 at -137, as the opening's fades already
       did through the matched setters on hal/scene_boot.cpp's FaderColor
       table).

       AND SERVED IN BOTH CALL SHAPES, as the sibling tables are. These two
       slots are the fifth class of the shape 0ec379b94 and 19a71216e retired on
       those tables, and the reasoning is 19a71216e's word for word. These two
       slots have THREE callers; this block once named only the first,
       src/_ZN8dScene_c14BeforeBehaviorEv.cpp, whose
       banner says why it cannot go through the real class and keep matching: it
       reaches the installed fader through a file-local FaderVTable of PLAIN
       FUNCTION POINTERS with an explicit first parameter,
       `void (*SetBackwardTime)(void *, u32, u32)`, and MSVC compiles that as
       __cdecl:

           push 0
           push 1Eh
           push eax            <- THE RECEIVER, a stack argument
           call eax
           add  esp,0Ch        <- the CALLER takes all twelve back

       and one instruction before the call it does `mov ecx,[eax]`, so ECX holds
       THE VTABLE at that call, not the object. A __thiscall member here reads
       ECX as `this` and its very first statement writes `speed` at this+8:
       measured on levels 26, 34 and 35 as

           FAULT c0000005 at ?SetBackwardTime@HalFaderWipe@@UAEHHH@Z+0x12
           ecx 007192a0 = ??_7HalFaderWipe@@6B@, WRITE to 007192a8 = that + 8

       i.e. the stub writing the fade speed into its own vtable, in .rdata. A
       __cdecl member takes `this` as its first stack word and cleans nothing,
       which is exactly the frame the call site builds. Run link100, lane
       SINGLES2; measured by lane SINGLES (out/SINGLES/handoff_BOOT1.md).

       The other two are src/FUN_020299f4.cpp (slot 0x10, from
       Player::St_DeadPit_Init on the local player's own pit death in a VS
       match) and src/FUN_02029980.cpp (slot 0x0c, from Player::St_Respawn_Main,
       the respawn after it). Both reach wipe 6 (data_0209f324 + 0x240) through
       a shadow struct of plain virtuals, which MSVC compiles __thiscall: the
       receiver in ecx, (0x1e, 0) pushed, the CALLEE to take them back. Against
       these __cdecl slots the call read 0x1e as the receiver ("WRONG RECEIVER
       at SetForwardTime"), left eight bytes on the caller's frame, and the
       caller returned into the pushed 0: FAULT c0000005 at eip 0 on level 29.
       Those two files are the matched source and stay so: the slots below
       take both shapes, as the sibling tables' slots do. Run mp1, lane
       PITCRASH1.

       Receiver check included for 19a71216e's stated reason: a caller of the
       old shape would hand these two a vtable or a frame count as the receiver,
       and a fault on that reports nothing. */
    /* BOTH SHAPES, as hal/scene_boot.cpp's FaderColor table (d6141aa019)
       and hal/fdr_arm9_fader_seat.cpp's dWipe_c table (43dd453d0d) serve
       them: naked trampolines below, the bodies port_hal_wipe_s0c / _s10. */
    virtual int SetBackwardTime(int frames, int);            /* 0x0c */
    virtual int SetForwardTime(int frames, int);             /* 0x10 */
    /* 0x14 / 0x18 -- the matched predicates, reached qualified */
    virtual int IsAtStart()
    { return ((dFdBrightness_c *)(void *)this)->dFdBrightness_c::IsAtStart(); }
    virtual int IsAtEnd()
    { return ((dFdBrightness_c *)(void *)this)->dFdBrightness_c::IsAtEnd(); }
    virtual int IsBetweenStartAndEnd()                         /* 0x1c */
    {
        return HalFaderWipe::IsAtStart() == 0 &&
               HalFaderWipe::IsAtEnd() == 0;
    }
    virtual void SetToEnd()                                    /* 0x20 */
    {
        hal_wipe_note("SetToEnd", this);
        ((dFdBrightness_c *)(void *)this)->dFdBrightness_c::SetToEnd();
    }
    virtual void SetToStart()                                  /* 0x24 */
    {
        hal_wipe_note("SetToStart", this);
        ((dFdBrightness_c *)(void *)this)->dFdBrightness_c::SetToStart();
    }
    virtual void HalTail28() { hal_wipe_note("tail slot 0x28", this); }
    virtual void HalTail2c() { hal_wipe_note("tail slot 0x2c", this); }
};

namespace {

/* Seven, the count Stage::InitResources passes.

   INSIDE THE SAVE STATE'S SPAN. On the cartridge the seven wipes are heap
   objects (Stage::InitResources builds them with func_02073470), so a snapshot
   of the heap carries them, meshes and interpolators alike. Here each wipe's
   Model words point at blocks the level boot carves out of the heap arena
   (port_fader_wipes_load, below), and hal/lk6_savestate.cpp rolls that arena
   back on a restore: the words have to roll back with it, or a restore hands
   the next level change's release a pointer into a block the restored arena
   never handed out. data_0209f5e8, the eighth object of this class, has been
   in the span since gate 31 for the same reason. */
DSSTATE_BEGIN
HalFaderWipe hal_wipes[7];
DSSTATE_END

int hal_wipe_index(const void *self)
{
    long long d = (const char *)self - (const char *)&hal_wipes[0];
    return (int)(d / (long long)sizeof(HalFaderWipe));
}

/* Every object this file hands out -- the seven wipes and the colour fader
   placement-new'd into data_0209f5e8 -- carries ??_7HalFaderWipe@@6B@ in its
   first word, so comparing against wipe 0's own vptr accepts all eight and
   rejects the vtable itself, a frame count and a null. Wipe 0 is constructed
   at static init, long before any scene runs. */
int hal_wipe_receiver_ok(const void *self)
{
    if (self == 0 || ((std::size_t)self & 3) != 0)
        return 0;
    return *(void *const *)self ==
           *(void *const *)(const void *)&hal_wipes[0];
}

void hal_wipe_shape_trap(const char *slot, const void *self)
{
    static int said;
    if (said >= 4) return;
    ++said;
    std::fprintf(stderr, "  [wipe] WRONG RECEIVER at %s: %p is not a wipe "
                 "object. A caller of the old __thiscall shape reached a "
                 "__cdecl slot; see the block above SetBackwardTime.\n",
                 slot, self);
    std::fflush(stderr);
}

/* ---- THE WIPE'S PICTURE -----------------------------------------------------
 *
 * WHAT THE CARTRIDGE DRAWS. A level exit, a death and a level entrance run one
 * of these seven wipes for 0x1e frames (dScene_c::BeforeBehavior's
 * SetForwardTime(0x1e) / SetBackwardTime(0x1e)), and FaderWipe::AdvanceFade
 * (src/engine/fader/_ZN9FaderWipe11AdvanceFadeEv.cpp) draws the wipe's MESH
 * over the scene: a flat model with a shaped hole (data/wipe/wipe_mario.bmd,
 * _luigi, _wario, _yoshi, _star, _koopa), placed at (0, 0, -1) in camera space,
 * turned 0x4000 about X and scaled by (0x1000 - interp) * 0x20, so the hole
 * closes on the player as interp runs to 0x1000. The top screen's colour
 * effects are switched off while it moves and set to EVY 16 black once it is
 * closed; the bottom screen darkens with interp (BLDY on engine B).
 *
 * WHAT THE PORT DREW. Nothing: the meshes were never loaded (Stage::
 * InitResources loads them with FaderWipe::LoadAndSetFile from data_020755e0 /
 * _5f0 / _600, and the port boots levels without that loop), and this class's
 * AdvanceFade was FaderColor's whole-screen EVY fade on both engines.
 *
 * WHAT RUNS NOW, per wipe, in two halves:
 *   LOAD. At every level boot, where Stage::InitResources runs its wipe
 *   loop (port_fader_wipes_load, called from hal/level_boot.cpp), Model's
 *   constructor and then FaderWipe::LoadAndSetFile run on each wipe's own
 *   +0x10 Model with the file id Stage::InitResources passes for this level:
 *   data_02075600 in VS off archive 0xbf, data_020755f0 on level 5,
 *   data_020755e0 otherwise. The level change frees them again
 *   (port_fader_wipes_reset), as Stage::CleanupResources does.
 *   STEP. FaderWipe::AdvanceFade's statements in its order (the step is the
 *   matched Fader::AdvanceInterp, the brightness writes are the matched
 *   SetBlendBrightness), except that the mesh's ModelComponents::Render is
 *   HANDED TO THE RENDER PHASE: the DS submits it into the geometry FIFO at
 *   phase 2 and it is drawn with the frame, while the port opens its geometry
 *   frame after phase 2 (tests/walk_window.cpp, the phase-4 gx_reset), which
 *   would drop it. port_fader_wipe_render submits the same call with the same
 *   matrix just before the frame is rasterised.
 *
 * SM64DS_WIPE_MESH_OFF=1 keeps the old whole-screen fade on the same exe. */

void *g_wipe_draw;                      /* the wipe whose mesh draws this frame */
int g_wipe_draw_scale;

int hal_wipe_mesh_off(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_WIPE_MESH_OFF");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

/* FaderWipe's destructor, its Model half: Model::~Model frees the transform
   buffer DoSetFile allocated and ModelBase::~ModelBase frees the file
   (src/_ZN5ModelD1Ev.cpp, src/_ZN9ModelBaseD2Ev.cpp), which is all
   Stage::CleanupResources' func_02073244(WIPES, 0x60, 8, ~FaderWipe) frees per
   wipe. The rest of that destructor chain only stores vtable pointers, which
   these host objects must keep (their table is ??_7HalFaderWipe@@6B@), so the
   Model's words go back to zero, the state the pool starts in. Returns how
   many wipes held a mesh. */
int hal_wipes_release(void)
{
    int n = 0;
    for (int i = 0; i < 7; ++i) {
        unsigned char *model = hal_wipes[i].model;
        if (*(void **)model) {
            if (*(void **)(model + 4))
                ++n;
            _ZN5ModelD1Ev(model);
        }
        for (int b = 0; b < 0x50; ++b)
            model[b] = 0;
    }
    return n;
}

/* SM64DS_WIPE_TRACE=1: what each wipe's mesh reads out of texture VRAM. At the
   load, every texture and palette span the file was given (texture slots at
   0x06800000 + (flags & 0xffff) << 3, palette slots at 0x06880000 +
   vramOffset: the offsets LoadTexAndPal wrote) and an FNV-1a hash of their
   bytes; at the wipe's first draw after that load, the same spans hashed
   again, and the PREVIOUS load's spans hashed now -- which is exactly what a
   mesh kept from the previous level would read. Inert unset. */
struct HalWipeSpans {
    int n;
    unsigned addr[8], size[8];
    unsigned hash;
    int level;
};
HalWipeSpans hal_wipe_spans_cur[7], hal_wipe_spans_prev[7];
int hal_wipe_drawn_since_load[7];

int hal_wipe_trace_on(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_WIPE_TRACE");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

unsigned hal_wipe_spans_hash(const HalWipeSpans &r)
{
    unsigned h = 2166136261u;
    for (int k = 0; k < r.n; ++k) {
        const unsigned char *p = (const unsigned char *)(std::size_t)r.addr[k];
        for (unsigned b = 0; b < r.size[k]; ++b)
            h = (h ^ p[b]) * 16777619u;
    }
    return h;
}

void hal_wipe_tex_note(int i)
{
    if (!hal_wipe_trace_on())
        return;
    const unsigned char *f = *(const unsigned char *const *)(hal_wipes[i].model + 4);
    HalWipeSpans r = {};
    const unsigned ntex = *(const unsigned *)(f + 0x14);
    const unsigned char *tex = *(const unsigned char *const *)(f + 0x18);
    for (unsigned t = 0; t < ntex && r.n < 8; ++t, tex += 0x14) {
        r.addr[r.n] = 0x06800000u + ((*(const unsigned *)(tex + 0x10) & 0xffffu) << 3);
        r.size[r.n] = *(const unsigned *)(tex + 8);
        ++r.n;
    }
    const unsigned npal = *(const unsigned *)(f + 0x1c);
    const unsigned char *pal = *(const unsigned char *const *)(f + 0x20);
    for (unsigned t = 0; t < npal && r.n < 8; ++t, pal += 0x10) {
        r.addr[r.n] = 0x06880000u + *(const unsigned *)(pal + 0xc);
        r.size[r.n] = *(const unsigned *)(pal + 8);
        ++r.n;
    }
    r.hash = hal_wipe_spans_hash(r);
    r.level = (int)data_0209f2f8;
    if (hal_wipe_spans_cur[i].n)
        hal_wipe_spans_prev[i] = hal_wipe_spans_cur[i];
    hal_wipe_spans_cur[i] = r;
    hal_wipe_drawn_since_load[i] = 0;
    std::fprintf(stderr, "  [wipe-trace] load wipe %d level %d: %d span(s), "
                 "first 0x%08x+0x%x, vram fnv %08x\n", i, r.level, r.n,
                 r.n ? r.addr[0] : 0, r.n ? r.size[0] : 0, r.hash);
}

void hal_wipe_tex_draw_note(int i)
{
    if (!hal_wipe_trace_on() || i < 0 || i >= 7 || hal_wipe_drawn_since_load[i])
        return;
    hal_wipe_drawn_since_load[i] = 1;
    const HalWipeSpans &c = hal_wipe_spans_cur[i];
    const HalWipeSpans &p = hal_wipe_spans_prev[i];
    const unsigned now = hal_wipe_spans_hash(c);
    std::fprintf(stderr, "  [wipe-trace] draw wipe %d level %d: its spans "
                 "(first 0x%08x) fnv %08x at load, %08x now (%s)",
                 i, (int)data_0209f2f8, c.n ? c.addr[0] : 0, c.hash, now,
                 now == c.hash ? "same" : "CHANGED");
    if (p.n) {
        const unsigned pnow = hal_wipe_spans_hash(p);
        std::fprintf(stderr, "; level %d's spans (first 0x%08x) fnv %08x at "
                     "their load, %08x now (%s)", p.level, p.addr[0], p.hash,
                     pnow, pnow == p.hash ? "same" : "CHANGED");
    }
    std::fprintf(stderr, "\n");
}

int hal_wipe_mesh_ready(void *self)
{
    const int i = hal_wipe_index(self);
    if (i < 0 || i >= 7 || self != (void *)&hal_wipes[i] ||
        hal_wipe_mesh_off())
        return 0;
    /* The mesh is the level boot's (port_fader_wipes_load, the line
       Stage::InitResources runs between the twelve preloads and
       Stage::LoadModel), loaded into this level's texture VRAM and freed at
       the level change with the wipe (port_fader_wipes_reset). Nothing loads
       here any more: a wipe whose file did not load keeps the whole-screen
       fade, and a boot that never ran the load (SM64DS_LEGACY_BOOT) says so
       once. */
    if (*(void **)(hal_wipes[i].model + 4))     /* Model::modelFile */
        return 1;
    static int said;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [wipe] wipe %d has no mesh on level %d (the "
                     "level boot did not load it): the whole-screen fade "
                     "stays\n", i, (int)data_0209f2f8);
    }
    return 0;
}

int hal_wipe_advance_rom(void *self)
{
    Fix12i &cur = *(Fix12i *)((char *)self + 4);
    Fix12i &spd = *(Fix12i *)((char *)self + 8);
    const unsigned short color = *(unsigned short *)((char *)self + 0xc);
    const Fix12i target = spd >= 0 ? 0x1000 : 0;
    const Fix12i old = cur;
    ((dFader_c *)self)->dFader_c::AdvanceInterp();
    if (cur == 0 && cur == old)
        return cur == target;
    if (cur == 0x1000) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4000050,
                                          0x3f, -0x10);
    } else {
        if (spd != 0 && old != 0x1000)
            *(volatile unsigned short *)0x4000050 = 0;
        if (cur != 0) {
            g_wipe_draw = self;
            g_wipe_draw_scale = (Fix12i)(((long long)(0x1000 - cur) * 0x20000
                                          + 0x800) >> 12);
        }
    }
    if (cur == old)
        return cur == target;
    const int m = color ? 0x10 : -0x10;
    const int r = (cur * m) >> 12;
    if (r != 0) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4001050,
                                          0x3f, (short)r);
    } else {
        *(volatile unsigned short *)0x4000050 = 0;
        *(volatile unsigned short *)0x4001050 = 0;
    }
    return cur == target;
}

}  /* anonymous namespace */

/* ---- gate 31: the COLOR fader, 0x0209f5e8 --------------------------------
   The eighth object, and the one that is not in the WIPES array.
   Scene::SetAndStopColorFader hands &data_0209f5e8 to Scene::SetFaders, which
   installs it and, if the fader it is replacing is at one end of its travel,
   dispatches the NEW one's SetToEnd or SetToStart -- ROM bytes 0x20 and 0x24,
   the same two slots the wipes carry. The symbol was zeroed host storage in
   hal/auto_bss.cpp, so that dispatch went through a null vptr and the very
   first LoadLevel faulted there. That is why the port could LEAVE a level
   through ExitLevel (SetNextLevel writes the two words itself and never
   touches a fader) but not ENTER one through LoadLevel, which opens with
   SetAndStopColorFader.

   It is the same object as a wipe, deliberately. FaderColor and FaderWipe are
   different ROM classes, but everything the port dispatches on either of them
   is the Fader interpolator both inherit, and Scene::StartSceneFade writes the
   colour into +0xc, which is where HalFaderWipe's `color` sits. Placement-new
   rather than a plain C++ definition because the storage has to keep its C
   name and its address. */
DSSTATE_BEGIN
extern "C" __declspec(align(8)) unsigned char
    data_0209f5e8[sizeof(HalFaderWipe)] = {0};
DSSTATE_END

/* The two setters' bodies, unchanged, with C linkage so the trampolines can
   name them. The trampoline's discriminator is the first STACK word, which
   the language fixes: a __cdecl caller (dScene_c::BeforeBehavior's
   FaderVTable) pushed the receiver there, a __thiscall caller
   (src/FUN_020299f4.cpp, src/FUN_02029980.cpp) pushed the frame count, and a
   frame count is never one of the eight objects this file hands out. */
extern "C" int __cdecl port_hal_wipe_is_object(const void *p)
{
    const char *c = (const char *)p, *b = (const char *)&hal_wipes[0];
    if (c >= b && c < (const char *)&hal_wipes[7] &&
        (std::size_t)(c - b) % sizeof(HalFaderWipe) == 0)
        return 1;
    return p == (const void *)data_0209f5e8;
}
extern "C" int __cdecl port_hal_wipe_s0c(void *s, int frames, int)
{
    if (!hal_wipe_receiver_ok(s)) {
        hal_wipe_shape_trap("SetBackwardTime (ROM slot 0x0c)", s);
        return 0;
    }
    HalFaderWipe *w = (HalFaderWipe *)s;
    w->speed = hal_fade_speed((unsigned)frames, 1);
    return w->HalFaderWipe::IsAtStart();
}
extern "C" int __cdecl port_hal_wipe_s10(void *s, int frames, int)
{
    if (!hal_wipe_receiver_ok(s)) {
        hal_wipe_shape_trap("SetForwardTime (ROM slot 0x10)", s);
        return 0;
    }
    HalFaderWipe *w = (HalFaderWipe *)s;
    w->speed = hal_fade_speed((unsigned)frames, 0);
    return w->HalFaderWipe::IsAtEnd();
}
__declspec(naked) int HalFaderWipe::SetBackwardTime(int, int)
{
    __asm {
        push ecx
        push dword ptr [esp+8]          /* the first stack word */
        call port_hal_wipe_is_object
        add  esp, 4
        pop  ecx
        test eax, eax
        jnz  shape_cdecl
        /* __thiscall: receiver in ecx, callee cleans 8 */
        push dword ptr [esp+8]
        push dword ptr [esp+8]
        push ecx
        call port_hal_wipe_s0c
        add  esp, 12
        ret  8
    shape_cdecl:
        jmp  port_hal_wipe_s0c
    }
}
__declspec(naked) int HalFaderWipe::SetForwardTime(int, int)
{
    __asm {
        push ecx
        push dword ptr [esp+8]
        call port_hal_wipe_is_object
        add  esp, 4
        pop  ecx
        test eax, eax
        jnz  shape_cdecl
        push dword ptr [esp+8]
        push dword ptr [esp+8]
        push ecx
        call port_hal_wipe_s10
        add  esp, 12
        ret  8
    shape_cdecl:
        jmp  port_hal_wipe_s10
    }
}

namespace {
struct HalColorFaderInit {
    HalColorFaderInit() { new (data_0209f5e8) HalFaderWipe(); }
};
HalColorFaderInit hal_color_fader_init;
}  /* anonymous namespace */

extern "C" {
DSSTATE_BEGIN
/* 0x0209f324: the pointer Stage::InitResources fills and every wipe caller
   derefs. WIPES is what the two fader-wipe entry points call the same
   word. */
void *data_0209f324 = &hal_wipes[0];

/* 0x0209f5bc: the fader currently installed. Scene::SetFaders reads it
   before overwriting it, and FUN_02029980 / FUN_020299f4 deref it with no
   null check at all -- the respawn path reaches the second one -- so it
   needs a real object from frame zero, not only after the first wipe
   starts. Wipe 0 is an arbitrary but valid choice: it sits at currInterp
   = 0, which is what "no fade in progress" looks like. */
void *data_0209f5bc = &hal_wipes[0];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:_WIPES=_data_0209f324")

/* ---- _ZTV9FaderWipe: the ROM's vtable SYMBOL, onto the host's table -------
   run link60 lane SL0. This is the one symbol the whole fader half of
   Stage::InitResources' closure was blocked on, and it is the last one:
   measured by compiling _ZN9FaderWipeC1Ev.c, _ZN9FaderWipeD1Ev.c,
   _ZN10FaderColorD2Ev.c and _ZN9FaderWipe14LoadAndSetFileEt.cpp with walk_window's
   own flags and checking every undefined external of the four against
   walk_window.map, the set wants exactly ONE name that the image does not
   already have, and it is this one.

   port/stage_lifecycle_map.txt section 4 sizes the fader wall at six symbols.
   THREE OF THOSE SIX HAVE SINCE CLOSED and the map is corrected here with the
   measurement that corrects it:

     _ZN10FaderColorD2Ev    ALREADY LINKED. The map lists it as "a matched TU that
                      exists in src/ and is on no active slice". It is on
                      port/slice_ov007.txt:678 and it is in the image at
                      walk_window.map:9986 (_ZN10FaderColorD2Ev.c.obj). Adding it to
                      a slice here would be an LNK2005, not a gain.
     data_0208eafc    ALL THREE ALREADY HOSTED, by hal/scene_boot.cpp:683-697,
     data_0208eacc    which stages them for func_02017278. The FaderWipe ctor
     data_0208eb2c    writes all three into the vptr on its way to this table,
                      so the ctor's closure is already paid.

   WHAT THE ALIAS CLAIMS, precisely. ??_7HalFaderWipe@@6B@ is a REAL vtable
   MSVC emits for a real class, and the class was laid out in the ROM's slot
   order on purpose -- two destructor slots so that AdvanceFade lands at byte
   0x08 rather than 0x04. That order is no longer inferred from the callers:
   the ROM record at 0x0208ea9c was read out of extracted/arm9_dec.bin and
   resolved word by word (the table is in the block above HalFaderWipe), and
   it agrees with this class byte for byte from 0x00 through 0x24.

   WHAT IT DOES NOT CLAIM. The two trailing words differ: the ROM has
   0x00000000 at 0x28 and a pointer to _ZTI15dFdBrightness_c at 0x2c, where this table
   has two callable stubs. Nothing in the port or in src reads past 0x24, so
   the difference is unobservable today; it is a divergence and it is written
   down rather than papered over.

   The alias, not a definition, so a target that one day compiles a real
   FaderWipe vtable keeps its own strong symbol and nothing collides. */
#pragma comment(linker, "/alternatename:__ZTV9dFdWipe_c=??_7HalFaderWipe@@6B@")

/* ---- FaderWipe::LoadAndSetFile, the C-name face --------------------------
   run link60 lane SL0, and the fifth of slot 0's five faces.
   src/engine/fader/_ZN9FaderWipe14LoadAndSetFileEt.cpp compiles the real C++
   method and publishes ?LoadAndSetFile@FaderWipe@@QAEXG@Z; Stage::InitResources
   declares and calls the ROM's C name.

   A FACE AND NOT AN /alternatename, and the difference is an ABI bug rather
   than a style preference. The MSVC method is __thiscall -- receiver in ECX --
   while the caller declares it
       void _ZN9FaderWipe14LoadAndSetFileEt(int thiz, u16 fileID);
   (src/_ZN5Stage13InitResourcesEv.cpp:127), a plain cdecl function with the
   receiver as its first STACK argument. Aliasing one onto the other would run
   the method with a garbage `this` out of ECX. hal/method_faces.cpp's
   Model::UpdateFileOffsets row is the same call and the same resolution. */
extern "C" void _ZN9dFdWipe_c14LoadAndSetFileEt(void *thiz, unsigned short fileID)
{ ((dFdWipe_c *)thiz)->dFdWipe_c::LoadAndSetFile(fileID); }

/* ---- THE ROM'S FRAME PHASE 2, AND THE ONE HOST STEP LEFT BESIDE IT --------
   run linkfull, lane RESET2.

   src/func_020197b8.c, the cartridge's frame loop, runs
       data_0209d50c = 2;   func_02019390();
   between the lid machine (0xb) and the input steps (0x16 / 0x17), so before
   phase 3 spawns and phase 4 ticks. src/func_02019390.c is

       if (data_0209d4a8 && data_0209d4a8->vt[0](data_0209d4a8) == 0) {
           func_0202345c(); func_02018efc(); return;         the EARLY arm
       }
       OAM::Reset();
       if (func_02019018()) func_0200f4b4();                 the GX reset
       func_0200f468();
       func_0202345c(); func_02018efc(); func_02018ec0();    the three fader steps

   THIS PORT RAN IT IN PIECES, AND LATE. The graphics block's word 0 was
   hal/scene_boot.cpp's port_graph_block_word0 (answer discarded), the two fade
   advances were this file's port_fader_advance -- func_02018efc, then a host
   stand-in for func_02018ec0 -- and OAM::Reset was hal/sub_screen.cpp's
   hal_sub_screen_frame_begin; both loops called the first two AFTER the actor
   tick. func_0200f4b4 and func_0202345c ran nowhere. All of it is the ROM's
   body now, called by the two loops at the ROM's point (tests/walk_window.cpp's
   level loop, hal/scene_boot.cpp's port_scene_tick), and the pieces are gone.

   WHAT THE ROM'S BODY DECIDES THAT THE PIECES DID NOT. Word 0's answer is used:
   the title's block (dScDSMT_c::graphCallback_c::GraphCallback0) and dScMB_c's
   answer 0, so on those scenes the cartridge skips OAM::Reset, the GX reset and
   the in-motion fader step every frame, exactly as src/func_02019390.c says;
   every other block this port can hold answers 1 (the thirty-two minigame
   blocks through dScMgBase_c's forwarder, scene 6's func_ov102_0214d1b0, the
   Stage's dGraph_c body), and on a level the block is the Stage's own
   (hal/level_boot.cpp makes it current at Stage::InitResources:284's point
   and clears it at Stage::CleanupResources:77's, run linkfull lane K7GC2),
   whose word 0 is that `return 1`, so a level takes the full arm every frame,
   as the DS does with the Stage's block. func_0202345c steps data_0209f1e4, which only the soft-reset
   branch of dScene_c::BeforeBehavior ever sets; that latch never rises on this
   port (func_02023498 is not linked), so it is the ROM's null test here.

   THE STEPPING BRACKET STAYS, because HalFaderWipe is still the host stand-in
   for FaderWipe: its AdvanceFade steps only while the frame loop drives it and
   snaps otherwise (see the class). func_02018efc and func_02018ec0 are the
   frame loop's two driven advances, so the whole phase runs inside it. Both
   dispatch vt[2] with the receiver pushed and in ecx (func_02018efc's compiled
   body: mov ecx,[blk] / push ecx / call [eax+8] / pop ecx), which is right for
   the colour faders' __cdecl l2_eb2c_s08 (hal/scene_boot.cpp) and for this
   class's __thiscall AdvanceFade alike; the old stand-in's qualified call ran
   HalFaderWipe's body on a FaderColor, which is the same arithmetic
   (src/engine/fader/_ZN10FaderColor11AdvanceFadeEv.cpp).

   THE SETTLE-CLEAR IS THE ONE HOST STEP KEPT, and it is not the ROM's. The
   cartridge never clears data_0209d4b0 because a fade finished: it clears it at
   named points -- Stage::CleanupResources (src/_ZN5Stage16CleanupResourcesEv.cpp
   `data_0209d4b0 = 0`), the Kuppa script's level command, Player's wipe
   routines (FUN_02029934 / FUN_02029a68), Stage::Behavior's VS end and
   func_02019440. This port does not run Stage::CleanupResources (the Stage's
   slot 3 is not hosted; hal/level_change.cpp), so a fade still pointed at when a
   level is torn down would stay pointed at into the next level and hold
   Stage::Behavior's pause gate (`data_0209d4b0 == 0`) shut. Until the teardown
   is the ROM's, a fader that has reached its target is dropped out of motion
   here, after the ROM's phase, as the stand-in always did, with the same blend
   register clear when it lands fully open. */
extern "C" {
extern int data_0209d4b0[8];
extern unsigned char data_0209d4a8[4];   /* hal/w8a_stage_storage.cpp */
extern unsigned char data_0208ee00;      /* func_02019018's word: the GX reset arm */
void func_02019390(void);

/* THE PHASE'S OWN ACCOUNT, the [r3e] / [b5input] pattern: one line per distinct
   graphics block phase 2 was handed (its vtable's word 0 is what decides the
   arm, resolvable through walk_window.map), and one line at exit with the call
   and settle-clear counts. Eight blocks at most; a process meets two or three. */
static unsigned g_p2_calls, g_p2_clears;
static void *g_p2_seen[8];
static unsigned g_p2_seen_n;
static void p2_report(void)
{
    std::fprintf(stderr, "[reset2] phase 2 (func_02019390): %u call(s), %u "
                 "settle-clear(s) of data_0209d4b0, %u distinct block(s)\n",
                 g_p2_calls, g_p2_clears, g_p2_seen_n);
}
static void p2_census(void)
{
    if (g_p2_calls++ == 0) {
        std::atexit(p2_report);
        std::fprintf(stderr, "[reset2] phase 2 first call: data_0208ee00=%u "
                     "(the GX reset arm)\n", (unsigned)data_0208ee00);
    }
    void *blk = *(void **)data_0209d4a8;
    for (unsigned i = 0; i < g_p2_seen_n; ++i)
        if (g_p2_seen[i] == blk) return;
    if (g_p2_seen_n >= sizeof g_p2_seen / sizeof g_p2_seen[0]) return;
    g_p2_seen[g_p2_seen_n++] = blk;
    void **vt = blk ? *(void ***)blk : 0;
    std::fprintf(stderr, "[reset2] phase 2 call %u: block %p vptr %p word0 %p "
                 "(%s)\n", g_p2_calls, blk, (void *)vt,
                 vt ? vt[0] : (void *)0,
                 blk ? "the block's word 0 decides the arm" : "no block: the full arm");
}

void port_frame_phase2(void)
{
    p2_census();
    g_hal_fader_stepping = 1;
    func_02019390();
    g_hal_fader_stepping = 0;

    /* The settle-clear: currInterp at +4 and speed at +8 in every fader class
       (HalFaderWipe, FaderColor, FaderBrightness), and the target is the one
       Fader::AdvanceInterp picks from the sign of speed. */
    HalFaderWipe *f = (HalFaderWipe *)(size_t)data_0209d4b0[0];
    if (!f)
        return;
    const Fix12i target = f->speed >= 0 ? 0x1000 : 0;
    if (f->currInterp != target)
        return;
    data_0209d4b0[0] = 0;
    ++g_p2_clears;
    if (f->currInterp == 0) {
        *(volatile unsigned short *)0x4000050 = 0;
        *(volatile unsigned short *)0x4001050 = 0;
    }
}

/* ---- THE WIPE POOL'S CONSTRUCTED STATE, given back at every level boot ----
 *
 * On the cartridge the seven wipes do not survive a level. Stage::InitResources
 * BUILDS them every time a level is opened -- the array note at the top of this
 * file has the line, func_02073470(7, 0x60, 8, FaderWipe::FaderWipe, ...) -- so
 * a level always starts with seven fresh FaderWipes holding the constructor's
 * two words, `currInterp = 0x1000; speed = 0` (include/FaderBrightness.h says
 * why those two belong to the constructor and what they mean: a fade starts
 * fully opaque and stationary). The port's stand-ins are the static objects in
 * this file, and Stage slot 3 (CleanupResources) is not hosted, so with nothing
 * to hand them back their state they carried one transition's interpolator into
 * the next one.
 *
 * WHAT THAT COST, measured rather than reasoned. The return from a course after
 * a star installs hal_wipes[0] and then, one frame later, hal_wipes[5], both
 * through dScene_c::SetFaders (src/_ZN8dScene_c9SetFadersEP15FaderBrightness.cpp),
 * whose whole job is to carry the outgoing fader's end state onto the incoming
 * one -- start onto start, end onto end, and NOTHING when the outgoing one is
 * mid-fade.
 *   FIRST star of a session: both wipes are still at 0x1000 with speed 0, so
 *   the outgoing one reads exactly at the start, the incoming one is snapped to
 *   the start, and the arrival fades in over thirty frames.
 *   SECOND star: hal_wipes[0] still carries the first transition's speed, so it
 *   steps on the very frame it is installed (0x1000 -> 0xf78) and writes EVY 16
 *   into 0x4000050 / 0x4000054. SetFaders then reads it as between start and
 *   end and leaves hal_wipes[5] alone -- and hal_wipes[5] is at 0 from the first
 *   fade-in, already at its target, so AdvanceFade's own `if (currInterp == old)
 *   return` means nobody writes those two registers again. The picture stays
 *   fully black, every pixel of it, until some other writer of the blend
 *   registers happens along: the level-clear menu's own dim, or a message box.
 *
 * The vptr is left alone on purpose: these are live host C++ objects whose table
 * is the one the constructor would store anyway, and a placement-new over an
 * object other code holds a pointer to is a bigger claim than this fix makes.
 * Every other word the constructor writes is written here. */
void port_fader_wipes_reset(void)
{
    g_wipe_draw = 0;        /* a mesh queued by the outgoing level's last advance */
    const int freed = hal_wipes_release();
    if (freed)
        std::fprintf(stderr, "  [wipe] level %d's %d wipe mesh(es) freed with "
                     "the level (Stage::CleanupResources' dFdWipe_c "
                     "destructors)\n", (int)data_0209f2f8, freed);
    for (int i = 0; i < 7; ++i) {
        hal_wipes[i].currInterp = 0x1000;
        hal_wipes[i].speed = 0;
        hal_wipes[i].color = 0;
        hal_wipes[i].unk0e = 0;
    }
}

/* ---- THE SEVEN WIPE MESHES, LOADED AT EVERY LEVEL BOOT --------------------
 *
 * Stage::InitResources :360-377, after the twelve shared preloads and before
 * Stage::LoadModel:
 *
 *     faderTbl = VS ? data_02075600 : level 5 ? data_020755f0 : data_020755e0;
 *     WIPES = func_02073470(7, 0x60, 8, FaderWipe::FaderWipe, ~FaderWipe);
 *     for (i = 0; i < 7; i++) FaderWipe::LoadAndSetFile(&WIPES[i], faderTbl[i]);
 *
 * and Stage::CleanupResources :102 destroys the array again, so each wipe's
 * file, its texture upload and its transform buffer belong to one level. The
 * port boots every level through hal/level_boot.cpp's copy of that body,
 * which calls this at the same point; the teardown half is
 * port_fader_wipes_reset above, in the level change's teardown beside
 * CleanCommonModelDataArr and port_model_vram_reset.
 *
 * WHY PER LEVEL AND NOT ONCE. Model::LoadTexAndPal uploads the file's
 * textures and palettes at the level's VRAM cursors and writes those offsets
 * INTO THE FILE (src/_ZN5Model13LoadTexAndPalER8BMD_File.cpp :38, :55, :60),
 * and every level boot rewinds the cursors (port_model_vram_reset, the ROM's
 * InitialiseVramGlobals) and uploads its own textures from the bottom again.
 * 0.5.1 loaded each file once per process and gave later levels the kept
 * Model back: the mesh then read whatever the NEW level had put at the first
 * level's offsets, which is the iris "with a face in it" players saw on a
 * death, a level exit and the cannon. The ROM never keeps one: the load here
 * uploads into this level's VRAM, and the release frees it with the level.
 *
 * The VS arm of the table choice (data_02075600) is left out, as 0.5.1 did. The
 * pool is this file's static array rather than a heap array (see
 * hal/stage_bridges.cpp's st_wipes_withdraw for why), so the constructor half
 * is the embedded Model's own, which is the only part of FaderWipe's
 * constructor a load reads; the interpolator words are port_fader_wipes_reset's.
 *
 * A mesh still held here means the boot came by a path whose teardown did not
 * release it (the title route after a Save and Quit, a level-to-scene
 * crossing): it is released first, as the ROM's teardown would have. */
extern "C" void port_fader_wipes_load(void)
{
    if (hal_wipe_mesh_off())
        return;
    const int stale = hal_wipes_release();
    const unsigned short *tbl = (data_0209f2f8 == 5) ? data_020755f0
                                                     : data_020755e0;
    int loaded = 0;
    for (int i = 0; i < 7; ++i) {
        _ZN5ModelC1Ev(hal_wipes[i].model);
        _ZN9dFdWipe_c14LoadAndSetFileEt(&hal_wipes[i], tbl[i]);
        if (*(void **)(hal_wipes[i].model + 4)) {
            ++loaded;
            hal_wipe_tex_note(i);
        } else {
            std::fprintf(stderr, "  [wipe] mesh for wipe %d (ov0 file 0x%04x, "
                         "level %d) NOT loaded: that wipe keeps the "
                         "whole-screen fade\n", i, tbl[i], (int)data_0209f2f8);
        }
    }
    std::fprintf(stderr, "  [wipe] level %d boot: %d/7 wipe meshes loaded "
                 "(Stage::InitResources' dFdWipe_c::LoadAndSetFile loop)%s\n",
                 (int)data_0209f2f8, loaded,
                 stale ? "; the previous level's were still held and were "
                         "freed first" : "");
}

/* THE WIPE MESH'S DRAW, for the render phase (THE WIPE'S PICTURE, above):
   FaderWipe::AdvanceFade's own ModelComponents::Render with its own matrix --
   (0, 0, -1) in camera space, scaled by the queued (0x1000 - interp) * 0x20,
   turned 0x4000 about X -- submitted into this frame's geometry just before it
   is rasterised (tests/walk_window.cpp). Nothing queued, nothing drawn. */
extern "C" void port_gx_interp_screen_begin(void);   /* ntr/gx.cpp */
extern "C" void port_gx_interp_screen_end(void);
extern "C" void port_fader_wipe_render(void)
{
    if (!g_wipe_draw)
        return;
    static int mtx[12];
    const int sc = g_wipe_draw_scale;
    hal_wipe_tex_draw_note(hal_wipe_index(g_wipe_draw));
    Matrix4x3_FromTranslation(mtx, 0, 0, -0x1000);
    Matrix4x3_ApplyInPlaceToScale(mtx, sc, sc, sc);
    Matrix4x3_ApplyInPlaceToRotationX(mtx, 0x4000);
    /* FADEFLASH1: the mesh is fixed to the screen; the pictures between ticks draw it as this tick drew it
       (ntr/gx.cpp, gx_interp_screen_begin) instead of blending it with the camera, which made one black picture
       when the camera jumped as a pit death began */
    port_gx_interp_screen_begin();
    _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(
        (char *)g_wipe_draw + 0x18, mtx, 0);
    port_gx_interp_screen_end();
    g_wipe_draw = 0;
}

/* Start a COLOR fade on the installed color fader (data_0209f5e8) and put it in
   motion. frames is the fade length; toEnd != 0 fades toward interp 1.0 (screen
   fully covered -- a fade-OUT to color), toEnd == 0 fades toward 0.0 (fade-IN,
   screen clears). color is the fade colour word: 0 = black, nonzero = white.
   This is the port's explicit stand-in for the Stage/Scene actor machinery that
   would otherwise arm data_0209d4b0. */
void port_fader_start_color(int frames, int toEnd, unsigned short color)
{
    HalFaderWipe *f = (HalFaderWipe *)(void *)data_0209f5e8;
    f->color = color;
    /* the same step the ROM's setters take (hal_fade_speed); both callers
       pass 16 frames, which fdiv and a plain divide agree on (256) */
    if (toEnd) {
        f->currInterp = 0;
        f->speed = frames > 0 ? hal_fade_speed((unsigned)frames, 0) : 0x1000;
    } else {
        f->currInterp = 0x1000;
        f->speed = frames > 0 ? hal_fade_speed((unsigned)frames, 1) : -0x1000;
    }
    /* install it as the scene's fader and arm it as the animating one */
    data_0209f5bc = f;
    data_0209d4b0[0] = (int)(size_t)f;
    std::fprintf(stderr, "[fade] color start: frames=%d dir=%s color=%s "
                 "interp=%d speed=%d\n", frames, toEnd ? "out" : "in",
                 color ? "white" : "black", f->currInterp, f->speed);
}

/* Has the ROM armed a level-entrance wipe during this level's boot?
   LoadEntranceObjects ends in StartEntranceFaderWipe (src/engine/fader/
   StartEntranceFaderWipe.cpp) for every entrance whose row in data_ov002_
   0210cb5c is not negative -- the castle rooms, sub-areas, secret courses and
   the Bowser stages and arenas, i.e. the places reached without a star select.
   It installs wipe N through Scene::SetFaders (data_0209f5bc AND data_0209d4ac,
   the one func_02018efc advances every frame) and parks it at the end of its
   travel; dScene_c::BeforeBehavior reverses it once the scene request is gone.
   port_title_select's cover has just put the COLOUR fader in data_0209f5bc, so
   a wipe sitting there now can only be the ROM's own, and the colour fader
   sitting there means no wipe was armed (a stale data_0209d4ac from the level
   before does not count).
   A caller that wants to lay its own colour fade over the level opening must
   not take over data_0209f5bc while this is true, because
   port_fader_start_color does not move data_0209d4ac: the wipe would stay the
   thing that is advanced, never be reversed, and leave the master brightness at
   full black (the black screen, black dialogue box and black arena after a
   debug level select into those areas). */
int port_fader_entrance_wipe_armed(void)
{
    const unsigned char *p = (const unsigned char *)data_0209f5bc;
    const unsigned char *w = (const unsigned char *)&hal_wipes[0];
    return p >= w && p < w + sizeof hal_wipes;
}

/* Is a fade in motion right now, and how far along is it? Returns 1 and fills
   *evy (0..16, the EVY coefficient the compositor darkens/brightens by) and
   *toWhite (nonzero for a white fade, zero for black) when a fade is being
   driven; 0 otherwise. Reads the blend registers the advance wrote, so the
   compositor sees exactly what the ROM's own hardware path produced. */
extern int data_020a0db0[8];            /* the frame clock (port_frame_clock_tick, below) */
extern unsigned char data_0209f20c;     /* the level-clear screen owns the game */
extern unsigned char data_0209f2d4;     /* LC_Update's state word */

/* MENUS093B (D3, "sudden darkening"): the course-clear save screen's dim. Stage::LC_Update case 2 writes BLDY -7 on both engines in ONE
   tick (src/_ZN5Stage9LC_UpdateEv.cpp:154), so the whole picture drops by seven sixteenths between two frames, and with the buttons already
   standing on the screen it reads as a flash (the forum video: 161 -> 97 mean brightness in one frame). The cartridge does exactly that and
   it is the one place the dim lands as a step; the port gives it a short ramp. ONLY a rise of engine A's coefficient while the level-clear
   screen is up is eased, over kLcEaseTicks of the game's own tick count (the frame clock, so interpolated frames between two ticks follow the
   same curve); a fall, every other fade and every other screen are untouched. The value the compositors read is this one, so the dimmed
   plates, lettering and 3D picture all take the same coefficient. */
static int lc_dim_ease(int e)
{
    const int kLcEaseTicks = 12;
    static int s_to, s_from, s_t0;
    const int tick = data_020a0db0[0];
    const int up = data_0209f20c != 0 && data_0209f2d4 >= 1 && data_0209f2d4 <= 3;
    if (!up) { s_to = e; s_from = e; return e; }
    int cur = s_to;
    if (s_to != s_from) {
        const int dt = tick - s_t0;
        cur = (dt >= kLcEaseTicks || dt < 0) ? s_to : s_from + (s_to - s_from) * dt / kLcEaseTicks;
    }
    if (e != s_to) {
        s_from = e > cur ? cur : e;            /* only a rise is eased */
        s_to = e;
        s_t0 = tick;
        cur = s_from;
    }
    return cur;
}

int port_fader_blend_state(int *evy, int *toWhite)
{
    unsigned short bldcnt = *(volatile unsigned short *)0x4000050;
    unsigned short bldy = *(volatile unsigned short *)0x4000054;
    int mode = (bldcnt >> 6) & 3;            /* 2 = brighten, 3 = darken */
    int e = 0;
    if (mode == 2 || mode == 3) {
        e = bldy & 0x1f;
        if (e > 16) e = 16;
    }
    e = lc_dim_ease(e);
    if (e == 0)
        return 0;
    if (mode != 2 && mode != 3)
        mode = 3;                            /* an eased tail after the ROM's own dim went off: a darken */
    if (evy) *evy = e;
    if (toWhite) *toWhite = (mode == 2);
    return 1;
}

/* THE SAME QUESTION ASKED OF THE SUB ENGINE, 0x4001050 / 0x4001054.
 *
 * THE BLEND UNIT IS PER ENGINE AND THE TWO ENGINES DO DIVERGE. Every fade the
 * fader itself drives writes both engines the same values (AdvanceFade above
 * writes 0x4000050/54 and 0x4001050/54 together), so for a long time "read
 * engine A and apply it to both screens" and "read each engine" were the same
 * answer, and the first one was the one the port had. The title's opening
 * screen is where they part: func_ov007_020b7138 (src/func_ov007_020b7138.c)
 * calls G2x::SetBlendBrightness on BOTH engines with first-target 0x3f and
 * -0x10, so both go to mode 3 with EVY 16, and then only engine A is faded back
 * in. From there to the first stylus tap engine B sits at mode 3 EVY 16, fully
 * black, while engine A is clear.
 *
 * MEASURED ON BOTH SIDES, SAME INPUT, NEITHER TOUCHED. The cartridge in
 * melonDS at frames 300-900: engine A BLDCNT 0x00bf mode 2 EVY 0, engine B
 * BLDCNT 0x00ef mode 3 EVY 16, MASTER_BRIGHT zero on both engines. The port's
 * own SM64DS_PPU_AUDIT over the same 915 frames: engine A BLDCNT 0x00bf x886
 * with BLDY 0, engine B BLDCNT 0x00ff x30 then 0x00ef x885 with BLDY 0x0010 on
 * every one of the 915 samples. The ROM code is writing the right registers on
 * the port. Nothing was reading engine B's.
 *
 * THE LAYER-TARGET MASK IS NOT HONOURED HERE, which is the same approximation
 * the engine-A path above has always made: the caller darkens the whole
 * finished panel rather than only the layers BLDCNT bits 0-5 select. On the
 * opening that is exact -- engine B's mask is 0x2f, which covers every layer it
 * has content on, and its OAM has no placed object at all -- but a scene that
 * put sprites on the sub screen with OBJ left out of the first-target set would
 * have them darkened here and not on hardware. Worth knowing before this is
 * leaned on for a screen other than the one it was measured against. */
int port_fader_blend_state_sub(int *evy, int *toWhite)
{
    unsigned short bldcnt = *(volatile unsigned short *)0x4001050;
    unsigned short bldy = *(volatile unsigned short *)0x4001054;
    int mode = (bldcnt >> 6) & 3;            /* 2 = brighten, 3 = darken */
    if (mode != 2 && mode != 3)
        return 0;
    int e = bldy & 0x1f;
    if (e > 16) e = 16;
    if (e == 0)
        return 0;
    if (evy) *evy = e;
    if (toWhite) *toWhite = (mode == 2);
    return 1;
}

/* ---- THE FRAME CLOCK, data_020a0db0 (run mg12 lane SELECT) ----------------
 *
 * WHAT IT IS. One int at 0x020a0db0, incremented once per frame by the ROM's
 * own main loop -- src/func_020197b8.c, between phase 5 (func_02019404) and
 * the render phases:
 *
 *     data_0209d50c = 5; func_02019404();
 *     data_0209d50c = 6;
 *     data_020a0db0 = data_020a0db0 + 1;      <- this line
 *
 * It is the game's BLINK CLOCK. Nothing reads its value; everything reads one
 * of its low bits and uses the alternation.
 *
 * THE READER CENSUS, AND THE INSTRUMENTS THAT TOOK IT. The ROM materialises
 * this address EIGHTEEN times: `grep -r "to:0x020a0db0" config/arm9/` returns
 * 18 relocations, every one kind:load, which is one literal-pool load per body
 * that touches it. Eighteen bodies, eighteen TUs in src/, and they split
 * SIXTEEN readers to TWO writers. Both writers are src/func_020197b8.c (the
 * increment above) and src/func_0203b684.c (the reset that zeroes it and
 * data_020a0db4 together), and NEITHER IS LINKED into this program -- which is
 * the whole of why the counter never moved.
 *
 * THIRTEEN of the sixteen readers are linked. "Linked" is measured, not
 * assumed, by two instruments that agree file for file: (1) the object file for
 * the TU exists under build/port/CMakeFiles/walk_window.dir/, and (2) the same
 * object name carries symbol lines in build/port/walk_window.map. Both answer
 * for the same thirteen and both refuse the same three. Re-run them before
 * editing this list; an earlier version of it was written from a source grep,
 * which cannot tell a compiled TU from an uncompiled one, and got six rows
 * wrong in both directions.
 *
 *   src/minigames/d_s_mg_m_carlo.cpp      & 8     Pair-a-Gone's card draw
 *   src/minigames/d_s_mg_m_carlo2.cpp      & 8     Pair-a-Gone And On's card draw
 *   src/minigames/d_s_mg_roulette_methods.cpp      & 8     Roulette's five bet markers: the bit
 *                                          steps the sprite frame by one
 *   src/minigames/d_s_mg_roulette_methods.cpp      & 8     Roulette's BALL sprite, drawn only
 *                                          while the bit is set (named off the
 *                                          Hud_RenderSprite call, not off a
 *                                          slice title -- it is not a banner)
 *   src/_ZN7Minimap6RenderEv.cpp   & 8
 *   src/_ZN3HUD15RenderStarCountEv.cpp     & 0x18   (see THE STAR COUNT below)
 *   src/MgCoins_Draw.c      & 0x10  the minigame name plate
 *   src/_ZN5Stage20RenderBouncingArrowsEv.cpp  & 0x10 and & 8 (two sites)
 *   src/_ZN7Message6UpdateEv.cpp   & (0x10 / data_0208ee44)   the text cursor
 *                                          (four sites)
 *   src/_ZN12dScStarSel_c6RenderEv.cpp    * 0x300 fed to a Y rotation
 *   src/minigames/d_s_mg_d3d_base.cpp   & 1
 *   src/minigames/d_s_mg_single3d_base.cpp    & 1     dScMgFlower_c::BeforeBehavior
 *   src/TrmpLine_StateSettle.c      & 1     Trampoline Terror's countdown
 *
 * NOT LINKED, and listed so the next reader does not re-add them:
 * src/func_020326ac.c (four sites), src/func_ov002_020f20f4.c and
 * src/func_ov002_020f23f0.c (two sites). They are readers in the ROM and they
 * are not in this binary, so this change cannot reach them.
 *
 * WHY IT NEEDED A DRIVER. func_020197b8 is the ROM's main loop and this port
 * does not run it -- both host loops (tests/walk_window.cpp's level loop and
 * hal/scene_boot.cpp's port_scene_tick) are their own frame. So the counter
 * sat at its BSS zero for the life of every process. Same shape and same cause
 * as phase 2 above, which the host loops did not run either until they called
 * port_frame_phase2 -- which is why the two drivers share a file.
 *
 * WHICH DIRECTION THE RISK RUNS, and this is the half that is easy to get
 * backwards. With the counter frozen at zero, `& n` is FALSE for every mask in
 * the list -- including `& 1`, because all three linked `& 1` sites test the
 * result against zero and take the branch when it is NON-zero. So those three
 * paths NEVER EXECUTED ONCE in this port, and turning the clock on RUNS THEM
 * FOR THE FIRST TIME. That is the real hazard here, and it is not a rate
 * change: nothing was running double and is now running half. The three are
 * two Particle::SysTracker::Update calls (Bounce and Pounce's and the Flower
 * class's BeforeBehavior) and the ApproachLinear2 countdown in
 * TrmpLine_StateSettle that clears the byte at +0x328. All three were exercised
 * under review on their own scenes, entered rather than merely reachable, and
 * came back clean.
 *
 * THE STAR COUNT is the one site where the frozen zero HID something rather
 * than merely holding it still. _ZN3HUD15RenderStarCountEv returns early --
 * drawing nothing -- when data_0209f2d4 == 3 and (data_020a0db0 & 0x18) == 0.
 *
 * Those two are the innermost pair and NOT the whole condition; an earlier
 * wording stopped there and so overstated how often the suppression happened.
 * Read out of src/_ZN3HUD15RenderStarCountEv.cpp in full, the early return is
 * reached only when all of these hold:
 *
 *     data_0209f2d8 != 1                    (else the top branch draws and
 *                                            returns before any of this)
 *     (data_0209f2fc != 2 && data_ov002_02111178 == 4)
 *       || (data_0209f2fc == 1 && data_ov002_02111178 >= 3
 *                              && data_ov002_02111178 < 6)
 *     data_0209f2ac != 0
 *     data_0209f2d4 == 3 && (data_020a0db0 & 0x18) == 0
 *
 * Miss any one of them and the function falls through to a draw. So this is a
 * particular HUD mode on particular levels, not every level.
 *
 * The claim that matters survives intact: WHEREVER that path is live, a frozen
 * zero made the innermost test true on every frame, so the star count was
 * suppressed ENTIRELY rather than blinking. With the clock running the mask is
 * zero for eight frames in thirty-two, which is the blink the ROM intends. This
 * one gets looked at on a level rather than a scene.
 *
 * WHAT IT COST, measured rather than argued (run mg12, lane SELECT). In
 * Pair-a-Gone (scene 381) the ONLY difference between a selected card and an
 * idle one is this blink: _ZN18dMgMCarloCardObj_c6RenderEv draws state 3 exactly as it
 * draws state 2 except that it skips the draw while bit 3 is set. With the
 * clock frozen the skip never fires, so tapping a card played its sound,
 * moved the state machine and changed NOTHING on screen. Two stacked captures
 * eight frames apart with card 1 held selected came back byte-identical over
 * the whole bottom screen. A player cannot see what he has picked, and the
 * mismatch rule then deselects it silently -- which is what "something is
 * wrong with the way you select cards" is from the other side of the glass.
 *
 * WHERE IT IS CALLED, and it sits ONE PHASE EARLY -- said plainly rather than
 * claimed to be exact. The ROM increments at phase 6, AFTER phase 5
 * (func_02019404). This port calls it from both loops after the actor tick and
 * BEFORE the render, so by the ROM's own numbering the step happens one phase
 * earlier in the frame than the ROM puts it. (Phase 2 itself runs before the
 * tick on both loops since run linkfull lane RESET2, in the ROM's order.)
 *
 * State the consequence exactly, because an earlier wording of this paragraph
 * got it wrong. It said the render "follows the step in both orders". It does
 * not: in the ROM the render is phase 5 and the step is phase 6, so WITHIN ONE
 * ITERATION the render PRECEDES the step. Saying it follows is only true if you
 * read the loop cyclically, which is true of any cyclic order and therefore
 * proves nothing. The accurate statement is about values, not order: in the ROM
 * the actor tick and the render both see N; in this port the actor tick sees N
 * and the render sees N+1.
 *
 * The conclusion is unchanged, and it is that no linked reader can tell. Not one
 * of the thirteen couples a behaviour-phase read to a render-phase read in the
 * same frame, so none of them can observe the two values disagreeing, and a
 * uniform one-frame offset in a blink is invisible by construction. Nothing
 * between the step and the render touches the word. The residue is where
 * the phase sits in the frame, not what any reader sees.
 *
 * Gated on the game tick rather than free-running -- with the debug menu open
 * the world holds still and the picture keeps being drawn, and a blink that
 * kept running would be the one thing still moving in a frozen frame. The DS
 * has no pause, so the gate is a port decision and this is the same decision
 * port_actor_tick makes.
 *
 * data_020a0db0 IS HOSTED AS int[8] in hal/auto_bss.cpp and every matched TU
 * reads it as a plain int, which is the first word.
 *
 * SM64DS_NO_FRAME_CLOCK=1 PUTS THE FROZEN CLOCK BACK on this same binary, the
 * same shape SM64DS_IRQ2_OFF and SM64DS_NO_KUPPA_TICK have and for the same
 * reason: this wakes thirteen readers at once, so the A/B that says which of
 * them moved has to be runnable without a second build. It is a diagnostic and
 * not a setting -- nothing in port/ or in a bundle sets it.
 *
 * WHAT THAT A/B ACTUALLY SHOWS on lane CRD's two banked play plans, one binary,
 * clock off against clock on. EXACTLY ONE census line moves in each run, and it
 * is the same line: dScMgSingle3DBase_c slot 26, the per-card draw dispatch.
 * Scene 381 over 3600 frames goes 65462 -> 64659, down 803. Scene 382 over 7800
 * frames goes 178184 -> 175953, down 2231. Those are the draws skipped while a
 * selected card is blinked out, so the drop IS the fix working and a run that
 * showed no drop would be the failure. Every other figure in both censuses is
 * unchanged, which is the sense in which the proofs replay: identical outcomes,
 * one moving counter, and it is the counter this change exists to move. */
extern int data_020a0db0[8];

void port_frame_clock_tick(void)
{
    static int off = -1;
    if (off < 0)
        off = std::getenv("SM64DS_NO_FRAME_CLOCK") ? 1 : 0;
    if (off)
        return;
    data_020a0db0[0] = data_020a0db0[0] + 1;
}
}  /* extern "C" */
