// Run link100 wave 9c, lane LINK21: the rows the last wall needed that no
// generator emits. This is port/hal/int4_rows.cpp's shape one wave on, and the
// same rule applies: every alias here is a NAME bridge whose two sides were
// read out of the link's own objects with dumpbin, never derived from a
// filename, and every host body says what it stands in for and why.

#include <cstdio>
#include <cstring>

#include "dsstate_seg.h"

// =========================================================================
// 1. Sound::PlayBank2_2D, the one name the two bannered drafts bring with them
// =========================================================================
//
// src/_ZN12dScMgSlot1_c8BehaviorEv.cpp, taken verbatim from origin/main, spells
// its own forward declaration as
//
//     namespace Sound { void PlayBank2_2D(unsigned int); }
//
// so it references ?PlayBank2_2D@Sound@@YAXI@Z. The body in this link is
// src/_ZN5Sound12PlayBank2_2DEj.cpp on port/slice_mg1.txt line 504, whose own
// spelling returns the handle:
//
//     ?PlayBank2_2D@Sound@@YAII@Z
//
// A NAME BRIDGE AND NOT AN ABI BRIDGE, on the standing test. Both sides are
// __cdecl (YA) with one unsigned int argument (I) and no receiver at all. The
// only difference is the return: void against unsigned int, and a cdecl caller
// that declares void simply never reads EAX. That is the same reading
// port/hal/fader_wipes.cpp:112 makes for func_0203ae58 against
// _Z14ApproachLinearRiii, and the same shape port/hal/cxx_aliases.cpp:3239
// already uses for the flat spelling of this very function:
//
//     /alternatename:__ZN5Sound12PlayBank2_2DEj=?PlayBank2_2D@Sound@@YAII@Z
//
// The alias rather than a host body, for that row's stated reason: a target
// which one day compiles a translation unit declaring the void form strongly
// keeps its own definition and nothing collides.
#pragma comment(linker, "/alternatename:?PlayBank2_2D@Sound@@YAXI@Z=?PlayBank2_2D@Sound@@YAII@Z")

