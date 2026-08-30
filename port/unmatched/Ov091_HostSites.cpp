/* ov091 host sites -- the FIVE translation units of the ARROW_PATH_LIFT (157) /
 * SQUARE_METAL_NET_LIFT (144) / FWOOSH (231) seat that cannot ride from src/.
 * Run rel0215 wave 3, lane w3-f2. Every refusal is itemised in
 * port/slice_w3f2.txt; this file is the transcription.
 *
 * The byte-matched originals STAY in src/ and are not edited. Each body below
 * is its src/ TU line for line, with only the named substitution applied, and
 * every substituted name is settled from that function's OWN relocation in
 * config/arm9/overlays/ov091/relocs.txt read against the raw bytes in
 * extracted/overlays/overlay_0091.bin -- never from a dsd label.
 *
 * (1) THREE POINTER-TO-MEMBER DISPATCH SITES (gate 16, the ov090 shape).
 *     The ROM's state record is EIGHT bytes, {function, delta}, and every
 *     delta in this overlay is 0. MSVC has no representation that matches it:
 *       * a PMF over a COMPLETE single-inheritance class is FOUR bytes, so an
 *         array of them strides half the ROM's record and index 1 reads
 *         record 0's delta word (RotatingUpDownPlatform::Behavior, and the
 *         `Holder { char pad[8]; PMF fn; }` view in Stump::Behavior);
 *       * a PMF over an INCOMPLETE class is the sixteen-byte unknown-
 *         inheritance form (func_ov091_02134044's `struct C;` typedef);
 *       * and either way the call is __thiscall, while the bodies the records
 *         name are flat cdecl C functions in this link.
 *     All three read the record's fn word directly and call it with an
 *     explicit self. hal/actor_classes_ov091.cpp's port_ov091_states_seat()
 *     has already rewritten that word from the ROM's DS address to the host
 *     body's, verifying the mounted word against the ROM first.
 *
 * (2) ONE ModelAnim SLOT-5 COLLISION, FOUND BY RUNNING IT. The banked recon
 *     ruled "Render LINKS PLAIN because _ZTV5Model is dual-filled -- do not
 *     host-copy it". That is TRUE OF THE LIFTS and FALSE OF FWOOSH, and the
 *     difference is which member the shadow sits over:
 *       RotatingUpDownPlatform::Render shadows the plain Model at +0xd4 (its
 *         D1 calls _ZN5ModelD1Ev on +0xd4), and hal/cxxname_bridge.cpp
 *         DUAL-FILLS _ZTV5Model at [4] AND [5], so it serves from src/ and
 *         does. Levels 39 and 37 run 300 frames rc 0 with it sliced.
 *       Stump::Render shadows the ModelAnim at +0x300 (Fwoosh_Spawn calls
 *         _ZN9ModelAnimC1Ev on +0x300), and _ZTV9ModelAnim is filled in MSVC
 *         numbering where slot 5 is Virtual18 and Render has moved. NOT
 *         dual-filled, so index 5 lands on the wrong body -- the ov090 /
 *         Whomp / Fish / Spiny case (T1).
 *     The first run of level 22 with the seat in faulted on frame 1 with
 *     c0000005 accessing 00000000, and the frames resolve exactly through the
 *     collision: Stump::Render+0x26 -> ModelAnim::Virtual18 ->
 *     ModelAnim::Virtual10 -> Model::Virtual10. So this host copy names
 *     ModelAnim::Render outright, the ov090 remedy. Offsets are Stump.h's own:
 *     mVariant 0x374, unk_0b0 0xb0, mModelAnim 0x300. The scale argument is a
 *     NULL Vector3, read off the source's literal `m5(0)` -- FWOOSH passes
 *     none, the way MantaRay, CheepCheep and Shark do and Skeeter does not.
 *
 * (3) ONE BODY PROPAGATED FROM origin/main BY ADDRESS, and this is the item
 *     the banked recon, this lane and the coordinator's first two rulings all
 *     got wrong in the same direction. src/func_ov091_021339fc.c IN THIS TREE
 *     is NONMATCHING-bannered with an ARM asm hatch MSVC cannot parse, so it
 *     cannot ride from src/. The first instinct was to transcribe the draft;
 *     the correct move was to check the address against main, where the body
 *     is ALREADY MATCHED, with no banner and no hatch. It is re-gated in this
 *     tree (match.py MATCHING at 2004/b56 --strict-relocs --module ov091) and
 *     carried with five names bridged by address. The long version is at the
 *     site. FWOOSH's Behavior calls it every tick and nothing else reaches it.
 */
