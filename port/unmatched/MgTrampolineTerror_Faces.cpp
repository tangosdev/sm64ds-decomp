// PORT_HOST_ABI.  dScMgTrampoline2_c's closure faces.  Run mg11, lane TTE.
// actor id 0x181 = scene 385, "Trampoline Terror".
//
// Four kinds of row live here and they are kept apart on purpose, because only
// one of the four is a pure linker rename and the other three change or assert
// something about the CALL:
//
//   1. MSVC-TYPED SPELLINGS of mounted C storage -- 67 /alternatename rows.
//      A matched .cpp TU declares a data symbol outside its extern "C" block,
//      so MSVC mangles the reference while port/tools/ovdata.py's mount defines
//      the one real C name.  Data only; a data alias has no this-register
//      contract to break.  This is hal/actor_classes_bbh.cpp's data_02082128
//      case, 67 words deep.
//   2. FIVE FUNCTION ALIASES, every one of them cdecl on BOTH sides.  The
//      hal/actor_classes_ov063.cpp law is that an /alternatename can never
//      bridge cdecl into __thiscall; it says nothing about cdecl to cdecl,
//      where the stack layout and the return register are identical and there
//      is no `this` to place.  Each is argued on its own line below.
//   3. THREE METHOD FACES, which are the cdecl-into-__thiscall cases an alias
//      cannot do.  Each is a real C++ definition that forwards to the
//      C-linkage body, with the receiver placed by the compiler.
//   4. FOUR FLOOR TRAPS: ov006 bodies on this class's live path with no src
//      file and no delinks block.  Count and return, never invent.
//
// EVERY ROW HERE WAS ASKED FOR BY A LINK.  The closure was NOT walked from the
// ROM's relocation graph -- port/slice_lky.txt section 9 records why that
// over-reaches -- it was closed the documented way: seed with the class's own
// TUs, link, add exactly what LNK2019 names, repeat.

