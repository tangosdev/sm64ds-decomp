/* HOST COPIES for TREASURE_CHEST (13, ov064, "11daObjTbox_c"), run rel0215
 * wave 3, lane w3-c -- the two pointer-to-member dispatchers, the receiver
 * face for state 0's tick, and the seat of the six SOURCE records.
 *
 * ================= THE BODY THE DECOMP DID NOT HAVE ======================
 *
 * It has it now, and this header's own measurement is the half that went
 * stale.  What it said (run rel0215 wave 3, repeating run linkw wave 12's
 * hal/actor_classes_ov064_w12.cpp): "one of the six records,
 * _ZN13TreasureChest6State0Ev (state 0's tick, 540 bytes at 0x0211a4c4), HAS
 * NO MATCHED TU anywhere in src/ and is in no port host copy", supported by
 * "config/arm9/overlays/ov064/delinks.txt has NO block for it -- the TU
 * before it, src/_ZN13TreasureChest10InitState1Ev.cpp, ends at 0x0211a4c4 and
 * the next, src/_ZN13TreasureChest10InitState0Ev.cpp, starts at 0x0211a6e0."
 *
 * RE-MEASURED at run link100 wave 15 (lane SEAT15A) on this tip:
 *   - src/_ZN13TreasureChest6State0Ev.cpp EXISTS, from 6e10cbbd1 ("Migrate
 *     TreasureChest states to real C++ class form", PR #1805), a real C++
 *     method body on include/TreasureChest.h like its five siblings.
 *   - config/arm9/overlays/ov064/delinks.txt line 475 DOES carry its block.
 *   - config/arm9/overlays/ov064/symbols.txt still names it
 *     kind:function(arm,size=0x21c) addr:0x0211a4c4, unchanged.
 * So the transcription below is retired for the matched TU, which rides
 * port/slice_l15stale.txt, and what stays here is the receiver face the seat
 * needs.  The rest of this header's ROM derivation is untouched and still
 * true: it is what the retired transcription was checked against.
 *
 *   - the ONLY reference to 0x0211a4c4 anywhere in ov064's relocs is the data
 *     word at 0x0211c4bc, one of the six { function, 0 } source pairs, and
 *     __sinit_ov064_0211b59c's own disassembly stores that pair at DEST+0x08 --
 *     Entry[0].tick. _ZN13TreasureChest8SetStateEi calls Entry[idx].pmf[0] (the enter)
 *     and _ZN13TreasureChest17CallStateBehaviorEv calls Entry[idx].pmf[1] (the tick), and the
 *     chest's idx (this+0x16c) is bss zero until something sets it. So it is
 *     state 0's tick and it is on the first Behavior frame of every chest.
 *
 * A LOUD FACE WAS NOT AVAILABLE when the transcription was written, which is
 * why it existed rather than the class being declined. The tree has a
 * precedent for facing a missing state body -- GOOMBOSS (198, ov074,
 * func_ov074_021201f0) is seated with the class registered and quarantines on
 * its first Behavior frame, carried by a tools/battery.py LEVEL_SKIPS row.
 * That route would have needed a skip on THREE levels (8, 9 and 18, the
 * levels whose spawn lists carry id 13), and the campaign plan's step 15 says
 * a lane that needs a skip has not finished. The matched TU closes the
 * question: there is no skip and no transcription.
 *
 * THE RETIRED TRANSCRIPTION was taken instruction by instruction out of
 * extracted/overlays/overlay_0064.bin at base 0x02115ee0 (T4 -- ov064 is
 * compressed:true and the dsd export is the wrong image). 135 instructions,
 * 0x0211a4c4..0x0211a6e0. Every call it makes is resolved through
 * config/arm9/overlays/ov064/relocs.txt and every one of the nine was ALREADY
 * in build/port/walk_window.map, checked rather than assumed:
 *     0x0203adbc DecIfAbove0_Short          0x02012790 func_02012790
 *     0x02010f3c Actor::FindWithID          0x0203b7ac Vec3_HorzAngle
 *     0x0203b0e8 AngleDiff                  0x02010ef0 Actor::FindWithActorID
 *     0x0200f97c Actor::SpawnSoundObj       0x02012694 func_02012694
 *     0x020d5a1c Player::Shock              0x0211a6ec (this file, below)
 * The matched TU calls the same nine through their MSVC member mangles, and
 * those were re-checked in this tree's map before the swap.
 *
 * WHAT IT IS. The chest-order puzzle -- four chests on level 8, three on level
 * 18, one on level 9. Each chest carries an order number in this+0x172 (the low
 * byte of its spawn parameter) and the player must open them in that order.
 * Each frame in state 0 the tick counts the chest's own timer down, plays sound
 * 14 when it reads 0x58, and once the timer expires and the chest has a linked
 * actor id in +0x15c, finds that actor, requires it to be a PLAYER (id 0xbf),
 * and requires the player to be within a quarter turn of the chest's facing.
 * Then it walks every id-13 actor, counting all of them in `total` and the
 * OTHER chests already in state 1 or 2 in `opened`. If opened + 1 equals this
 * chest's own order number the open is correct -- if every chest is now open it
 * spawns the sound object and sets +0x173, otherwise it plays sound 0x26 -- and
 * the chest enters state 1. If it does NOT match, the chest re-arms its timer
 * to 0x5a, shocks the player, and puts every OTHER id-13 chest back to state 0.
 *
 * ================= THE TWO DISPATCHERS ====================================
 *
 * _ZN13TreasureChest8SetStateEi and _ZN13TreasureChest17CallStateBehaviorEv each form an mwcc
 * pointer-to-member over a FORWARD-DECLARED struct, so MSVC hands them the
 * general worst-case-inheritance representation where the ROM has a plain
 * { function, 0 } pair; dispatching that mangles `this`. The KnockDownPlank /
 * MontyMole / Scuttlebug case, the same shape as gate 179's two Piece
 * dispatchers. Both also index the table, which MSVC strides at its own
 * sizeof(PMF) rather than the ROM's 8 -- the second half of the same fault.
 * Read here as a plain { fn, 0 } array of three sixteen-byte Entries.
 *
 * TreasureChest::Behavior and its Render are NOT here: Behavior is a plain
 * three-call body that reaches the state machine only through
 * _ZN13TreasureChest17CallStateBehaviorEv (so it stays in the slice), and Render is the ModelAnim
 * slot-5 shadow, host-copied in unmatched/Ov064_Clam.cpp where that file's own
 * header says it belongs.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };
struct PortEntry { PortPmf pmf[2]; };      /* 16 bytes, the ROM's own stride */
struct PortVec3 { int x, y, z; };

