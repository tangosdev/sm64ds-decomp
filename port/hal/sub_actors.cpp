// The bottom screen's own actors, and the vtables their constructors install.
//
// HUD (334) and Minimap (335) are the two ids the castle-grounds census has
// been printing as "not registered, skipped" since the level boot started
// spawning. Both are arm9 classes whose every method is matched; both are
// spawned by Stage::LoadClsnAndObjects like everything else; and both draw
// exclusively onto the sub screen, which is why they waited for gate 25.
//
// ---- the vtable -------------------------------------------------------------
//
// EIGHTEEN SLOTS, not the twenty the Player and the ov002 actor classes carry.
// The ROM settles it: _ZTV8dMeter_c is ov002 0x0210c2c0 (found by the word
// that points at its typeinfo, 0x0210c224, which sits right after the class
// name string "8dMeter_c"), and reading forward from the vptr at +8 gives
//
//     0  020fda04  HUD::InitResources             9  020fd5e0  HUD::Render
//     1  02043c78  ActorBase::BeforeInitResources 10 02043ac8  ActorBase::BeforeRender
//     2  02013ef4  ActorDerived::AfterInitRes.    11 02043ac4  ActorBase::AfterRender
//     3  020fd5d4  HUD::CleanupResources          12 020fd5dc  HUD::OnPendingDestroy
//     4  02043bac  ActorBase::BeforeCleanupRes.   13 0204357c  ActorBase::Virtual34
//     5  02043b2c  ActorBase::AfterCleanupRes.    14 0204349c  ActorBase::Virtual38
//     6  020fd7a4  HUD::Behavior                  15 02043494  ActorBase::OnHeapCreated
//     7  02043afc  ActorBase::BeforeBehavior      16 020fb8f8  ~HUD (D1)
//     8  02043af8  ActorBase::AfterBehavior       17 020fb928  ~HUD (D0)
//
// and slot 18 is not a function pointer at all. Every one of those addresses
// resolves to the name above in config/arm9/symbols.txt, so this is read out of
// the ROM rather than inferred from the header -- which matters twice over.
//
// FIRST: filling slots 18 and 19 the way the twenty-slot classes do would write
// past the table.
//
// SECOND, and this one crashed the port: EVERY shared slot here is ActorBase's
// own, not Actor's. The twenty-slot classes in hal/actor_classes.cpp share ten
// slots that resolve to Actor::, and copying that fill to an eighteen-slot
// class is a heap corruption with a fifty-frame fuse. Actor::AfterInitResources
// ends on
//
//     *(u32 *)(long long)(int)&mFlags |= 0x38;      /* Actor + 0xb0 */
//
// and dMeter_c is 124 bytes -- ActorBase::operator new(124) in the HUD's own
// constructor. 0xb0 is 0x34 past the end of it, so that read-modify-write lands
// in whatever the game heap handed out next. On the castle grounds that is the
// BUTTERFLY spawned immediately before, and 0xb0 falls exactly on its
// SceneNode's owner back-pointer: the OR turned a valid actor pointer into
// pointer|0x38, and the next scene-tree walk dereferenced it.
//
// Slot 2 is the one genuine override of the three: ActorDerived::, not
// ActorBase:: and not Actor::. The other nine are ActorBase's throughout.
//
// Otherwise the law is hal/actor_classes.cpp's: MSVC slot order, every entry a
// __fastcall thunk so ecx carries `this`, every thunk calls QUALIFIED, and
// slots the port cannot service trap by name.
//
// ---- _ZTV7dBase_c -----------------------------------------------------------
//
// Both constructors write the base vtable first and their own over it
// (`p[0] = _ZTV7dBase_c; p[0] = _ZTV8dMeter_c;` -- the ROM's own two stores,
// which is what the decomp recovered). Nothing dispatches through the base
// one, so it is storage and no more.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "ActorDerived.h"
#include "dsstate_seg.h"

extern "C" {
/* HUD's own C-named halves: the two destructors */
void *_ZN3HUDD1Ev(void *self);
void *_ZN3HUDD0Ev(void *self);

/* Minimap's own C-named halves: the two destructors and Behavior */
void *_ZN7MinimapD1Ev(void *self);
void *_ZN7MinimapD0Ev(void *self);
int _ZN7Minimap8BehaviorEv(void *self);
/* the sixteen-wide per-player blip tables the host copies own (Minimap_wide.h);
   map_render's SM64DS_MM_TRACE reads slot 0 out of them */
extern int g_mm_iconX[16];
extern int g_mm_iconY[16];

/* vtable storage the three constructors install by name (declared in
   include/decl_common.h, defined nowhere until now). dMeter_c is the HUD's and
   dMap_c is the Minimap's -- the ROM class names, which is what the matched
   constructors write, rather than the config names _ZTV8dMeter_c sits under. */
void *_ZTV7dBase_c[18];
void *_ZTV8dMeter_c[18];   /* vtspan: _ZTV3HUD */
void *_ZTV6dMap_c[18];   /* vtspan: _ZTV7Minimap */

const char *port_actor_class_name(unsigned id);
void port_scene_canary(const char *where);
extern int data_0209caa0[];
extern unsigned char data_0209f2d8, data_0209f2c4, data_0209f20c, data_0209f294;
static int port_sub_oam_nonzero(void)
{
    const unsigned char *p = (const unsigned char *)0x07000400;
    int n = 0;
    for (int i = 0; i < 1024; ++i) n += p[i] != 0;
    return n;
}
extern int data_0209f334[]; extern unsigned char data_0209f2e8[];
extern unsigned short *data_0209f340;
extern char _ZN3OAM8MM_ARROWE[];
extern signed char data_ov002_02111148; extern unsigned char data_0209d454;
extern void *data_0209f394[];   /* per-player Actor* */
extern unsigned char data_0209f250;   /* local player index */

/* the ov001 sprite-template mount (see port/ov001_syms.txt) */
void port_ov001_syms_patch(void);
void port_ov001_pack_check(void);
}