#include <cstddef>
#include "types.h"
#include "common.h"
/* NOT decl_common.h. It carries `extern void func_ov091_02134044(void *, void *)`
   at line 2914 while the src TU this file replaces defines it returning int, and
   including both is C2556/C2371. The int is the faithful one -- the ROM body
   returns the dispatched state's own r0 -- so the shared header is left out
   rather than the return type changed. Every name this file needs from the
   decl_* family comes from the three below, and none of them includes
   decl_common.h. */
#include "decl_Enemy.h"
#include "decl_Player.h"
#include "decl_SaveData.h"
#include "ModelAnim.h"

/* The ROM's pointer-to-member SOURCE record, as bytes rather than as a C++
   pointer-to-member. Nine of these are seated by port_ov091_states_seat(). */
struct Ov091PmfRec { unsigned int fn; unsigned int adj; };

extern "C" {

/* ---- arm9 / ov002 leaves, each named by its caller's own relocation ------- */
void func_020393d4(void *p, void *v);                       /* 0x020393d4 */
void _Z14ApproachLinearRiii(int *cur, int target, int rate); /* 0x0203ae58 */
void _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void); /* 0x0203923c */
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);     /* ov002 0x020ee830 */
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b); /* ov002 0x020ee870 */
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);       /* ov002 0x020ee7cc */

int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *wm); /* ov002 0x020ade78 */
void _ZN12CylinderClsn5ClearEv(void *self);                 /* 0x02015024 */
void _ZN12CylinderClsn6UpdateEv(void *self);                /* 0x02014ff0 */
unsigned short DecIfAbove0_Short(unsigned short *p);        /* 0x0203adbc */
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *c); /* 0x02010d40 */
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, unsigned j); /* ov002 0x020aebf8 */
void _ZN9Animation7AdvanceEv(void *self);                   /* 0x02015c3c */

void *_ZN5Actor10FindWithIDEj(unsigned int id);             /* 0x02010f3c */
/* 0x020aea30, and the spelling matters -- see the shared-window note at (3).
   This link defines BOTH _func_ov002_020aea30 and _func_020aea30 from the same
   object (port/unmatched/Enemy_UpdateDeath.cpp); main's body calls the ov002
   spelling, so that is the one declared. */
void func_ov002_020aea30(void *c, void *a, unsigned int unused);
void _ZN5Actor8PoofDustEv(void *a);                         /* 0x0200fe3c */
void _ZN9ActorBase18MarkForDestructionEv(void *a);          /* 0x02043824 */
void _ZN6Player16IncMegaKillCountEv(void *p);               /* ov002 0x020bdc58 */
void func_02012694(int a, void *p);                         /* 0x02012694 */
int _ZN6Player15IsCollectingCapEv(void *p);                 /* ov002 0x020bea94 */
void _ZN6Player18SetNewHatCharacterEjjb(void *p, unsigned a, unsigned b, unsigned c2); /* ov002 0x020be0f8 */
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned id, unsigned flags,
        struct Vector3 *pos, short *rot, int a, int b);     /* 0x02010e2c */

/* ---- the ov091 leaves these four call ------------------------------------ */
void func_ov091_02134094(char *c);
void func_ov091_021339fc(char *c);

/* ---- the mounted state cells --------------------------------------------- */
extern unsigned int data_ov091_021354e0[];   /* three records, 157/144 */

}  /* extern "C" */