/* the shared ring, every one already linked before this lane */
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_02012790(int id);
void func_02012694(int id, void *pos);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, void *prev);
void _ZN8dActor_c13SpawnSoundObjEj(void *self, unsigned a);
short Vec3_HorzAngle(const void *v0, const void *v1);
int AngleDiff(int a, int b);
void _ZN6Player5ShockEj(void *self, unsigned a);

/* the five state bodies that DO have matched TUs (slice_w3c.txt) */
int _ZN13TreasureChest6State2Ev(void *c);   /* Entry[2].tick  */
int _ZN13TreasureChest10InitState2Ev(void *c);   /* Entry[2].enter */
int _ZN13TreasureChest6State1Ev(void *c);   /* Entry[1].tick  */
int _ZN13TreasureChest10InitState1Ev(void *c);   /* Entry[1].enter */
int _ZN13TreasureChest10InitState0Ev(void *c);   /* Entry[0].enter */

/* the six SOURCE records the sinit copies from (data, { fn, 0 }) */
extern PortPmf data_ov064_0211c49c[];   /* {0211a2c4, 0} -> Entry[2].tick  */
extern PortPmf data_ov064_0211c4a4[];   /* {0211a39c, 0} -> Entry[1].tick  */
extern PortPmf data_ov064_0211c4ac[];   /* {0211a380, 0} -> Entry[2].enter */
extern PortPmf data_ov064_0211c4b4[];   /* {0211a49c, 0} -> Entry[1].enter */
extern PortPmf data_ov064_0211c4bc[];   /* {0211a4c4, 0} -> Entry[0].tick  */
extern PortPmf data_ov064_0211c4c4[];   /* {0211a6e0, 0} -> Entry[0].enter */

/* the RUNTIME table, three 16-byte Entries, filled by __sinit_ov064_0211b59c */
extern PortEntry data_ov064_0211c98c[3];

int _ZN13TreasureChest6State0Ev(void *self);

/* the arm9 body the receiver face below forwards into. Its matched TU is
   src/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp, already in
   port/slice_gate32.txt -- it compiles into the build today and the linker
   discards it for want of a reference, which is why the base map does not
   carry it. */
void *_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
        void *self, signed char *trackStarID, unsigned starID,
        const void *spawnPos, unsigned char howToSpawnStar);

}  /* extern "C" */