/* HUD's other five slots are real MSVC members in their own TUs, each of
   which declares its own `struct HUD` with just the methods it defines. The
   mangled name of a member does not depend on the class layout, so one
   declaration here reaches all of them. include/HUD.h is deliberately NOT
   included: it declares the same class without InitResources or Behavior. */
struct HUD {
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
    void OnPendingDestroy();
    void CalculateDigits(unsigned short n);
    void RenderCoinCount();
    void RenderLifeCount();
    void RenderTimeTimer();
    void RenderHealthMeter();
    void UpdateHealthMeter();
    static void RenderCameraButtons();
};

/* Same story for the Minimap, and the same reason include/Minimap.h is not
   included: it declares three of the four members and Render's own TU declares
   a fourth against a struct of its own. Behavior is the odd one -- its TU
   defines the C name taking the object as an argument, so it needs no face. */
struct Minimap {
    int InitResources();
    int CleanupResources();
    int Render();
    void OnPendingDestroy();
    /* STATIC on the ROM -- it takes no `this` at all, and InitResources calls
       it with no argument. Declaring it a member would ask the linker for
       ?UpdateLevelSpecific@Minimap@@QAEXXZ and get the S-form's nothing. */
    static void UpdateLevelSpecific();
};

/* UpdateLevelSpecific is the reverse shape: its own TU defines a real MSVC
   member and InitResources calls it by the Itanium C name, so it needs a face
   the way HUD::RenderCoinCount and friends do. */
extern "C" void _ZN7Minimap19UpdateLevelSpecificEv(void)
{ Minimap::UpdateLevelSpecific(); }

// ---- the arm9 bss the Minimap reads ----------------------------------------
//
// Four of these are ONE STRUCT the delink split at the boundaries code happened
// to touch, and separate host arrays would put the pieces on whatever the
// linker chose. UpdateMinimap settles it: it writes a 16-byte descriptor to
// data_0209f3c8 and four words to data_0209f3c4 + 0x14..0x20 -- addresses that
// land INSIDE data_0209f3c8's own 0x20 bytes. So f3c4 and f3c8 are one object
// and the run has to be contiguous, the mechanism hal/level_boot.cpp uses for
// the save block. Sizes are each symbol's own delta in config/arm9/symbols.txt:
// f3a4 +0x20, f3c4 +4, f3c8 +0x20, f3e8 +0x24.
#define MMBLK(sec, name, size)                                    \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(4))     \
    unsigned char name[size] = {0}

MMBLK(".dsstate$mmblk0000", data_0209f3a4, 0x20);   /* 8 Obj* -- the red-coin markers */
MMBLK(".dsstate$mmblk0001", data_0209f3c4, 0x04);
MMBLK(".dsstate$mmblk0002", data_0209f3c8, 0x20);
MMBLK(".dsstate$mmblk0003", data_0209f3e8, 0x24);   /* 9 Obj* -- the star markers */

#undef MMBLK

// ---- THE MINIMAP'S AFFINE, per frame ----------------------------------------
//
// BG3-sub in BG mode 3 is an EXTENDED AFFINE layer and the minimap is drawn on
// it, so every one of its pixels goes through BG3PA..BG3PD and the BG3X/BG3Y
// reference point before it picks a map entry. The port seeded those to the
// identity once at boot (hal/sub_screen.cpp) because nothing was writing them
// per frame, and the minimap drew at exactly 1:1 instead of the level's own
// scale. That is the unscaled minimap.
//
// The numbers were never missing. Minimap::Behavior ends in
//
//     UpdateMinimap(&self->f50, self->f60, self->f64,
//                   self->f60 - 0x80, self->f64 - 0x60)
//
// and UpdateMinimap (matched) copies that sixteen-byte descriptor to
// data_0209f3c8 and the four scalars to data_0209f3c4 + 0x14..0x20, which land
// inside data_0209f3c8's own bytes because the run above is contiguous:
//
//     +0x00  the 2x2 matrix, four 20.12 words
//     +0x10  x, y     the map-space point to put under the reference
//     +0x18  cx, cy   that point less (128, 96), the bottom screen's centre
//
// WHAT WAS MISSING IS THE BEAT. Stage::InitResources stores &data_0209f3c4 into
// data_0209d4a8 -- the current scene's graphics block -- and func_02019144, the
// per-frame graphics beat, opens by calling that block's vtable slot 2. For the
// Stage that slot is Stage::GraphCallback2, whose entire body is
//
//     G2x::SetBGyAffine(&reg_G2S_DB_BG3PA, self + 4,
//                       self->unk14, self->unk18, self->unk1c, self->unk20);
//     return 1;
//
// Both of those are MATCHED and in src/. G2x::SetBGyAffine -- which is where
// all the arithmetic is, the 8.8 narrowing and the reference point that solves
// (x, y) onto the screen centre -- is already linked (slice gate 10) and is
// what this calls. Stage::GraphCallback2 itself is NOT linked, and cannot be:
// it takes the address of reg_G2S_DB_BG3PA, and a host build has no way to put
// a global at the absolute address 0x04001030 where the sub engine's BG3PA is
// mapped. So the six-argument forward is what the seam below stands in for,
// and the forward is all Stage::GraphCallback2 is.
//
// Neither is the dispatch hosted: the port does not run func_02019144 (its tail
// is the layer-mask publish and the OAM upload, which hal/sub_screen.cpp does
// its own way) and nothing seats data_0209d4a8, so the ROM's "is the Stage's
// block current" test has nothing to read. The block answers the same question
// itself. It is zeroed storage until Minimap::Behavior calls UpdateMinimap, and
// an all-zero matrix is degenerate anyway -- every screen pixel would map to
// map pixel (0,0) and the layer would sample one tile -- so until the minimap
// has spoken, leave the identity boot seeded.
extern "C" {
struct PortMtx2x2 { int m[4]; };
void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(volatile unsigned short *reg,
                                              PortMtx2x2 *m,
                                              int a, int b, int c, int d);
}