// =========================================================================
// 2. THREE ALIAS ROWS THAT HAD BEEN READ AS SEATS
// =========================================================================
//
// port/hal/faces4_rows.cpp:94-99 refused four static-or-free rows with one
// sentence: admissible in shape, and the ROM body is not in the link, so all of
// them are seat rows rather than alias rows. Re-measured here against a freshly
// built universe of walk_window's own link inputs (49918 defined externals over
// 8687 objects, tmp/universe.txt), three of the four are wrong: the body IS in
// the link, under the decoration its own owning translation unit emits. The
// fourth, ?SetIRQs@IRQ@@SAII@Z, really was a seat and section 3 seats it.
//
// All three pass the admissibility rule port/hal/cxx_aliases.cpp:3142 states: a
// flat or cdecl reference may be aliased onto a free function or a static
// member, never onto a QAE non-static member. Both sides of every row below are
// YA or SA, so there is no receiver and no calling convention to disagree about.
//
//   ?Spawn@Actor@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z
//       The one referrer is port/unmatched/Bully_AfterClsn.cpp:22, whose own
//       shadow spells the class Actor and the last two arguments int, int. The
//       body is src/_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as.cpp on
//       port/slice_gate10.txt:127, which emits
//       ?Spawn@dActor_c@@SAPAU1@IIABUVector3@@PBUVector3_16@@CF@Z and is already
//       the right hand side of port/hal/cxx_aliases.cpp:3168, so it is certainly
//       linked. Both sides are SA, cdecl with no receiver, and the cartridge's
//       char and short arguments occupy a full four-byte cdecl slot each on the
//       host, so the six pushes are the same six pushes. The caller at
//       Bully_AfterClsn.cpp:56 passes *(s8 *)(self + 0xcc) and -1, values that
//       are already correct in a widened slot.
//
//   ?_ZN3G2S13GetBG0CharPtrEv@@YAPAHXZ
//       Not a class member at all: a free function literally NAMED
//       _ZN3G2S13GetBG0CharPtrEv, which is what
//       src/_ZN11dScMgBase_c9Virtual88Eiiii.cpp:36 gets for declaring the flat
//       ROM name `extern int*` instead of extern "C". The body is
//       src/_ZN3G2S13GetBG0CharPtrEv.cpp on port/slice_gate25.txt:34, emitting
//       ?GetBG0CharPtr@G2S@@YAIXZ. The return types differ, int* against
//       unsigned int, and that is the same four bytes in eax; the tree already
//       ships exactly this pair twice, port/hal/scene_mg_amida.cpp:673 and
//       port/hal/scene_mg_booseek.cpp:201, each aliasing a pointer-returning
//       spelling of this same function onto the unsigned one.
//
//   ?_ZN4cstd4sqrtEy@@YAH_K@Z
//       The same flat-name-at-C++-linkage shape, from four referrers, and the
//       cleanest row of the three: the signatures are identical, int return and
//       one unsigned long long argument, so only the name differs. The body is
//       generated from src/_ZN4cstd4sqrtEy.cpp through GATE8_SYMS
//       (port/CMakeLists.txt:1122) because it drives the DS hardware square root
//       through raw MMIO and needs hostgen's routing. It emits
//       ?sqrt@cstd@@YAH_K@Z, already the right hand side of cxx_aliases.cpp:3240.
#pragma comment(linker, "/alternatename:?Spawn@Actor@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z=?Spawn@dActor_c@@SAPAU1@IIABUVector3@@PBUVector3_16@@CF@Z")
#pragma comment(linker, "/alternatename:?_ZN3G2S13GetBG0CharPtrEv@@YAPAHXZ=?GetBG0CharPtr@G2S@@YAIXZ")
#pragma comment(linker, "/alternatename:?_ZN4cstd4sqrtEy@@YAH_K@Z=?sqrt@cstd@@YAH_K@Z")

// =========================================================================
// 3. IRQ::SetIRQs, the one row of that group that really was a seat
// =========================================================================
//
// src/_ZN3IRQ7SetIRQsEj.cpp (arm9 0x02056e18, size 0x34) was on no slice list at
// all, and faces4_rows.cpp is right about it: no spelling of this body was in
// the link. It is on port/slice_link21.txt now, through hostgen, because the
// body pokes IME at 0x4000208 and IE at 0x4000210 through literal casts and
// hostgen's MMIO rewrite is what routes those into the port's own mapped I/O
// window. Two rewrites, measured rather than assumed: the body spells both
// registers through macros, and the rewrite reaches inside the macro bodies.
//
// The seat emits ?SetIRQs@IRQ@@YAII@Z, because the owning TU spells
// `namespace IRQ`, and the wall asks for ?SetIRQs@IRQ@@SAII@Z, because the
// referrer src/func_0205f77c.cpp:2-7 declares its own `struct IRQ` with static
// members. Y against S is a free function against a static member and neither
// has a receiver: both are plain cdecl with one unsigned int argument and an
// unsigned int return. That is the pair faces4_rows.cpp:100-101 already ships
// twice over for this function's sibling DisableIRQs.
#pragma comment(linker, "/alternatename:?SetIRQs@IRQ@@SAII@Z=?SetIRQs@IRQ@@YAII@Z")

