// RUN LINKW WAVE 6 (lane w6-A): THE BOSS REWARD -- ov089, KEY (282) and
// LAST_STAR (283), the two classes the koopaN_boss arenas drop when Bowser
// goes down.
//
// ---- WHY THIS OVERLAY IS ALREADY HERE --------------------------------------
//
// ov089 has been mounted since gate 18 (port/ov089_syms.txt), for the castle
// doors: daDoor_c's InitResources hands a key-model index to LoadKeyModels,
// which walks data_ov089_02132894[] / data_ov089_021328b4[]. hal/actor_overlays
// .cpp already runs its pack_check, syms_patch, keymodels fixup and its single
// sinit. Its two OWN classes were never registered -- "only its data and its
// sinit are needed, which is exactly what an overlay mount is", as its syms
// header puts it. This lane needs the classes.
//
// ---- WHY THE ARENA REACHES THEM AT ALL -------------------------------------
//
// LoadOrUnloadObjectOverlays short-circuits idx 0x24/0x26/0x28 (levels 36/38/
// 40) to ov060 with an early return -- but the SELECTOR LOOP runs BEFORE that
// return, and row [0,0,0,0,1,0,0] loads ov089 too. w5-E's reviewer found this
// (review/w5e_review.md); the first draft of hal/actor_classes_ov060.cpp said
// "ov060 alone" in four places and was wrong. So the arena has ov089 resident,
// and Bowser's own InitResources already proves it by loading
// data_ov089_02132c50 (a Model SharedFilePtr in ov089's bss) on its first
// frame.
//
// ---- WHY THE ROWS ARE NOT OPTIONAL -----------------------------------------
//
// func_ov060_021135fc -- Bowser's defeat path, reached from func_ov060_02112ee0
// and func_ov060_021130c0 -- does this:
//
//     void *spawned = Actor::Spawn(0x11a /* 282 KEY */, ...);
//     *(short *)((char *)spawned + 0x440) = *(short *)(c + 0x402);
//
// with no null check. The port's Actor::Spawn returns null for an unregistered
// class, so an unregistered KEY turns the end of the fight into a c0000005.
// The sibling spawn in the same body, 0x11b (283 LAST_STAR), IS ignored -- but
// it costs one row and one SpawnInfo that the same mount already carries, and
// leaving it unregistered leaves a hole one branch away.
//
// ---- ONE TABLE, TWO IDS ----------------------------------------------------
//
// ov089 defines exactly ONE vtable, _ZTV3Key at 0x02132ba8 (31 slots, plain
// Actor), and BOTH factories store it -- src/Key_Spawn.c and
// src/LastStar_Spawn.c are byte-identical apart from their names, down to the
// 1136-byte allocation and the five member constructors. So LAST_STAR's
// registry row carries a null fill and rides KEY's, the BLUE_FLAME/RED_FLAME
// and GreenShellBlockTag shape. Both factories store the table by a REAL name,
// so neither needs a VT1 wrapper.
//
// Own slots, read off the table's reloc run: 0/3/6/9/16/17 plus 18 and 19 --
// this class overrides Actor::OnYoshiTryEat and Actor::OnTurnIntoEgg
// (func_ov089_02131f4c / func_ov089_02131f04), which no other class in this
// lane does, so the fill writes them after the shared half.
//
// ---- THE ONE HELD-OUT BODY -------------------------------------------------
//
// Key::Render is the ModelAnim slot-5 collision (host copy in
// port/unmatched/Ov089_Renders.cpp, banner there). Everything else is matched
// src and links: Init/Behavior/Cleanup are real C++ methods against Key.h and
// get C-name faces at the bottom of this file; D1/D0 and the two slot-18/19
// bodies are .c and are callable directly.
#include <cstdio>

#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half, the same family every hosted 31-slot table carries */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* KEY's own bodies */
int _ZN3Key13InitResourcesEv(void *self);         /* slot 0  */
int _ZN3Key16CleanupResourcesEv(void *self);      /* slot 3  */
int _ZN3Key8BehaviorEv(void *self);  /* slot 6, HOST COPY (Ov089_StateDispatch) */
int _ZN3Key6RenderEv(void *self);   /* slot 9, HOST COPY (Ov089_Renders.cpp) */
int *_ZN3KeyD1Ev(int *self);                      /* slot 16 */
int *_ZN3KeyD0Ev(int *self);                      /* slot 17 */
/* slot 18 takes NO argument in src (it returns a constant 4 and never reads
   the receiver); slot 19 takes the Player the dispatch site pushes and USES
   it -- it forwards it to func_ov089_02131df4/02131dcc. Both signatures are
   the matched TUs' own, so the thunks below drop nothing. */