extern "C" void port_minimap_affine_update(void)
{
    PortMtx2x2 *const m = (PortMtx2x2 *)data_0209f3c8;
    const int *const s = (const int *)(data_0209f3c8 + 0x10);

    if ((m->m[0] | m->m[1] | m->m[2] | m->m[3]) == 0)
        return;

    /* BG3PA of engine B. Stage::GraphCallback2 spells it &reg_G2S_DB_BG3PA. */
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(
        (volatile unsigned short *)0x04001030, m, s[0], s[1], s[2], s[3]);

    /* SM64DS_MINIMAP_TRACE=1: the matrix and the reference point it solved,
       once a second, for checking the scale is the level's rather than 1:1. */
    static int trace = -1, frame;
    if (trace < 0) trace = std::getenv("SM64DS_MINIMAP_TRACE") != 0;
    if (trace && (frame++ % 30) == 0)
        std::fprintf(stderr, "[mmaf] m=(%d,%d,%d,%d) at=(%d,%d) centre=(%d,%d) "
                     "PA|PB=%08x PC|PD=%08x X=%d Y=%d\n",
                     m->m[0], m->m[1], m->m[2], m->m[3], s[0], s[1], s[2], s[3],
                     *(volatile unsigned *)0x04001030,
                     *(volatile unsigned *)0x04001034,
                     *(volatile int *)0x04001038,
                     *(volatile int *)0x0400103c);
}

extern "C" {
DSSTATE_BEGIN
/* Standalone, and zero on this boot -- nothing on the castle grounds writes
   them, so the branches they gate stay off and say nothing. */
unsigned char data_0209f288;          /* "draw the second marker set" flag */
unsigned char data_0209f370[0xc];     /* 9 marker tile indices, walked by Render */
unsigned data_020a60a4;               /* GXS ext-palette: the saved VRAM bank */

/* The stylus half of the Ctrl block. On the DS these are bytes 0x10 and 0x11
   of each 0x18-byte record and the readers index them [player * 0x18], which
   is why they are sized for the whole four-record block rather than one byte.
   The port keeps them as separate storage the way hal/actor_vtables.cpp
   already does for data_0209f4ac / data_0209f4ae. Nothing on the host writes
   them: the host stylus goes to data_020a0de8, so the recentre-the-minimap
   branch Minimap::Behavior gates on data_0209f4ac stays shut and these are
   never read for a value that matters. */
unsigned char data_0209f4a8[0x60];
unsigned char data_0209f4a9[0x60];
DSSTATE_END
}

// ---- the faces --------------------------------------------------------------
//
// HUD::Render calls its own leaves by their ITANIUM C name with `this` spelled
// out as an argument -- `_ZN3HUD17RenderHealthMeterEv((void *)this)` -- while
// every one of those leaves is defined in its own TU as a real MSVC
// __thiscall member. A linker alias would hand a __thiscall body an ecx that
// never held `this`, so each needs a real face. Same mechanism as
// hal/method_faces.cpp.
#define HUD_FACE(sym, meth)                                                   \
    extern "C" void sym(void *s) { ((HUD *)s)->HUD::meth(); }

/* Only the leaves whose own TU defines a __thiscall MEMBER. The other four --
   RenderStarCount, RenderSilverStars, RenderRedCoins, RenderVsTimer -- define
   the C name themselves, so a face here would be a duplicate symbol. Which of
   the two shapes a leaf has is not a pattern to guess at: it is whichever the
   decomp recovered, and the linker names the ones that disagree. */
HUD_FACE(_ZN3HUD15RenderCoinCountEv, RenderCoinCount)
HUD_FACE(_ZN3HUD15RenderLifeCountEv, RenderLifeCount)
HUD_FACE(_ZN3HUD15RenderTimeTimerEv, RenderTimeTimer)
HUD_FACE(_ZN3HUD17RenderHealthMeterEv, RenderHealthMeter)
HUD_FACE(_ZN3HUD17UpdateHealthMeterEv, UpdateHealthMeter)

#undef HUD_FACE

/* RenderCameraButtons is a STATIC member (no `this` at all -- it draws the two
   arrows and the zoom button from OAM's own templates), so its face takes and
   passes nothing. */
extern "C" void _ZN3HUD19RenderCameraButtonsEv(void *) { HUD::RenderCameraButtons(); }

/* Player::IsInsideOfCannon is a real MSVC member, and HUD::RenderHealthMeter
   reaches it as a free function taking void*. That was a C++ free function
   when this face was written; main's mangled-declaration sweep moved
   include/decl_Player.h into an extern "C" block, so the call is now the plain
   name and the face follows it. The alias keeps the older C++ spelling
   resolvable, the same shape as the other faces in this family. */
struct Player { int IsInsideOfCannon(); int Unk_020ca8f8(); };
extern "C" int _ZN6Player16IsInsideOfCannonEv(void *s)
{ return ((Player *)s)->Player::IsInsideOfCannon(); }
#pragma comment(linker, "/alternatename:?_ZN6Player16IsInsideOfCannonEv@@YAHPAX@Z=__ZN6Player16IsInsideOfCannonEv")

/* Minimap::Behavior reaches the same class the same way: it calls
   Player::Unk_020ca8f8 -- "is he in a state that hides the minimap" -- as a
   free C function while its own TU defines a real MSVC member. */
extern "C" int _ZN6Player12Unk_020ca8f8Ev(void *s)
{ return ((Player *)s)->Player::Unk_020ca8f8(); }