/* ==========================================================================
 * (1a) RotatingUpDownPlatform::Behavior -- ids 157 and 144.
 *
 * src/_ZN22RotatingUpDownPlatform8BehaviorEv.cpp, transcribed. THREE
 * substitutions, and TWO of them are names that are DEFINED NOWHERE IN THIS
 * LINK, so the src TU could not have been sliced even if the pointer-to-member
 * were representable:
 *
 *   data_ov091_021354e0[old].pmf   -> the record's fn word, called with self.
 *                                     The mounted pool word at 0x02132204 is
 *                                     the table's own address, so this is the
 *                                     same three records the seat rewrote.
 *   UpdatePosWithVelocitySym       -> _ZN16MeshColliderBase21UpdatePosWithVelocity...
 *                                     A PLACEHOLDER: declared in
 *                                     include/decl_common.h:1967 and defined
 *                                     nowhere. The TU's own literal pool at
 *                                     0x02132208 reads 0x0203923c, which
 *                                     config/arm9/symbols.txt:1403 names
 *                                     MeshColliderBase::UpdatePosWithVelocity
 *                                     -- the same symbol the two sibling state
 *                                     bodies (func_ov091_02131ef0 and
 *                                     _02131f9c, both sliced) already spell in
 *                                     full and which this link defines.
 *   ApproachLinearI                -> _Z14ApproachLinearRiii. The other
 *                                     placeholder (decl_common.h:1896, defined
 *                                     nowhere); the call site at 0x021321a8
 *                                     relocates to 0x0203ae58, which
 *                                     config/arm9/symbols.txt:1508 names
 *                                     ApproachLinear(int&, int, int).
 *
 * The three Platform methods are called by their FLAT ROM names with an
 * explicit self (0x020ee830 / 0x020ee870 / 0x020ee7cc), the level_boot.cpp:4940
 * shape, rather than through the local six-line `struct Platform` shadow the
 * src TU declares -- that shadow mangles to `?UpdateModelPosAndRotY@Platform@@`
 * against a class this file does not define.
 * ========================================================================== */
extern "C" int _ZN22RotatingUpDownPlatform8BehaviorEv(void *self)
{
    char *s = (char *)self;
    int old = *(int *)(s + 0x320);

    {
        Ov091PmfRec *rec = (Ov091PmfRec *)data_ov091_021354e0;
        ((void (*)(void *))(size_t)rec[old].fn)(s);
    }

    *(unsigned short *)(s + 0x354) += 1;
    if (old != *(int *)(s + 0x320)) {
        *(short *)(s + 0x354) = 0;
        func_020393d4(s + 0x124, 0);
    } else {
        func_020393d4(s + 0x124,
            (void *)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    }
    if (*(unsigned char *)(s + 0x352) == 0) {
        int rate = 0x5000;
        int saved = *(int *)(s + 0x60);
        _Z14ApproachLinearRiii((int *)(s + 0x34c),
                               (*(unsigned char *)(s + 0x356) != 0) ? 0x1e000 : 0,
                               rate);
        *(int *)(s + 0x60) -= *(int *)(s + 0x34c);
        *(int *)(s + 0x60) = saved;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(s);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(s, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(s);
    *(unsigned char *)(s + 0x356) = 0;
    return 1;
}

/* ==========================================================================
 * (1b) func_ov091_02134044 -- FWOOSH's state ENTER setter.
 *
 * src/func_ov091_02134044.cpp is four lines:
 *   struct C; typedef int (C::*PMF)();
 *   struct C { char pad[0x364]; PMF *pp; };
 *   func_ov091_02134044(C *c, PMF *p) { c->pp = p; PMF *q = c->pp;
 *                                       if (*q == 0) return 1; return (c->**q)(); }
 * The typedef is formed while C is INCOMPLETE, so MSVC gives PMF the
 * sixteen-byte unknown-inheritance representation and `*q == 0` compares four
 * words. Here it stores the record-array pointer at +0x364 the way the ROM
 * does and dispatches record[0]'s fn word with an explicit self.
 * ========================================================================== */
extern "C" int func_ov091_02134044(void *c, void *p)
{
    *(void **)((char *)c + 0x364) = p;
    Ov091PmfRec *q = (Ov091PmfRec *)*(void **)((char *)c + 0x364);
    if (q->fn == 0)
        return 1;
    return ((int (*)(void *))(size_t)q->fn)(c);
}

/* ==========================================================================
 * (1c) Stump::Behavior -- FWOOSH's tick half.
 *
 * src/_ZN5Stump8BehaviorEv.cpp reaches the same cell through
 *   struct Holder { char pad[8]; PMF fn; };
 *   Holder *q = *(Holder **)(c + 0x364);  if (q->fn != 0) (this->*(q->fn))();
 * i.e. record[1]'s fn word -- the TICK half, exactly the ov090 record-0-enter /
 * record-1-tick split. Everything else is the src body line for line, with
 * every leaf named by that body's own relocation.
 * ========================================================================== */
extern "C" int _ZN5Stump8BehaviorEv(void *self)
{
    char *c = (char *)self;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x144) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov091_02134094(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        Ov091PmfRec *recs = *(Ov091PmfRec **)(c + 0x364);
        if (recs[1].fn != 0)
            ((void (*)(void *))(size_t)recs[1].fn)(c);
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    {
        int v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int hi = *(int *)(c + 0xa0);
        if (v >= hi)
            hi = v;
        int tmp = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = hi;
        *(int *)(c + 0xac) = tmp;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, (void *)(c + 0x110));
    func_ov091_021339fc(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);

    if (*(int *)(c + 0x374) == 1) {
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
        return 1;
    }

    _ZN9Animation7AdvanceEv(c + 0x350);
    func_ov091_02134094(c);
    return 1;
}

/* ==========================================================================
 * (2) Stump::Render -- the ModelAnim slot-5 collision, host-copied after the
 * first level-22 run faulted through it. src/_ZN5Stump6RenderEv.cpp draws with
 *     struct V { virtual void m0(); ... virtual int m5(int); };
 *     ((V *)((char *)&mModelAnim))->m5(0);
 * and index 5 of the host _ZTV9ModelAnim is Virtual18. This names
 * ModelAnim::Render, the body the ROM's slot 5 holds (arm9 0x020167f8).
 * ==========================================================================*/
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch. */
extern "C" int _ZN5Stump6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x374) == 1)
        return 1;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;
    ((ModelAnim *)(c + 0x300))->ModelAnim::Render((const Vector3 *)0);
    return 1;
}