// =========================================================================
// 4. fBase_c's constructor, and the seat that was measured and refused
// =========================================================================
//
// ??0fBase_c@@QAE@XZ is referenced by src/_ZN8dActor_cC1Ev.cpp and by
// src/_ZN11dScMgBase_cC2Ev.cpp, both real C++ constructors, which is why they
// call the base constructor by its decorated name.
//
// THE SEAT WAS TRIED FIRST AND IT COSTS FOUR ROWS. src/_ZN7fBase_cC2Ev.cpp is a
// real C++ constructor of a polymorphic class, so compiling it makes MSVC emit
// ??_7fBase_c@@6B@ and ??_GfBase_c@@UAEPAXI@Z, and a vftable references every
// slot of the class. Compiled standalone with this build's own flags and read
// with dumpbin against the 49918-name universe, that object defines the
// constructor and references FIVE names the link does not have:
//
//     ?Virtual34@fBase_c@@UAEHII@Z   ?Virtual38@fBase_c@@UAEHII@Z
//         the link has the Q spelling of both, because
//         src/_ZN7fBase_c9Virtual34Ejj.cpp and its sibling declare a
//         NON-virtual local shadow, so the vftable's virtual spelling has no
//         body anywhere
//     __ZN6Memory10DeallocateEPvP4Heap   _data_020a0eac
//         from include/fBase_c.h:161's inline operator delete, which the
//         deleting destructor odr-uses; the link has __ZN6Memory10DeallocateEPv
//         and _data_020a0eac_c, and neither of those is that name
//     ??_7type_info@@6B@
//         this one resolves out of the CRT rather than off the wall
//
// Four arrivals to close one row is the wrong trade. The other option the brief
// names, aliasing MSVC's vftable name onto a ROM-shaped table, answers a
// question this tree does not have: ??_7fBase_c@@6B@ is ALREADY defined in the
// link, by src/_ZN7fBase_cD1Ev.cpp on port/slice_gate13.txt.
//
// THE ANSWER IS THE SHADOW, and it is port/hal/int4_rows.cpp:341's shape. A
// mangle depends on the name, the class, the convention and the signature and
// on nothing else, so a NON-polymorphic shadow named fBase_c produces exactly
// ??0fBase_c@@QAE@XZ while emitting no vftable, no RTTI and no deleting
// destructor. The body calls the port's own transcription of the cartridge
// constructor, port/hal/actor_vtables.cpp:151, which that file's own banner
// describes as a full field-for-field stand-in for arm9 0x02043dec.
//
// AND IT IS THE FAITHFUL ONE ON THE DISPATCH QUESTION, which is why it is not
// merely the cheap one. The DTORS-A ruling asks whether anything reads word 0 of
// the object between the constructor's vtable store and the derived class's own
// store. The transcription stores data_02099edc, the ROM-shaped eighteen-word
// table that port/hal/cxx_aliases.cpp:1487 publishes as __ZTV9ActorBase, and
// then makes direct calls and no indirect call at all: _ZN7fBase_c9SceneNodeC1Ev
// on this+0x14, func_0203b438, the two priority stores read through
// data_020a4bb8, and func_02043810. Nothing dispatches through word 0 inside the
// window. The seat would have stored MSVC's own vftable there instead, which is
// the D1/D0-folded shape that puts every slot after the destructor one slot
// early, so the shadow keeps the cartridge's table exactly where the seat would
// have replaced it.
extern "C" void *_ZN7fBase_cC2Ev(char *self);   /* hal/actor_vtables.cpp:151 */

struct fBase_c { fBase_c(); };
fBase_c::fBase_c() { _ZN7fBase_cC2Ev((char *)this); }