/* Stage::RenderBouncingArrows draws from an OamAttr template at ov001
   0x020abd88. HUD::Render reaches it only when data_0209f284 is set, which is
   zeroed bss on the port's boot, so the stub below has never actually fired.
   It says so if that ever changes.

   THE OLD REASON HERE WAS WRONG, and slice_gate27.txt repeats it. This comment
   used to say the template is one "config does not name", and concluded that
   "naming it is a config change, not a port change". Config names it already --
   config/arm9/overlays/ov001/symbols.txt:111 reads

       _ZN3OAM14BOUNCING_ARROWE kind:data(any) addr:0x020abd88 ambiguous

   -- the same `ambiguous` shape as the five HUD tables aliased at the bottom of
   this file, and for the same reason: ov000 and ov001 share base 0x020aa420, so
   dsd could not attribute the address. config/arm9/overlays/ov000/symbols.txt
   :282 carries the ov000 misattribution. So this is a PORT change, and a small
   one. The rest of the old comment does survive checking: the only two relocs
   to 0x020abd88 are from:0x02023db4, inside Stage::RenderBouncingArrows itself
   (its own literal pool), and ov075 from:0x02117058, the literal pool at the
   tail of func_ov075_02116f40 -- both CODE, so nothing in emitted DATA points
   at it and ovdata's pointer pass genuinely cannot pull it in.

   THE SEAT LANDED (w6-c, run linkw). The four steps the paragraph below used
   to list are all in: the template is mounted (w4-e put _ZN3OAM14BOUNCING_ARROWE
   in port/ov001_syms.txt with the derivation), the alternatename binding the
   matched body's func_020abd88 spelling to the mounted template sits with the
   five HUD tables at the bottom of this file, the matched TU rides
   port/slice_w1l5.txt, and the stub that used to sit right here is gone --
   src/_ZN5Stage20RenderBouncingArrowsEv.cpp owns the symbol now. HUD::Render
   is the call edge that keeps it live (data_0209f284-gated, zeroed bss on this
   boot, so the arrows draw only on a level that sets it).

   GUARD NOTE, because the wave-5 R1/R2 class makes this worth spelling out:
   the alternatename below is the healthy weak-alias shape, not a defeat
   waiting to happen. The matched TU DECLARES func_020abd88 (`extern void
   func_020abd88(void);`) and never defines it -- the LHS stays undefined in
   the link, the alias fires, and both names resolve to the ov001 mount's
   template bytes. alternatename_guard sees LHS and RHS at one address: OK.

   Checked rather than assumed at seat time: the matched body's six data
   references (data_0208ee44, data_020a0db0, data_0209f2c4, data_0209f284,
   data_0209f2d8, data_0209f248) are all in walk_window.map (most in
   hal/auto_bss.cpp; data_0208ee44 in romdata, data_0209f2d8 in
   Player_InitResources.cpp -- w3-c review attribution), and the
   ten-argument OAM::Render it calls is in walk_window.map from its own
   matched TU (_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii.cpp.obj). */

/* ...and the reverse. CalculateDigits' TU is a .c file, so it defines the C
   name, while RenderCoinCount and RenderLifeCount call it as a member. */
extern "C" void _ZN3HUD15CalculateDigitsEt(void *self, unsigned short n);
void HUD::CalculateDigits(unsigned short n)
{
    _ZN3HUD15CalculateDigitsEt(this, n);
}

namespace {

/* -1, not 0, and for the reason this whole lane exists: 0 is a VALID slot
   index, so an unwritten slot variable reads as "slot 0 (InitResources)" --
   exactly the false report that sent the 2026-08-16 exit-course crash to the
   wrong slot for a session. Every trap below records its own index before
   reporting, so this value should be unreachable; -1 makes it say so out loud
   instead of naming an innocent slot if it ever is not. */
int g_trap_slot = -1;
const char *const kSlotName[18] = {
    "InitResources", "BeforeInitResources", "AfterInitResources",
    "CleanupResources", "BeforeCleanupResources", "AfterCleanupResources",
    "Behavior", "BeforeBehavior", "AfterBehavior",
    "Render", "BeforeRender", "AfterRender",
    "OnPendingDestroy", "Virtual34", "Virtual38", "OnHeapCreated",
    "~D1", "~D0"};

/* THE SLOT NUMBER AND THE SLOT NAME WERE BOTH WRONG, and this file had the
   right idea in only two of eighteen places. sa_fill_shared installed a bare
   sa_trap in every slot and then overwrote 13 and 14 with thunks that record
   their index, so the other sixteen reported whatever g_trap_slot last held --
   its .bss zero, i.e. "slot 0 (InitResources)". On the Stage's copy of this
   same bug that misdirection cost the 2026-08-16 exit-course report a whole
   session (hal/stage_bridges.cpp carries the full account).

   `kSlotName[g_trap_slot & 17]` is the second half of it: 17 is 0b10001, a
   bitmask standing in for a bounds clamp, so it keeps bits 0 and 4 only. It
   mis-named even the two slots that DID record themselves -- 13 came out as
   kSlotName[1] "BeforeInitResources" and 14 as kSlotName[0] "InitResources".

   Both fixed: a thunk per slot below, and a real range test here. */
int __fastcall sa_trap(void *s, void *)
{
    const char *name = "the trap did not record its slot -- fix the thunk";
    if (g_trap_slot >= 0 && g_trap_slot < 18)
        name = kSlotName[g_trap_slot];
    std::fprintf(stderr, "FATAL: %s vtable slot %d (%s) is not hosted\n",
                 port_actor_class_name(
                     s ? *(unsigned short *)((char *)s + 0xc) : 0u),
                 g_trap_slot, name);
    std::abort();
    return 0;
}

/* The ten shared slots, every one of them reached QUALIFIED so the call cannot
   re-dispatch through the vtable it is filling. ActorBase throughout, except
   slot 2. */
int __fastcall sa_binit(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeInitResources(); }
void __fastcall sa_ainit(void *s, void *, unsigned a)
{ ((ActorDerived *)s)->ActorDerived::AfterInitResources(a); }
int __fastcall sa_bclean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeCleanupResources(); }
void __fastcall sa_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
int __fastcall sa_bbeh(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeBehavior(); }
void __fastcall sa_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
int __fastcall sa_bren(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeRender(); }
void __fastcall sa_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
int __fastcall sa_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
#define SA_TRAP(n)                                                           \
    int __fastcall sa_trap##n(void *s, void *) { g_trap_slot = (n); return sa_trap(s, 0); }
