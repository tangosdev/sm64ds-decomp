//cpp
/* daKing_Donketu_c -- the King Donketu boss, ov073.
 *
 * Reconstructed translation unit: ROM ordinals 0..42 of the linker run
 * 0x0211f000..0x02121f90, i.e. .text 0x0211f000..0x021218a0, 43 functions.
 * Assembled from the 43 one-function legacy sources, then reconciled by hand.
 *
 * THE UNIT IS 47 FUNCTIONS AND THIS FILE IS THE LOWER 43. tools/tu_map.py and
 * `tubuild inspect` both report 46, because the boundary they draw stops at the
 * last MANGLED member (OnAimedAtWithEgg, 0x02121ec0) and does not carry on over
 * daKing_Donketu_c_classInit at 0x02121ec8, which is named but unmangled and
 * sits in no vtable slot. The relocation census below puts classInit inside the
 * run, so 46 is that tool's floor, not the unit's size.
 *
 * WHY THIS IS A SUB-RANGE: ROM ordinal 43, daKing_Donketu_c::Behavior
 * (0x021218a0, size 0x42c), is a member this TU cannot yet reproduce. Its
 * legacy shard matches the cartridge on its own, but only under a file-scope
 * `#pragma opt_propagation off`, and that pragma is file-global last-wins: set
 * anywhere in a merged TU it recompiles every other member. Measured on this
 * TU, carrying it turns 45 matching members into 38 -- it buys Behavior and
 * costs eight. So Behavior keeps its own shard and its own delinks.txt entry,
 * and with it the three members above it (InitResources, OnAimedAtWithEgg and
 * classInit), because nothing in this tree can express a .text claim with a
 * hole in it -- no file entry in any delinks.txt anywhere carries two .text
 * claims -- and 43 below the split beats 3 above it.
 *
 * That pragma's scope was MEASURED, not assumed, on four cells: file-global
 * `opt_propagation off` (38/46), a bracketed off/on pair around Behavior alone
 * (45/46, Behavior still differing), the same bracket under
 * `#pragma defer_codegen off` (45/46, unchanged), and both pragmas file-global
 * under defer_codegen off (38/46). The bracket does NOT bind for
 * opt_propagation on this TU even with codegen undeferred -- the pragma is
 * file-global in both regimes -- so there is no arrangement of it that keeps
 * Behavior without paying the other eight.
 *
 * `#pragma opt_loop_invariants off` IS carried, at file scope. It comes from
 * the func_ov073_0211f61c shard, and on this TU it is free: it buys that
 * member (53 divergent words -> match) and moves no other member at all.
 *
 * THE RUN'S TWO BOUNDARIES ARE MEASURED, NOT ASSUMED. A ROM-wide relocation
 * census -- every relocs.txt in the tree, filtered to targets whose module SET
 * contains 73, with each referring word resolved back to its containing
 * function or data symbol through ov073's own symbols.txt -- gives every member
 * of 0x0211f000..0x02121f90 a referrer set drawn from exactly three places:
 * this class's own _ZTV16daKing_Donketu_c, its factory record
 * g_profile_KING_DONKETU, and a direct call from another member of this same
 * run. No word outside the run refers into it. The low edge is the section's
 * own start; _ZN16daKing_Donketu_cD1Ev at 0x0211f000 is referenced only by its
 * own vtable. The high edge is _ZN8CccArenaD1Ev at 0x02121f90, referenced only
 * by _ZTV8CccArena. The census discriminates rather than answering the same
 * thing everywhere: run the same filter over the claim window and it resolves
 * to {daKing_Donketu_c: 3, ChiefChilly: 22} with zero CccArena referrers; run
 * it over the CccArena window directly above and it resolves to {CccArena: 2}
 * with zero daKing_Donketu referrers.
 *
 * THE ONE SHARD IN THE RANGE NOT NAMED FOR THIS CLASS IS THE SAME CLASS.
 * ChiefChilly_ChangeState (0x0212157c, ROM ordinal 38, folded into this file
 * from the shard that used to be its own source) carries the boss's English
 * gloss, not a foreign class: include/daKing_Donketu_c.h records that
 * _ZTV11ChiefChilly was this type's historical vtable spelling and that
 * ChiefChilly_Spawn was an alias for daKing_Donketu_c_classInit. The census
 * agrees -- that function has 22 referrers across 17 functions, every one of
 * them inside this run and none outside it.
 *
 * VTABLE, MEASURED OUT OF extracted/dsd/arm9_overlays/ov073.bin.
 * _ZTV16daKing_Donketu_c is 0x7c bytes, 31 slots, the symbol sitting at the
 * address point 0x02123090; the next symbols.txt row is data_ov073_0212310c,
 * and the relocated run in relocs.txt is unbroken from the preamble's typeinfo
 * word at 0x0212308c through 0x02123108 with the first unrelocated word at
 * 0x02123110. All three readings agree. Eight slots pin members by name --
 * slot 0 InitResources, slot 3 CleanupResources, slot 6 Behavior, slot 9
 * Render, slot 12 OnPendingDestroy, slot 16 this file's D1, slot 17 its D0,
 * slot 29 OnAimedAtWithEgg -- and that is the positive control for the census
 * above. The 31-word length is what proves the base is an actor rather than a
 * dBgActor_c, whose vtables run 32.
 *
 * SLOTS 0 AND 3 ARE NOT ABSTRACT HERE, AND THAT HYPOTHESIS IS REFUTED, NOT
 * DROPPED. Both carry live relocated words, so the "abstract slot 0/3 hides
 * unlabelled helpers at the TU edges" reading buys nothing on this class. The
 * unlabelled helpers in this run are unlabelled for the ordinary reason: they
 * are non-virtual file-local members reached by direct call, and the census is
 * what attaches them to the class.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below. With codegen deferred (the
 * default) mwccarm 2004/b56 emits one .text section per function in the REVERSE
 * of source order; generating at parse time emits them in source order. Do not
 * reorder.
 *
 * THE DESTRUCTOR IS OUT OF LINE AND THE CARTRIDGE ORDERS IT D1 (0x0211f000)
 * BELOW D0 (0x0211f098), adjacent, with no room between them for a D2 -- and
 * there is no _ZN16daKing_Donketu_cD2Ev anywhere in the image. One out-of-line
 * definition, written FIRST, produces D1 then D0 and nothing in between, but
 * only in the undeferred regime: with codegen deferred the same definition
 * emits the variant cluster in the other order (D2, Vector3's D1, D0, D1), and
 * the licensed run then starts D0-before-D1. That was measured both ways.
 *
 * Because the destructor is out of line and this TU owns the class's key
 * function, mwcc emits the whole inheritance chain's vtable and typeinfo as
 * vague-linkage passengers, plus Vector3's own destructor, which types.h
 * declares deliberately -- see the manifest's compiler_only_output block. The
 * cartridge's copies of all of them live outside this claim: the class's own
 * _ZTV/_ZTI/_ZTS sit in ov073's unclaimed .data gap, and the base chain's
 * records are owned by arm9 and ov002.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x0211f000  the legacy D1 shard
 *   [1] 0x0211f098  the legacy D0 shard
 *   [2] 0x0211f144  the legacy func_ov073_0211f144 shard
 *   [3] 0x0211f2c0  the legacy func_ov073_0211f2c0 shard
 *   [4] 0x0211f494  the legacy func_ov073_0211f494 shard
 *   [5] 0x0211f61c  the legacy func_ov073_0211f61c shard
 *   [6] 0x0211fa74  the legacy func_ov073_0211fa74 shard
 *   [7] 0x0211fbec  the legacy func_ov073_0211fbec shard
 *   [8] 0x0211fbf4  the legacy func_ov073_0211fbf4 shard
 *   [9] 0x0211fc70  the legacy func_ov073_0211fc70 shard
 *  [10] 0x0211fc78  the legacy func_ov073_0211fc78 shard
 *  [11] 0x0211fe84  the legacy func_ov073_0211fe84 shard
 *  [12] 0x0211fe8c  the legacy func_ov073_0211fe8c shard
 *  [13] 0x0212000c  the legacy func_ov073_0212000c shard
 *  [14] 0x0212005c  the legacy func_ov073_0212005c shard
 *  [15] 0x02120098  the legacy func_ov073_02120098 shard
 *  [16] 0x021200e0  the legacy func_ov073_021200e0 shard
 *  [17] 0x02120390  the legacy func_ov073_02120390 shard
 *  [18] 0x021203ac  the legacy func_ov073_021203ac shard
 *  [19] 0x021205f0  the legacy func_ov073_021205f0 shard
 *  [20] 0x02120610  the legacy func_ov073_02120610 shard
 *  [21] 0x0212081c  the legacy func_ov073_0212081c shard
 *  [22] 0x02120844  the legacy func_ov073_02120844 shard
 *  [23] 0x021208e4  the legacy func_ov073_021208e4 shard
 *  [24] 0x02120910  the legacy func_ov073_02120910 shard
 *  [25] 0x02120ad8  the legacy func_ov073_02120ad8 shard
 *  [26] 0x02120b78  the legacy func_ov073_02120b78 shard
 *  [27] 0x02120c08  the legacy func_ov073_02120c08 shard
 *  [28] 0x02120c7c  the legacy func_ov073_02120c7c shard
 *  [29] 0x02120d80  the legacy func_ov073_02120d80 shard
 *  [30] 0x02120dec  the legacy func_ov073_02120dec shard
 *  [31] 0x02120e60  the legacy func_ov073_02120e60 shard
 *  [32] 0x02120ed0  the legacy func_ov073_02120ed0 shard
 *  [33] 0x0212122c  the legacy func_ov073_0212122c shard
 *  [34] 0x0212128c  the legacy func_ov073_0212128c shard
 *  [35] 0x02121378  the legacy func_ov073_02121378 shard
 *  [36] 0x02121388  the legacy func_ov073_02121388 shard
 *  [37] 0x02121538  the legacy func_ov073_02121538 shard
 *  [38] 0x0212157c  the legacy ChiefChilly_ChangeState shard
 *  [39] 0x021215cc  the legacy func_ov073_021215cc shard
 *  [40] 0x021217e0  the legacy CleanupResources shard
 *  [41] 0x0212186c  the legacy OnPendingDestroy shard
 *  [42] 0x02121870  the legacy Render shard
 *
 * NOT folded, and still carrying their own shards and delinks.txt entries:
 *  [43] 0x021218a0  Behavior          -- the pragma split described above
 *  [44] 0x02121ccc  InitResources     -- above the split
 *  [45] 0x02121ec0  OnAimedAtWithEgg  -- above the split
 *  [46] 0x02121ec8  daKing_Donketu_c_classInit (src/d_a_king_donketu.cpp)
 */

