/* The CAP's (WaterfallMist / daObjMarioCap_c, actor 269) nine-state PMF table
 * seat, the SignPost_StateDispatch treatment.
 *
 * WaterfallMist::Behavior dispatches a pointer-to-member out of the object's
 * +0x3bc, which WaterfallMist::InitResources sets by calling the shared
 * func_ov002_020b7f2c(this, &table) -- the state-set helper that stores the
 * table pointer and calls its first PMF. The nine tables the cap uses are
 * data_ov002_0210df04..0210df84, each an S16 of two {function, delta} PMF
 * pairs, and they are one contiguous run in ov002's bss (0x10 apart, df04
 * through df94), so eighteen PMF pairs in address order.
 *
 * __sinit_ov002_02101064 (linked at gate 10) copies the eighteen source
 * statics at 0x0210951c..0x021095a4 into the tables, and those source words
 * are the ov002 image's own -- DS CODE ADDRESSES, the ovdata contract. This
 * seat rewrites each stored word with its host body after checking it against
 * the ROM address that body was compiled from, so a mount pointing at the
 * wrong bytes says so instead of calling into the overlay image. All eighteen
 * are the nonvirtual {function, 0} form (a complete class), so the pair is
 * just the function pointer.
 *
 * The tables are the cap's OWN -- only WaterfallMist::InitResources references
 * them (no other class shares this run) -- and all eighteen functions are
 * matched src, so there is no hole here: no state traps.
 */
#include <cstdio>
#include <cstdlib>

/* ---- the cross-language faces func_ov002_020b7e1c.cpp asks for -------------
   That state TU was recovered as C++ and calls Actor::Spawn, Actor::SetRanges
   and two SaveData statics as class methods, so MSVC emits the decorated names
   while the definitions the port already links are the Itanium ones. Spawn and
   the two SaveData methods are static (__cdecl, the ABI matches whatever the
   return type), so an alias is exact -- Spawn's existing alias in
   actor_faces_bob is the pointer-returning form (SAPAU1), this TU declared the
   int-returning form (SAH), same function. SetRanges is a __thiscall member
   against a cdecl (self-first) definition, so it needs a real face: ecx holds
   this, the four Fix12i ranges come off the stack. */
extern "C" {
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int c, int d);
}
/* SetRanges is a __thiscall member (this in ecx) against the cdecl self-first
   definition, so it is a shadow-member face -- the ChangeState treatment in
   actor_faces_bob -- not an alias: a member of a shadow struct is __thiscall,
   and the alias points the caller's decorated name at the shadow member's. */
struct ActorSetRangesFace { void SetRanges(int a, int b, int c, int d); };
void ActorSetRangesFace::SetRanges(int a, int b, int c, int d)
{ _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, a, b, c, d); }
#pragma comment(linker, "/alternatename:?SetRanges@dActor_c@@QAEXHHHH@Z=?SetRanges@ActorSetRangesFace@@QAEXHHHH@Z")
/* Spawn and the two SaveData methods are static (__cdecl), so plain aliases. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines __ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, and nothing references ?Spawn@dActor_c@@SAHIIABUVector3@@PBUVector3_16@@HH@Z, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?Spawn@dActor_c@@SAHIIABUVector3@@PBUVector3_16@@HH@Z=__ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as")
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEFEATED: the left hand side is a real definition in this link now (_ZN8SaveData16HasPlayerLostCapEv.cpp.obj), so the directive is inert and alternatename_guard fails on it. */
// #pragma comment(linker, "/alternatename:?HasPlayerLostCap@SaveData@@SAHXZ=__ZN8SaveData16HasPlayerLostCapEv")
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEFEATED: the left hand side is a real definition in this link now (_ZN8SaveData13PlayerLoseCapEv.cpp.obj), so the directive is inert and alternatename_guard fails on it. */
// #pragma comment(linker, "/alternatename:?PlayerLoseCap@SaveData@@SAXXZ=__ZN8SaveData13PlayerLoseCapEv")
/* src/_Z11UpdateAngleRssis.cpp defines the angle helper as the C++ free
   function UpdateAngle (MSVC decorates it ?UpdateAngle@@YAXAAFFHF@Z), but
   func_ov002_020b781c.c calls it by its Itanium C name. Data-free cdecl on both
   sides, so the C-decorated reference aliases onto the C++ definition. */
#pragma comment(linker, "/alternatename:__Z11UpdateAngleRssis=?UpdateAngle@@YAXAAFFHF@Z")
/* func_ov002_020b6fcc calls Player::InitMetalWario by its Itanium C name, but
   src/actors/Player.cpp is a real C++ __thiscall method
   (?InitMetalWario@Player@@QAEXXZ, this in ecx). The C caller passes self on
   the stack, so this is a cdecl->thiscall face: read self off the stack and
   call the real method with this in ecx. InitVanishLuigi's own TU is C-linkage,
   so it needs no face. */
struct PlayerMetalWario { void InitMetalWario(); };  /* the real method's class */
extern "C" void _ZN6Player14InitMetalWarioEv(void *self)
{ ((PlayerMetalWario *)self)->InitMetalWario(); }
#pragma comment(linker, "/alternatename:?InitMetalWario@PlayerMetalWario@@QAEXXZ=?InitMetalWario@Player@@QAEXXZ")
/* data_ov002_020ff480 is char** in the caller's C++ decl; the mount emits the C
   name. Data has no calling convention, so an alias is exact. */
