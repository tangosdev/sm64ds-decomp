// PORT_HOST_ABI. dScMgMemory2_c's own faces and its one named trap.
// Run mg6, lane MEM.
//
// Everything here was named by a LINK of the seat, not predicted. Wave 1
// produced 41 unresolved externals on walk_window, and the same set on
// walk_window_hires and smoke_player. port/tools/facegen.py classified them
// (with an ABSOLUTE --out, which is finding 2 of port/mg_fanout_costs.txt
// section 10 -- a relative one FALSE-FAILS) and this file carries the rows the
// classification and the hand rulings landed on. The rest were slice lines and
// two host copies; port/slice_mem.txt says which.
//
//   19  ALIAS      mechanical, generated, copied in below
//    1  FACE       BlendModelAnim::SetAnim, a __thiscall method mangle
//    1  FACE       SysTracker::~SysTracker, a __thiscall destructor mangle
//   10  REFUSED    ruled by hand below; nine become aliases, one is the trap
//    9  UNKNOWN    six are slice lines, three are the g0/g1/g2 host copy
//
// WAVE 3 ADDED TWO MORE and neither was predictable from wave 1: one more P2
// pair (0x0213ac48) and one more arm9 slice line (src/func_02045ec4.c), each
// the closure of something wave 2 brought in. Waves converge here the way
// port/mg_fanout_costs.txt section 10 says they do, 41 then 2 then 0.
//
// ---- 1. THE NINE HAND RULINGS ---------------------------------------------
//
// facegen refuses a struct-typed global on purpose, because a struct wrapper
// hides a member pointer -- ?data_ov006_02141950@@3PAUEntry@@A was
// dScMgCurling_c's twenty-five-slot state table and slipped both of its guards
// (finding 1 of section 10). So every one of these was read at its consumer
// before it became an alias, and the test is the rulebook corollary section 4
// states: A PAIR WHOSE CONSUMER SPELLS IT AS TWO INTS IS SAFE AS AN ALIAS; A
// PAIR WHOSE CONSUMER NAMES A MEMBER-POINTER TYPE NEEDS A HOST COPY.
//
//   ?data_ov006_0213ac48@@3UP2@@A   src/func_ov006_020c1764.cpp declares
//   ?data_ov006_0213ac50@@3UP2@@A   `struct P2 { int w[2]; };` and
//   ?data_ov006_0213ac88@@3UP2@@A   src/func_ov006_020c0264.cpp and
//                                   src/func_ov006_020c1420.cpp the same, and
//                                   copies each with `*(P2 *)c = ...`. ALL
//                                   THREE ARE REAL mwcc PAIRS -- {0x020c14bc,
//                                   0}, {0x020c11c0, 0} and {0x020c0364, 0} in
//                                   the image -- and all three are SAFE as
//                                   aliases for exactly the reason the
//                                   corollary gives: two ints is eight bytes on
//                                   both machines, so the copy moves what the
//                                   ROM moves. What consumes the pair
//                                   afterwards is the field dispatcher in
//                                   unmatched/MgMemory2_FieldPmf.cpp, which is
//                                   host-copied, and all three code words are
//                                   routed in its switch.
//                                   0x0213ac48 ARRIVED IN WAVE 3 and is the
//                                   correction to that file's pair-run range,
//                                   which had been written 0x0213ac50 up.
//   ?data_ov004_020beb68@@3PAUG@@A  src/func_ov004_020b6234.cpp declares
//                                   `struct G { int v; };` and reads `g->v`.
//                                   A pointer to a one-int struct; the global
//                                   is dScMgBase_c's own "current minigame
//                                   scene" pointer, which
//                                   src/func_ov004_020b2adc.c writes.
//   ?func_020beb68@@3PAUV@@A        THE SAME GLOBAL under a name that exists in
//                                   no config, and at C++ linkage on top of it.
//                                   src/func_ov006_020c14bc.cpp declares
//                                   `extern V* func_020beb68;` where V is a
//                                   22-virtual dummy class -- a POINTER, not a
//                                   pair. Same defect class as
//                                   src/func_ov006_020e3578.c's bare
//                                   `func_020adc74`, which
//                                   port/mg_fanout_costs.txt section 6 records
//                                   and hal/scene_mg_faces.cpp already aliases,
//                                   and the same one MgCoin_Faces.cpp carries
//                                   as the plain-C spelling.
//   ?data_020a0ebc@@3UVector3@@A    src/func_ov006_020c092c.cpp reads .x/.y/.z
//   ?data_020a0edc@@3UVector3_16@@A off both and copies them into the object at
//                                   +0xc8 and +0xe8. Three ints and three
//                                   shorts of plain arm9 data; no pointer of
//                                   any kind.
//   ?data_ov004_020bfa20@@3P6AXXZA  a FUNCTION-POINTER global, which facegen
//                                   refuses because P6AXXZA is outside its
//                                   proven scalar subset. Ruled by reading its
//                                   writers: ten src TUs assign it, and every
//                                   one assigns a HOST function by name
//                                   (`data_ov004_020bfa20 = (void*)func_ov004_
//                                   020b5368;`) or zero. No DS address ever
//                                   reaches it, so func_ov004_020b65e4's
//                                   `if (p) p();` calls a host address. The
//                                   same global also arrives spelled @@3HA and
//                                   @@3PAXA from two other TUs; all three
//                                   alias onto the one mount definition.
//   ?LoadBGPltt@GXS@@YAXPBXII@Z     src/func_ov006_0210a534.cpp declares
//                                   `namespace GXS { void LoadBGPltt(const
//                                   void*, u32, u32); }` while the matched TU
//                                   src/_ZN3GXS10LoadBGPlttEPKvjj.c defines the
//                                   plain C name with the identical three
//                                   arguments. Both __cdecl, so a plain alias
//                                   is right and facegen's refusal is the right
//                                   OUTCOME for the wrong reason -- finding 4
//                                   of section 10, on Memory::Allocate.
//   ?PlayBank2_2D@Sound@@SAXI@Z     the STATIC-MEMBER spelling of
//                                   _ZN5Sound12PlayBank2_2DEj, which four TUs
//                                   in this slice reach. Both __cdecl, one
//                                   unsigned argument. hal/scene_mg_flower.cpp
//                                   already aliases the same target from a
//                                   third spelling (_func_02012754).
//   ?RenderAll@ShadowModel@@YAXXZ   NOT a missing body. src/_ZN11ShadowModel9-
//                                   RenderAllEv.cpp is already in the build
//                                   through port/slice_w5d.txt and defines
//                                   ?RenderAll@ShadowModel@@SAXXZ, because
//                                   include/ShadowModel.h makes it a STATIC
//                                   MEMBER. src/func_ov006_020c1804.cpp
//                                   declares it as a NAMESPACE function, so the
//                                   two mangles differ by S versus Y and
//                                   nothing else: same __cdecl, same void(void).
//
// ---- 2. THE TWO METHOD FACES ----------------------------------------------
//
// An alias cannot change a calling convention, which is the ruling
// port/mg_fanout_costs.txt section 4 states for Scene::AfterInitResources.
// Both rows below are __thiscall METHOD mangles whose bodies are __cdecl C
// functions, so each needs a real definition that lands `this`.
//
//   BlendModelAnim::SetAnim  generated by facegen and copied in verbatim.
//     Three TUs in this slice call it as a method (func_ov006_020c0264,
//     _020c0d68, _020c1764) while src/_ZN14BlendModelAnim7SetAnimER8BCA_-
//     Fileii5Fix12IiEt is the plain C name.
//     THE PLAUSIBLE-SIBLING CHECK method_faces.cpp's item (a) asks for was run
//     and it MATTERS here: hal/bob_enemy_bridges.cpp already defines a
//     BlendModelAnim::SetAnim with the same five parameters and an INT return,
//     which mangles ?SetAnim@BlendModelAnim@@QAEHAAUBCA_File@@HHHG@Z. This
//     class's callers spell it void, so the mangle is the X form and the two do
//     not collide -- the baseline map carries the H form and the wave-1 link
//     named the X form unresolved beside it, which is the check passing rather
//     than an argument that it would.
//   SysTracker::~SysTracker  src/func_ov006_020f5564.cpp -- vtable slot 16, D2
//     -- declares a LOCAL `struct SysTracker { ~SysTracker(); char pad[4]; };`
//     and calls `((SysTracker*)(c + 0x471c))->~SysTracker()`. MSVC emits ??1
//     and the scalar-deleting ??_G that references it. Its sibling
//     src/func_ov006_020f55b8.cpp (slot 17, D0) spells the SAME call as the C
//     name _ZN8Particle10SysTrackerD1Ev, which is why only one of the two
//     destructors produced a symbol. The face is one line and lands `this`.
//
// ---- 3. THE TRAP THAT USED TO BE HERE, AND WHAT REPLACED IT ----------------
//
// func_ov006_020f5b98 is the sixth call vtable slot 9 (Render) makes:
// src/func_ov006_020f73f4.c line 16 calls it on `this`.
// config/arm9/overlays/ov006/symbols.txt names it and sizes it 0xa8, there is
// NO delink block covering it -- the block before it ends exactly at
// 0x020f5b98 and the next starts after -- and, until run mg7 lane MEMCARDS,
// no src file in either extension.
// src/func_ov002_020f5b98.c exists and is a DIFFERENT function: ov002 and ov006
// share an address window, which is the module-residency trap
// port/slice_ccn.txt names, and that file's body takes a `struct Ent *` and
// belongs to another overlay entirely. IT IS STILL NOT A SUBSTITUTE and must
// never be the file that satisfies this symbol.
//
// The trap that stood here counted itself and returned 0, which is the honest
// thing for a body nobody has read. Reading it out of the overlay image ended
// that: it is the ONLY code in dScMgMemory2_c that draws a card, and with it
// stubbed the whole minigame deals sixteen cards into an empty green table.
// src/func_ov006_020f5b98.c is the decompilation and port/slice_mem.txt carries
// it, so this file no longer defines the symbol at all.
//
// hal/scene_mg_memory2.cpp reports the card records themselves now, which is
// the measurement the trap count was standing in for: sixteen live records
// spread over x 80..176 and y 24..168 with three sprites in sub OAM was the
// whole bug, stated in numbers.

