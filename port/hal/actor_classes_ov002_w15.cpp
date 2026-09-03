// RUN LINKW WAVE 15 (lane w15): OV002's MEGA MUSHROOM TAG PAIR (319 + 320).
//
// ONE ROM class under TWO spawn-table ids sharing ONE table -- the ov031
// SlideDecoration and ov073 CccIce shape, at the cheapest end of the board:
// ov002 is the engine overlay and has been mounted since gate 14, so this is
// two records, one host array, one fill, two registry rows and eight TUs, with
// no new mount and no new sinit.
//
//   id   class                     x        table                       width
//   319  MEGA_MUSHROOM_CREATE_TAG  L22 L24  0x02108cf4                    31
//   320  MEGA_MUSHROOM_TAG         L22 L24  (the same table)              31
//
// Both attribution routes agree on both ids (each record's word[0] lands
// inside ov002 and each reads its own id back at +4) and the identity comes
// from the RTTI typeinfo at vtable[-1] -- 0x02108cf0 -> 0x02108c94 ->
// "16daObjKinokoTag_c", kinoko being mushroom -- which agrees with both dsd
// record names. The derivation is in port/ov002_syms.txt and
// port/slice_w15b.txt.
//
// ONE CLASS, TWO IDS, AND THE CLASS KNOWS WHICH. Its Behavior branches on
// `actorID == 0x140` (320) for the pairing half and otherwise falls through to
// the id-0x13f (319) half, so a single closure of bodies serves both rows and
// the second row costs a registry line. Both Spawns are byte-identical bodies
// at different addresses -- 272-byte object, Actor::C2, store the table,
// construct the MovingCylinderClsn at +0xd4 -- and both store the table by a
// real name as their final write, so both register directly with no factory
// wrapper.
//
// ---- THE WIDTH IS 31 -------------------------------------------------------
//
// The reloc run reads 34 and the next-symbol landing reads 31, and 31 is
// right: the three words the run overruns into (0x02108d70..0x02108d7c) are
// the NEXT class's Itanium typeinfo record, {__si_class_type_info 0x0209a764,
// name 0x02108d7c, base ov002 0x021089ec}, an object with its own reader --
// the ov052 overrun in miniature. Slot 30 is
// Actor::OnAimedAtWithEggReturnVec, the plain-Actor tail every 31-slot table
// in the port ends on, and there is no slot 31: this class is not a Platform.
//
// ---- THE VTABLE IS HOST STORAGE --------------------------------------------
//
// Excluded from the ov002 mount and declared here as a host array, the
// ov095/ov080/data_ov002_021081e4 rule: a mounted vtable hands the factory a
// table of DS code addresses. dsd names 0x02108cf4 TWICE --
// _ZTV21MegaMushroomCreateTag, which both Spawns store, and
// _ZTV16daObjKinokoTag_c, which the D0 restores -- so the RTTI spelling is
// aliased onto the one host array. Neither name is mounted anywhere, so the
// alias LHS is undefined and cannot be defeated.
//
// ---- SLOT 16 IS SPELLED HERE, AND WHY --------------------------------------
//
// src/_ZN21MegaMushroomCreateTagD1Ev.cpp is the shadow-class MSVC destructor
// shape, the SlideDecorationSilverStar case (hal/actor_classes_ov031_w10.cpp):
// the TU declares its own `struct Actor` / `struct MovingCylinderClsn` /
// `struct MegaMushroomCreateTag : Actor` and defines the destructor, so MSVC
// emits ??1MegaMushroomCreateTag@@UAE@XZ whose body calls
// ??1MovingCylinderClsn@@QAE@XZ and ??1Actor@@UAE@XZ -- two decorated names
// that exist nowhere in this link, because the port carries those bodies under
// the ROM's C names. Linking that TU is an unresolved external, so slot 16 is
// transcribed from the ROM D1 instead, disassembled from
// extracted/overlays/overlay_0002.bin at 0x020b46a0 (0x30 bytes):
//     020b46a0  PUSH {r4, lr}
//     020b46a4  MOV  r4, r0
//     020b46a8  LDR  r1, [pc, #0x1c]      (pool 0x020b46cc -> 0x02108cf4)
//     020b46ac  ADD  r0, r4, #0xd4
//     020b46b0  STR  r1, [r4]             install the table
//     020b46b4  BL   0x020149a4           _ZN18MovingCylinderClsnD1Ev
//     020b46b8  MOV  r0, r4
//     020b46bc  BL   0x020112c8           _ZN5ActorD2Ev
//     020b46c0  MOV  r0, r4               return this
//     020b46c4  POP  {r4, lr} / BX lr
// A statement-for-statement transcription, not a re-derivation. The D0 is NOT
// hosted: it is a .c TU carrying the ROM's Itanium name in extern "C", it is
// in port/slice_w15b.txt, and it already does that chain plus
// Memory::Deallocate on the game heap.
//
// ---- SLOTS 13/14/30 TRAP ---------------------------------------------------
//
// The ccm/ov031/ov052/jrb convention. 13/14 are ActorBase::Virtual34/Virtual38,
// two u32 arguments each and bodies not linked into the port at all; 30 is
// Actor::OnAimedAtWithEggReturnVec, an SRET method no __fastcall thunk shape
// here models.
#include <cstdio>
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

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

