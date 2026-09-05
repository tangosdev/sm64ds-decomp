// RUN LINKW WAVE 17 (lane w17): SEAWEED (id 296, ov002), RTTI 13daObjWakame_c.
//
// Twelve instances -- x8 on level 18 (Dire Dire Docks) and x4 on level 8
// (Jolly Roger Bay), both already mounted -- which makes this the largest
// single-id population left on the cast board.
// Lane w16 ruled all five of its marker-carrying bodies REAL DECOMP against
// the ROM and left the id blocked on one mount line in port/ov002_syms.txt.
// The full derivation, the width correction and the two hazards are in
// port/slice_w17a.txt; the short version:
//
//   * the vtable is 0x02109bb8, 31 slots, identity from the RTTI record at
//     vtable[-1] and not from a dsd label;
//   * dsd's _ZTV7Seaweed (0x02109c74) is a DIFFERENT class -- RTTI
//     12daObjHeart_c, HealingHeart, which this port already seats under that
//     spelling -- so nothing here touches it;
//   * the record is 0x1c, not the 0x24 the dsd next-symbol delta reads: the
//     two extra words are the table's own Itanium prologue;
//   * slot 17 restores the table through the shared placeholder VT0, which is
//     a LIVE 20-word dummy in hal/actor_vtables.cpp, so it takes a per-source
//     -DVT0=_ZTV13daObjWakame_c;
//   * slot 9 is the ModelAnim slot-5 collision and is hosted below.
//
// 312-byte object: ModelAnim at +0xd4, the actor's own rotation matrix at
// +0xf0 and the scaled bounds at +0x114..0x11c (func_ov002_020bc488 writes
// both).

#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "Model.h"
#include "ModelAnim.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
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

/* the class's own bodies (port/slice_w17a.txt) */
int func_ov002_020bc540(char *self);   /* slot 0  InitResources */
int func_ov002_020bc4c8(void);         /* slot 3  CleanupResources */
int func_ov002_020bc520(void *self);   /* slot 6  Behavior */
int *func_ov002_020bc414(int *self);   /* slot 16 D1 */
int *func_ov002_020bc444(int *self);   /* slot 17 D0 */
void *Seaweed_Spawn(void);

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp  */
void port_actor_render_probe(const char *what, char *model);

DSSTATE_BEGIN
void *_ZTV13daObjWakame_c[31];
DSSTATE_END
}

// ---- slot 9: the ModelAnim slot-5 collision, hosted -------------------------
//
// src/func_ov002_020bc4f8.cpp dispatches Render through a local six-virtual
// shadow at +0xd4. That counts in the ROM's Itanium numbering (two destructor
// slots), so its slot 5 is ModelAnim::Render -- but the host _ZTV9ModelAnim is
// MSVC-numbered and its slot 5 is ModelAnim::Virtual18, which takes a second
// argument the shadow never passes. hal/cxxname_bridge.cpp says so in its own
// comment and predicts exactly this TU. The matched source is dropped from the
// slice and the body is transcribed here, the Whomp/Butterfly/Fish recipe in
// port/unmatched/ModelAnim_Renders.cpp.
//
// The ROM body, overlay_0002.bin at 0x020bc4f8, 0x28 bytes:
//     ldr r2, [r0, #0xd4]!     the ModelAnim's vptr, r0 now &mModelAnim
//     mov r1, #0               the scale argument
//     ldr r2, [r2, #0x14]      slot 5 = Render
//     blx r2
//     mov r0, #1 / bx lr
//
// This body is byte-identical to _ZN7Seaweed6RenderEv in
// port/unmatched/ModelAnim_Renders.cpp -- which is HealingHeart's, not this
// class's, and whose own comment already predicted "fixes both HealingHeart
// and any on-screen SEAWEED". MSVC's /OPT:ICF folds the two, so
// func_ov002_020bc4f8 does not appear in walk_window.map as a separate name;
// it is defined here under the ROM's own symbol anyway, so the vtable slot
// names the body the ROM names and nothing depends on the fold.
extern "C" {
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Butterfly
 * case -- the host _ZTV9ModelAnim slot 5 is Virtual18. */
int func_ov002_020bc4f8(void *selfv)
{
    char *c = (char *)selfv;
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    return 1;
}
}

// ---- the trap ---------------------------------------------------------------
static void wk_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: Seaweed vtable slot %d is not hosted (actor id "
                 "%u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "unhosted Seaweed vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define WK_TRAP(n) \
    static int __fastcall wk_trap##n(void *s, void *) \
    { wk_trap_report(s, n); return 0; }
WK_TRAP(13) WK_TRAP(14)
#undef WK_TRAP

/* Slots 13/14/30 are the ccm/ov031/ov052/w15 convention: 13/14 are
   ActorBase::Virtual34/Virtual38, two u32 arguments each and bodies this port
   does not link, and 30 is Actor::OnAimedAtWithEggReturnVec, an SRET method no
   __fastcall thunk shape here models. */

static int __fastcall wk_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall wk_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall wk_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall wk_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall wk_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall wk_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall wk_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall wk_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall wk_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall wk_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall wk_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall wk_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall wk_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall wk_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall wk_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall wk_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall wk_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall wk_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall wk_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall wk_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall wk_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall wk_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* the class's own five */
static int __fastcall wk_init(void *s, void *)
{ return func_ov002_020bc540((char *)s); }
static int __fastcall wk_clean(void *s, void *)
{ return func_ov002_020bc4c8(); }
static int __fastcall wk_behavior(void *s, void *)
{ return func_ov002_020bc520(s); }
static int __fastcall wk_render(void *s, void *)
{
    port_actor_render_probe("SEAWEED", (char *)s + 0xd4);
    return func_ov002_020bc4f8(s);
}
static int __fastcall wk_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020bc414((int *)s); }
static int __fastcall wk_d0(void *s, void *)
{ return (int)(size_t)func_ov002_020bc444((int *)s); }

extern "C" void hal_fill_seaweed_vtable(void)
{
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV13daObjWakame_c;
    vt[0]  = (void *)wk_init;
    vt[1]  = (void *)wk_binit;
    vt[2]  = (void *)wk_ainit;
    vt[3]  = (void *)wk_clean;
    vt[4]  = (void *)wk_bclean;
    vt[5]  = (void *)wk_aclean;
    vt[6]  = (void *)wk_behavior;
    vt[7]  = (void *)wk_bbeh;
    vt[8]  = (void *)wk_abeh;
    vt[9]  = (void *)wk_render;
    vt[10] = (void *)wk_bren;
    vt[11] = (void *)wk_aren;
    vt[12] = (void *)wk_pdes;
    vt[13] = (void *)wk_trap13;
    vt[14] = (void *)wk_trap14;
    vt[15] = (void *)wk_heap;
    vt[16] = (void *)wk_d1;
    vt[17] = (void *)wk_d0;
    vt[18] = (void *)wk_yoshi;
    vt[19] = (void *)wk_turn_egg;
    vt[20] = (void *)wk_v50;
    vt[21] = (void *)wk_pounded;
    vt[22] = (void *)wk_atk1;
    vt[23] = (void *)wk_atk2;
    vt[24] = (void *)wk_kicked;
    vt[25] = (void *)wk_pushed;
    vt[26] = (void *)wk_cannon;
    vt[27] = (void *)wk_mega;
    vt[28] = (void *)wk_under;
    vt[29] = (void *)wk_egg;
    vt[30] = (void *)port_actor_s30_base;
}