// =========================================================================
// 5. Player::ST_WAIT, a static object that is ROM data
// =========================================================================
//
// ?ST_WAIT@Player@@2UState@1@A is referenced by Player.cpp and by
// src/_ZN6Player21St_WaitQuicksand_MainEv.cpp, and include/Player.h:450-454 says
// in terms why no translation unit defines it: the objects are the ROM's, carved
// out into the gap, so a definition in any translation unit would be a
// duplicate.
//
// THE ADDRESS IS 0x02110154, ov002 .bss, 0x18 bytes, and it is derived rather
// than assumed. config/arm9/overlays/ov002/symbols.txt carries both spellings at
// that address, data_ov002_02110154 kind:bss and _ZN6Player7ST_WAITE kind:bss,
// and the two referring functions confirm it from the other side. Player::CanWarp
// (ov002 0x020c2ef0) has exactly two pooled loads in its span,
// from:0x020c2f34 to:0x02110154 and from:0x020c2f38 to:0x021102a4, which are the
// two IsState arguments its source spells &ST_WAIT and &data_ov002_021102a4; and
// Player::St_WaitQuicksand_Main (ov002 0x020d2184) has exactly one,
// from:0x020d2220 to:0x02110154, for its single &ST_WAIT.
//
// THE STORAGE IS ALREADY IN THE LINK and this row adds none. ovdata.py emits
// those 24 bytes as _data_ov002_02110154 out of port/ov002_syms.txt into
// build/port/host-src/ov002_data.c, which is a direct source of walk_window,
// walk_window_hires and smoke_player alike.
//
// HOSTED THE WAY ITS NEIGHBOURS ARE. port/hal/cxx_aliases.cpp:2039-2042 and
// port/hal/actor_faces_bob.cpp:206-209 bind four sibling Player::State objects
// from a decorated UState@Player@@A spelling onto exactly these mounted names.
// This is that bridge with the name the linker itself printed on the left, and
// it is data to data: no receiver and no convention, so none of the thiscall
// hazards those files warn about apply.
#pragma comment(linker, "/alternatename:?ST_WAIT@Player@@2UState@1@A=_data_ov002_02110154")

