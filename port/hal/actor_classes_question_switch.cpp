// GATE 212: QUESTION_SWITCH (26), ov002.
//
// _ZTV14QuestionSwitch, ov002 0x02108e5c (RTTI daObjHatenaSwitch_c). The big
// "?" switch the player ground-pounds or lands on to flip a level-wide bit
// (data_0209caa0[1] bit 31, the pressed flag), then a one-line message. A
// Platform subclass on the 32-slot Platform table (slot 31 is Platform::Kill),
// re-derived on this tree, not taken from the brief:
//     python port/tools/vtspan.py . _ZTV14QuestionSwitch  ->  32, Platform subclass
// Every route agrees on 32 and every one of the 32 words resolves. The class
// owns slots 0 InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 16 D1,
// 17 D0 and 21 OnGroundPounded (func_ov002_020b4fc4); slot 12 stays
// ActorBase::OnPendingDestroy and 31 stays Platform::Kill. The Behavior is a
// plain state walk with no pointer-to-member, so no state table and no host
// state seat. Full derivation and the id cross-check are in
// port/slice_gate212.txt.
//
// SLOT 21 IS THE ONE MARKER BODY. src/func_ov002_020b4fc4.c carries dsd's
// "recovered from vtable slot identity" line, so inferred_stub_guard refuses it
// unless it has a ROM ruling. It was byte-matched FIRST (tools/match.py,
// --module ov002, 3 of 3 words: mov r1,#0; strb r1,[r0,#0x718]; bx lr) and is
// recorded REAL_DECOMP in port/tools/inferred_stub_adjudicated.txt. Its effect
// is to zero the +0x718 countdown so a ground pound flips the switch on the
// same frame the landing edge (+0x71a, set by the collider callback
// func_ov002_020b56c4 -> _567c) arrives.
//
// THE LAW, same as every sibling fill: ROM slot order, host __fastcall thunks
// that bridge cdecl/__thiscall to the matched bodies, unhosted slots trapped
// by name. Slots 13/14/30 are ActorBase::Virtual34/38 and the SRET
// Actor::OnAimedAtWithEggReturnVec, the trio every Actor-shaped fill traps.
//
// SLOT 9 IS A HOST COPY. The matched Render dispatches slot 5 of a six-virtual
// local shadow over the ModelAnim at 0x6b4: ModelAnim::Render(0) in ROM
// numbering, Virtual18(matrix, scale) in the host _ZTV9ModelAnim (MSVC
// numbering spends one slot on the destructor). Compiled from src/ it faulted
// in Model::Virtual10 with a null matrix on the first drawn frame, so the
// one-line body lives in unmatched/ModelAnim_Renders.cpp (the QuestionBlock
// case) and the matched TU stays off the slice.
//
// Both destructors are matched extern-C bodies (D1 is a .cpp with an extern
// "C" block, D0 is .c); they store _ZTV14QuestionSwitch then _ZTV8Platform by
// hand and call the member dtors as C symbols, so both are enrolled and the
// two slots call them directly. hal_fill_platform_vtable() runs first so the
// _ZTV8Platform they store mid-chain is the filled host table (the
// BlueCoinSwitch precedent).

#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "QuestionSwitch.h"

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
/* the shared lifecycle halves, the same arm9 bodies every sibling fill writes */
int  _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int  _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int  _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int  _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int  _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
extern int data_0209caa0[];          /* the level flag words; [1] bit 31 is "pressed" */
extern int data_ov002_0210dd60[];    /* the model's SharedFilePtr cell (probe only) */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the class's own matched bodies, all on slice_gate212.txt */
int  _ZN14QuestionSwitch8BehaviorEv(char *self);   /* slot 6, extern-C explicit this */
int  _ZN14QuestionSwitch6RenderEv(void *self);      /* slot 9, HOST COPY in unmatched/ModelAnim_Renders.cpp */
char *_ZN14QuestionSwitchD1Ev(struct QuestionSwitch *self); /* slot 16, extern-C */
int *_ZN14QuestionSwitchD0Ev(struct QuestionSwitch *self);  /* slot 17, .c */
void func_ov002_020b4fc4(char *self);               /* slot 21, OnGroundPounded */

/* The array the ROM factory installs: QuestionSwitch_Spawn does
   `p[0] = (int)_ZTV14QuestionSwitch`. Defined here, not just declared: the
   `int` type and C linkage match `extern int _ZTV14QuestionSwitch[]` in
   include/decl_common.h that the factory and both destructors read. */
int _ZTV14QuestionSwitch[32];
}

/* The class's .cpp bodies reach data under C++ mangled names; bridge each to
   the C mount symbol. InitResources declares the four SharedFilePtr cells as a
   plain `extern int` outside its extern "C" block (@@3HA), CleanupResources as
   `extern int []` (@@3PAHA); Behavior reads the level flag words as
   `extern u32 []` (@@3PAIA). MSVC folds each onto the one C symbol. */
#pragma comment(linker, "/alternatename:?data_ov002_0210dd50@@3HA=_data_ov002_0210dd50")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd58@@3HA=_data_ov002_0210dd58")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd60@@3HA=_data_ov002_0210dd60")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd68@@3HA=_data_ov002_0210dd68")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd50@@3PAHA=_data_ov002_0210dd50")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd58@@3PAHA=_data_ov002_0210dd58")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd60@@3PAHA=_data_ov002_0210dd60")
#pragma comment(linker, "/alternatename:?data_ov002_0210dd68@@3PAHA=_data_ov002_0210dd68")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3PAIA=_data_0209caa0")