extern "C" {
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *, int,
                                                        int, int, unsigned short);
void _ZN8Particle10SysTrackerD1Ev(void *self);
}

// ---- the two method faces --------------------------------------------------

struct BCA_File;

/* GENERATED by port/tools/facegen.py, copied in verbatim. */
struct BlendModelAnim {
    void SetAnim(BCA_File & a0, int a1, int a2, int a3, unsigned short a4);
};

void BlendModelAnim::SetAnim(BCA_File & a0, int a1, int a2, int a3, unsigned short a4)
{ _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(this, &a0, a1, a2, a3, a4); }

/* Hand-written, section 2. The declaration matches the one
   src/func_ov006_020f5564.cpp spells; MSVC's ??1 mangle does not encode
   members, so `char pad[4]` there and here need not agree for the join, and it
   is kept identical anyway so a reader sees the same shape. */
struct SysTracker { ~SysTracker(); char pad[4]; };

SysTracker::~SysTracker() { _ZN8Particle10SysTrackerD1Ev(this); }

// ---- the nineteen generated aliases ----------------------------------------
//
// GENERATED by port/tools/facegen.py -- the mechanical rows only, every one
// through the exact-name join and the arity gate.

#pragma comment(linker, "/alternatename:?data_0209d45c@@3EA=_data_0209d45c")
#pragma comment(linker, "/alternatename:?data_ov004_020bc7d0@@3HA=_data_ov004_020bc7d0")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9e4@@3FA=_data_ov004_020bf9e4")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9ec@@3HA=_data_ov004_020bf9ec")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9f0@@3HA=_data_ov004_020bf9f0")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9f4@@3HA=_data_ov004_020bf9f4")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9f8@@3HA=_data_ov004_020bf9f8")
#pragma comment(linker, "/alternatename:?data_ov004_020bf9fc@@3HA=_data_ov004_020bf9fc")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa00@@3HA=_data_ov004_020bfa00")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa04@@3HA=_data_ov004_020bfa04")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa10@@3HA=_data_ov004_020bfa10")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa18@@3HA=_data_ov004_020bfa18")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa20@@3HA=_data_ov004_020bfa20")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa20@@3PAXA=_data_ov004_020bfa20")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa34@@3DA=_data_ov004_020bfa34")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa34@@3PADA=_data_ov004_020bfa34")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa56@@3PAEA=_data_ov004_020bfa56")
#pragma comment(linker, "/alternatename:?data_ov006_0213d4d4@@3PAXA=_data_ov006_0213d4d4")
#pragma comment(linker, "/alternatename:?data_ov006_0213e448@@3PAXA=_data_ov006_0213e448")