int func_ov089_02131f4c(void);                    /* slot 18, OnYoshiTryEat  */
int func_ov089_02131f04(void *self, void *p);     /* slot 19, OnTurnIntoEgg  */
void *Key_Spawn(void);
void *LastStar_Spawn(void);

DSSTATE_BEGIN
void *_ZTV3Key[31];
/* func_ov089_0213162c (KEY state 3) ends with `data_02111b68 = 1`, a write
   into the twenty-way-shared LEVEL-OVERLAY window. ov089's relocs say that
   address belongs to overlays(8,13,15,17,23,24,33,41,43,47,53,54,55,56) --
   the ordinary course overlays where a key is normally collected -- and NOT
   to ov046, whose image ends at 0x021116e0, so in a koopaN_boss arena the ROM
   is writing past its own level overlay into ov060's loaded image.

   CORRECTED at the wave-6 close (the lane wrote "no mount owns the cell",
   which is wrong): TWO of the fourteen overlays in that very list are mounted
   in this build and both already claim 0x02111b68.

     ov013 mounts it as DATA -- ov013_syms.txt carries
     `data_ov013_02111b68:0x4`, four bytes of the clock furniture's statics.
     ov015 hosts it as CODE -- KnockDownPlank_Spawn IS ov015 0x02111b68
     (hal/actor_classes.inc, the MOVING_BAR / KNOCK_DOWN_PLANK row).

   What is true is narrower and is the only reason this definition is legal:
   NO MOUNT DEFINES THIS C NAME. ov013 spells its cell `data_ov013_02111b68`
   and ov015 spells its body `KnockDownPlank_Spawn`, so the plain
   `data_02111b68` the KEY body names is undefined without the line below, and
   nothing in this build reads it. It gets hosted zero storage (the
   auto_bss.cpp shape, kept here because this lane owns the only reference)
   and is routed into .dsstate like every hosted DS global.

   There is no live divergence today only because KEY is reachable solely
   under ov046, the arena, where none of the fourteen owners is loaded. THE
   MUST-GO CAVEAT: the moment a level overlay that owns 0x02111b68 is mounted
   in a build where the KEY can also run -- or a mount is ever generated under
   the bare C name -- this definition has to go and the name has to resolve to
   that mount instead, or the port silently keeps two objects for one DS cell.
   The same caveat is written beside the ov013 and ov015 mounts and in
   ov089_syms.txt so a mount lane meets it where it works. */
int data_02111b68;
DSSTATE_END
}

/* The RTTI spelling the D1/D0 restore the table by (the daKpa2Bg_c line in
   hal/actor_classes_ov060.cpp, once more): same table, sibling name. */
#pragma comment(linker, "/alternatename:__ZTV10daObjKey_c=__ZTV3Key")

/* The MSVC-decorated spellings the two Key .cpp bodies use for storage the
   per-symbol ov089/ov002 mounts already define under the C name (the
   actor_faces_bob / bowserpuzzle @@3PA precedent; data aliases are exact,
   there is no `this` to lose). InitResources and CleanupResources declare the
   SAME cells with different element types -- `char` in one, `int *` in the
   other -- so each cell needs both decorations aliased onto the one storage. */
#pragma comment(linker, "/alternatename:?data_ov002_0211094c@@3DA=_data_ov002_0211094c")
#pragma comment(linker, "/alternatename:?data_ov002_02110964@@3PAHA=_data_ov002_02110964")
#pragma comment(linker, "/alternatename:?data_ov089_02132b40@@3DA=_data_ov089_02132b40")
#pragma comment(linker, "/alternatename:?data_ov089_021328b4@@3PAHA=_data_ov089_021328b4")
#pragma comment(linker, "/alternatename:?data_ov089_02132ca4@@3DA=_data_ov089_02132ca4")
#pragma comment(linker, "/alternatename:?data_ov089_02132c40@@3DA=_data_ov089_02132c40")
#pragma comment(linker, "/alternatename:?data_ov089_02132c40@@3PAHA=_data_ov089_02132c40")
#pragma comment(linker, "/alternatename:?data_ov089_02132c48@@3DA=_data_ov089_02132c48")
#pragma comment(linker, "/alternatename:?data_ov089_02132c48@@3PAHA=_data_ov089_02132c48")
#pragma comment(linker, "/alternatename:?data_ov089_02132c60@@3DA=_data_ov089_02132c60")
#pragma comment(linker, "/alternatename:?data_ov089_02132c60@@3PAHA=_data_ov089_02132c60")
#pragma comment(linker, "/alternatename:?data_ov089_02132c70@@3DA=_data_ov089_02132c70")
#pragma comment(linker, "/alternatename:?data_ov089_02132c70@@3PAHA=_data_ov089_02132c70")
#pragma comment(linker, "/alternatename:?data_0209cef0@@3HA=_data_0209cef0")