// =========================================================================
// 6. THE TWO STATIC INITIALIZERS, written out as the DS runs them
// =========================================================================
//
// ___sinit_ov044_02111314 and ___sinit_ov100_02147a70 are mwccarm's own static
// initializers for two translation units this link already compiles. They are
// not names the port can bridge: MSVC registers its equivalent in .CRT$XCU under
// a ??__E spelling and exports nothing, and the ROM spelling has left config
// besides, because main folded each sinit into its owning class TU
// (__sinit_d_a_obj_kb1_billboard.cpp at ov044 0x02111314 and
// __sinit_d_a_star_gate.cpp at ov100 0x02147a70).
//
// So each is written here as a host body doing what the cartridge's own body
// does, read instruction by instruction out of extracted/overlays/ with every
// pooled word resolved through config/**/relocs.txt. The coordinator's ruling
// for this wave is that this is a BEHAVIOUR DECISION and it is logged as one:
// the DS re-runs a sinit on every overlay load and the host CRT runs its own
// equivalent once, so on the host both happen. NEITHER BODY IS GUARDED WITH A
// PER-LOAD FLAG, and the reason is measured rather than assumed (see each body).
// Neither is a silent no-op stub.
extern "C" {

/* the two MSL veneers both sinits call, hosted in hal/cxx_aliases.cpp:382 and
   :437: the SharedFilePtr constructor and the global-destructor chain push. */
int func_02017acc(void *self, unsigned id);
int func_02017ab4(int x);
void func_020731dc(void *object, void *dtor, void *node);

/* ov044's two objects. The first is the class TU's own SharedFilePtr (the mount
   retired it at ALIAS2 because d_a_obj_kb1_billboard.cpp defines it); the second
   is still mounted, port/ov044_syms.txt:88, 0x18 bytes. */
extern char data_ov044_02111680[];
extern char data_ov044_02111688[];

/* ov100's file pointer (the class TU's, mount retired at SYNC6), its chain node
   and the eight pointer-to-member source pairs, all mounted:
   port/ov100_syms.txt:48-50 and :80. */
extern char data_ov100_02148934[];
extern char data_ov100_0214893c[];
extern char data_ov100_02148334[]; extern char data_ov100_0214833c[];
extern char data_ov100_02148344[]; extern char data_ov100_0214834c[];
extern char data_ov100_02148354[]; extern char data_ov100_0214835c[];
extern char data_ov100_02148364[]; extern char data_ov100_0214836c[];

/* the five 0x10-byte State objects the sinit fills, mounted under their Itanium
   names at port/ov100_syms.txt:80-82, and __ptmf_null at arm9 0x02086b58. */
extern char _ZN12daStarGate_c13ST_OPEN_CLOSEE[];
extern char _ZN12daStarGate_c14ST_STAY_CLOSEDE[];
extern char _ZN12daStarGate_c7ST_WAITE[];
extern char _ZN12daStarGate_c20ST_TALKING_TO_PLAYERE[];
extern char _ZN12daStarGate_c12ST_UNLOCKINGE[];
extern char data_02086b58[];    /* __ptmf_null, two words */

/* ---- ov044 -------------------------------------------------------------
   arm9-resolved disassembly of 0x02111314..0x02111354, size 0x40:

     ldr r0,=0x02111680 ; ldr r1,=0x622 ; bl 0x02017acc
     ldr r0,=0x02111680 ; ldr r1,=0x02017ab4 ; ldr r2,=0x02111688
     bl 0x020731dc

   Two steps and nothing else. SharedFilePtr::Construct on
   data_ov044_02111680 with file handle 1570, which build/assets/handles.tsv
   resolves to data/special_obj/kb1_ball/kb1_ball.bmd, and then the global
   destructor chain push that records func_02017ab4 against that object in the
   node at data_ov044_02111688.

   NO PER-LOAD FLAG, and both halves were checked for it. Construct writes the
   file id into eight bytes and allocates nothing (the bytes are pulled by
   Model::LoadFile and dropped by Release, neither of which is here), so
   re-running it rewrites the same id over the same bytes. The chain push is the
   half that is not idempotent -- run twice it makes the node point at itself --
   and it is inert because nothing in this port walks that chain: the writer is
   func_020731dc and port/hal/ctor_runner.cpp:495 records that the reader is not
   in the link. The one caller, port_ov44_bringup in
   port/hal/actor_classes_ov044.cpp, has its own DSSTATE-bracketed guard anyway,
   so on the host this body runs once per savestate epoch. */
void __sinit_ov044_02111314(void)
{
    func_02017acc(data_ov044_02111680, 0x622u);
    func_020731dc(data_ov044_02111680, (void *)(size_t)&func_02017ab4,
                  data_ov044_02111688);
}

/* ---- ov100 -------------------------------------------------------------
   0x02147a70..0x02147bc0, size 0x150. The same two steps with handle 0x4a1
   (data/normal_obj/stargate/obj_stargate.bmd) on data_ov100_02148934 and the
   node at 0x0214893c, and then a third step the ov044 body does not have: five
   0x10-byte State objects filled from eight pointer-to-member statics and from
   __ptmf_null, in the order mwccarm emitted the stores. Read off the pooled
   loads at 0x02147b78..0x02147bbc, every one of them a relocation in
   config/arm9/overlays/ov100/relocs.txt:1027-1045:

     ST_WAIT              .init = __ptmf_null        .main = 0x0214836c
     ST_STAY_CLOSED       .init = __ptmf_null        .main = 0x02148334
     ST_TALKING_TO_PLAYER .init = 0x02148364         .main = 0x02148344
     ST_UNLOCKING         .init = 0x0214834c         .main = 0x0214835c
     ST_OPEN_CLOSE        .init = 0x02148354         .main = 0x0214833c

   THAT COPY IS LOAD-BEARING ON THE HOST and it is why this body is not a stub.
   port_star_door_callbacks_seat in port/hal/actor_overlays.cpp:537 rewrites the
   eight source pairs to host thunks BEFORE the sinit runs, and Door::Behavior
   dispatches the second pair of whichever node its +0x110 pointer holds, so
   without the copy those five mounted nodes stay zero and the star door
   dispatches through a null pointer-to-member.

   NO PER-LOAD FLAG here either. The copy is sixteen bytes memcpy'd from fixed
   sources, which is idempotent by construction; the Construct and the chain push
   are the ov044 case word for word. The ORDER is the thing that matters and it
   is already enforced upstream: the seat runs at actor_overlays.cpp:1440 and
   aborts loudly if the sources have already been rewritten, and this body is
   called at :1448. */
static void sg_state(char *dst, const char *init, const char *main_)
{
    std::memcpy(dst, init, 8);
    std::memcpy(dst + 8, main_, 8);
}

void __sinit_ov100_02147a70(void)
{
    func_02017acc(data_ov100_02148934, 0x4a1u);
    func_020731dc(data_ov100_02148934, (void *)(size_t)&func_02017ab4,
                  data_ov100_0214893c);

    sg_state(_ZN12daStarGate_c7ST_WAITE,              data_02086b58,        data_ov100_0214836c);
    sg_state(_ZN12daStarGate_c14ST_STAY_CLOSEDE,      data_02086b58,        data_ov100_02148334);
    sg_state(_ZN12daStarGate_c20ST_TALKING_TO_PLAYERE, data_ov100_02148364, data_ov100_02148344);
    sg_state(_ZN12daStarGate_c12ST_UNLOCKINGE,        data_ov100_0214834c,  data_ov100_0214835c);
    sg_state(_ZN12daStarGate_c13ST_OPEN_CLOSEE,       data_ov100_02148354,  data_ov100_0214833c);
}

// =========================================================================
// 7. _data_ov062_0211d9c0, the root heap arena start: NOT MOUNTED
// =========================================================================
//
// One referrer, src/func_0201a458.c, and it is a base and limit pair rather than
// an object:
//
//     _ZN4Heap14CreateRootHeapEPvj(&data_ov062_0211d9c0,
//         (unsigned)(&func_02140d80 - &data_ov062_0211d9c0));
//
// On the cartridge that is 0x0211d9c0..0x02140d80, 0x233c0 bytes of the overlay
// region reused as the next scene's heap. On the host there is no overlay region
// to carve: every overlay this port hosts is resident at once, at its own
// address, so a mount here would hand CreateRootHeap live overlay images to
// allocate out of. That is the worst version of the undersized-globals trap and
// out/HALROWS/bugs.md item 3 and the DTORS2 brief both name it a never.
//
// port/hal/scene_boot.cpp:2481 already traps this pair, and it traps it under
// the PRE-SYNC spellings func_0211d9c0 and func_02140d80. The sync respelled the
// first as data and left the second alone, which is exactly why one of the two
// is on the wall and the other is not. The post-sync spelling joins that trap
// here rather than there, so the trap's own block stays one lane's, and it is
// the same shape: a loud body whose ADDRESS is what the caller actually takes.
//
// NOTHING ABOUT THE RUNTIME CHANGES. The pre-sync pair has been two unrelated
// trap addresses since that block was written, so the subtraction has never
// produced a sane size; this makes the second half of the pair reachable again
// rather than unresolved. The arena question is logged in out/LINK21/bugs.md and
// left for the audit, per Tango's rule.
int data_ov062_0211d9c0(void);
int data_ov062_0211d9c0(void)
{
    static int said;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [link21] UNMATCHED arena base entered: "
                     "data_ov062_0211d9c0 (returns 0; the ov062 heap arena is "
                     "deliberately not mounted)\n");
        std::fflush(stderr);
    }
    return 0;
}