// ---- the nine hand rulings, section 1 --------------------------------------

#pragma comment(linker, "/alternatename:?data_ov006_0213ac48@@3UP2@@A=_data_ov006_0213ac48")
#pragma comment(linker, "/alternatename:?data_ov006_0213ac50@@3UP2@@A=_data_ov006_0213ac50")
#pragma comment(linker, "/alternatename:?data_ov006_0213ac88@@3UP2@@A=_data_ov006_0213ac88")
#pragma comment(linker, "/alternatename:?data_ov004_020beb68@@3PAUG@@A=_data_ov004_020beb68")
#pragma comment(linker, "/alternatename:?func_020beb68@@3PAUV@@A=_data_ov004_020beb68")
#pragma comment(linker, "/alternatename:?data_020a0ebc@@3UVector3@@A=_data_020a0ebc")
#pragma comment(linker, "/alternatename:?data_020a0edc@@3UVector3_16@@A=_data_020a0edc")
#pragma comment(linker, "/alternatename:?data_ov004_020bfa20@@3P6AXXZA=_data_ov004_020bfa20")
#pragma comment(linker, "/alternatename:?LoadBGPltt@GXS@@YAXPBXII@Z=__ZN3GXS10LoadBGPlttEPKvjj")
#pragma comment(linker, "/alternatename:?PlayBank2_2D@Sound@@SAXI@Z=__ZN5Sound12PlayBank2_2DEj")
#pragma comment(linker, "/alternatename:?RenderAll@ShadowModel@@YAXXZ=?RenderAll@ShadowModel@@SAXXZ")

/* THE NAME-SPELLING ALIAS ONTO A DATA WORD. src/func_ov006_020f7394.c (slot 18)
   and src/func_ov006_020f74b4.cpp (slot 0) both store through a symbol they
   spell `func_020bc7d4`, a name that exists in no config; the reloc's module is
   ov004 and data_ov004_020bc7d4 is the co-resident answer, a plain int the ov004
   mount defines. Twelve ov004 src TUs already spell it the right way, which is
   what makes this a spelling defect rather than a convention. */
#pragma comment(linker, "/alternatename:_func_020bc7d4=_data_ov004_020bc7d4")

// ---- section 3: the card draw is a real body now ---------------------------
//
// Nothing is defined here for 0x020f5b98 any more. src/func_ov006_020f5b98.c
// defines it and port/slice_mem.txt compiles it, so a tree that loses that
// slice line fails to LINK rather than quietly drawing nothing.
