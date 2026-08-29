// ============================================================================
// THE NEW-FILE OPENING'S CAST -- CUTSCENE_OBJECT (actor 0x160, ov002)
// ============================================================================
//
// Run lvled, lane intro-cutscene. Its own file rather than an append to
// actor_classes.cpp: another session's VS campaign is seating ov075 actors in
// there, and one class in one file keeps the two sets apart. The registry row
// and the three declarations are the only lines this lane adds to a shared
// file (hal/actor_classes.inc).
//
// WHAT IT IS. _ZTV14CutsceneObject, ov002 0x0210bd60. ONE class that is every
// visible thing in the opening -- the script spawns it repeatedly with a
// different model each time:
//
//     kuppa cmd 0x17   the Peach letter   data/normal_obj/opening/EUR/
//                                         peach_letter_US.bmd
//     kuppa cmd 0x12   the Lakitu flight  data/enemy/c_jugem/c_jugem.bmd
//     kuppa cmd 0x13   Peach
//     kuppa cmd 0x14/0x15/0x16            the three warp pipes
//     kuppa cmd 0x19   the cloud
//
// It was fully matched and offered to no slice, so with the intro gate opened
// the very first thing the opening did was decline its whole cast:
//     [spawn-declined] actor 0x160 not registered
// Now offered through port/slice_intro.txt.
//
// SLOTS 16/17 ARE HOST THUNKS, and that is the gate-17 reading the Cannon note
// and the HauntedChair fill both give. Two separate reasons here:
//
//   D1: src/_ZN14CutsceneObjectD1Ev.cpp is a real C++ destructor written over a
//       SHADOW hierarchy (`struct Actor { virtual ~Actor(); };`), so compiling
//       it would have MSVC synthesise a base-destructor call against a layout
//       that is not the ROM's. The ROM's D1 body is empty apart from the vptr
//       store and the Actor chain, which is exactly what co_d1 does.
//
//   D0: src/_ZN14CutsceneObjectD0Ev.c is a weaker recovery than its siblings --
//       it reaches the vtable and the heap through decl_common.h's GENERIC
//       placeholders (`extern int VT[];`, `extern void *HEAP;`) rather than
//       through the real symbols the way LakituBro's D0 does
//       (_ZTV11daC_Jugem_c and data_020a0eac by name). Those placeholders do
//       resolve -- actor_vtables.cpp and cxxname_bridge.cpp define them -- so
//       it would have LINKED and stored the wrong vtable pointer into every
//       destroyed cutscene object, and freed to whatever heap HEAP happens to
//       be. co_d0 writes the ROM's own two symbols instead.
//
//       data_020a0eac is not a guess: src/_ZN9ActorBasenwEj.cpp allocates every
//       actor out of it, and CutsceneObject_Spawn is an ActorBase::operator new
//       of 260 bytes, so it is the heap this object came from.
//
// Neither D-source is listed in slice_intro.txt; nothing else references either
// symbol (the ROM's D0 inlines the chain rather than calling D1).
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "CutsceneObject.h"
#include "Model.h"   /* the model class the vtable seat below describes */

extern "C" {
/* the shared lifecycle halves, the same functions every fill in this tree
   writes -- see hal/actor_classes_bbh.cpp for the slot map */
int _ZN5Actor19BeforeInitResourcesEv(void *self);             /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                  /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                    /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                   /* slot 18 */
int _ZN5Actor9Virtual50Ev(void *self);                        /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *s, void *o);   /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *s, void *p);       /* 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *s, void *o);        /* 28 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *s, void *p);         /* 19 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* 29 */

/* the class's own, all from port/slice_intro.txt */
int _ZN14CutsceneObject13InitResourcesEv(void *self);     /* face, below */
int _ZN14CutsceneObject8BehaviorEv(void *self);           /* face, below */
int _ZN14CutsceneObject6RenderEv(void *self);             /* face, below */
int _ZN14CutsceneObject16CleanupResourcesEv(void *self);  /* face, below */
void _ZN14CutsceneObject16OnPendingDestroyEv(void);       /* C linkage in src */
void *_ZTV14CutsceneObject[31];