// =========================================================================
// 8. _data_ov008_02111b6c, a word inside an overlay that IS mounted whole
// =========================================================================
//
// ov008 is mounted whole at port/CMakeLists.txt:2410, base 0x021111a0, end
// 0x02111c20, so 0x02111b6c is inside it at offset 0x9cc with 0xb4 bytes to
// spare. There is no port/ov008_syms.txt and there must not be one: ovdata.py's
// --whole emitter publishes port_ov008_image and nothing by name, and adding a
// per-symbol mount beside it would be a SECOND storage for one ROM address,
// which is the thing this row was refused over.
//
// MSVC's /alternatename cannot carry an offset, so the name is defined here as
// the two words the cartridge's own ov008 image holds at that address, read out
// of extracted/overlays/overlay_0008.bin at 0x9cc:
//
//     02111b6c  0000fa14        02111b70  25e26810
//
// THE READ IS A SHARED-WINDOW READ and the port has already ruled on it. The
// referrer, src/unnamed/ov063/func_ov063_02117cdc.c:160, tests
// (&data_ov008_02111b6c)[0] == 0x1f000 in the Boo cutscene's TALK sub-state, and
// config/arm9/overlays/ov002/relocs.txt:6676 says thirteen overlays claim that
// address. port/unmatched/Boo_StateTalk.c:19-31 is the host copy of this very
// function and its banner records that the 0x1f000 sentinel's home overlay is
// NOT identified, that on level 12 the window holds ov020's image and the test
// is false, and that it returns 0 elsewhere for the same reason.
//
// ov008's own bytes give the SAME BRANCH as that host copy's zero: 0xfa14 is not
// 0x1f000, so the input-flag arm decides either way. This row therefore closes
// the link without deciding the question the banner leaves open, and the
// question is carried into out/LINK21/bugs.md rather than answered here.
// IN .dsstate, because it is a hosted DS global and dsstate_guard says so: the
// first build in this campaign that reached a link caught this row outside the
// captured span, where a save state would not roll it back. Bracketed the way
// hal/dsstate_seg.h asks, and no later extern re-declaration of the name
// appears in this file, which is the silent way the bracket gets undone.
DSSTATE_BEGIN
int data_ov008_02111b6c[2] = { 0x0000fa14, 0x25e26810 };
DSSTATE_END