// ---- the trap --------------------------------------------------------------
static void qs_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define QS_TRAP(n) \
    static int __fastcall qs_trap##n(void *s, void *) \
    { qs_trap_report(s, n); return 0; }
QS_TRAP(13) QS_TRAP(14)
#undef QS_TRAP

// ---- the shared half (a Platform table's Actor-shaped middle) --------------
static int  __fastcall qs_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall qs_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int  __fastcall qs_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall qs_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int  __fastcall qs_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall qs_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int  __fastcall qs_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall qs_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int  __fastcall qs_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int  __fastcall qs_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int  __fastcall qs_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int  __fastcall qs_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int  __fastcall qs_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int  __fastcall qs_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int  __fastcall qs_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int  __fastcall qs_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int  __fastcall qs_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int  __fastcall qs_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int  __fastcall qs_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int  __fastcall qs_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int  __fastcall qs_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int  __fastcall qs_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ---- the class's own slots -------------------------------------------------
/* SM64DS_QS_PROBE=1: one line per Behavior tick, so a headless run shows the
   switch DOING something, not merely surviving a census (the Klepto lesson).
   Prints the position, the countdown (+0x718), the landing edge (+0x71a), the
   talk latch (+0x71b), which collider is live (up = the raised mesh at +0x324,
   down = the pressed mesh at +0x4ec) and the level's pressed bit. Off by
   default, prints nothing. */
static void qs_probe(char *c)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_QS_PROBE") != 0;
    if (!on) return;
    std::printf("[qsprobe] %p pos (%d,%d,%d) count %d landed %d talk %d "
                "collider %s pressed %d anim %d\n", (void *)c,
                *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12,
                *(int *)(c + 0x64) >> 12,
                (int)*(signed char *)(c + 0x718), (int)*(unsigned char *)(c + 0x71a),
                (int)*(unsigned char *)(c + 0x71b),
                *(char **)(c + 0x320) == c + 0x324 ? "up" : "down",
                (data_0209caa0[1] & 0x80000000) ? 1 : 0,
                *(int *)(c + 0x70c) >> 12);
    std::fflush(stdout);
}

extern "C" void port_actor_render_probe(const char *cls, void *model);

static int __fastcall qs_init(void *s, void *)
{ return ((QuestionSwitch *)s)->QuestionSwitch::InitResources(); }
static int __fastcall qs_clean(void *s, void *)
{ return ((QuestionSwitch *)s)->QuestionSwitch::CleanupResources(); }
static int __fastcall qs_behavior(void *s, void *)
{ qs_probe((char *)s); return _ZN14QuestionSwitch8BehaviorEv((char *)s); }
static int __fastcall qs_render(void *s, void *)
{
    port_actor_render_probe("QUESTION_SWITCH", (char *)s + 0x6b4);
    if (std::getenv("SM64DS_QS_PROBE")) {
        const char *m = (const char *)s + 0x6b4;
        std::fprintf(stderr, "[qsprobe] render model %p file %p transforms %p "
                     "cell dd60 {%08x %08x}\n",
                     (const void *)m, *(void *const *)(m + 0x0c),
                     *(void *const *)(m + 0x14),
                     data_ov002_0210dd60[0], data_ov002_0210dd60[1]);
    }
    return _ZN14QuestionSwitch6RenderEv(s);
}
static int __fastcall qs_d1(void *s, void *)
{ return (int)(size_t)_ZN14QuestionSwitchD1Ev((struct QuestionSwitch *)s); }
static int __fastcall qs_d0(void *s, void *)
{ return (int)(size_t)_ZN14QuestionSwitchD0Ev((struct QuestionSwitch *)s); }
/* slot 21, OnGroundPounded(Actor&): the matched body ignores the other actor */
static int __fastcall qs_pounded(void *s, void *, void *)
{ func_ov002_020b4fc4((char *)s); return 0; }

extern "C" void hal_fill_question_switch_vtable(void)
{
    void **vt = (void **)_ZTV14QuestionSwitch;
    hal_fill_platform_vtable();
    vt[0]  = (void *)qs_init;
    vt[1]  = (void *)qs_binit;
    vt[2]  = (void *)qs_ainit;
    vt[3]  = (void *)qs_clean;
    vt[4]  = (void *)qs_bclean;
    vt[5]  = (void *)qs_aclean;
    vt[6]  = (void *)qs_behavior;
    vt[7]  = (void *)qs_bbeh;
    vt[8]  = (void *)qs_abeh;
    vt[9]  = (void *)qs_render;
    vt[10] = (void *)qs_bren;
    vt[11] = (void *)qs_aren;
    vt[12] = (void *)qs_pdes;
    vt[13] = (void *)qs_trap13;
    vt[14] = (void *)qs_trap14;
    vt[15] = (void *)qs_heap;
    vt[16] = (void *)qs_d1;
    vt[17] = (void *)qs_d0;
    vt[18] = (void *)qs_yoshi;
    vt[19] = (void *)qs_egg;
    vt[20] = (void *)qs_v50;
    vt[21] = (void *)qs_pounded;
    vt[22] = (void *)qs_atk1;
    vt[23] = (void *)qs_atk2;
    vt[24] = (void *)qs_kicked;
    vt[25] = (void *)qs_pushed;
    vt[26] = (void *)qs_cannon;
    vt[27] = (void *)qs_mega;
    vt[28] = (void *)qs_under;
    vt[29] = (void *)qs_aimed;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)qs_kill;
}