SA_TRAP(0)  SA_TRAP(1)  SA_TRAP(2)  SA_TRAP(3)  SA_TRAP(4)  SA_TRAP(5)
SA_TRAP(6)  SA_TRAP(7)  SA_TRAP(8)  SA_TRAP(9)  SA_TRAP(10) SA_TRAP(11)
SA_TRAP(12) SA_TRAP(13) SA_TRAP(14) SA_TRAP(15) SA_TRAP(16) SA_TRAP(17)
#undef SA_TRAP

void *const sa_trap_thunk[18] = {
    (void *)sa_trap0,  (void *)sa_trap1,  (void *)sa_trap2,  (void *)sa_trap3,
    (void *)sa_trap4,  (void *)sa_trap5,  (void *)sa_trap6,  (void *)sa_trap7,
    (void *)sa_trap8,  (void *)sa_trap9,  (void *)sa_trap10, (void *)sa_trap11,
    (void *)sa_trap12, (void *)sa_trap13, (void *)sa_trap14, (void *)sa_trap15,
    (void *)sa_trap16, (void *)sa_trap17};

void sa_fill_shared(void **vt)
{
    for (int i = 0; i < 18; ++i) vt[i] = sa_trap_thunk[i];
    vt[1] = (void *)sa_binit;
    vt[2] = (void *)sa_ainit;
    vt[4] = (void *)sa_bclean;
    vt[5] = (void *)sa_aclean;
    vt[7] = (void *)sa_bbeh;
    vt[8] = (void *)sa_abeh;
    vt[10] = (void *)sa_bren;
    vt[11] = (void *)sa_aren;
    /* 13 and 14 used to be re-written here with the only two self-recording
       thunks this file had. The fill loop above now installs a self-recording
       thunk in every slot, so those two lines said nothing the loop had not
       already said and are gone. */
    vt[15] = (void *)sa_heap;
}

// ---- THE VS COIN COUNTER, MOVED TO THE TOP SCREEN --------------------------
//
// A DELIBERATE PORT-SIDE DEVIATION, and the only one in this file. Tango, on
// the round-1 HUD captures: "HUD looks good but coins needs to be on the top
// screen not bottom. basically where they are on the bottom screen they need to
// be on the top." It lands under his standing ruling that online VS is a
// host-layer opt-in and already a mod; it is VS-only, it has a kill switch, and
// everything it draws is the ROM's own sprite through the ROM's own OAM::Render.
//
// WHAT THE ROM DOES, and it is one boolean. src/_ZN3HUD15RenderCoinCountEv.cpp
// has two arms and they differ in exactly that argument:
//
//     VS  (data_0209f2d8 == 1)   OAM::Render(TRUE,  ...)   the BOTTOM screen
//     adventure                  OAM::Render(FALSE, ...)   the TOP screen
//
// Every coordinate is shared between the two arms, so "where they are on the
// bottom screen" and "where they need to be on the top" are the SAME numbers,
// and moving them is passing the adventure arm's boolean on the VS arm's path.
//
// ---- AND THAT ALONE IS INVISIBLE, BECAUSE THE SPOT IS TAKEN -----------------
//
// Measured, and it is the whole reason this function has a Y offset. In VS the
// ROM draws the STAR count on the TOP screen and the COIN count on the BOTTOM
// screen AT THE SAME COORDINATES. Both are three sprites at y=2, x 240 / 223 /
// 207, and they differ only in the third glyph. The engine OAM census
// (SM64DS_VS_HUD=1) with the move off:
//
//     A OAM[3] y=2 x=240 tile=192 pal=1     A OAM[5] y=2 x=207 tile=158 pal=6
//     B OAM[0] y=2 x=240 tile=192 pal=1     B OAM[2] y=2 x=207 tile=154 pal=0
//
// -- engine A is the star count, engine B is the coin count, same three slots.
// So flipping the boolean and nothing else lands the coins on top of the star
// count: engine A went from 6 placed sprites to 9, every one with real tile
// data.
//
// AN EARLIER REVISION SAID THE TOP SCREEN CAME OUT "BYTE-IDENTICAL" AND READ
// THAT AS THE COINS HIDING BEHIND THE STARS. The observation was real and the
// explanation was wrong, and the difference matters now that it is fixed. That
// capture was taken BEFORE the BG0CNT priority seat existed, when NO
// priority-1 sprite rendered at all -- neither the coins nor the star count
// they were supposedly hiding behind. The screen was identical because nothing
// at that priority reached it, not because three sprites lined up.
//
// RE-MEASURED with the priority seat in, SM64DS_VS_COINS_TOP_Y=0 against the
// ROM's bottom-screen draw: 268 px on the top screen (p1), 220 px (p2), in the
// coin block's own rows. The two triples share their coordinates but not their
// pixels -- the third glyph differs (tile 154 pal 0 against tile 158 pal 6),
// and the digits differ whenever the two counts do. So a dy of 0 does not hide
// the coins, it draws them ON the star count, which is worse than either.
//
// The coins therefore go to the top screen at the ROM's own x and one sprite
// row DOWN, stacked under the star count on the same right edge, which is the
// only free slot that keeps "where they are on the bottom screen" true
// horizontally.
// SM64DS_VS_COINS_TOP_Y sets the offset; 18 is the default and the number is
// the one thing here that is a judgement rather than a measurement, so it is a
// variable and it is going to the owner with a capture rather than being
// asserted.
//
// WHY IT IS HOSTED HERE RATHER THAN PATCHED. RenderCoinCount is not a vtable
// slot; it is a direct call from inside HUD::Render, which is matched src this
// lane may not touch. So the port stands in for HUD::Render's VS ARM ONLY --
// the ROM's own four leaves in the ROM's own order behind the ROM's own two
// guards -- and substitutes a copy of the coin leaf. RenderVsTimer,
// RenderStarCount and RenderCameraButtons are the ROM's, called unchanged.
// Anything that is not the VS arm (adventure, the paused branch, the
// data_0209fc9c early-out) falls through to HUD::Render untouched.
//
// THE ROM'S BOTTOM-SCREEN DRAW IS SUPPRESSED, and that is not an overreach: the
// ask is a MOVE, and leaving the sub-screen copy standing would be a duplicate
// rather than a move.
//
// SM64DS_VS_COINS_TOP=0 puts the coins back on the bottom screen on the same
// binary, which is also how the before/after capture pair is taken.
// SM64DS_VS_COINS_TOP_Y=<n> moves the stack; 0 puts the coins back on the star
// count's own coordinates, which is how the 268 px overlap above is reproduced.
extern "C" {
void _ZN3HUD13RenderVsTimerEv(void *self);
void _ZN3HUD15RenderStarCountEv(void *self);
void _ZN3HUD19RenderCameraButtonsEv(void *self);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int sub, void *attr, int x, int y,
                                              int a, int b, void *m);