#pragma defer_codegen off
#pragma opt_loop_invariants off

#include "common.h"
#include "daKing_Donketu_c.h"
#include "decl_common.h"
#include "decl_Message.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "dBgCh_Lin.h"

/* ---------------------------------------------------------------------------
 * Shadow types, reconciled across the forty-seven merged shards.
 *
 * Where a real project header already declares a name the shards spelled
 * locally, the HEADER WINS and the local copy is gone: Vector3, Fix12i,
 * s8/s16/s32/u8/u16/u32/s64 (types.h), Animation (Animation.h), BMD_File,
 * BCA_File, dActor_c and fBase_c. Fourteen of the shards already compiled
 * against those headers; four spelled a private POD `Vector3` instead, and
 * types.h's is non-POD (it has a declared destructor, deliberately). Those
 * four members are individually byte-verified below under the header's
 * spelling -- see the per-member notes.
 *
 * What remains is what no header declares.
 * ------------------------------------------------------------------------- */

/* THE STATE MACHINE'S RECEIVER. `mState` (0x37c in the class header) holds a
 * pointer to one of the twelve file-scope state objects; ChiefChilly_ChangeState
 * stores that pointer and immediately dispatches through it, and Behavior
 * compares it by ADDRESS against those objects to gate five separate things.
 *
 * `struct C` is COMPLETE here because both shards that form the pointer-to-member
 * type spelled it complete, and mwccarm 2004/b56 picks its pointer-to-member
 * representation from the completeness of the class -- leaving it incomplete
 * would change the codegen of every dispatch through it. */
struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

/* func_ov073_02120c7c reaches the SAME object through a field view rather than
 * the pad view above. Two layouts cannot share one name in a merged TU, so that
 * shard's view keeps the offsets it had under a distinct name, and its two
 * ChiefChilly_ChangeState call sites take the (codegen-free) pointer conversion
 * -- the merge rule is that the more complete observation keeps the shared name. */
struct CB {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x94-0x90];
    short field_94;
    char pad96[0x98-0x96];
    int field_98;
    char pad9c[0x35c-0x9c];
    char anim_35c;
    char pad35d[0x4c9-0x35d];
    unsigned char field_4c9;
    char pad4ca[0x4cb-0x4ca];
    unsigned char field_4cb;
    char pad4cc[0x4d4-0x4cc];
    int field_4d4, field_4d8, field_4dc;
};

/* NO local `Fix12` typedef. types.h says a private `typedef s32 Fix12;` is
 * allowed in a single src/ file, but include/math/Fix12.h declares Fix12 as a
 * class TEMPLATE and this TU reaches it through types.h, so the four shards
 * that spelled a parameter `Fix12` take `Fix12i` here instead. Both are s32;
 * the mangled names those declarations carry are unchanged. */

/* Three POD point/vector spellings the shards used where they did NOT want
 * types.h's Vector3. Kept under their own names so the layouts stay distinct. */
struct V3 { int x; int y; int z; };
typedef struct { int x, y, z; } Vec3;
typedef struct { int a, b; } P2;

enum Bool { FALSE, TRUE };

/* Matrix spellings: the ROM's 4x3 matrix has two views in these shards, a
 * row-vector one and a flat twelve-word one, and different members use each. */
typedef struct Mtx43 { Vec3 r0, r1, r2, t; } Mtx43;
struct Mat4x3 { int m[12]; };

/* The ground-ray parameter block, as Behavior spells it (types.h Vector3). */
struct RayParams { Vector3 start, end, in, out; };

/* A minimal virtual-dispatch view used by one member to reach slot 5 of an
 * object whose real class is not recovered. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

extern "C" {

/* THE STATE TABLE. Sixteen file-scope state objects, all in this overlay's
   .bss and all filled by __sinit_ov073_02122d48. Across the shards they were
   spelled `char[]`, `void *`, `int[]` and `PMF`; a merged TU can carry only one
   spelling each, and every ChiefChilly_ChangeState call site takes the
   codegen-free `(PMF *)`.

   NINE of the sixteen already come from include/decl_common.h and are NOT
   redeclared here. That header is not self-consistent about them -- it spells
   seven as `char[]` (which decay to the address the state pointer wants) and
   data_ov073_021233e0 / _02123410 as `void *` OBJECTS (whose address must be
   taken explicitly). Both spellings are honoured at the call sites rather than
   overridden, because overriding one would silently turn an address-of into a
   pointer LOAD. The seven decl_common does not declare are below. */
extern char data_ov073_02123330[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123370[];
extern char data_ov073_021233b0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233f0[];

/* The state-machine entry point, declared with the signature its own
   DEFINITION below carries (ROM ordinal 38). Sixteen shards had re-declared it
   with `void *` for one or both parameters, and under C linkage only one
   prototype can exist; the definition's is the complete observation, so every
   call site converts instead. */
int ChiefChilly_ChangeState(C *c, PMF *p);

/* THE SHARED FILE HANDLES. Nine SharedFilePtr objects, eight in this overlay
   and one borrowed from ov002. Every shard agreed on the two things done with
   them -- hand the OBJECT to Animation::LoadFile / Model::LoadFile and to
   Release, and read the BCA_File* in the SECOND word -- but not on how to
   spell that: `void *[]`, `void **`, `int *`, `P2 {int a,b;}`, a local
   `struct G {int a; BCA_File *b;}` and `SharedFilePtr` all appear, and each
   shard's indexing was sized to its own spelling.

   include/SharedFilePtr.h declares the class with NO FIELDS, deliberately --
   the layout is not recovered and committing to one there would change every
   other file's indexing. So the merged TU takes the one view that serves both
   uses without inventing a layout: a two-element array of BCA_File*, whose
   name decays to the object's address for the method calls and whose [1] is
   the same +4 load every shard was already doing. */
extern struct BCA_File *data_ov073_02123280[2];
extern struct BCA_File *data_ov073_02123288[2];
extern struct BCA_File *data_ov073_02123290[2];
extern struct BCA_File *data_ov073_02123298[2];
extern struct BCA_File *data_ov073_021232a0[2];
extern struct BCA_File *data_ov073_021232a8[2];
extern struct BCA_File *data_ov073_021232b0[2];
extern struct BCA_File *data_ov073_021232b8[2];
extern struct BCA_File *data_ov002_0210da30[2];
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(const void* in, void* m, void* out);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const void*, void*);
extern void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void func_0200d8c8(void *cam, void *v, int strength);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *self, void *v, int b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void *data_0209f318;
/* The shared scratch matrix. Five shards spelled it five ways -- int[],
   int, Mat4x3, Mtx43 and struct Matrix4x3. common.h's Matrix4x3 is
   `s32 m[12]`, byte-identical to the local Mat4x3 and Mtx43 views, so the
   project type keeps the name and the narrower uses take a codegen-free
   cast or address-of. */