/* PORT_HOST_ABI: a RECEIVER FACE, not an alias. src/_ZN13TreasureChest6State2Ev.cpp
   declares Actor::UntrackAndSpawnStar as a real C++ method returning void and
   calls it on `this`, so it emits the __thiscall member mangle
   `?UntrackAndSpawnStar@dActor_c@@QAEXAACIABUVector3@@I@Z` with the receiver in
   ecx. The definition in the tree is the Itanium C name, a __cdecl body whose
   FIRST STACK ARGUMENT is self. An /alternatename between the two is exactly
   the receiver-ABI fault the campaign keeps measuring -- ecx would be ignored
   and the four stack arguments would be read one slot low -- so this is a real
   __thiscall definition that moves the receiver and forwards.

   It cannot be written against include/Actor.h: line 340 declares the same
   method returning `Actor *`, a different mangle and not a legal overload of
   this one. Hence the local declaration, in a file that includes no headers.
   The ov034 lane's `?_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h@@YAX...@Z`
   alias is NOT this case: that caller spelled it as a free function taking
   void* first, which is already __cdecl and needs no face. */
struct Vector3 { int x, y, z; };
struct Actor {
    void UntrackAndSpawnStar(signed char &trackStarID, unsigned starID,
                             const Vector3 &spawnPos, unsigned howToSpawnStar);
};
void Actor::UntrackAndSpawnStar(signed char &trackStarID, unsigned starID,
                                const Vector3 &spawnPos,
                                unsigned howToSpawnStar)
{
    _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
        this, &trackStarID, starID, &spawnPos,
        (unsigned char)howToSpawnStar);
}

/* BOTH DISPATCHERS ARE BACK ON THE SLICE -- src/_ZN13TreasureChest8SetStateEi.cpp and
   src/_ZN13TreasureChest17CallStateBehaviorEv.cpp, port/slice_pmf3.txt (run link100 lane PMF3).
   /vmg /vmm gives MSVC the ROM's 8-byte {function, delta} record, so Entry is
   the ROM's 0x10 and both bodies TAIL JUMP through the record's own function
   word; the mangled table reference is bridged in port/hal/pmf3_aliases.cpp.
   The declaration below is what the transcribed body further down calls.

   THE ONE GUARD THAT LEAVES WITH THEM: the host copies tested `if (fn)` and
   SILENTLY SKIPPED the dispatch. The ROM does not test. A silent skip is not
   something a green battery can vouch for, so this row rests on the seat
   instead: port_treasure_chest_states_seat verifies all six records against
   the ROM's own addresses, aborts on a nonzero adjustment word, and rewrites
   every function word with a host body -- including _ZN13TreasureChest6State0Ev, the
   body transcribed in this file -- before a chest can exist. The six source
   pairs at ov064 0x0211c49c..0x0211c4c4 were re-read out of overlay_0064.bin
   with their relocations, every adjustment word ROM zero. */
extern "C" void _ZN13TreasureChest8SetStateEi(void *self, int i);

/* ============ THE TRANSCRIBED BODY IS RETIRED ==============================
   run link100 wave 15, lane SEAT15A -- the stale-banner harvest, round 2.

   The 0x21c-byte transcription that stood here was tagged "No matched TU
   exists anywhere in the tree; this is transcribed from the overlay image",
   and this file's header carried the measurement behind it: "delinks.txt has
   NO block for it".  Both were true when written.  On this tip
   src/_ZN13TreasureChest6State0Ev.cpp exists (6e10cbbd1, "Migrate
   TreasureChest states to real C++ class form", PR #1805) and
   config/arm9/overlays/ov064/delinks.txt line 475 DOES carry its block.  The
   TU is on port/slice_l15stale.txt now, which holds the whole derivation.

   WHAT REPLACES IT IS A RECEIVER FACE, NOT AN ALIAS.  MSVC compiles the
   matched TU to ?State0@TreasureChest@@QAEXXZ, a __thiscall member taking the
   receiver in ecx.  The seat's thunk tc_st0_pmf below calls the FLAT C name
   with the receiver on the stack, exactly as it does for the other five
   records, and those five are answered by hal/faces_sync_gen.cpp's generated
   flat faces (port/faces_sync.txt rows 979-985).  An /alternatename between
   the two names would be the ecx-versus-stack fault this file's
   UntrackAndSpawnStar note already measures, so this is the same generated
   shape written out by hand: the class is re-declared locally because this
   file includes no headers, and `State0` is public and non-virtual in
   include/TreasureChest.h, which is what fixes the Q-and-AE mangle.

   THE `if (fn)` GUARD DOES NOT COME BACK.  The seat still verifies all six
   ROM words and aborts on wrong bytes; what changes is that record 0's tick
   is now the ROM's own body rather than a transcription of it. */