extern void *func_020aba70[];    /* the ten digit sprites          */
extern char func_020ab9c8;       /* the glyph at the running x     */
extern char func_020abad8;       /* the coin glyph, x - 0x10       */
extern unsigned short data_0209f358[];   /* per-player coin counts */
extern unsigned char data_0209fc9c;      /* HUD::Render's VS early-out */
}

/* Returns 1 when it has drawn the whole VS arm itself and the caller must NOT
   also run HUD::Render; 0 means "not my case, run the ROM's". */
static int port_vs_hud_render_coins_on_top(HUD *self)
{
    static int want = -1, dy = 0;
    if (want < 0) {
        const char *e = std::getenv("SM64DS_VS_COINS_TOP");
        want = (e && e[0] == '0') ? 0 : 1;
        e = std::getenv("SM64DS_VS_COINS_TOP_Y");
        dy = e ? std::atoi(e) : 18;
        if (dy < 0) dy = 0;
        if (dy > 160) dy = 160;
    }
    if (!want || data_0209f2d8 != 1)
        return 0;
    /* HUD::Render's own two VS guards, in its own order. Neither is reproduced
       loosely: the first is its `goto end` and the second picks the full arm
       over the time-up-only arm. Anything but the full arm is the ROM's job. */
    if (data_0209fc9c != 0)
        return 0;
    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) != 0)
        return 0;

    _ZN3HUD13RenderVsTimerEv((void *)self);
    _ZN3HUD15RenderStarCountEv((void *)self);

    /* src/_ZN3HUD15RenderCoinCountEv.cpp's VS arm, verbatim except for the
       leading argument of the three OAM::Render calls. digits[3] is at HUD+0x74
       and CalculateDigits is the ROM's own. */
    {
        int sb = 0xf0;
        self->CalculateDigits(data_0209f358[data_0209f250]);
        const signed char *digits = (const signed char *)((char *)self + 0x74);
        for (int i = 2; i >= 0; i--) {
            const signed char d = digits[i];
            if (d >= 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[d],
                                                         sb, 2 + dy, -1, 1, 0);
                sb -= 9;
            }
        }
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &func_020ab9c8, sb, 0xa + dy,
                                                 -1, 1, 0);
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &func_020abad8, sb - 0x10,
                                                 2 + dy, -1, 1, 0);
    }

    _ZN3HUD19RenderCameraButtonsEv((void *)self);
    return 1;
}

// ---- HUD -------------------------------------------------------------------
int __fastcall hud_init(void *s, void *) { return ((HUD *)s)->HUD::InitResources(); }
int __fastcall hud_clean(void *s, void *) { return ((HUD *)s)->HUD::CleanupResources(); }
int __fastcall hud_behavior(void *s, void *) { return ((HUD *)s)->HUD::Behavior(); }
int __fastcall hud_render(void *s, void *)
{
    /* SM64DS_HUD_TRACE=1: the two globals every HUD render leaf dereferences
       before it draws anything -- the per-player Actor table and the local
       player index. A null there faults inside a matched leaf at a small
       offset, which reads as a codegen bug and is not one. */
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_HUD_TRACE") != 0;
    if (on) {
        static int said;
        if (said < 3) {
            ++said;
            std::printf("  [hud] render: this=%p f394[0]=%p f250=%u "
                        "caa0[8]=%02x f2d8=%u f2c4|f20c|f294=%02x oam=%d\n", s,
                        (void *)data_0209f394[0], (unsigned)data_0209f250,
                        (unsigned)((unsigned char *)data_0209caa0)[8],
                        (unsigned)data_0209f2d8,
                        (unsigned)(data_0209f2c4 | data_0209f20c | data_0209f294),
                        port_sub_oam_nonzero());
        }
    }
    /* the VS coin move, above; it answers 0 on every path that is not the VS
       full-render arm, and the ROM's own Render runs then exactly as before */
    if (port_vs_hud_render_coins_on_top((HUD *)s))
        return 1;
    return ((HUD *)s)->HUD::Render();
}
void __fastcall hud_pdes(void *s, void *) { ((HUD *)s)->HUD::OnPendingDestroy(); }
void *__fastcall hud_d1(void *s, void *) { return _ZN3HUDD1Ev(s); }
void *__fastcall hud_d0(void *s, void *) { return _ZN3HUDD0Ev(s); }