/* the Actor chain co_d1/co_d0 end on, and the heap every actor is allocated
   from (src/_ZN9ActorBasenwEj.cpp) */
void _ZN5ActorD2Ev(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);
extern void *data_020a0eac;

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);            /* actor_registry */
void port_actor_slot_decline(const char *what);            /* func_02043fdc */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */
}

// ---- the trap --------------------------------------------------------------
static void co_trap_report(void *self, int slot)
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
#define CO_TRAP(n) \
    static int __fastcall co_trap##n(void *s, void *) \
    { co_trap_report(s, n); return 0; }
CO_TRAP(13) CO_TRAP(14) CO_TRAP(30)
#undef CO_TRAP

static int __fastcall co_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall co_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall co_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5/8/11 are ARM tail-call veneers on the ROM; call the target directly
   so the argument riding in r1 is not dropped. */
static void __fastcall co_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall co_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall co_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall co_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall co_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall co_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall co_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall co_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall co_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall co_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall co_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall co_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall co_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall co_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall co_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall co_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall co_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall co_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

// ---- the class's own six ---------------------------------------------------
static int __fastcall co_init(void *s, void *)
{ return _ZN14CutsceneObject13InitResourcesEv(s); }
static int __fastcall co_clean(void *s, void *)
{ return _ZN14CutsceneObject16CleanupResourcesEv(s); }
static int __fastcall co_behavior(void *s, void *)
{ return _ZN14CutsceneObject8BehaviorEv(s); }
static int __fastcall co_render(void *s, void *)
{ port_actor_render_probe("CUTSCENE_OBJECT", (char *)s + 0xdc);
  return _ZN14CutsceneObject6RenderEv(s); }
static int __fastcall co_pdes(void *, void *)
{ _ZN14CutsceneObject16OnPendingDestroyEv(); return 0; }

/* Slot 16, the complete-object destructor: the ROM's D0 chain WITHOUT the
   Memory::Deallocate at its tail, because the caller of slot 16
   (ActorBase::AfterCleanupResources) deallocates itself. CutsceneObject adds no
   members of its own that need destroying -- its D1 body is empty -- so this is
   the vptr store and the Actor chain and nothing else. */
static int __fastcall co_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV14CutsceneObject;
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
/* Slot 17, the deleting destructor: the same chain plus the free, to the heap
   ActorBase::operator new took it from. */
static int __fastcall co_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV14CutsceneObject;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

/* ---- A VTABLE SLOT THE SCRIPT'S OWN MODEL CLASS CALLS THROUGH --------------
   data_ov002_0210bae4 is a VTABLE, not a spawn record -- an earlier version of
   this comment called it a factory and that was wrong. Reading the body settles
   it: src/func_ov002_020f69a8.cpp stores data_ov002_0210bae4 into *this, calls
   SharedFilePtr::Release, then Model::~Model, __destroy_arr and
   Memory::operator_delete2. That is a DELETING DESTRUCTOR (a D0), and
   config/arm9/overlays/ov002/relocs.txt puts it in slot 1:
       from:0x0210bae8 kind:load to:0x020f69a8 module:overlay(2)

   The mount carries the ROM's bytes, which means the DS address, and the class
   is reached from the opening through
     ... Actor::Spawn -> func_02043098 -> CutsceneObject::InitResources
     -> func_ov002_020f6960 -> ModelBase::SetFile -> [this slot]
   so a DS address here is a jump into unmapped memory. Offering
   src/func_ov002_020f69a8.cpp put the body in the binary but did NOTHING about
   the pointer, which is the same lesson romdata.py's header records for
   data_020876e4.

   Seated here because this fill is what the class registry runs before any
   spawn can happen, and checked against the ROM's own address first so a mount
   that ever changes shape says so instead of calling whatever it now holds.

   THIS SLOT IS CORRECT AND THE CLASS IS STILL NOT WHOLE: only slot 1 is seated,
   because only slot 1 is what the ROM's relocation names. The class's remaining
   slots are the open floor this lane reports rather than guesses at. */