/* ==========================================================================
 * (3) func_ov091_021339fc -- FWOOSH's per-frame player interaction.
 *
 * THIS BODY IS PROPAGATED FROM origin/main BY ADDRESS. It is not a
 * transcription and not this tree's draft.
 *
 * WHAT THIS TREE HAS AT 0x021339fc IS STALE. src/func_ov091_021339fc.c here is
 * NONMATCHING-bannered ("hand-written asm, not a C decompilation ... Reverts to
 * a draft until someone reproduces the bytes from real C") and carries an ARM
 * `asm { ldr curHat0,[a,#8]; mov newHat,#1 }` hatch that MSVC's x86 inline
 * assembler will not parse -- eight errors, C2065 through C2181 -- so it cannot
 * ride from src/ at all. THE BANNER IS STALE AGAINST MAIN: checked by address,
 * origin/main's delinks.txt carries `.text start:0x021339fc end:0x02133c6c` for
 * src/func_ov091_021339fc.c and its blob (31897fa47d0d96862ba8ea690ed7060a3b80c507,
 * against this tree's bf322550ca9c83bfcd394344c3ce31e6a784cc77) has NO banner and
 * NO hatch. Someone cracked the wall by restructuring: main hoists
 *     u8 capFlag = *(u8 *)(a + 0x6ff);
 *     u8 hat     = *(u8 *)(a + 0x6d9);
 * together before the two byte tests, which is exactly the scheduling the draft
 * needed the hatch to force.
 *
 * RE-GATED IN THIS TREE, not taken on main's word. tools/match.py on main's
 * blob at 0x021339fc size 0x270, 2004/b56, --strict-relocs (default) and
 * --module ov091, against extracted/overlays/overlay_0091.bin at base
 * 0x02130f00: MATCHING, every one of the 155 instructions and the one pool
 * word. Evidence: ...runs/rel0215/out/w3-f2/f021339fc_fidelity.txt.
 *
 * FIVE NAMES ARE BRIDGED, EACH SETTLED BY ADDRESS against both trees' config,
 * because main renamed the actor hierarchy to its ROM RTTI spellings and this
 * tree has not taken that rename. Left column is main's, right is what THIS
 * link defines at the same address:
 *   0x02010f3c  _ZN8dActor_c10FindWithIDEj        -> _ZN5Actor10FindWithIDEj
 *   0x0200fe3c  _ZN8dActor_c8PoofDustEv           -> _ZN5Actor8PoofDustEv
 *   0x02043824  _ZN7fBase_c18MarkForDestructionEv -> _ZN9ActorBase18MarkForDestructionEv
 *   0x020adb40  _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc
 *                                                 -> _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc
 *   0x02010e2c  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as
 *                                                 -> _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii
 * The last pair also disagrees about the LAST TWO PARAMETERS -- main recovered
 * them `as` (signed char, short) and this tree `ii` -- and under C linkage the
 * mangled string IS the symbol, so the call must spell this tree's. It is
 * ABI-identical anyway: cdecl promotes both narrow arguments to int, and the
 * ROM passes them in a stacked word each (`str r5,[sp,#0]` / `str r4,[sp,#4]`).
 *
 * A SIXTH NAME NEEDED NO BRIDGE AND IS THE ONE WORTH WRITING DOWN.
 * main's TU calls `func_ov002_020aea30`, and 0x020aea30 is a SHARED-WINDOW
 * address that names two different functions:
 *     this tree  ov002/symbols.txt:26  func_ov002_020aea30            size 0x8c
 *     this tree  ov004/symbols.txt:50  _ZN5Enemy12KillByAttackER5Actor size 0x48
 * and main's config attaches the name `func_ov004_020aea30` to the ov002 body.
 * The ov091 relocation itself says `to:0x020aea30 module:overlays(2,4)` -- dsd
 * declining to choose. ov002 is the resident one at level time, and this link
 * already defines `_func_ov002_020aea30` (port/unmatched/Enemy_UpdateDeath.cpp),
 * so main's spelling resolves here unchanged and no bridge is written. Reading
 * the address alone would have picked KillByAttack, which is a different
 * function of a different size.
 * ==========================================================================*/