#pragma comment(linker, "/alternatename:?data_ov002_020ff480@@3PAPADA=_data_ov002_020ff480")

extern "C" {
/* int, as every definition in src/ spells them: the cells are
   `int (C::*)()` and the state setter returns what the body returns. */
int func_ov002_020b7d94(void *); int func_ov002_020b7d6c(void *);
int func_ov002_020b7cdc(void *); int func_ov002_020b7c30(void *);
int func_ov002_020b7d58(void *); int func_ov002_020b7cec(void *);
int func_ov002_020b7b70(void *); int func_ov002_020b781c(void *);
int func_ov002_020b71f0(void *); int func_ov002_020b71e8(void *);
int func_ov002_020b76ec(void *); int func_ov002_020b74d0(void *);
int func_ov002_020b7f24(void *); int func_ov002_020b7e1c(void *);
int func_ov002_020b7330(void *); int func_ov002_020b7200(void *);
int func_ov002_020b7e08(void *); int func_ov002_020b7d9c(void *);

struct PortCapPair { unsigned fn; int delta; };
extern PortCapPair data_ov002_0210df04[];   /* the flat 18-pair run */
}  /* extern "C" */

enum { PORT_CAP_PAIRS = 18 };

/* ---- THE EIGHTEEN FACES (run linkfull, lane BOWSTAKE1; lane PMFB2's shape
   in OneUpMushroom_Behavior.cpp) ------------------------------------------
   Every reader of these cells is a pointer-to-member call, which MSVC emits
   with the receiver in ECX and nothing pushed: the state setter
   func_ov002_020b7f2c (a tail jump), the cap Behavior's per-frame dispatch,
   and the copy of that setter MSVC inlines into
   daObjMarioCap_c::OnTurnIntoEgg. The eighteen bodies are plain cdecl and
   read the receiver off the stack, so a raw body in a cell took whatever
   the caller's frame held there: the setter's own argument by luck, the
   walker's saved EDI in Behavior, and the Player (or Player+0x6a0) in
   OnTurnIntoEgg. That last one is Yoshi swallowing a character cap: the
   enter body func_ov002_020b76ec worked on the Player, faulted at +0x8a and
   the quarantine froze Yoshi. On the cartridge every one of these calls
   hands the body the cap in r0. So each cell holds a __fastcall face, and
   the face calls the body with the receiver ECX carried and returns what
   the body returns. */
#define CAP_FACE(sym)                                                         \
    static int __fastcall pmf_face_##sym(void *self, void *dead_edx)          \
    { return sym(self); }
CAP_FACE(func_ov002_020b7d94) CAP_FACE(func_ov002_020b7d6c)
CAP_FACE(func_ov002_020b7cdc) CAP_FACE(func_ov002_020b7c30)
CAP_FACE(func_ov002_020b7d58) CAP_FACE(func_ov002_020b7cec)
CAP_FACE(func_ov002_020b7b70) CAP_FACE(func_ov002_020b781c)
CAP_FACE(func_ov002_020b71f0) CAP_FACE(func_ov002_020b71e8)
CAP_FACE(func_ov002_020b76ec) CAP_FACE(func_ov002_020b74d0)
CAP_FACE(func_ov002_020b7f24) CAP_FACE(func_ov002_020b7e1c)
CAP_FACE(func_ov002_020b7330) CAP_FACE(func_ov002_020b7200)
CAP_FACE(func_ov002_020b7e08) CAP_FACE(func_ov002_020b7d9c)
#undef CAP_FACE

#define CF(sym) (void (*)(void *))pmf_face_##sym
static const struct { unsigned rom; void (*host)(void *); }
g_cap_states[PORT_CAP_PAIRS] = {
    {0x020b7d94, CF(func_ov002_020b7d94)}, {0x020b7d6c, CF(func_ov002_020b7d6c)},
    {0x020b7cdc, CF(func_ov002_020b7cdc)}, {0x020b7c30, CF(func_ov002_020b7c30)},
    {0x020b7d58, CF(func_ov002_020b7d58)}, {0x020b7cec, CF(func_ov002_020b7cec)},
    {0x020b7b70, CF(func_ov002_020b7b70)}, {0x020b781c, CF(func_ov002_020b781c)},
    {0x020b71f0, CF(func_ov002_020b71f0)}, {0x020b71e8, CF(func_ov002_020b71e8)},
    {0x020b76ec, CF(func_ov002_020b76ec)}, {0x020b74d0, CF(func_ov002_020b74d0)},
    {0x020b7f24, CF(func_ov002_020b7f24)}, {0x020b7e1c, CF(func_ov002_020b7e1c)},
    {0x020b7330, CF(func_ov002_020b7330)}, {0x020b7200, CF(func_ov002_020b7200)},
    {0x020b7e08, CF(func_ov002_020b7e08)}, {0x020b7d9c, CF(func_ov002_020b7d9c)},
};
#undef CF

extern "C" void port_cap_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    PortCapPair *p = data_ov002_0210df04;
    for (int i = 0; i < PORT_CAP_PAIRS; ++i) {
        if (p[i].fn != g_cap_states[i].rom || p[i].delta != 0) {
            std::fprintf(stderr, "FATAL: Cap state pair %d: the sinit left "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p[i].fn, p[i].delta, g_cap_states[i].rom);
            std::abort();
        }
        p[i].fn = (unsigned)(size_t)g_cap_states[i].host;
    }
}