// ---- 1. MSVC-TYPED SPELLINGS OF MOUNTED C STORAGE -------------------------
#pragma comment(linker, "/alternatename:?data_0209f5f8@@3EA=_data_0209f5f8")
#pragma comment(linker, "/alternatename:?data_020a0dbc@@3RCFC=_data_020a0dbc")
#pragma comment(linker, "/alternatename:?data_ov006_0213b104@@3PAHA=_data_ov006_0213b104")
#pragma comment(linker, "/alternatename:?data_ov006_0213b2e0@@3HA=_data_ov006_0213b2e0")
#pragma comment(linker, "/alternatename:?data_ov006_0213b3a4@@3UBTA_File@@A=_data_ov006_0213b3a4")
#pragma comment(linker, "/alternatename:?data_ov006_0213b3c4@@3HA=_data_ov006_0213b3c4")
#pragma comment(linker, "/alternatename:?data_ov006_02140540@@3HA=_data_ov006_02140540")
#pragma comment(linker, "/alternatename:?data_ov006_0214054c@@3HA=_data_ov006_0214054c")
#pragma comment(linker, "/alternatename:?data_ov006_02140554@@3PADA=_data_ov006_02140554")
#pragma comment(linker, "/alternatename:?data_ov006_02140560@@3HA=_data_ov006_02140560")
#pragma comment(linker, "/alternatename:?data_ov006_02140564@@3HA=_data_ov006_02140564")
#pragma comment(linker, "/alternatename:?data_ov006_02140568@@3HA=_data_ov006_02140568")
#pragma comment(linker, "/alternatename:?data_ov006_0214056c@@3HA=_data_ov006_0214056c")
#pragma comment(linker, "/alternatename:?data_ov006_0214057c@@3HA=_data_ov006_0214057c")
#pragma comment(linker, "/alternatename:?data_ov006_02140580@@3HA=_data_ov006_02140580")
#pragma comment(linker, "/alternatename:?data_ov006_02140590@@3HA=_data_ov006_02140590")
#pragma comment(linker, "/alternatename:?data_ov006_0214059c@@3HA=_data_ov006_0214059c")
#pragma comment(linker, "/alternatename:?data_ov006_021405a0@@3HA=_data_ov006_021405a0")
#pragma comment(linker, "/alternatename:?data_ov006_021405a4@@3HA=_data_ov006_021405a4")
#pragma comment(linker, "/alternatename:?data_ov006_021405b8@@3HA=_data_ov006_021405b8")
#pragma comment(linker, "/alternatename:?data_ov006_021405bc@@3HA=_data_ov006_021405bc")
#pragma comment(linker, "/alternatename:?data_ov006_021405c0@@3HA=_data_ov006_021405c0")
#pragma comment(linker, "/alternatename:?data_ov006_021405c4@@3HA=_data_ov006_021405c4")
#pragma comment(linker, "/alternatename:?data_ov006_021405d0@@3USharedFilePtr@@A=_data_ov006_021405d0")
#pragma comment(linker, "/alternatename:?data_ov006_021405d8@@3USharedFilePtr@@A=_data_ov006_021405d8")
#pragma comment(linker, "/alternatename:?data_ov006_021405e0@@3USharedFilePtr@@A=_data_ov006_021405e0")
#pragma comment(linker, "/alternatename:?data_ov006_021405e8@@3USharedFilePtr@@A=_data_ov006_021405e8")
#pragma comment(linker, "/alternatename:?data_ov006_021405f0@@3USharedFilePtr@@A=_data_ov006_021405f0")
#pragma comment(linker, "/alternatename:?data_ov006_021405f8@@3USharedFilePtr@@A=_data_ov006_021405f8")
#pragma comment(linker, "/alternatename:?data_ov006_02140600@@3USharedFilePtr@@A=_data_ov006_02140600")
#pragma comment(linker, "/alternatename:?data_ov006_02140608@@3USharedFilePtr@@A=_data_ov006_02140608")
#pragma comment(linker, "/alternatename:?data_ov006_02140610@@3USharedFilePtr@@A=_data_ov006_02140610")
#pragma comment(linker, "/alternatename:?data_ov006_02140618@@3USharedFilePtr@@A=_data_ov006_02140618")
#pragma comment(linker, "/alternatename:?data_ov006_02140620@@3USharedFilePtr@@A=_data_ov006_02140620")
#pragma comment(linker, "/alternatename:?data_ov006_02140628@@3USharedFilePtr@@A=_data_ov006_02140628")
#pragma comment(linker, "/alternatename:?data_ov006_02140630@@3USharedFilePtr@@A=_data_ov006_02140630")
#pragma comment(linker, "/alternatename:?data_ov006_02140638@@3USharedFilePtr@@A=_data_ov006_02140638")
#pragma comment(linker, "/alternatename:?data_ov006_02140848@@3Udata_t@@A=_data_ov006_02140848")
#pragma comment(linker, "/alternatename:?data_ov006_02140850@@3Udata_t@@A=_data_ov006_02140850")
#pragma comment(linker, "/alternatename:?data_ov006_02140858@@3Udata_t@@A=_data_ov006_02140858")
#pragma comment(linker, "/alternatename:?data_ov006_02141a48@@3PAXA=_data_ov006_02141a48")
#pragma comment(linker, "/alternatename:?data_ov006_02142f80@@3USharedFilePtr@@A=_data_ov006_02142f80")
#pragma comment(linker, "/alternatename:?data_ov006_0213b344@@3US2@@A=_data_ov006_0213b344")
#pragma comment(linker, "/alternatename:?data_ov006_02140828@@3HA=_data_ov006_02140828")
#pragma comment(linker, "/alternatename:?data_ov006_02140838@@3HA=_data_ov006_02140838")
#pragma comment(linker, "/alternatename:?data_ov006_0213b0f0@@3HA=_data_ov006_0213b0f0")
#pragma comment(linker, "/alternatename:?data_ov006_0213b12c@@3UG2@@A=_data_ov006_0213b12c")
#pragma comment(linker, "/alternatename:?data_ov006_0213b14c@@3US8@@A=_data_ov006_0213b14c")
#pragma comment(linker, "/alternatename:?data_ov006_0213b184@@3UG2@@A=_data_ov006_0213b184")
#pragma comment(linker, "/alternatename:?data_ov006_0213b1d4@@3PAHA=_data_ov006_0213b1d4")
#pragma comment(linker, "/alternatename:?data_ov006_0213b1dc@@3PAHA=_data_ov006_0213b1dc")
#pragma comment(linker, "/alternatename:?data_ov006_0213b1ec@@3PAHA=_data_ov006_0213b1ec")
#pragma comment(linker, "/alternatename:?data_ov006_0213b22c@@3PAPAXA=_data_ov006_0213b22c")
#pragma comment(linker, "/alternatename:?data_ov006_02140544@@3HA=_data_ov006_02140544")
#pragma comment(linker, "/alternatename:?data_ov006_02140558@@3HA=_data_ov006_02140558")
#pragma comment(linker, "/alternatename:?data_ov006_02140564@@3UG2@@A=_data_ov006_02140564")
#pragma comment(linker, "/alternatename:?data_ov006_02140574@@3HA=_data_ov006_02140574")
#pragma comment(linker, "/alternatename:?data_ov006_02140578@@3HA=_data_ov006_02140578")
#pragma comment(linker, "/alternatename:?data_ov006_0214058c@@3HA=_data_ov006_0214058c")
#pragma comment(linker, "/alternatename:?data_ov006_02140598@@3HA=_data_ov006_02140598")
#pragma comment(linker, "/alternatename:?data_ov006_0214059c@@3PAXA=_data_ov006_0214059c")
#pragma comment(linker, "/alternatename:?data_ov006_021405a8@@3HA=_data_ov006_021405a8")
#pragma comment(linker, "/alternatename:?data_ov006_021405b0@@3HA=_data_ov006_021405b0")
#pragma comment(linker, "/alternatename:?data_ov006_021405b4@@3HA=_data_ov006_021405b4")
#pragma comment(linker, "/alternatename:?data_ov006_021405c8@@3HA=_data_ov006_021405c8")
#pragma comment(linker, "/alternatename:?data_ov006_021405c8@@3PAHA=_data_ov006_021405c8")
#pragma comment(linker, "/alternatename:?data_ov006_02141a40@@3PAXA=_data_ov006_02141a40")