struct TreasureChest {
    public: void State0();
};
extern "C" int _ZN13TreasureChest6State0Ev(void *self)
{
    ((TreasureChest *)self)->TreasureChest::State0();
    return 0;
}

/* ---- the seat ------------------------------------------------------------ */

/* ---- RUN link100 LANE PMFSWEEP: THE SIX SEATED WORDS TAKE THEIR RECEIVER IN
   ECX, so they are __fastcall thunks and not the flat C faces themselves.
   Both dispatchers are back on the slice, and BOTH pass the receiver in ecx
   and push nothing. Read off this build's own image:

     ?SetState@TreasureChest@@QAEXH@Z  +0x1f
       mov  dword ptr [eax+0x16c], edx                  ; mState = state
       shl  edx, 4                                      ; the ROM's 0x10 stride
       mov  ecx, dword ptr [edx+_data_ov064_0211c98c+4] ; the adjustment word
       add  ecx, eax                                    ; this + delta
       mov  eax, dword ptr [edx+_data_ov064_0211c98c]   ; the code word
       call eax                                         ; A REAL CALL
     ?CallStateBehavior@TreasureChest@@QAEXXZ  +0x19
       mov  ecx, dword ptr [edx+_data_ov064_0211c98c+12]
       add  ecx, eax
       mov  eax, dword ptr [edx+_data_ov064_0211c98c+8]
       jmp  eax

   The header above rules a raw body correct because the dispatchers tail jump
   and leave the caller's argument in place. That is true of a FLAT C
   dispatcher whose first stack argument is the receiver, and false of both of
   these: they are __thiscall members of TreasureChest, so at the jmp there is
   nothing above the return address but the caller's own frame, and SetState's
   [ebp+8] is the state INDEX, not a receiver. Five of the six cells held
   hal/faces_sync_gen.cpp's flat C face,

       __ZN13TreasureChest10InitState0Ev:
         push ebp / mov ebp,esp / mov ecx,[ebp+8] / pop ebp / jmp InitState0

   and the sixth held this file's own __ZN13TreasureChest6State0Ev, a cdecl
   body reading [ebp+8]. This is 5ae983797's correction at another class, the
   one 27a24ff5a, 651b5e853, f9936e798, 45ce69707 and 00732a5ab each had to
   make one class over. Each thunk NAMES its matched body or face, so trap T2's
   rule still holds, and the seat's abort-on-wrong-bytes check is untouched. */
static int __fastcall tc_is0_pmf(void *self, void *)
{ return _ZN13TreasureChest10InitState0Ev(self); }
static int __fastcall tc_st0_pmf(void *self, void *)
{ return _ZN13TreasureChest6State0Ev(self); }
static int __fastcall tc_is1_pmf(void *self, void *)
{ return _ZN13TreasureChest10InitState1Ev(self); }
static int __fastcall tc_st1_pmf(void *self, void *)
{ return _ZN13TreasureChest6State1Ev(self); }
static int __fastcall tc_is2_pmf(void *self, void *)
{ return _ZN13TreasureChest10InitState2Ev(self); }
static int __fastcall tc_st2_pmf(void *self, void *)
{ return _ZN13TreasureChest6State2Ev(self); }

static const struct { PortPmf *slot; unsigned rom; int (__fastcall *host)(void *, void *); }
g_treasure_chest_states[] = {
    {data_ov064_0211c4c4, 0x0211a6e0, tc_is0_pmf},  /* Entry[0].enter */
    {data_ov064_0211c4bc, 0x0211a4c4, tc_st0_pmf},  /* Entry[0].tick  */
    {data_ov064_0211c4b4, 0x0211a49c, tc_is1_pmf},  /* Entry[1].enter */
    {data_ov064_0211c4a4, 0x0211a39c, tc_st1_pmf},  /* Entry[1].tick  */
    {data_ov064_0211c4ac, 0x0211a380, tc_is2_pmf},  /* Entry[2].enter */
    {data_ov064_0211c49c, 0x0211a2c4, tc_st2_pmf},  /* Entry[2].tick  */
};

extern "C" void port_treasure_chest_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_treasure_chest_states /
                             sizeof g_treasure_chest_states[0]; ++i) {
        PortPmf *p = g_treasure_chest_states[i].slot;
        if (p->fn != g_treasure_chest_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: TreasureChest source %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_treasure_chest_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_treasure_chest_states[i].host;
    }
}