// ---- Minimap ---------------------------------------------------------------
int __fastcall map_init(void *s, void *) { return ((Minimap *)s)->Minimap::InitResources(); }
int __fastcall map_clean(void *s, void *) { return ((Minimap *)s)->Minimap::CleanupResources(); }
int __fastcall map_behavior(void *s, void *) { return _ZN7Minimap8BehaviorEv(s); }
int __fastcall map_render(void *s, void *)
{
    /* SM64DS_MM_TRACE=1: the marker's own minimap coordinates and the live sub
       OAM count. A minimap that draws its map and no marker reads as a Render
       bug and is usually a sprite-template one -- see hal/oam_lists.cpp. */
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_MM_TRACE") != 0;
    if (on) {
        static int n;
        if (n < 2 || (n % 120) == 0) {
            char *b = (char *)s;
            /* The local player's blip coords moved off the struct (+0x70/+0x80)
               into g_mm_icon* for the sixteen-player widen; read slot 0 there. */
            std::printf("  [mm] render #%d: marker=(%d,%d) mode=%u hidden=%u "
                        "scale=%d oam=%d\n", n,
                        g_mm_iconX[0], g_mm_iconY[0],
                        (unsigned)*(unsigned char *)(b + 0x251),
                        (unsigned)*(unsigned char *)(b + 0x255),
                        ((int *)(b + 0x214))[0], port_sub_oam_nonzero());
            std::fflush(stdout);
        }
        ++n;
    }
    return ((Minimap *)s)->Minimap::Render();
}
void __fastcall map_pdes(void *s, void *) { ((Minimap *)s)->Minimap::OnPendingDestroy(); }
void *__fastcall map_d1(void *s, void *) { return _ZN7MinimapD1Ev(s); }
void *__fastcall map_d0(void *s, void *) { return _ZN7MinimapD0Ev(s); }

}  // namespace

extern "C" void hal_fill_hud_vtable(void)
{
    void **vt = _ZTV8dMeter_c;
    sa_fill_shared(vt);
    vt[0] = (void *)hud_init;
    vt[3] = (void *)hud_clean;
    vt[6] = (void *)hud_behavior;
    vt[9] = (void *)hud_render;
    vt[12] = (void *)hud_pdes;
    vt[16] = (void *)hud_d1;
    vt[17] = (void *)hud_d0;
    /* the base table is never dispatched through, but a null slot in it would
       be indistinguishable from a bug if one ever were */
    for (int i = 0; i < 18; ++i) _ZTV7dBase_c[i] = sa_trap_thunk[i];

    /* the sprite templates the render leaves index, and the pointer pass that
       puts OAM::NUMBERS' ten digit pointers on host addresses */
    port_ov001_syms_patch();
    port_ov001_pack_check();
}

// The cross-overlay pointer tables used to be hand-written here.
//
// OAM::MM_PLAYER_ICONS is a pointer table in ov002 (0x0210c174) whose entries
// all point into ov001, at the MM_*_ICON OamAttr records at 0x020ab800..0x920.
// ovdata's per-mount pointer pass rewrites pointers whose TARGET is inside the
// same mount, and each mount is its own invocation, so this one came out
// holding DS addresses and OAM::RenderSub was handed 0x020ab800 -- which read
// as "the minimap draws its map and no player marker", because the OAM entry
// it built out of whatever sat at that host address was not a sprite.
//
// port_cross_patch() (tools/ovdata.py --cross) now does this and every other
// crossing, from relocs.txt rather than from a table copied out of the ROM by
// hand. It reproduces the sixteen entries this function used to write, in the
// same order, and adds the seventeenth the ROM table has and this did not, the
// aliasing copy inside port_ov002_gap_0210c168, and forty-odd more between the
// actor overlays. The harnesses call it next to port_ov002_patch().

extern "C" void hal_fill_minimap_vtable(void)
{
    void **vt = _ZTV6dMap_c;
    sa_fill_shared(vt);
    vt[0] = (void *)map_init;
    vt[3] = (void *)map_clean;
    vt[6] = (void *)map_behavior;
    vt[9] = (void *)map_render;
    vt[12] = (void *)map_pdes;
    vt[16] = (void *)map_d1;
    vt[17] = (void *)map_d0;
}

// ---- the ov001 name aliases -------------------------------------------------
//
// The HUD's render TUs name six sprite tables by address because dsd could
// not attribute them: ov000 and ov001 share the base 0x020aa420, and ov000
// holds filename strings where ov001 holds OamAttr records. config names all
// six in ov001, and both names are DATA at the same address, so each alias is
// exact -- no calling convention to get wrong. The sixth is the bouncing-arrow
// template Stage::RenderBouncingArrows draws from (the w4-e mount, seated
// w6-c; see the seat note above).
#pragma comment(linker, "/alternatename:_func_020ab948=__ZN3OAM10LIFE_ICONSE")
#pragma comment(linker, "/alternatename:_func_020ab9c8=__ZN3OAM5TIMESE")
#pragma comment(linker, "/alternatename:_func_020aba70=__ZN3OAM7NUMBERSE")
#pragma comment(linker, "/alternatename:_func_020abad0=__ZN3OAM10POWER_STARE")
#pragma comment(linker, "/alternatename:_func_020abad8=__ZN3OAM4COINE")
#pragma comment(linker, "/alternatename:_func_020abd88=__ZN3OAM14BOUNCING_ARROWE")
/* the same table under its ov000-prefixed misattribution, which one TU uses */
#pragma comment(linker, "/alternatename:_data_ov000_020aba70=__ZN3OAM7NUMBERSE")
#pragma comment(linker, "/alternatename:_data_ov000_020ab9c8=__ZN3OAM5TIMESE")