// ---- 2. FIVE FUNCTION ALIASES, ALL cdecl ON BOTH SIDES --------------------
//
// Model::LoadTextureToVram.  src/func_ov006_020d0b2c.cpp declares
// `namespace Model { int LoadTextureToVram(char *, unsigned int); }` and calls
// it qualified, so MSVC wants ?LoadTextureToVram@Model@@YAHPADI@Z -- a free
// function in a NAMESPACE called Model, __cdecl.  include/Model.h:68 declares
// the real one `static u32 LoadTextureToVram(char *texData, u32 size)` inside
// struct Model, which MSVC emits as ?LoadTextureToVram@Model@@SAIPADI@Z --
// STATIC, so also __cdecl, with the same two stack arguments and the same
// return register.  There is no `this` on either side, so the alias is the
// whole fix and it is not the hal/actor_classes_ov063.cpp hazard.  The int/u32
// difference is a spelling: both are four bytes returned in eax.
#pragma comment(linker, "/alternatename:?LoadTextureToVram@Model@@YAHPADI@Z=?LoadTextureToVram@Model@@SAIPADI@Z")
//
// Particle::System::New.  src/func_ov006_020c8f20.cpp declares a local
// `struct System { static System* New(...); }` with seven parameters, so MSVC
// wants ?New@System@@SAPAU1@IIHHHPBUVector3_16f@@PAUCallback@@@Z -- SA, a
// static member, __cdecl.  The matched body is
// src/_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE.c,
// a .c file, so it is defined at C linkage with the same seven arguments in the
// same order.  Static member and C function are the same call surface on x86:
// seven stack arguments, caller cleans, pointer back in eax.  That TU is
// already linked by port/slice_gate29.txt, so this row only renames.
#pragma comment(linker, "/alternatename:?New@System@@SAPAU1@IIHHHPBUVector3_16f@@PAUCallback@@@Z=__ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE")
//
// Scene::AfterRender.  include/decl_common.h:1936 declares
// `extern void Scene_AfterRender(void*, unsigned int);` while the matched body
// is src/_ZN5Scene11AfterRenderEj.cpp, already linked by port/slice_w1l2.txt
// under the ROM's own Itanium name (config/arm9/symbols.txt:1105,
// _ZN5Scene11AfterRenderEj at 0x0202e398 -- the address dScMgD3DBase_c's slot
// 11 tail-jumps to, which is how this lane found it).  Both are C linkage and
// both take (void*, unsigned): the receiver is an explicit first parameter on
// both sides, so this is a rename and not a receiver placement.
#pragma comment(linker, "/alternatename:_Scene_AfterRender=__ZN5Scene11AfterRenderEj")
//
// AND ONE THAT IS NOT EVEN A TYPE RENAME.  src/func_ov006_020e7124.c declares
// `extern int func_020beb74[];` and USES IT AS DATA
// (`func_020beb74[1] = (int)obj; data_0209d4a8 = (void *)func_020beb74;`).
// There is no function at 0x020beb74: config/arm9/overlays/ov004/symbols.txt
// has data_ov004_020beb74, kind bss, and 0x020beb74 is inside ov004's bss span
// 0x020beb60..0x020bfec0.  A func_-prefixed name on a data address is a
// decomp-side naming defect; the port renames rather than editing the
// byte-gated tree.  ROUTED to the decomp side as a symbol-name correction.
#pragma comment(linker, "/alternatename:_func_020beb74=_data_ov004_020beb74")
//
// ApproachLinear2, the INT overload.  src/_Z15ApproachLinear2Riii.cpp defines
// `int ApproachLinear2(int &, int, int)` with NO extern "C", so MSVC decorates
// it ?ApproachLinear2@@YAHAAHHH@Z, while three .c TUs in this closure
// (src/func_ov006_020cd744.c, _020cf790.c and _020cf820.c) spell the ROM's
// Itanium name at C linkage.  port/unmatched/Ov085_Behaviors.cpp:142 records
// the same fact and solves it the other way, by declaring the C++ signature in
// a C++ TU; a .c caller cannot do that.  Both sides are plain __cdecl free
// functions taking (int&, int, int) and returning int in eax, so the alias has
// no receiver contract to break -- it is the ?LoadTextureToVram@Model@@ case
// above with a reference instead of a pointer.
#pragma comment(linker, "/alternatename:__Z15ApproachLinear2Riii=?ApproachLinear2@@YAHAAHHH@Z")
//
// Sound_PlayBank1Panned.  src/func_ov006_020c94e0.cpp and
// src/func_ov006_020c7c68.c call `func_ov006_020e6df0(a, b, c)` by address-
// derived name, while config/arm9/overlays/ov006/symbols.txt:690 gives 0x020e6df0
// the recovered name Sound_PlayBank1Panned (size 0x4c) and the matched TU is
// src/Sound_PlayBank1Panned.cpp, a slice line here.  Same address, same three
// C-linkage arguments; the row is a rename and nothing else.
#pragma comment(linker, "/alternatename:_func_ov006_020e6df0=_Sound_PlayBank1Panned")