extern "C" void func_ov091_021339fc(char *c)
{
    char *a;
    u32 fl;
    u32 id = *(u32 *)(c + 0x134);

    if (id == 0)
        return;
    a = (char *)_ZN5Actor10FindWithIDEj(id);
    if (*(u32 *)(c + 0x374) == 0) {
        fl = *(u32 *)(c + 0x130);
        if ((fl & 0x40000) != 0) {
            *(u32 *)(c + 0x10c) = 4;
            func_ov002_020aea30(c, a, 0);
            return;
        }
        if ((fl & 0x2000) != 0) {
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        {
            int b = (int)(*(u16 *)(a + 0xc) == 0xbf);
            if (b == 0)
                return;
        }
        if (*(u8 *)(a + 0x6f9) == 1) {
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        if ((fl & 0x10) == 0)
            return;
        _ZN5Actor8PoofDustEv(c);
        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(c, a, 0);
        _ZN6Player16IncMegaKillCountEv(a);
        func_02012694(0x1d, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
        return;
    }

    {
        int b = (int)(*(u16 *)(a + 0xc) == 0xbf);
        if (b == 0)
            return;
    }
    if (*(u8 *)(a + 0x6f9) != 0)
        return;
    if (*(u8 *)(a + 0x703) != 0)
        return;
    if (_ZN6Player15IsCollectingCapEv(a) != 0)
        return;
    _ZN6Player8BlowAwayEs(a, *(s16 *)(c + 0x94));
    {
        u8 capFlag = *(u8 *)(a + 0x6ff);
        u8 hat = *(u8 *)(a + 0x6d9);
        if (capFlag != 0)
            return;
        if (*(u8 *)(a + 0x6fd) != 0)
            return;
        {
            u32 cur = *(u32 *)(a + 8);
            u32 param = 1;
            if (hat != cur) {
                _ZN6Player18SetNewHatCharacterEjjb(a, hat, 0, 0);
            } else {
                if (_ZN8SaveData16HasPlayerLostCapEv() != 0)
                    return;
                _ZN8SaveData13PlayerLoseCapEv();
            }
            {
                u32 curHat1 = *(u32 *)(a + 8);
                Vector3_16 rot;
                void *spawned;
                rot.x = 0;
                rot.y = 0;
                rot.z = 0;
                rot.y = *(s16 *)(c + 0x94);
                param = param | (curHat1 << 8);
                /* (short *)&rot: this tree recovered Spawn's fourth parameter
                   as `short *` and main as `Vector3_16 *`, which is the same
                   three consecutive s16 under two spellings -- the ROM passes
                   one pointer either way. The narrow fifth and sixth arguments
                   promote to int under cdecl, which is why this tree's `ii`
                   mangling is ABI-identical to main's `as`. */
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d, param, (Vector3 *)(c + 0x5c), (short *)&rot,
                    *(s8 *)(c + 0xcc), -1);
                if (spawned == 0)
                    return;
                *(u32 *)((char *)spawned + 0x98) = 0x32000;
                *(u32 *)((char *)spawned + 0xa4) = 0;
                *(u32 *)((char *)spawned + 0xa8) = 0x14000;
                *(u32 *)((char *)spawned + 0xac) = 0;
            }
        }
    }
}