// ---- C++-mangled references onto C-named definitions -----------------------
//
// The HUD's leaves declare their callees inside `namespace OAM { ... }` or as
// plain C++ free functions, so MSVC emits C++ manglings for symbols that are
// C-named everywhere else in the port. Every one of these is cdecl on both
// sides, so the alias is exact -- the mechanism hal/cxx_aliases.cpp documents.
#pragma comment(linker, "/alternatename:?Render@OAM@@YAX_NPAUOamAttr@@HHHHPAUMatrix2x2@@@Z=__ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2")
#pragma comment(linker, "/alternatename:?RenderSub@OAM@@SAXPAUOamAttr@@HHHH@Z=__ZN3OAM9RenderSubEP7OamAttriiii")
#pragma comment(linker, "/alternatename:?GetOwnerLanguage@@YAHXZ=_GetOwnerLanguage")
#pragma comment(linker, "/alternatename:?_ZN5Timer7GetTimeEv@@YA_KPAX@Z=__ZN5Timer7GetTimeEv")
#pragma comment(linker, "/alternatename:?LoadOBJPltt@GX@@YAXPBXII@Z=__ZN2GX11LoadOBJPlttEPKvjj")
#pragma comment(linker, "/alternatename:?LoadOBJPltt@GXS@@YAXPBXII@Z=__ZN3GXS11LoadOBJPlttEPKvjj")
#pragma comment(linker, "/alternatename:?data_0209fc9c@@3EA=_data_0209fc9c")
#pragma comment(linker, "/alternatename:?data_ov002_0210c29c@@3PAHA=_data_ov002_0210c29c")
#pragma comment(linker, "/alternatename:?data_ov002_0210c310@@3PAFA=_data_ov002_0210c310")
#pragma comment(linker, "/alternatename:?data_ov002_02111178@@3EA=_data_ov002_02111178")
#pragma comment(linker, "/alternatename:?GiveLives@@YAXH@Z=_GiveLives")

/* The same for the Minimap's: its InitResources declares these at file scope
   OUTSIDE the extern "C" block, so MSVC asks for a C++ mangling of storage the
   rest of the port owns under the C name. */
#pragma comment(linker, "/alternatename:?data_0209f2e8@@3EA=_data_0209f2e8")
#pragma comment(linker, "/alternatename:?data_0209f334@@3PAGA=_data_0209f334")
#pragma comment(linker, "/alternatename:?data_0209f394@@3PAPAXA=_data_0209f394")
#pragma comment(linker, "/alternatename:?data_0209d454@@3EA=_data_0209d454")
#pragma comment(linker, "/alternatename:?data_ov002_02111148@@3CA=_data_ov002_02111148")
#pragma comment(linker, "/alternatename:?data_ov002_02111150@@3EA=_data_ov002_02111150")
#pragma comment(linker, "/alternatename:?data_ov002_0211064c@@3UState@@A=_data_ov002_0211064c")
#pragma comment(linker, "/alternatename:?data_ov002_02110664@@3UState@@A=_data_ov002_02110664")
#pragma comment(linker, "/alternatename:?GetBG3CharPtr@G2S@@YAPAXXZ=__ZN3G2S13GetBG3CharPtrEv")
#pragma comment(linker, "/alternatename:?GetBit@Event@@SAHI@Z=__ZN5Event6GetBitEj")
#pragma comment(linker, "/alternatename:?SublevelToLevel@@YAHH@Z=_SublevelToLevel")

/* OAM's camera-button templates are static DATA members of class OAM in the
   TU that draws them, and ov002 data named _ZN3OAM..E in the mount. */
#pragma comment(linker, "/alternatename:?CAM_BUTTON_L@OAM@@2UOamAttr@@A=__ZN3OAM12CAM_BUTTON_LE")
#pragma comment(linker, "/alternatename:?CAM_BUTTON_R@OAM@@2UOamAttr@@A=__ZN3OAM12CAM_BUTTON_RE")
#pragma comment(linker, "/alternatename:?CAM_BUTTON_L_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM20CAM_BUTTON_L_PRESSEDE")
#pragma comment(linker, "/alternatename:?CAM_BUTTON_R_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM20CAM_BUTTON_R_PRESSEDE")
#pragma comment(linker, "/alternatename:?S_CAM_BUTTON_L@OAM@@2UOamAttr@@A=__ZN3OAM14S_CAM_BUTTON_LE")
#pragma comment(linker, "/alternatename:?S_CAM_BUTTON_R@OAM@@2UOamAttr@@A=__ZN3OAM14S_CAM_BUTTON_RE")
#pragma comment(linker, "/alternatename:?S_CAM_BUTTON_L_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM22S_CAM_BUTTON_L_PRESSEDE")
#pragma comment(linker, "/alternatename:?S_CAM_BUTTON_R_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM22S_CAM_BUTTON_R_PRESSEDE")
#pragma comment(linker, "/alternatename:?CAM_ZOOM_BUTTON@OAM@@2UOamAttr@@A=__ZN3OAM15CAM_ZOOM_BUTTONE")
#pragma comment(linker, "/alternatename:?CAM_ZOOM_BUTTON_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM23CAM_ZOOM_BUTTON_PRESSEDE")
#pragma comment(linker, "/alternatename:?S_CAM_ZOOM_BUTTON@OAM@@2UOamAttr@@A=__ZN3OAM17S_CAM_ZOOM_BUTTONE")
#pragma comment(linker, "/alternatename:?S_CAM_ZOOM_BUTTON_PRESSED@OAM@@2UOamAttr@@A=__ZN3OAM25S_CAM_ZOOM_BUTTON_PRESSEDE")