// ---- 3. THREE METHOD FACES -- THE cdecl-INTO-__thiscall CASES -------------
//
// hal/actor_classes_ov063.cpp states the law these obey: "the face bridges
// cdecl into __thiscall -- an /alternatename can never do that".  Both bodies
// below are matched TUs compiled at C linkage; both callers spell a member
// call.  A linker alias would hand the body a `this` that was never pushed.

extern "C" {
void _ZN6Player12St_Null_InitEv(void);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        void *self, void *file, int flags, int speed, unsigned startFrame);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        void *self, void *f, int flags, int speed, unsigned start);
}

/* Player::St_Null_Init.  src/func_ov006_02123bf4.cpp -- this class's state
   0x02123bf4 -- declares `struct Player { void St_Null_Init(); };` and calls
   `func_ov006_020cedf0(c)->St_Null_Init()`, so MSVC wants
   ?St_Null_Init@Player@@QAEXXZ, __thiscall with the receiver in ecx.

   THE RECEIVER IS DROPPED ON PURPOSE AND THE ROM DROPS IT TOO.  The matched
   body src/_ZN6Player12St_Null_InitEv.cpp takes `(void)` -- it walks
   data_ov006_02140554 for data_ov006_021405bc entries and then calls
   func_ov006_020c8a64, and never reads a receiver.  On ARM the caller's r0 is
   simply not read.  So this is NOT the dropped-receiver disease
   port/mg_fanout_costs.txt warns about: there is no argument to lose, and the
   face is what lets MSVC's ecx-passing caller reach a body that takes none. */
struct Player { void St_Null_Init(); };
void Player::St_Null_Init() { _ZN6Player12St_Null_InitEv(); }