// =========================================================================
// 9. _func_0206a4a0, the GBA cartridge probe
// =========================================================================
//
// arm9 0x0206a4a0, size 0x120, and the port maps no cartridge window. Read out
// of extracted/arm9_dec.bin with the pool resolved, the body is a Slot-2
// presence check against the firmware's power-on record at 0x027ffc30: it
// returns 0 at once if the recorded identifier is 0xffff, returns 0 at once if
// the record's pulled-out bit is already set, and otherwise takes the ARM7/ARM9
// lock, grants Slot-2 to the ARM9 through EXMEMCNT, and compares the record
// against the AGB header at 0x080000b2, 0x080000be, 0x080000ac and 0x0801fffe,
// latching the pulled-out bit and returning 0 on any mismatch.
//
// WITH NO CARTRIDGE INSERTED THE ANSWER IS 0 ON EVERY PATH, and three
// independent readings agree:
//   * the firmware records 0xffff when nothing was there, which is the first
//     early exit, and it is taken before the bus is read at all;
//   * if execution did reach the header, the AGB bus is open bus with nothing
//     inserted, so 0x080000b2 reads back 0x59 rather than 0x96 and the fallback
//     compare against 0x0801fffe reads 0xffff, which cannot equal a recorded
//     identifier the first check already proved is not 0xffff -- so it falls
//     into the latch and returns 0;
//   * the state is sticky: once the pulled-out bit is set every later call
//     returns 0 through the second early exit.
//
// The one caller, src/func_0205f77c.cpp:36, does exactly one thing with it:
// `if (func_0206a4a0() == 0) mode &= ~0x10;`, dropping the cartridge wake
// trigger from the sleep mask. Zero is the correct instruction for a host with
// no Slot-2 hardware, and the only path in the tree that reaches that caller,
// src/func_02019ed0.c:6, passes mode 0xc, which never sets bit 0x10 at all.
//
// A host body rather than a seat, for the reason port/slice_hostgen4.txt:130
// gives: seating src/func_0206a4a0.c brings five more names with it
// (data_020a9de0 and the four lock and EXMEMCNT helpers), closes nothing else,
// and opens a fresh closure into the OS boot path. This is logged as a behaviour
// decision. The cartridge window is never mapped.
int func_0206a4a0(void);
int func_0206a4a0(void)
{
    static int said;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [link21] GBA Slot-2 probe func_0206a4a0 called; "
                     "the host maps no cartridge window, so it answers 0 "
                     "(no cartridge inserted)\n");
        std::fflush(stderr);
    }
    return 0;
}

}  /* extern "C" */