extern "C" {
/* Addresses only -- the seat below stores them, it never calls them, so the
   arity here is irrelevant to correctness. The three Model faces are DEFINED at
   the foot of this file with their real signatures; these are just the
   addresses the vtable words get. */
void func_ov002_020f6a00(void);
void func_ov002_020f69a8(void);
void _ZN5Model6RenderEPK7Vector3(void);
extern unsigned data_ov002_0210bae4[];
}

/* The SIX CODE SLOTS, in the ROM's own order, each paired with the address
   config/arm9/overlays/ov002/relocs.txt says belongs there. Slots +0x18 and
   +0x1c are DATA (0x0209a754 and 0x0210ba44) and are left exactly as the mount
   carries them -- this seat only rewrites words that are called. */
extern "C" {
int _ZN5Model9DoSetFileEPcii(void *, char *, int, int);
void _ZN5Model11UpdateVertsEv(void *);
void _ZN5Model9Virtual10ER9Matrix4x3(void *, Matrix4x3 &);
}

static const struct { unsigned rom; void *host; const char *what; }
g_co_vt[6] = {
    { 0x020f6a00, (void *)&func_ov002_020f6a00,            "+0x00" },
    { 0x020f69a8, (void *)&func_ov002_020f69a8,            "+0x04 (the deleting dtor)" },
    { 0x02016bf8, (void *)&_ZN5Model9DoSetFileEPcii,       "+0x08 Model::DoSetFile" },
    { 0x02016c98, (void *)&_ZN5Model11UpdateVertsEv,       "+0x0c Model::UpdateVerts" },
    { 0x02016bb8, (void *)&_ZN5Model9Virtual10ER9Matrix4x3,"+0x10 Model::Virtual10" },
    { 0x02016b78, (void *)&_ZN5Model6RenderEPK7Vector3,    "+0x14 Model::Render" },
};

static void co_seat_model_vtable(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 6; ++i) {
        if (data_ov002_0210bae4[i] != g_co_vt[i].rom) {
            std::fprintf(stderr, "FATAL: CUTSCENE_OBJECT model vtable slot %s: "
                         "the mount holds %08x, the ROM's own reloc says %08x "
                         "-- WRONG BYTES\n", g_co_vt[i].what,
                         data_ov002_0210bae4[i], g_co_vt[i].rom);
            std::abort();
        }
        data_ov002_0210bae4[i] = (unsigned)(size_t)g_co_vt[i].host;
        (void)0;
    }
}

extern "C" void hal_fill_cutscene_object_vtable(void)
{
    co_seat_model_vtable();
    void **vt = _ZTV14CutsceneObject;
    vt[1] = (void *)co_binit;
    vt[2] = (void *)co_ainit;
    vt[4] = (void *)co_bclean;
    vt[5] = (void *)co_aclean;
    vt[7] = (void *)co_bbeh;
    vt[8] = (void *)co_abeh;
    vt[10] = (void *)co_bren;
    vt[11] = (void *)co_aren;
    vt[13] = (void *)co_trap13;
    vt[14] = (void *)co_trap14;
    vt[15] = (void *)co_heap;
    vt[18] = (void *)co_yoshi;
    vt[19] = (void *)co_turn_egg;
    vt[20] = (void *)co_v50;
    vt[21] = (void *)co_pounded;
    vt[22] = (void *)co_atk1;
    vt[23] = (void *)co_atk2;
    vt[24] = (void *)co_kicked;
    vt[25] = (void *)co_pushed;
    vt[26] = (void *)co_cannon;
    vt[27] = (void *)co_mega;
    vt[28] = (void *)co_under;
    vt[29] = (void *)co_aimed;
    vt[30] = (void *)co_trap30;
    /* the class's own six */
    vt[0] = (void *)co_init;
    vt[3] = (void *)co_clean;
    vt[6] = (void *)co_behavior;
    vt[9] = (void *)co_render;
    vt[12] = (void *)co_pdes;
    vt[16] = (void *)co_d1;
    vt[17] = (void *)co_d0;
}