/* TextureTransformer::SetFile.  src/func_ov006_020cecc0.cpp declares a local
   `struct TextureTransformer { ...; void SetFile(BTA_File&, int, int, unsigned); }`
   and calls it on `c + 0x194`, so MSVC wants
   ?SetFile@TextureTransformer@@QAEXAAUBTA_File@@HHI@Z -- __thiscall, receiver
   in ecx, four stack arguments.  The matched body
   src/_ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj.c is a .c file, so
   it is C linkage taking FIVE cdecl arguments with the receiver first.  The
   face places the receiver; nothing else moves.  Contrast
   TextureTransformer::Prepare, which hal/method_faces.cpp resolves with a
   forward for the opposite reason -- that one is declared static and has no
   receiver at all. */
struct BTA_File;
struct TextureTransformer { void SetFile(BTA_File &f, int a, int b, unsigned c); };
void TextureTransformer::SetFile(BTA_File &f, int a, int b, unsigned c)
{ _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(this, &f, a, b, c); }

/* ModelAnim::SetAnim, THE void* SPELLING.  hal/bob_enemy_shadow_faces.cpp:88
   already carries this face with a `BCA_File *` first parameter, which MSVC
   mangles ?SetAnim@ModelAnim@@QAEXPAUBCA_File@@HHI@Z.  Two TUs in this closure
   -- src/func_ov006_020c9c8c.c and src/func_ov006_020cc198.c -- declare their
   local ModelAnim with the first parameter as `void *`, so MSVC wants
   ?SetAnim@ModelAnim@@QAEXPAXHHI@Z instead.  Different mangled name, same body,
   same four stack arguments and the same receiver in ecx; the existing face
   cannot serve it and a linker alias cannot rename across the two because both
   are __thiscall members whose signatures differ only in a pointer type MSVC
   encodes.  This is the second face for one body, not a second body.

   THE DEFINITION IS NOT HERE ANY MORE.  Lane TTI wrote the identical face --
   same signature, same one-line forward -- in
   unmatched/MgTrampolineTime_Faces.cpp, and the mg11 merge found the two
   colliding (LNK2005 on ?SetAnim@ModelAnim@@QAEXPAXHHI@Z).  ONE copy survives
   and it is TTI's, which landed first in merge order; nothing semantic was at
   stake, the two were the same face character for character.  The paragraph
   above is kept because it names the two TUs in THIS class's closure that need
   the void* spelling, which TTI's copy does not record. */

// ---- 4. FOUR FLOOR TRAPS --------------------------------------------------
//
// Three ov006 bodies on this class's live path have NO src file and NO delinks
// block in config/arm9/overlays/ov006/delinks.txt.  They are trapped count-and-
// return, never invented, and their want counts are printed by
// hal/scene_mg_trampoline2.cpp's census.  A trap-shaped floor also HIDES its
// callees from static closure, so retiring one of these costs its own slice
// lines on top of the body.
//
//   func_ov006_020cf2fc  0x45c  void(char*)                 src/func_ov006_020d09e0.c
//   func_ov006_020d01e0  0x800  void(short*,short*,short*)  src/func_ov006_020d0b78.c
//   func_ov006_020d0c38  0x3ac  int(u16*,u16*)              src/func_ov006_02122f24.c
//                                                           (THIS class's slot 23)
//   func_ov006_020cfc74  0x56c  void(char*)                 src/func_ov006_020cfa44.c
//
// THE THIRD ONE IS A PLAYER-FACING GAP AND IT IS NAMED AS ONE.  Slot 23
// (OnAttacked2) packs the current and previous stylus points into four
// halfwords and asks func_ov006_020d0c38 whether the stroke connected; the true
// arm plays func_02012718(0x1af, ...) and the false arm plays
// func_02012790(0xe).  A trap that returns 0 takes the false arm every time, so
// on the port every stroke reads as a miss at that test.  The trap says so on
// its first call rather than reading as a quiet correct answer.  Its arity and
// return type are taken from the two CALL SITES that name it
// (src/func_ov006_0212101c.c and src/func_ov006_02122f24.c), which agree.
//
// SECTION 4 IS HISTORY NOW.  All four of those bodies are seated -- the first
// three by runs mg12/mg13 in unmatched/MgTrampolineTime_Floors.cpp's slice, and
// the fourth, func_ov006_020cfc74, by run mg13 lane BOUNCE.  This file traps
// nothing.  The paragraph above is kept because it is the derivation of the
// arities the seats use, not because any of it is still a floor.