extern struct Matrix4x3 data_020a0e68;
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern Fix12i Vec3_HorzLen(Vec3 *v);
extern short data_02082214[];
extern u16 data_0209e650;
extern u16 DecIfAbove0_Short(void* p);
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov073_0211f494(void *pa, void *pb);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
extern void func_02012694(int a, void* b);
extern int RandomIntInternal(u16* seed);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3* v, void* rot, s32 e, s32 f);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 id);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12i f, u32 b, u32 c, u32 d);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, struct BCA_File* f, int i, int j, Fix12i fx, u16 k);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern short _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* prev);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void* self, void* pos);
extern void _ZN7fBase_c18MarkForDestructionEv(void* self);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* actor);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, struct Vector3* v, unsigned int d);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern int _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msg, const struct Vector3* pos, unsigned int a, unsigned int b);
extern void _Z14ApproachLinearRsss(short* a, short b, short c);
extern s16 Vec3_VertAngle(const void* a, const void* b);
extern void _ZN6Player12Unk_020c6a10Ej(void* self, u32 a);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern void func_ov073_0211f2c0(void *self, int strength);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern int func_ov073_0211f61c(void *c);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern "C" int _ZN9Animation8FinishedEv(void* anim);
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern int _ZN6Player12GetHurtStateEv(void *self);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern void UnloadKeyModels(int i);
extern Vector3 data_ov073_02123040;
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *self, void *clsn);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *wmc, unsigned int flags);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void func_ov073_021215cc(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
extern void LoadKeyModels(int idx);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov073_0211f2c0, NOT applied: extern void Matrix4x3_FromRotationY(void *m, short angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_0211f2c0, NOT applied: extern void MulVec3Mat4x3(void *in, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_, from the legacy file for func_ov073_0211f494, NOT applied: extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12 x, Fix12 y, Fix12 z); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_, from the legacy file for func_ov073_0211f61c, NOT applied: extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12 x, Fix12 y, Fix12 z); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for func_ov073_0211fa74, NOT applied: extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p, void* pos, void* rot, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_0211fa74, NOT applied: extern void func_02012694(int a, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov073_0211fa74, NOT applied: extern void* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_0211fc78, NOT applied: extern void MulVec3Mat4x3(void* in, void* m, void* out); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_0211fc78, NOT applied: extern void func_02012694(int a, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_0211fc78, NOT applied: extern int ChiefChilly_ChangeState(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov073_0211fc78, NOT applied: extern void* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player9StartTalkER7fBase_cb, from the legacy file for func_ov073_0211fe8c, NOT applied: extern void _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c18HorzAngleToCPlayerEv, from the legacy file for func_ov073_0211fe8c, NOT applied: extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void* actor); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_0211fe8c, NOT applied: extern void MulVec3Mat4x3(void* a, void* m, void* b); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_0211fe8c, NOT applied: extern int ChiefChilly_ChangeState(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov073_0211fe8c, NOT applied: extern void* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_0212000c, NOT applied: extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char*,struct BCA_File&,int,int,int,unsigned short); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_0212005c, NOT applied: extern "C" int ChiefChilly_ChangeState(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_021200e0, NOT applied: extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* self, void* f, int a, int b, int fix, u32 j); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_021200e0, NOT applied: extern void func_02012694(int a, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_021200e0, NOT applied: extern s16 Vec3_HorzAngle(const void* a, const void* b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov073_021200e0, NOT applied: extern void Matrix4x3_FromRotationY(void* m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_021200e0, NOT applied: extern void MulVec3Mat4x3(void* a, void* b, void* c); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_021200e0, NOT applied: extern void ChiefChilly_ChangeState(void* self, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_021203ac, NOT applied: extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_021203ac, NOT applied: extern void _Z14ApproachLinearRsss(short *p, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_021203ac, NOT applied: extern short Vec3_HorzAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov073_021203ac, NOT applied: extern short Vec3_VertAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov073_021203ac, NOT applied: extern void Matrix4x3_FromRotationY(void *m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_021203ac, NOT applied: extern void MulVec3Mat4x3(void *in, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov073_021203ac, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f2c0, from the legacy file for func_ov073_021203ac, NOT applied: extern void func_ov073_0211f2c0(void *self, int v); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_021203ac, NOT applied: extern int func_02012694(int a, void *pos); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_021203ac, NOT applied: extern void ChiefChilly_ChangeState(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_020a0e68, from the legacy file for func_ov073_021203ac, NOT applied: extern int data_020a0e68; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_02120610, NOT applied: extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02120610, NOT applied: extern void _Z14ApproachLinearRsss(short *p, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_02120610, NOT applied: extern short Vec3_HorzAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov073_02120610, NOT applied: extern short Vec3_VertAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov073_02120610, NOT applied: extern void Matrix4x3_FromRotationY(void *m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_02120610, NOT applied: extern void MulVec3Mat4x3(void *in, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov073_02120610, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f2c0, from the legacy file for func_ov073_02120610, NOT applied: extern void func_ov073_0211f2c0(void *self, int v); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_02120610, NOT applied: extern int func_02012694(int a, void *pos); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120610, NOT applied: extern void ChiefChilly_ChangeState(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_020a0e68, from the legacy file for func_ov073_02120610, NOT applied: extern int data_020a0e68; */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02120844, NOT applied: extern int _Z14ApproachLinearRsss(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound8PlayLongEjjjRK7Vector3s, from the legacy file for func_ov073_02120844, NOT applied: extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov073_02120844, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f2c0, from the legacy file for func_ov073_02120844, NOT applied: extern int func_ov073_0211f2c0(); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_02120844, NOT applied: extern int func_02012694(); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120844, NOT applied: extern int ChiefChilly_ChangeState(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound8PlayLongEjjjRK7Vector3s, from the legacy file for func_ov073_02120910, NOT applied: extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, const Vector3 *, unsigned int); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for func_ov073_02120910, NOT applied: extern void Matrix4x3_FromRotationY(void *m, short ang); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov073_02120910, NOT applied: extern void MulVec3Mat4x3(void *v, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_02120910, NOT applied: extern short Vec3_HorzAngle(const void *v0, const void *v1); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02120910, NOT applied: extern void _Z14ApproachLinearRsss(short *, short, short); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120910, NOT applied: extern int ChiefChilly_ChangeState(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_02120ad8, NOT applied: extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov073_02120b78, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f2c0, from the legacy file for func_ov073_02120b78, NOT applied: extern void func_ov073_0211f2c0(void* c, int a); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_02120b78, NOT applied: extern short Vec3_HorzAngle(const void* v0, const void* v1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_02120c08, NOT applied: extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123288, from the legacy file for func_ov073_02120c08, NOT applied: extern P2 data_ov073_02123288; */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120c7c, NOT applied: extern "C" void ChiefChilly_ChangeState(void* c, PMF* p); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov073_02120c7c, NOT applied: extern "C" void _Z14ApproachLinearRiii(int& v, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021233c0, from the legacy file for func_ov073_02120c7c, NOT applied: extern PMF data_ov073_021233c0; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021233d0, from the legacy file for func_ov073_02120c7c, NOT applied: extern PMF data_ov073_021233d0; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123360, from the legacy file for func_ov073_02120c7c, NOT applied: extern PMF data_ov073_02123360; */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02120dec, NOT applied: extern void _Z14ApproachLinearRsss(short *, short, short); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120dec, NOT applied: extern int ChiefChilly_ChangeState(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02120ed0, NOT applied: extern void ChiefChilly_ChangeState(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c18HorzAngleToCPlayerEv, from the legacy file for func_ov073_02120ed0, NOT applied: extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of AngleDiff, from the legacy file for func_ov073_02120ed0, NOT applied: extern int AngleDiff(int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_02120ed0, NOT applied: extern void func_02012694(int a, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov073_02120ed0, NOT applied: extern void _Z14ApproachLinearRiii(int *p, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov073_02120ed0, NOT applied: extern unsigned int RandomIntInternal(void *seed); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02120ed0, NOT applied: extern void _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_, from the legacy file for func_ov073_02120ed0, NOT applied: extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int cc); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for func_ov073_02120ed0, NOT applied: extern void *_ZN8dActor_c13ClosestPlayerEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_02120ed0, NOT applied: extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_02120ed0, NOT applied: extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int frame, u16 flags); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f2c0, from the legacy file for func_ov073_02120ed0, NOT applied: extern void func_ov073_0211f2c0(void *c, int a); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209e650, from the legacy file for func_ov073_02120ed0, NOT applied: extern void *data_0209e650; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt, from the legacy file for func_ov073_0212122c, NOT applied: extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123288, from the legacy file for func_ov073_0212122c, NOT applied: extern P2 data_ov073_02123288; */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_0212128c, NOT applied: extern int ChiefChilly_ChangeState(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov073_0212128c, NOT applied: extern void* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123360, from the legacy file for func_ov073_0212128c, NOT applied: extern void* data_ov073_02123360; */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov073_02121388, NOT applied: extern short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov073_02121388, NOT applied: extern void _Z14ApproachLinearRsss(short* p, short a, short b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov073_02121388, NOT applied: extern void func_02012694(int a, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for func_ov073_02121388, NOT applied: extern int ChiefChilly_ChangeState(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov073_02121388, NOT applied: extern void* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Lsl, from the legacy file for func_ov073_021215cc, NOT applied: extern void Vec3_Lsl(Vec3* d, Vec3* s, int sh); */
/* TUBUILD CONFLICT -- alternate declaration of MulMat4x3Mat4x3, from the legacy file for func_ov073_021215cc, NOT applied: extern void MulMat4x3Mat4x3(void* a, void* b, void* c); */
/* TUBUILD CONFLICT -- alternate declaration of data_020a0e68, from the legacy file for func_ov073_021215cc, NOT applied: extern Mtx43 data_020a0e68; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123280, from the legacy file for _ZN16daKing_Donketu_c16CleanupResourcesEv, NOT applied: extern void* data_ov073_02123280; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123288, from the legacy file for _ZN16daKing_Donketu_c16CleanupResourcesEv, NOT applied: extern void* data_ov073_02123288; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021232a8, from the legacy file for _ZN16daKing_Donketu_c16CleanupResourcesEv, NOT applied: extern void* data_ov073_021232a8; */
/* TUBUILD CONFLICT -- alternate declaration of data_020a0e68, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern Mat4x3 data_020a0e68; */
/* TUBUILD CONFLICT -- alternate declaration of DecIfAbove0_Short, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern unsigned short DecIfAbove0_Short(unsigned short *p); */
/* TUBUILD CONFLICT -- alternate declaration of MulMat4x3Mat4x3, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void MulMat4x3Mat4x3(void *d, void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Lsl, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void func_02012694(int a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c17HugeLandingDustAtER7Vector3b, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *self, Vector3 *v, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromRotationY, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void Matrix4x3_FromRotationY(void *m, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void MulVec3Mat4x3(void *a, void *m, void *out); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov073_0211f61c, from the legacy file for _ZN16daKing_Donketu_c8BehaviorEv, NOT applied: extern void func_ov073_0211f61c(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123280, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_02123280; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021232a0, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_021232a0; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123288, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_02123288; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021232a8, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_021232a8; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123290, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_02123290; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021232b0, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_021232b0; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_021232b8, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_021232b8; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov002_0210da30, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov002_0210da30; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov073_02123298, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern SharedFilePtr data_ov073_02123298; */
/* TUBUILD CONFLICT -- alternate declaration of ChiefChilly_ChangeState, from the legacy file for _ZN16daKing_Donketu_c13InitResourcesEv, NOT applied: extern int ChiefChilly_ChangeState(void* c, PMF* p); */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN16daKing_Donketu_cD1Ev, 0x0211f000, size 0x98 */
/* ROM ordinal 1 -- _ZN16daKing_Donketu_cD0Ev, 0x0211f098, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daKing_Donketu_cD1Ev
// @symbol _ZN16daKing_Donketu_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Three array cleanups, four member destructors and the chain into dEnemyBase_c, all
 * of it reverse declaration order out of daKing_Donketu_c.h. Nothing here is
 * written by hand.
 *
 * The arrays are what took the longest to name. The ROM destroys them with
 * __cxa_vec_cleanup(this + 0x3e8, 8, 0xc, _ZN7Vector3D1Ev), and a POD array needs no
 * cleanup at all -- so the element type had to be a 0xc class with a declared
 * destructor. It is Vector3: InitResources fills each element as x/y/z from
 * the actor's position, and 0x020072c0 is four bytes of `bx lr`, an empty
 * destructor, now named _ZN7Vector3D1Ev in config/arm9/symbols.txt.
 * 0x0207328c carried the invented name __destroy_arr until it was renamed to the
 * compiler's own spelling __cxa_vec_cleanup (PR #1353 identified them as the same
 * entry), which is what the compiler emits here.
 *
 * D0 HAS NO SOURCE OF ITS OWN. ROM ordinal 1 is the DELETING destructor:
 * destroy through this class and its bases -- which is why more than one vptr
 * store appears -- then return the object to its heap, through an inlined
 * operator delete, which is why nothing below mentions one. Nobody writes that
 * separately. mwccarm 2004/b56 emits D1 and D0 from this ONE definition, so
 * ordinal 1's `@symbol` marker is parked here at its ROM ordinal rather than on
 * a second body, and the two legacy shards that each carried an identical empty
 * destructor are both retired by this fold.
 */
daKing_Donketu_c::~daKing_Donketu_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov073_0211f144, 0x0211f144, size 0x17c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211f144
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov073_0211f144(void* self) {
    char* c = (char*)self;

    Vector3 pos, in, out;
    int y;
    ((int*)&pos)[0] = *(int*)(c + 0x5c);
    y = *(int*)(c + 0x60);
    ((int*)&pos)[1] = y;
    ((int*)&pos)[2] = *(int*)(c + 0x64);
    ((int*)&pos)[1] = y + (int)(((long long)*(int*)(c + 0x80) * 0x1e000 + 0x800) >> 12);

    Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3d8));
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x94));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    *(unsigned int*)(c + 0x4f8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4f8), 0x77, pos.x, pos.y, pos.z, 0, 0);
    *(unsigned int*)(c + 0x4fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4fc), 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (*(unsigned int*)(c + 0x4f8) != 0) {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4f8));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
    if (*(unsigned int*)(c + 0x4fc) == 0) return;
    {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4fc));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov073_0211f2c0, 0x0211f2c0, size 0x1d4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211f2c0
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov073_0211f2c0(void *self, int strength)
{
  char *c = (char *)self;
  struct V3 src;
  volatile struct V3 pv;
  struct V3 *new_var;
  struct V3 in;
  struct V3 res;
  struct V3 out;
  struct V3 dv;
  int ty;
  int tx;
  int tz;
  int x;
  int y;
  int z;
  func_0200d8c8(data_0209f318, c + 0x5c, strength);
  data_020a0e68 = *((struct Matrix4x3 *) (c + 0x328));
  MulMat4x3Mat4x3(((char *) (*((void **) (c + 0x320)))) + ((*((int *) (c + 0x4bc))) * 0x30), &data_020a0e68, &data_020a0e68);
  *((int *) ((((int) c) + 0x4bc))) ^= 1;
  ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
  tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
  src.y = ty;
  tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
  src.z = tz;
  src.x = tx;
  Vec3_Lsl(&out, &src, 3);
  x = out.x;
  y = out.y;
  z = out.z;
  src.x = x;
  src.y = y;
  src.z = z;
  if ((*((void **) (c + 0x37c))) == (&data_ov073_02123360))
  {
    dv.x = x;
    dv.y = y;
    dv.z = z;
    _ZN8dActor_c17HugeLandingDustAtER7Vector3b(c, &dv, 1);
    return;
  }
  pv.x = *((int *) (c + 0x5c));
  pv.y = *((int *) (c + 0x60));
  pv.z = *((int *) (c + 0x64));
  {
    unsigned char *b = (unsigned char *) c;
    void *anim = *((void **) (b + 0x37c));
    if (anim == (&data_ov073_021233c0))
    {
      goto do_mtx;
    }
    if (anim != (&data_ov073_021233f0))
    {
      goto spawn;
    }
  }
  do_mtx:
  in.z = 0;

  in.z = -0xe6000;
  in.x = 0;
  in.y = 0;
  res.x = 0;
  res.y = 0;
  res.z = 0;
  new_var = &res;
  Matrix4x3_FromRotationY(&data_020a0e68, *((short *) (c + 0x8e)));
  MulVec3Mat4x3(&in, &data_020a0e68, &res);

  {
    int px = pv.x;
    int rx = (*new_var).x;
    px = px + rx;
    rx = px;
    int pz = pv.z;
    int rz = res.z;
    {
      int t = pz + rz;
      rz = t;
    }
    pv.x = rx;
    pv.z = rz;
  }

  spawn:
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x88, pv.x, pv.y, pv.z);

  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x89, pv.x, pv.y, pv.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov073_0211f494, 0x0211f494, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211f494
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov073_0211f494(void *pa, void *pb)
{
    char *a = (char *)pa;
    char *b = (char *)pb;
    Vec3 t, p, q, d;
    int ay, ax, vx, tmp, cos_ax, sin_ax, cos_ay, sin_ay, rx, ry, rz;
    int y0;
    int *bp = (int *)(((int)b + 0x5c));

    p.x = *(int *)(a + 0x5c);
    y0 = *(int *)(a + 0x60);
    p.y = y0;
    {
        Vec3 *pq = &q;
        int zval = *(int *)(a + 0x64);
        Vec3 *pp = &p;
        pp->z = zval;
        pq->x = bp[0];
        pq->y = bp[1];
        pq->z = bp[2];
        {
            int qy = pq->y + 0x46000;
            int h = *(int *)(a + 0x118);
            vx = *(int *)(a + 0x114);
            pp->y = y0 + h;
            pq->y = qy;
            Vec3_Sub(&d, pq, pp);
        }
    }
    t.x = d.x; t.y = d.y; t.z = d.z;
    ay = _ZN4cstd5atan2E5Fix12IiES1_(t.x, t.z);
    ax = _ZN4cstd5atan2E5Fix12IiES1_(t.y, Vec3_HorzLen(&t));
    ax = (int)((unsigned short)ax >> 4);
    ay = (int)((unsigned short)ay >> 4);
    sin_ax = data_02082214[ax * 2 + 1];
    cos_ax = data_02082214[ax * 2];
    tmp = (int)(((long long)vx * sin_ax + 0x800) >> 12);
    ry = (int)(((long long)vx * cos_ax + 0x800) >> 12);
    cos_ay = data_02082214[ay * 2];
    sin_ay = data_02082214[ay * 2 + 1];
    rx = (int)(((long long)tmp * cos_ay + 0x800) >> 12);
    rz = (int)(((long long)tmp * sin_ay + 0x800) >> 12);
    p.x += rx; p.y += ry; p.z += rz;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x84, p.x, p.y, p.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x85, p.x, p.y, p.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov073_0211f61c, 0x0211f61c, size 0x458 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211f61c
extern "C" s32 func_ov073_0211f61c(void* self)
{
    char* c = (char*)self;
    void* r5;
    s32 r4;
    u32 id;

    if (DecIfAbove0_Short(c + 0x4cc) != 0)
        return 0;
    id = *(u32*)(c + 0x134);
    if (id == 0)
        return 0;
    r5 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r5)
        return 0;

    r4 = 0;
    if (*(s32*)(c + 0x130) & 0x6000) {
        func_ov073_0211f494(c, c);
        *(s32*)(c + 0x98) = 0x20000;
        r4 = 1;
    }

    {
        int isBf = (int)(*(u16*)((char*)r5 + 0xc) == 0xbf);
        if (isBf == 1) {
            if (*(u8*)((char*)r5 + 0x703) != 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x107e0;
                if (flags) {
                    *(s32*)(c + 0x98) = 0x41000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x50380;
                if (flags) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x2d000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                if ((*(s32*)(c + 0x130) & 0x70) || (*(u8*)((char*)r5 + 0x6f9) != 0)
                    || (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, r5) != 0)) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x20000;
                    r4 = 1;
                }
            }
        }
    }

    if (r4 == 0) {
        if (*(s32*)(c + 0x130) & 0x400) {
            func_ov073_0211f494(c, c);
            *(s32*)(c + 0x98) = 0x3d000;
            r4 = 1;
        }
    }

    if (r4 != 0) {
        s32 count;
        s32 i;
        s32 shortY;
        Vector3 v;
        s32 rnd;

        *(u16*)(c + 0x4cc) = 0x10;
        func_02012694(0x16b, c + 0x74);
        rnd = RandomIntInternal(&data_0209e650);
        if (((rnd >> 8) & 7) == 0) {
            v.x = *(s32*)(c + 0x5c);
            v.y = *(s32*)(c + 0x60);
            v.z = *(s32*)(c + 0x64);
            count = (rnd >> 0xc) & 3;
            v.y = v.y + 0x64000;
            if (count == 0)
                count = 1;
            i = 0;
            if (count > 0) {
                do {
                    void* actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                    if (actor != 0) {
                        rnd = RandomIntInternal(&data_0209e650);
                        shortY = ((s32)((((u32)rnd >> 8) & 0xf) << 0x1c)) >> 0x10;
                        *(s16*)((char*)actor + 0x92) = 0;
                        *(s16*)((char*)actor + 0x94) = (s16)shortY;
                        *(s16*)((char*)actor + 0x96) = 0;
                        *(s32*)((char*)actor + 0x98) = 0xa000;
                        if (_ZN8SaveData19IsCharacterUnlockedEj(2) != 0) {
                            *(s32*)(void*)(int)(c + 0x4c0) += 1;
                            if (*(s32*)(c + 0x4c0) > 0x1e) {
                                void* actor2 = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x115, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                                if (actor2 != 0) {
                                    *(s16*)((char*)actor2 + 0x92) = 0;
                                    *(s16*)((char*)actor2 + 0x94) = (s16)shortY;
                                    *(s16*)((char*)actor2 + 0x96) = 0;
                                    *(s32*)((char*)actor2 + 0x98) = 0xa000;
                                }
                                *(s32*)(c + 0x4c0) = 0;
                            }
                        }
                    }
                    i = i + 1;
                } while (i < count);
            }
        }
        {
            void* anim = *(void**)(c + 0x37c);
            if (anim != &data_ov073_021233d0 && anim != &data_ov073_021233a0) {
                ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233a0));
            }
        }
        return 1;
    }

    {
        Vector3 v2;
        v2.x = *(s32*)(c + 0x5c);
        v2.y = *(s32*)(c + 0x60);
        v2.z = *(s32*)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v2, 0, 0x14000, 1, 0, 1) == 0)
            goto done0;
    }
    {
        s32* pv = (s32*)((u32)r5 + 0x5c);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8a, pv[0], pv[1], pv[2]);
    }
    if (*(void**)(c + 0x37c) == &data_ov073_021233d0) {
        func_02012694(0x16b, c + 0x74);
        return 1;
    }
    {
        s16* py = (s16*)((u32)c + 0x94);
        *py = (s16)(*py + 0x8000);
    }
    func_02012694(0x16a, c + 0x74);
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a0[1], 4, 0x40000000, 0x1000, 0);
    *(s32*)(c + 0x368) = 0x1000;
    *(u8*)(c + 0x4c8) = 2;
    *(s32*)(c + 0x4d0) = 0x2000;
    *(s16*)(c + 0x100) = 0x1e;
done0:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov073_0211fa74, 0x0211fa74, size 0x178 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fa74
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fa74(char* c) {
    void* cam;
    void* spawned;
    void* found;
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    _Z14ApproachLinearRiii((int*)(c + 0x80), 0, 0x80);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    func_ov073_0211f144(c);
    if (*(int*)(c + 0x80) >= 0x100) goto end;
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);
    _ZN7Message7EndTalkEv();
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_02011cfc();
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    pos.y = pos.y + 0x190000;
    func_0200fa8c(c, 1);

    {
        unsigned short a = *(unsigned short*)(c + 0x8c);
        unsigned short b = *(unsigned short*)(c + 0x8e);
        rot.x = b ? a : a;
        rot.y = b;
        rot.z = *(unsigned short*)(c + 0x90);
        rot.y = (unsigned short)_ZN8dActor_c18HorzAngleToCPlayerEv(c);
    }

    spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, 4, &pos, 0, *(signed char*)(c + 0xcc), -1);
    found = _ZN8dActor_c15FindWithActorIDEjPS_(0x13d, 0);
    func_02012694(0xbb, c + 0x74);
    if (found != 0) {
        struct Vector3 fp;
        int pv = (int)(((int)found + 0x5c));
        fp.x = *(int*)pv;
        fp.y = *(int*)(pv + 4);
        fp.z = *(int*)(pv + 8);
        _ZN8dActor_c10PoofDustAtERK7Vector3(c, &fp);
        _ZN7fBase_c18MarkForDestructionEv(found);
    }
    if (spawned != 0) {
        *(int*)(((int)cam + 0x154)) &= ~8;
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
end:
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov073_0211fbec, 0x0211fbec, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fbec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fbec(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov073_0211fbf4, 0x0211fbf4, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fbf4
extern "C" {
extern void func_ov073_0211f144(void*);
int func_ov073_0211fbf4(char* c){
  void* pl = *(void**)(c+0x3e4);
  *(unsigned int*)(c+0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c+0x500), 3, 0x170, (struct Vector3 *)(c+0x74), 0);
  _ZN6Camera9SetFlag_3Ev(data_0209f318);
  func_ov073_0211f144(c);
  if(_ZN6Player12GetTalkStateEv(pl) == -1){
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123370));
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov073_0211fc70, 0x0211fc70, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fc70
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fc70(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov073_0211fc78, 0x0211fc78, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fc78
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fc78(char* c) {
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    void* player;
    void* cam;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);
    if (player == 0) return 1;

    func_ov073_0211f144(c);

    {
        int pv = (int)((int)player + 0x5c);
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    cam = data_0209f318;
    msgpos[1].x = *(int*)(c + 0x5c);
    msgpos[1].y = *(int*)(c + 0x60);
    msgpos[1].z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = *(int*)(c + 0x3d8);
    la.y = *(int*)(c + 0x3dc);
    la.z = *(int*)(c + 0x3e0);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    if (player != 0) {
        int msg;
        *(void**)(c + 0x3e4) = player;
        msg = (short)(*(int*)((char*)*(void**)(c + 0x3e4) + 8) + 0xe7);
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        _ZN7Message11PrepareTalkEv();
        if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x3e4), c, 1)) {
            _ZN6Camera9SetFlag_3Ev(cam);
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3e4), c, msg, &msgpos[1], 0, 2)) {
                func_02012694(0x12a, (void*)(c + 0x74));
                ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_02123410));
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov073_0211fe84, 0x0211fe84, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fe84
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fe84(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov073_0211fe8c, 0x0211fe8c, size 0x180 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0211fe8c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0211fe8c(char* c) {
    struct Vector3 look, pos, in, out;
    void* player;
    void* cam;

    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player != 0 && *(unsigned char*)((char*)player + 0x6de) == 0) {
        int angle;
        _ZN6Player9StartTalkER7fBase_cb(player, c, 1);
        angle = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
        *(short*)((char*)player + 0x8c) = 0;
        *(short*)((char*)player + 0x8e) = angle + 0x8000;
        *(short*)((char*)player + 0x90) = 0;
    }

    func_ov073_0211f144(c);

    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = *(int*)(c + 0x5c);
    look.y = *(int*)(c + 0x60);
    look.z = *(int*)(c + 0x64);
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    pos.y -= 0x100000;
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);

    if (*(unsigned short*)(c + 0x100) == 0) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_021233e0));
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov073_0212000c, 0x0212000c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0212000c
extern "C" short func_ov073_0212000c(char *c){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123280[1], 4, 0, 0x1000, 0);
    *(short*)(c+0x100)=0x32;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov073_0212005c, 0x0212005c, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0212005c
extern "C" int func_ov073_0212005c(char* c){
  if(((Animation*)(c+0x35c))->Finished()){
    *(unsigned char*)(c+0x4c5)=0xff;
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov073_02120098, 0x02120098, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120098
struct BCA_File;
extern "C" {
int func_ov073_02120098(char* c){
  *(char*)(c+0x4ca) = 0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov073_021200e0, 0x021200e0, size 0x2b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_021200e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_021200e0(u8* thiz)
{
    u16 state = *(u16*)(thiz + 0x100);
    if (state != 0) {
        if (state == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(thiz + 0x30c, data_ov073_021232b0[1], 4, 0x40000000, 0x1000, 0);
            *(int*)(thiz + 0x368) = 0x1000;
            *(int*)(thiz + 0xa8) = 0x46000;
            *(u8*)(thiz + 0x4c5) = 0xff;
            *(int*)(thiz + 0x9c) = -0x3000;
            if (*(u8*)(thiz + 0x4ca) != 0) {
                func_02012694(0x16f, thiz + 0x74);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (*(int*)(thiz + 0x4b4) == 0) {
        Vec3 offset;
        Vec3 rotated;
        s16 horz;
        _Z14ApproachLinearRsss((short*)(thiz + 0x8c), 0, 0x2000);
        horz = Vec3_HorzAngle((const Vector3 *)(thiz + 0x5c), (const Vector3 *)(thiz + 0x3d8));
        Vec3_VertAngle((const void*)(thiz + 0x5c), (const void*)(thiz + 0x3d8));
        _Z14ApproachLinearRsss((short*)(thiz + 0x94), horz, 0x800);
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (*(u8*)(thiz + 0x4cb) == 2) {
            offset.z = 0x32000;
        } else {
            offset.z = 0x1e000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, horz);
        MulVec3Mat4x3(&offset, &data_020a0e68, &rotated);
        *(int*)(thiz + 0xa4) = rotated.x;
        *(int*)(thiz + 0xac) = rotated.z;
        if (*(int*)(thiz + 0xa8) < 0) {
            int id = *(int*)(thiz + 0x134);
            if (id != 0) {
                void* actor = _ZN8dActor_c10FindWithIDEj((u32)id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(*(u16*)((u8*)actor + 0xc) == 0xbf);
                    if (eq != FALSE) {
                        Vec3 pos = *(Vec3*)((u8*)actor + 0x5c);
                        if (*(int*)(thiz + 0x60) > pos.y) {
                            _ZN6Player12Unk_020c6a10Ej(actor, 1);
                        }
                    }
                }
            }
        }
    }
    /* 0x1a8 */
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x150) != 0) {
        if (*(int*)(thiz + 0x4b4) == 0) {
            *(int*)(thiz + 0x98) = 0;
            *(int*)(thiz + 0xa4) = 0;
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0xac) = 0;
            func_ov073_0211f2c0(thiz, 0x1388000);
            func_02012694(0x16d, thiz + 0x74);
        }
        if (*(u8*)(thiz + 0x4cb) == 1 && *(u8*)(thiz + 0x4ca) == 0) {
            *(u8*)(thiz + 0x4c5) = 0;
            if (*(int*)(thiz + 0x4b4) == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(thiz + 0x30c, data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
            }
            *(int*)(((int)thiz + 0x4b4)) += 1;
            if (*(int*)(thiz + 0x4b4) < 0x82) {
                return 1;
            }
        }
        ChiefChilly_ChangeState((C *)(thiz), (PMF *)(data_ov073_02123380));
    }
    *(s16*)(thiz + 0x8e) = *(s16*)(thiz + 0x94);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov073_02120390, 0x02120390, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120390
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120390(char *p)
{
    *(int *)(p + 0x4b4) = 0;
    *(short *)(p + 0x100) = 10;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov073_021203ac, 0x021203ac, size 0x244 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_021203ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_021203ac(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int *src;
    int v[3];

    if (*(unsigned short *)(c + 0x100) == 0)
        goto mainblock;

    if (*(unsigned short *)(c + 0x100) == 1) {

        int neg = 0x5000;
        *(int *)(c + 0xa8) = 0x5a000;
        *(int *)(c + 0x9c) = -neg;
        {
            unsigned char *p = (unsigned char *)(c + 0x4c4);
            *p = *p + 1;
            *p = *p & 7;
        }

        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0, 0x1000, 0);
        *(int *)(c + 0x368) = 0x2000;
    }
    return 1;

mainblock:
    if (*(unsigned char *)(c + 0x4cb) == 2) {
        src = (int *)(c + 0x448 + *(unsigned char *)(c + 0x4c4) * 0xc);
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    } else {
        src = (int *)(c + 0x3e8 + *(unsigned char *)(c + 0x4c4) * 0xc);
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    }
    {
        short hz = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)v);
        short vt = Vec3_VertAngle(c + 0x5c, v);
        int in[3];
        int out[3];
        _Z14ApproachLinearRsss((short *)(c + 0x94), hz, 0x800);
        in[0] = 0; in[1] = 0; in[2] = 0;
        out[0] = 0; out[1] = 0; out[2] = 0;
        in[2] = 0x50000;
        Matrix4x3_FromRotationY(&data_020a0e68, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
        MulVec3Mat4x3(in, &data_020a0e68, out);
        *(int *)(c + 0xa4) = out[0];
        *(int *)(c + 0xac) = out[2];
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x150) != 0) {
        *(unsigned char *)(c + 0x4c5) = *(unsigned char *)(c + 0x4c4);
        *(int *)(c + 0x98) = 0;
        *(int *)(c + 0xa4) = 0;
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0xac) = 0;
        func_ov073_0211f2c0(c, 0x7d0000);
        func_02012694(0x16d, c + 0x74);
        {
            int *cnt = (int *)(c + 0x4b8);
            *cnt = *cnt + 1;
        }
        if (*(int *)(c + 0x4b8) > 7) {
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123340));
        } else {
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123320));
        }
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov073_021205f0, 0x021205f0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_021205f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_021205f0(char *p)
{
    *(int *)(p + 0x368) = 0;
    *(int *)(p + 0x4b4) = 0;
    *(short *)(p + 0x100) = 2;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov073_02120610, 0x02120610, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120610
extern "C" {  /* .c-derived member: C linkage for the whole block */
    int func_ov073_02120610(void *thiz)
    {
        unsigned char *c = (unsigned char *)thiz;
        int *src;
        int v[3];

        if (*(unsigned short *)(c + 0x100) == 0)
            goto mainblock;

        if (*(unsigned short *)(c + 0x100) == 1) {
            *(int *)(c + 0xa8) = 0x5a000;
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0, 0x1000, 0);
            *(int *)(c + 0x368) = 0x2000;
        }
        return 1;

    mainblock:
        _Z14ApproachLinearRsss((short *)(c + 0x8c), 0, 0x2000);
        if (*(unsigned char *)(c + 0x4cb) == 2) {
            src = (int *)(c + 0x448 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        } else {
            src = (int *)(c + 0x3e8 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        }
        {
            short hz = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)v);
            short vt = Vec3_VertAngle(c + 0x5c, v);
            int in[3];
            int out[3];
            in[2] = 0;
            in[0] = 0; in[1] = 0; out[0] = 0; out[1] = 0; out[2] = 0;
            in[2] = 0x28000;
            Matrix4x3_FromRotationY(&data_020a0e68, hz);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
            MulVec3Mat4x3(in, &data_020a0e68, out);
            *(int *)(c + 0xa4) = out[0];
            if (*(int *)(c + 0x4b4) != 0 || *(int *)(c + 0xa8) < 0) {
                *(int *)(c + 0xa8) = out[1];
                *(int *)(c + 0x4b4) = 1;
            }
            *(int *)(c + 0xac) = out[2];
        }
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x150) != 0) {
            *(unsigned char *)(c + 0x4c5) = *(unsigned char *)(c + 0x4c4);
            *(int *)(c + 0x98) = 0;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            func_ov073_0211f2c0(c, 0x7d0000);
            func_02012694(0x16d, c + 0x74);
            *(int *)(c + 0x4b8) = 1;
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123320));
        }
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        return 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov073_0212081c, 0x0212081c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0212081c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0212081c(char* self)
{
    *(unsigned short*)(self + 0x100) = 0xa;
    *(int*)(self + 0x4b4) = 0;
    *(int*)(self + 0x4b8) = 0;
    *(unsigned char*)(self + 0x4c5) = 0xff;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov073_02120844, 0x02120844, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120844
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120844(int *t)
{
    _Z14ApproachLinearRsss((short *)((char*)t + 0x8c), -0x4000, 0x400);
    t[0x140] = _ZN5Sound8PlayLongEjjjRK7Vector3s(t[0x140], 3, 0x170, (struct Vector3 *)((char*)t + 0x74), 0);
    if (t[0xf7] > t[0x18] && _ZNK10dBgCh_Actr10IsOnGroundEv((char*)t + 0x150)) {
        func_ov073_0211f2c0(t, 0x7d0000);
        func_02012694(0x16c, (char*)t + 0x74);
        t[0x26] = 0;
        ChiefChilly_ChangeState((C *)(t), (PMF *)(data_ov073_021233b0));
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov073_021208e4, 0x021208e4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_021208e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_021208e4(char *r0) {
    *(int *)(r0 + 0x98) = 0x14000;
    *(int *)(r0 + 0xa8) = 0x1e000;
    *(int *)(r0 + 0x9c) = -0x3000;
    *(int *)(r0 + 0x500) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov073_02120910, 0x02120910, size 0x1c8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120910
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120910(char *c)
{
    Vector3 in;
    Vector3 out;

    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;

    if (*(unsigned short *)(c + 0x100) == 0) {
        *(c + 0x4cb) = 1;
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123340));
        *(c + 0x4ca) = 1;
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        return 1;
    }

    *(unsigned int *)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(c + 0x500), 3, 0x16e, (Vector3 *)(c + 0x74), 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(c + 0x8e) + 0x8000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    in.x = *(int *)(c + 0x4a8) + out.x;
    in.y = *(int *)(c + 0x4ac);
    in.z = *(int *)(c + 0x4b0) + out.z;
    Vec3_ApproachHorz(c + 0x5c, &in, 0x14000);
    *(short *)(c + 0x8e) = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x3d8));
    *(short *)(c + 0x94) = *(short *)(c + 0x8e) + 0x8000;
    *(int *)(((int)c + 0x4b4)) += 0x500;
    _Z14ApproachLinearRsss((short *)(c + 0x8c),
        ((s64)*(int *)(c + 0x4d0) * data_02082214[((unsigned short)(short)*(int *)(c + 0x4b4) >> 4) * 2] + 0x800) >> 12,
        0x400);
    if (func_ov073_0211f61c(c)) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233f0));
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov073_02120ad8, 0x02120ad8, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120ad8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120ad8(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x12d] = 0;
    t[0x27] = 0;
    t[0x26] = 0;
    t[0x29] = 0;
    t[0x2a] = 0;
    t[0x2b] = 0;
    t[0x134] = -0x1000;
    t[0x12a] = t[0x13b];
    t[0x12b] = t[0x13c];
    t[0x12c] = t[0x13d];
    t[0x140] = 0;
    *(short *)((char *)t + 0x100) = 0x5a;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov073_02120b78, 0x02120b78, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120b78
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120b78(char* c){
    _Z14ApproachLinearRsss((short*)(c+0x8c), -0x4000, 0x400);
    if(*(int*)(c+0x3dc) > *(int*)(c+0x60)){
        if(_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x150)){
            func_ov073_0211f2c0(c, 0xfa0000);
            *(int*)(c+0x98) = 0;
            *(short*)(c+0x94) = Vec3_HorzAngle((const Vector3 *)(c+0x5c), (const Vector3 *)(c+0x3d8));
            *(short*)(c+0x8e) = *(short*)(c+0x94);
            func_02012694(0x16c, c+0x74);
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123400));
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov073_02120c08, 0x02120c08, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120c08
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120c08(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    t[0x26] = 0x14000;
    t[0x2a] = 0x1e000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x27] = -0x3000;
    t[0x12d] = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov073_02120c7c, 0x02120c7c, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120c7c
extern "C" int func_ov073_02120c7c(CB* c)
{
    int a = c->field_98; if (a < 0) a = -a;
    if (a > 0xa) {
        int i = 0;
        CB* p = c;
        do {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(p->field_4d4, p->field_4d8, p->field_4dc);
            i++;
            p = (CB*)((char*)p + 0xc);
        } while (i < 2);
    }
    if (c->field_4c9 == 1) {
        *(unsigned char*)((int)c + 0x4cb) -= 1;
        if (c->field_4cb != 0)
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233c0));
        else
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233d0));
        return 1;
    }
    _Z14ApproachLinearRiii(&c->field_98, 0, 0x1000);
    if (_ZN9Animation8FinishedEv(&c->anim_35c)) {
        int b = c->field_98; if (b < 0) b = -b;
        if (b < 0xa) {
            c->field_98 = 0;
            c->field_94 = c->field_8e;
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov073_02120d80, 0x02120d80, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120d80
extern "C" {

int func_ov073_02120d80(char *c)
{
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0x40000000, fix, t);
    *(int *)(c + 0x368) = fix;
    ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    *(short *)(c + 0x94) = ang;
    *(short *)(((int)c + 0x94)) =
        (short)((int)*(short *)(((int)c + 0x94)) + 0x8000);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov073_02120dec, 0x02120dec, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120dec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120dec(char *c) {
    _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x4c6), 0x500);
    if (AngleDiff(*(short *)(c + 0x4c6), *(short *)(c + 0x8e)) < 0x100) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
        *(short *)(c + 0x100) = 0x1e;
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov073_02120e60, 0x02120e60, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120e60
extern "C" {
int func_ov073_02120e60(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
  *(int*)(c+0x368) = 0x2000;
  *(short*)(c+0x4c6) = Vec3_HorzAngle((const Vector3 *)(c+0x5c), (const Vector3 *)(c+0x3d8));
  *(int*)(c+0x4b4) = 0;
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov073_02120ed0, 0x02120ed0, size 0x35c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02120ed0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02120ed0(void *self)
{
    u8 *c = (u8 *)self;

    if (c[0x4c9] == 1) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123390));
        return 1;
    }

    switch (c[0x4c8]) {
    case 0:
        if (*(u16 *)(c + 0x100) == 0) {
            *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
            *(int *)(c + 0x4b4) = 0;
            if (AngleDiff(*(s16 *)(c + 0x94), *(s16 *)(c + 0x4c6)) <= 0x2000) {
                *(s16 *)(c + 0x100) = 0x1e;
                break;
            } else {
                *(s16 *)(c + 0x100) = 0;
                *(int *)(c + 0x4d0) = 0x1000;
                (*(u8 *)(((int)c + 0x4c8)))++;
                func_02012694(0x169, c + 0x74);
                break;
            }
        } else {
            _Z14ApproachLinearRiii((int *)(c + 0x98), 0x1e000, 0x3000);
            if (*(int *)(c + 0x4b4) == 0) {
                if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x1f4000) {
                    *(int *)(c + 0x4b4) = 1;
                    if ((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x18) & 7) == 0) {
                        c[0x4c8] = 3;
                        *(int *)(c + 0x98) = 0;
                        *(s16 *)(c + 0x100) = 0xf;
                    }
                }
            }
            _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x4c6), 0x1d0);
            *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
            *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
            *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
            break;
        }

    case 3:
        if (*(u16 *)(c + 0x100) == 0) {
            *(int *)(c + 0x4d0) = 0x1000;
            c[0x4c8] = 1;
        }
        break;

    case 1:
    case 2:
    {
        int d;
        int i;
        u8 *r5;
        *(int *)(c + 0x4b4) = 0;
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d > 0xa) {
            r5 = c;
            i = 0;
            do {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    *(int *)(r5 + 0x4d4), *(int *)(r5 + 0x4d8), *(int *)(r5 + 0x4dc));
                i++;
                r5 += 0xc;
            } while (i < 2);
        }
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0, *(int *)(c + 0x4d0));
        if (c[0x4c8] == 1) {
            *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
            _Z14ApproachLinearRsss((s16 *)(c + 0x8e), *(s16 *)(c + 0x4c6), 0x500);
        }
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d >= 0xa) break;
        if (*(u16 *)(c + 0x100) != 0) break;
        if (c[0x4c8] == 2) {
            u8 *p = (u8 *)_ZN8dActor_c13ClosestPlayerEv(c);
            if (p != 0) {
                int t;
                if (_ZN6Player12GetHurtStateEv(p) == 4) goto hz;
                if (_ZN6Player12GetHurtStateEv(p) == 5) goto hz;
                t = p[0x709] ? 1 : 0;
                if (t == 1) {
hz:
                    *(s16 *)(c + 0x4c6) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3d8));
                } else {
                    *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
                }
            }
        }
        *(int *)(c + 0x98) = 0;
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        *(s16 *)(c + 0x100) = 0x1e;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
        *(int *)(c + 0x368) = 0x2000;
        *(int *)(c + 0x4b4) = 0;
        c[0x4c8] = 0;
        break;
    }
    }

    if (c[0x4c8] == 0) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x35c, 0) != 0 ||
            _ZNK9Animation12WillHitFrameEi(c + 0x35c, 0xe) != 0) {
            func_ov073_0211f2c0(c, 0x3e8000);
            func_02012694(0x168, c + 0x74);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov073_0212122c, 0x0212122c, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0212122c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0212122c(int *t)
{
    *(short *)((char *)t + 0x100) = 0;
    *(char *)((char *)t + 0x4c8) = 0;
    t[0x134] = 0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x2000;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov073_0212128c, 0x0212128c, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_0212128c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_0212128c(char* c)
{
    struct Vector3 la;
    struct Vector3 ps;
    void* cam;
    void* player;

    cam = data_0209f318;
    player = *(void**)(c + 0x3e4);
    _ZN6Camera9SetFlag_3Ev(cam);

    la.x = *(int*)(c + 0x5c);
    la.y = *(int*)(c + 0x60);
    la.z = *(int*)(c + 0x64);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    if (_ZN6Player12GetTalkStateEv(player) == -1) {
        *(int*)(((int)cam + 0x154)) &= ~8;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d08();
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov073_02121378, 0x02121378, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02121378
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02121378(char *p)
{
    p[1224] = 0; return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov073_02121388, 0x02121388, size 0x1b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02121388
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov073_02121388(char* c) {
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    void* player;
    void* cam;
    int msg;
    char* p;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player == 0) return 1;

    {
        int pv = (int)((int)player + 0x5c);
        vplayer.x = *(int*)pv;
        vplayer.y = *(int*)(pv + 4);
        vplayer.z = *(int*)(pv + 8);
    }
    cam = data_0209f318;
    vmsg.x = *(int*)(c + 0x5c);
    vmsg.y = *(int*)(c + 0x60);
    vmsg.z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    la.x = *(int*)(c + 0x5c);
    la.y = *(int*)(c + 0x60);
    la.z = *(int*)(c + 0x64);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    vmsg.y = vmsg.y + 0x64000;
    _Z14ApproachLinearRsss((short*)(c + 0x94), Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &vplayer), 0x800);

    *(void**)(c + 0x3e4) = player;
    p = *(char**)(c + 0x3e4);
    msg = (short)(*(int*)(p + 8) + 0xe3);
    if (_ZN6Player9StartTalkER7fBase_cb(p, c, 1)) {
        if (*(unsigned char*)(c + 0x4c8) == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
            *(unsigned char*)(c + 0x4c8) = 0;
        }

        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3e4), c, msg, &vmsg, 0, 2)) {
            func_02012694(0x12a, c + 0x74);
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123350));
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov073_02121538, 0x02121538, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_02121538
extern "C" {
int func_ov073_02121538(char *c){
  *(char*)(c+0x4c8)=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_021232b8[1], 4, 0, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- ChiefChilly_ChangeState, 0x0212157c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol ChiefChilly_ChangeState
/* `struct C` and PMF are the reconciled ones declared at the top of this TU;
   this shard's own identical copy is gone with the merge. */
extern "C" int ChiefChilly_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov073_021215cc, 0x021215cc, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov073_021215cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov073_021215cc(void* self)
{
    char* c = (char*)self;
    int sh;
    Vec3 v;
    Vec3 out;
    void* m;
    Mtx43* saved;
    int i;
    Vec3* p;
    char* q;

    Vec3_Asr(&v, (Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

    p = (Vec3*)(c + 0x4d4);
    *(struct Matrix4x3*)(c + 0x328) = data_020a0e68;
    saved = (Mtx43*)(c + 0x328);
    sh = 3;
    i = 0;
    q = c;
    for (; i < 2; i++) {
        data_020a0e68 = *(struct Matrix4x3 *)saved;
        MulMat4x3Mat4x3(*(Mtx43**)(c + 0x320) + (i + 2), &data_020a0e68, &data_020a0e68);
        p->x = data_020a0e68.m[9];
        p->y = data_020a0e68.m[10];
        p->z = data_020a0e68.m[11];
        Vec3_Lsl(&out, p, sh);
        *(int*)(q + 0x4d4) = out.x;
        *(int*)(q + 0x4d8) = out.y;
        *(int*)(q + 0x4dc) = out.z;
        p++;
        q += 0xc;
    }

    m = *(void**)(c + 0x37c);
    if (m == (void*)data_ov073_021233b0) return;
    if (m == (void*)&data_ov073_021233e0) return;
    if (m == (void*)&data_ov073_02123410) return;
    if (m == (void*)data_ov073_021233f0) return;
    if (m == (void*)data_ov073_02123370) return;

    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xa000) >> 3, *(int*)(c + 0x64) >> 3);
    *(struct Matrix4x3*)(c + 0x3a8) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x380, c + 0x3a8, 0x12c000, 0x3e8000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- _ZN16daKing_Donketu_c16CleanupResourcesEv, 0x021217e0, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daKing_Donketu_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases NINE shared files and unloads the key models -- by far the largest
 * claim of any class migrated so far, and one of the nine (data_ov002_0210da30)
 * lives in ov002 rather than this overlay, borrowed from the shared pool and
 * still released here.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daKing_Donketu_c::CleanupResources(){
  UnloadKeyModels(4);
  ((SharedFilePtr *)data_ov073_02123280)->Release();
  ((SharedFilePtr *)data_ov073_021232a0)->Release();
  ((SharedFilePtr *)data_ov073_02123288)->Release();
  ((SharedFilePtr *)data_ov073_021232a8)->Release();
  ((SharedFilePtr *)data_ov073_02123290)->Release();
  ((SharedFilePtr *)data_ov073_021232b0)->Release();
  ((SharedFilePtr *)data_ov073_021232b8)->Release();
  ((SharedFilePtr *)data_ov002_0210da30)->Release();
  ((SharedFilePtr *)data_ov073_02123298)->Release();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- _ZN16daKing_Donketu_c16OnPendingDestroyEv, 0x0212186c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daKing_Donketu_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty in the ROM -- a single `bx lr`, an override that exists to suppress
 * the base's behaviour.
 */
void daKing_Donketu_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- _ZN16daKing_Donketu_c6RenderEv, 0x02121870, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daKing_Donketu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daKing_Donketu_c::Render()
{
  ((Base *)&mBlendModelAnim)->M((char*)&mScaleX);
  return 1;
}