/* the two the transcribed D1 calls, both already linked */
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN5ActorD2Ev(void *self);

/* the class's own bodies (port/slice_w15b.txt) */
int _ZN21MegaMushroomCreateTag13InitResourcesEv(char *self);   /* slot 0  */
int _ZN21MegaMushroomCreateTag16CleanupResourcesEv(void);      /* slot 3  */
int *_ZN21MegaMushroomCreateTagD0Ev(int *self);                /* slot 17 */
void *MegaMushroomCreateTag_Spawn(void);
void *MegaMushroomTag_Spawn(void);

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */

DSSTATE_BEGIN
void *_ZTV21MegaMushroomCreateTag[31];
DSSTATE_END
}

/* The RTTI spelling the D0 restores the table by. Both names are excluded from
   the ov002 mount, so this LHS is undefined everywhere and the alias cannot be
   defeated. */
#pragma comment(linker, "/alternatename:__ZTV16daObjKinokoTag_c=__ZTV21MegaMushroomCreateTag")

/* Behavior is a real C++ method in src (its TU compiles as C++ against a local
   shadow class), so it needs the extern-"C" face the vtable names. The other
   four own bodies are extern-C in src already. */
#include "MegaMushroomCreateTag.h"
extern "C" int _ZN21MegaMushroomCreateTag8BehaviorEv(void *self)
{ return ((MegaMushroomCreateTag *)self)
             ->MegaMushroomCreateTag::Behavior(); }

// ---- the trap --------------------------------------------------------------
static void kt_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: KinokoTag vtable slot %d is not hosted (actor id "
                 "%u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "unhosted KinokoTag vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define KT_TRAP(n) \
    static int __fastcall kt_trap##n(void *s, void *) \
    { kt_trap_report(s, n); return 0; }
KT_TRAP(13) KT_TRAP(14) KT_TRAP(30)
#undef KT_TRAP

static int __fastcall kt_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall kt_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall kt_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall kt_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall kt_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall kt_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall kt_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall kt_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall kt_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall kt_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall kt_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall kt_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall kt_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall kt_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall kt_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall kt_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall kt_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall kt_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall kt_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall kt_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall kt_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall kt_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* the class's own six */
static int __fastcall kt_init(void *s, void *)
{ return _ZN21MegaMushroomCreateTag13InitResourcesEv((char *)s); }
static int __fastcall kt_clean(void *s, void *)
{ return _ZN21MegaMushroomCreateTag16CleanupResourcesEv(); }
static int __fastcall kt_behavior(void *s, void *)
{ return _ZN21MegaMushroomCreateTag8BehaviorEv(s); }
/* slot 9 is 0x02043af0, ActorBase::Render, in the ROM's own table: this class
   is a tag and has no model of its own -- no render probe, nothing to probe. */
static int __fastcall kt_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
/* Slot 16, the ROM D1 transcribed; see this file's header. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (kt_d1) spelled the same chain by hand. */
static int __fastcall kt_d0(void *s, void *)
{ return (int)(size_t)_ZN21MegaMushroomCreateTagD0Ev((int *)s); }

/* ONE fill for BOTH rows. The registry calls `if (k->fill) k->fill();` once
   per row, so this runs twice; it is stores only, no verify-then-rewrite seat
   and no sinit, which is what makes naming it from both rows safe (the
   hal_fill_one_up_mushroom_vtable lesson from lane w11). */
extern "C" void hal_fill_mega_mushroom_tag_vtable(void)
{
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV21MegaMushroomCreateTag;
    vt[0]  = (void *)kt_init;
    vt[1]  = (void *)kt_binit;
    vt[2]  = (void *)kt_ainit;
    vt[3]  = (void *)kt_clean;
    vt[4]  = (void *)kt_bclean;
    vt[5]  = (void *)kt_aclean;
    vt[6]  = (void *)kt_behavior;
    vt[7]  = (void *)kt_bbeh;
    vt[8]  = (void *)kt_abeh;
    vt[9]  = (void *)kt_render;
    vt[10] = (void *)kt_bren;
    vt[11] = (void *)kt_aren;
    vt[12] = (void *)kt_pdes;
    vt[13] = (void *)kt_trap13;
    vt[14] = (void *)kt_trap14;
    vt[15] = (void *)kt_heap;
    vt[16] = (void *)hal_cppd1_MegaMushroomCreateTag;
    vt[17] = (void *)kt_d0;
    vt[18] = (void *)kt_yoshi;
    vt[19] = (void *)kt_turn_egg;
    vt[20] = (void *)kt_v50;
    vt[21] = (void *)kt_pounded;
    vt[22] = (void *)kt_atk1;
    vt[23] = (void *)kt_atk2;
    vt[24] = (void *)kt_kicked;
    vt[25] = (void *)kt_pushed;
    vt[26] = (void *)kt_cannon;
    vt[27] = (void *)kt_mega;
    vt[28] = (void *)kt_under;
    vt[29] = (void *)kt_egg;
    vt[30] = (void *)kt_trap30;
}