#include <cstdio>
#include <cstdlib>

/* THE FIRST THREE TRAPS MOVED AT THE MERGE.  Lane TTI trapped the same three
   bodies in unmatched/MgTrampolineTime_Floors.cpp -- 0x180 and 0x181 share
   this machinery -- and the two copies collided at link (LNK2005 x3).  ONE
   definition each survives, in TTI's file, and it uses THIS lane's signatures:
   both lanes ruled all three UNDECOMPILED with the same sizes, and the only
   difference was that TTI spelled the first two `int f(void)` as placeholders
   while these were derived from the ROM call sites that name them.  The counts
   below still come out of the same trap bodies; port_mg_shared_trap_counts is
   TTI's file handing them back, and the census this file feeds is unchanged.

   The FOURTH trap stays here: func_ov006_020cfc74 is behind one of this
   class's 3D-Mario states and only 0x181 reaches it. */
extern "C" void port_mg_shared_trap_counts(unsigned *f2fc, unsigned *f1e0,
                                           unsigned *fc38);


/* THIS FILE'S OWN TRAP IS RETIRED.  It used to define func_ov006_020cfc74
   count-and-return, 0x56c, with no src and no delinks block, and run mg12 lane
   TRM had already read the listing and called it the bounce.  Run mg13 lane
   BOUNCE decompiled it: src/func_ov006_020cfc74.c is a slice_tte.txt line now,
   an honest NONMATCHING seat at 8 divergences against 339 ROM code words with
   the size, the 8-word literal pool and all 28 call offsets exactly the ROM's,
   and with every one of those 8 divergences a permutation or a re-association
   of the same computation (its banner enumerates them).  It needs no hostgen:
   the body has no MMIO store and its pool holds no 0x04xxxxxx word.

   WHAT THAT CHANGES FOR A READER OF THE CENSUS.  This body is what a trampoline
   RECORD's per-tick state runs to decide whether any of the up-to-five
   characters in data_ov006_0214097c crossed the trampoline this frame.  While
   it was trapped it was reached constantly and did nothing, so the trampolines
   drew (run mg13 lane TRAMP seated the mesh) and every character fell straight
   through them -- which is exactly what Tango reported on the 9c4517c2b build.
   The observable that moves is in port/hal/trampoline_records.cpp's readout:
   this body DECREMENTS the record's remaining count at +0x324 on each catch and
   bumps the caught count at +0x326, and sets the render gate at +0x328 to 3 when
   the count reaches zero.  With the trap in place +0x324 could only ever hold
   the value the installer left there. */
static const unsigned g_tte_trap_020cfc74 = 0;   /* RETIRED, see above */

/* THE CHARACTER CENSUS, added with the seat by run mg13 lane BOUNCE.
   src/func_ov006_020cfc74.c walks data_ov006_0214097c -- the up-to-five
   character records func_ov006_020cae9c (stride 0xdc) and func_ov006_020cd658
   (stride 0xd0) register -- and writes each caught character's bounce state to
   its +0x18.  Without this readout a run in which nothing bounces cannot be told
   apart from a run in which nothing was ever REGISTERED, and those two want
   opposite follow-ups.  It reads and prints; it writes nothing.  It hangs off
   port_mg_tte_trap_counts because that is this file's own function and the
   census in hal/scene_mg_trampoline2.cpp already calls it once at the end of a
   run -- no other lane's file is touched to get it. */
extern "C" {
extern unsigned char *data_ov006_0214097c[];
extern unsigned char data_ov006_02140990[];      /* the four records, stride 0x32c */
}

/* THE GEOMETRY THE TEST ACTUALLY READS.  func_ov006_020cfc74 projects each
   character onto the record's two axes at +0x14 and +0x20, about the origin at
   +0x08, and tests against the segment that runs +/- the half-length at +0x58
   along the first axis.  Those four fields are written by the installer
   func_ov006_020d01e0, not by the bounce, so printing them separates "the test
   ran and nothing crossed" from "the test ran against a segment of length
   zero".  Plain memory reads of a symbol the ov006 mount already defines: no
   vtable is called and nothing is written. */