// ---- the trap --------------------------------------------------------------
static void ov89_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov089 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov089 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV89_TRAP(n) \
    static int __fastcall ov89_trap##n(void *s, void *) \
    { ov89_trap_report(s, n); return 0; }
OV89_TRAP(13) OV89_TRAP(14)
#undef OV89_TRAP

static int __fastcall ov89_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov89_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov89_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov89_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov89_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov89_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov89_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov89_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov89_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov89_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov89_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov89_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov89_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov89_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov89_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov89_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov89_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov89_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov89_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov89_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* KEY's own slots */
static int __fastcall key_init(void *s, void *)
{ return _ZN3Key13InitResourcesEv(s); }
static int __fastcall key_clean(void *s, void *)
{ return _ZN3Key16CleanupResourcesEv(s); }
static int __fastcall key_behavior(void *s, void *)
{ return _ZN3Key8BehaviorEv(s); }
static int __fastcall key_render(void *s, void *)
{ port_actor_render_probe("KEY", (char *)s + 0x114);
  return _ZN3Key6RenderEv(s); }
static int __fastcall key_d1(void *s, void *)
{ return (int)(size_t)_ZN3KeyD1Ev((int *)s); }
static int __fastcall key_d0(void *s, void *)
{ return (int)(size_t)_ZN3KeyD0Ev((int *)s); }
static int __fastcall key_yoshi(void *s, void *)
{ (void)s; return func_ov089_02131f4c(); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. And it
   FORWARDS that Player: func_ov089_02131f04 hands it to
   func_ov089_02131df4/02131dcc, so dropping the argument here would be a
   silent wrong call, not a harmless extra. Slot 18 next to it genuinely takes
   nothing, which is what made the wrong shape look plausible. */
static int __fastcall key_turn_egg(void *s, void *, void *p)
{ return func_ov089_02131f04(s, p); }

extern "C" void hal_fill_key_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV3Key;
    vt[1]  = (void *)ov89_binit;
    vt[2]  = (void *)ov89_ainit;
    vt[4]  = (void *)ov89_bclean;
    vt[5]  = (void *)ov89_aclean;
    vt[7]  = (void *)ov89_bbeh;
    vt[8]  = (void *)ov89_abeh;
    vt[10] = (void *)ov89_bren;
    vt[11] = (void *)ov89_aren;
    vt[12] = (void *)ov89_pdes;
    vt[13] = (void *)ov89_trap13;
    vt[14] = (void *)ov89_trap14;
    vt[15] = (void *)ov89_heap;
    vt[20] = (void *)ov89_v50;
    vt[21] = (void *)ov89_pounded;
    vt[22] = (void *)ov89_atk1;
    vt[23] = (void *)ov89_atk2;
    vt[24] = (void *)ov89_kicked;
    vt[25] = (void *)ov89_pushed;
    vt[26] = (void *)ov89_cannon;
    vt[27] = (void *)ov89_mega;
    vt[28] = (void *)ov89_under;
    vt[29] = (void *)ov89_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[0]  = (void *)key_init;
    vt[3]  = (void *)key_clean;
    vt[6]  = (void *)key_behavior;
    vt[9]  = (void *)key_render;
    vt[16] = (void *)key_d1;
    vt[17] = (void *)key_d0;
    vt[18] = (void *)key_yoshi;
    vt[19] = (void *)key_turn_egg;
}

// ---- method faces ----------------------------------------------------------
// The three bodies src defines as real C++ methods against Key.h. The
// IceSheet/ov045 recipe: the face is the C-name bridge INTO the matched
// method, not a host copy of it.
#include "Key.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
extern "C" {
int _ZN3Key13InitResourcesEv(void *self)
{ return ((Key *)self)->Key::InitResources(); }
int _ZN3Key16CleanupResourcesEv(void *self)
{ return ((Key *)self)->Key::CleanupResources(); }
}