// ---- method faces ----------------------------------------------------------
// InitResources, Behavior, Render and CleanupResources are real MSVC members
// against include/CutsceneObject.h; the vtable thunks spell them by Itanium
// name. Each is a real __thiscall method, so a linker alias would hand the body
// an ecx that never held `this` -- the face bridges cdecl to __thiscall.
// OnPendingDestroy is already C linkage in its .c src, so it gets no face.
extern "C" {
int _ZN14CutsceneObject13InitResourcesEv(void *self)
{ return ((CutsceneObject *)self)->CutsceneObject::InitResources(); }
int _ZN14CutsceneObject8BehaviorEv(void *self)
{ return ((CutsceneObject *)self)->CutsceneObject::Behavior(); }
int _ZN14CutsceneObject6RenderEv(void *self)
{ return ((CutsceneObject *)self)->CutsceneObject::Render(); }
int _ZN14CutsceneObject16CleanupResourcesEv(void *self)
{ return ((CutsceneObject *)self)->CutsceneObject::CleanupResources(); }
}

// ---- Model's three vtable methods, faced -----------------------------------
// The model class data_ov002_0210bae4 describes reaches slots 2, 3 and 4 through
// Model's own methods. Their src TUs define real MSVC __thiscall members
// (Model::DoSetFile, Model::UpdateVerts, Model::Virtual10, declared virtual in
// include/Model.h at exactly those slots), while the vtable seat above spells
// them by Itanium name. A linker alias would hand each body an ecx that never
// held `this`, so these are faces, not aliases -- the same reason
// hal/actor_classes_bbh.cpp faces HauntedChair's three.
extern "C" {
int _ZN5Model9DoSetFileEPcii(void *self, char *file, int a, int b)
{ return ((Model *)self)->Model::DoSetFile(file, a, b); }
void _ZN5Model11UpdateVertsEv(void *self)
{ ((Model *)self)->Model::UpdateVerts(); }
void _ZN5Model9Virtual10ER9Matrix4x3(void *self, Matrix4x3 &mat)
{ ((Model *)self)->Model::Virtual10(mat); }
}

// ---- the C++/C linkage bridges ---------------------------------------------
// Three of CutsceneObject's TUs declare a shared symbol OUTSIDE their extern "C"
// block, so MSVC mangles the reference as a C++ name while the one real symbol
// is defined with C linkage (romdata / the ov002 and ov085 mounts). Point the
// mangled spellings at the C symbols -- the same "propagate config renames BY
// ADDRESS" hazard applied to a type, and the precedent is the data_02082128
// bridge at the foot of hal/actor_classes_bbh.cpp.
#pragma comment(linker, "/alternatename:?_Znwj@@YAPAXH@Z=__Znwj")
#pragma comment(linker, "/alternatename:?data_ov085_0213074c@@3DA=_data_ov085_0213074c")
#pragma comment(linker, "/alternatename:?data_0209b41c@@3UM48@@A=_data_0209b41c")

// func_02113c20 IS NOT A FUNCTION. CutsceneObject::InitResources takes its
// ADDRESS (`&func_02113c20`) and hands it to func_ov002_020f6618 as data; the
// recovery named an overlay-ambiguous address with the func_ prefix. Every
// overlay that reaches 0x02113c20 declares it as DATA -- data_ov009_02113c20 is
// bss in ov009, which is the overlay LEVEL 1 mounts and therefore the one the
// opening runs against. Bridge the misnamed spelling to the real ov009 symbol
// rather than editing src/, and note that this alias is only correct because
// the opening is a level-1 cutscene.
#pragma comment(linker, "/alternatename:_func_02113c20=_data_ov009_02113c20")

// Two more of the same C++/C linkage shape, both named by the linker rather
// than guessed. OAM::RenderSub is ALREADY HOSTED -- src/_ZN3OAM9RenderSubEP7Oam
// Attrii.c rides in through slice_gate25 -- so this is only the mangled
// spelling CutsceneObject::Render reaches it by.
#pragma comment(linker, "/alternatename:?RenderSub@OAM@@SAXPAUOamAttr@@HH@Z=__ZN3OAM9RenderSubEP7OamAttrii")
#pragma comment(linker, "/alternatename:?data_ov002_0210bddc@@3UOamAttr@@A=_data_ov002_0210bddc")