static void port_mg_tte_record_geometry(void)
{
    int i;
    for (i = 0; i < 4; ++i) {
        const unsigned char *r = &data_ov006_02140990[i * 0x32c];
        const int *o = (const int *)(r + 0x08);
        const int *u = (const int *)(r + 0x14);
        const int *v = (const int *)(r + 0x20);
        std::printf("[scene] trampoline record %d geometry: origin +0x08 = "
                    "(%d, %d, %d), axis +0x14 = (%d, %d, %d), axis +0x20 = "
                    "(%d, %d, %d), half-length +0x58 = %d, gate +0x328 = %u\n",
                    i, o[0], o[1], o[2], u[0], u[1], u[2], v[0], v[1], v[2],
                    *(const int *)(r + 0x58), (unsigned)r[0x328]);
    }
}

static void port_mg_tte_character_census(void)
{
    int i;
    int present = 0;
    int stated = 0;

    port_mg_tte_record_geometry();

    for (i = 0; i < 5; ++i) {
        const unsigned char *o = data_ov006_0214097c[i];
        if (o == 0) {
            std::printf("[scene] dScMgTrampoline2_c character slot %d: ABSENT "
                        "(data_ov006_0214097c[%d] == 0)\n", i, i);
            continue;
        }
        ++present;
        {
            const unsigned st = *(const unsigned short *)(o + 0x18);
            const int px = *(const int *)(o + 0x04);
            const int py = *(const int *)(o + 0x08);
            const int pz = *(const int *)(o + 0x0c);
            if (st != 0) ++stated;
            std::printf("[scene] dScMgTrampoline2_c character slot %d: at %p, "
                        "bounce state +0x18 = %u, axis +0x04 = "
                        "(%d, %d, %d), +0x10 = %d, +0x14 = %d\n",
                        i, (const void *)o, st, px, py, pz,
                        *(const int *)(o + 0x10), *(const int *)(o + 0x14));
            /* RAW WORDS, so a run in which nothing crossed can be read against
               the record geometry above without calling a virtual. The record's
               segment sits at y = -720896 (1.12, so -176.0); a character whose
               words never reach that band never had a crossing to find. Read
               only; 0xdc is the element stride func_ov006_020cae9c registers.
               Behind SM64DS_TTE_CHARDUMP=1 because it is 55 words per character
               and this census prints on every scene-385 run. */
            if (std::getenv("SM64DS_TTE_CHARDUMP")) {
                int k;
                std::printf("[scene] dScMgTrampoline2_c character slot %d raw:",
                            i);
                for (k = 0; k < 0xdc; k += 4)
                    std::printf(" +%02x=%d", k, *(const int *)(o + k));
                std::printf("\n");
            }
        }
    }
    std::printf("[scene] dScMgTrampoline2_c characters: %d of 5 registered, %d "
                "carrying a nonzero bounce state RIGHT NOW. DO NOT READ A 0 HERE "
                "AS 'NOTHING WAS CAUGHT': +0x18 is a state the character's own "
                "state machine consumes within the frame, so a run that caught "
                "prints 0 of 5 here whenever the census lands after the bounce "
                "finished. WHAT SURVIVES A CATCH is the record's live count at "
                "+0x324 and its caught count at +0x326 in the record readout "
                "above, and the caught character's own +0x04..+0x0c axis with "
                "+0x10 and +0x14 in the per-slot lines -- those are written by "
                "the hit tail of func_ov006_020cfc74 and nothing else clears "
                "them. Read those three, not this one\n", present, stated);
}

extern "C" void port_mg_tte_trap_counts(unsigned *a, unsigned *b, unsigned *c,
                                        unsigned *d)
{
    /* the first three come out of the unified trap bodies in
       unmatched/MgTrampolineTime_Floors.cpp; the fourth was this file's own and
       is now a retirement, not a measurement -- it can only read 0 */
    port_mg_shared_trap_counts(a, b, c);
    if (d) *d = g_tte_trap_020cfc74;
    port_mg_tte_character_census();
}

/* This symbol exists so a reader grepping for who owns the aliases above finds
   a definition rather than only a comment. */
extern "C" const char port_mg_tte_faces_marker[] =
    "dScMgTrampoline2_c faces, run mg11 lane TTE";
