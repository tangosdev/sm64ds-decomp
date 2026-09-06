// Stage::InitResources' closure: the host bodies and faces slot 0 needs.
//
// Run link60 Stage 4, lane SD0. port/stage_lifecycle_map.txt section 7 sizes
// the slot-0 swap at SIXTEEN named symbols measured by a real link; this file
// is the code half of closing them, port/tools/romdata.py and
// hal/ptr_tables.cpp are the data half, and port/slice_s4_slot0.txt is the
// wiring. Everything here is one of three shapes and each shape is separated
// below so a reviewer can check them by class rather than one at a time:
//
//   PORT_HOST_ABI bodies   a DS subsystem the port has decided not to have,
//                          whose observable is nothing
//   C-name faces           the TU is compiled and publishes only the MSVC
//                          mangling; the caller spells the Itanium C name
//   storage                two 4-byte BSS words and one alias
//
// ===========================================================================
// PART 1 -- THE THREE PORT_HOST_ABI BODIES
// ===========================================================================
//
// Each is the one-line companion to a decision the port made in another
// lane's file and wrote down there. Neither the decision nor the wording is
// new here; what is new is that Stage::InitResources now calls them by name.
//
// PORT_HOST_ABI: UnloadArchive releases a NARC the DS card loader mounted, and
// the host has no mount to release. hal/scene_boot.cpp's LoadArchive face
// records the whole trade in full: hal/fs.cpp resolves archive-interior file
// ids (>= 0x8000) LAZILY out of port_archive_map, so on the host an archive is
// never "mounted" and never "not mounted" -- the id resolves either way.
// LoadArchive's host body therefore answers "is archive N available" with 1
// for every archive in the table, and the symmetric answer for UnloadArchive
// is to do nothing. It returns void in the ROM and has no out-parameter, so
// there is no observable to reproduce.
//   Stage::InitResources calls it in two places: the 2..5 sweep that drops
// every level-specific archive except the one this level wants, and the tail
// release when func_0203da3c() != 2. Both are heap hygiene on a heap the port
// does not fill this way.
//   The alternative is src/UnloadArchive.c, which wants func_02018908 and the
// DS card overlay chain under it plus data_0208ecf4 hosted as raw ROM bytes
// with its DS string pointers. Eleven TUs to undo a mount that never happened.
//
// PORT_HOST_ABI: LoadLevelOverlays / UnloadLevelOverlays load and unload the
// per-level DS overlay, and the port has no overlay loader at all. Every
// overlay it hosts is a static host array mounted at build time
// (port/scene_boot_map.txt section 2 is the ruling and states it in these
// words: THE OVERLAY LOAD IS NOT SKIPPED, IT DOES NOT EXIST ON THE HOST).
// port_level_mount_register / port_level_mounts_install in hal/level_boot.cpp
// is what stands where the loader would be, and it runs before the boot rather
// than from inside it.
//   src/_Z17LoadLevelOverlaysi.cpp would compile: its four callees
// (LoadOverlay, LoadOrUnloadObjectOverlays, and the two id tables) are already
// hosted or stubbed. It is left out anyway, because what it would do is drive
// hal/scene_boot.cpp's empty LoadOverlay with an id read out of
// data_020758c8 and then latch that id into data_02092130 as "the resident
// level overlay" -- a residency claim about a loader that does not exist,
// which the next UnloadLevelOverlays would then act on. An empty body makes no
// claim. Both return void and neither has an out-parameter.
//
// THE THIRD BODY IS NOT DEMANDED BY THIS SLICE AND IS SUPPLIED ANYWAY.
// Measured (port/tools/closure.py over port/slice_s4_slot0.txt against
// walk_window.map): the slice's link wants _UnloadArchive and
// __Z17LoadLevelOverlaysi and does NOT want __Z19UnloadLevelOverlaysi, because
// nothing on it calls that one -- its caller is Stage::CleanupResources, which
// is slot 3 and is not in this lane. port/stage_lifecycle_map.txt section 7
// names three bodies and section 5 says why the third belongs with them, so it
// is here, unreferenced, one line, pre-paying a symbol slot 3 will want. Said
// out loud because "the handoff asked for three and the link asks for two" is
// exactly the kind of quiet discrepancy this repo has been bitten by.
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

extern "C" {

// PORT_HOST_ABI: releases a NARC the DS card loader mounted; the host has no mount to release (hal/fs.cpp resolves archive ids lazily, so an archive is never mounted).
void UnloadArchive(int)             {}

}  /* extern "C" */

// ---- THE TWO OVERLAY BODIES ARE GONE (run link100, lane STAGE) -------------
//
// `void _Z17LoadLevelOverlaysi(int) {}` and `void _Z19UnloadLevelOverlaysi(int)
// {}` stood here. Both are RETIRED, and the ROM's own bodies -- src/
// _Z17LoadLevelOverlaysi.cpp, src/_Z19UnloadLevelOverlaysi.c and the
// src/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp they share -- are enrolled by
// port/slice_gate213.txt instead. The two C-linkage names come back through
// /alternatename in hal/cxx_aliases.cpp, because both .cpp files compile as C++
// and publish MSVC manglings.
//
// WHAT THE OLD RULING SAID, and why it does not survive. It said an empty body
// "makes no claim", while the ROM's body would "drive hal/scene_boot.cpp's
// empty LoadOverlay with an id read out of data_020758c8 and then latch that id
// into data_02092130 as the resident level overlay -- a residency claim about a
// loader that does not exist". That is a claim about what happens WHEN THE BODY
// RUNS, and neither body runs:
//
//   LoadLevelOverlays is reached only from Stage::InitResources, which this
//   file's own PART 4 gates behind SM64DS_SLOT0_ROM, default off.
//   UnloadLevelOverlays is reached only from Stage::CleanupResources, which is
//   _ZTV5Stage slot 3 -- seated by this run and, since run link100 lane
//   STAGEFIX, gated behind SM64DS_STAGE_SLOT3_ROM (presence = on, default off),
//   so the shipped dispatch is the named abort it always was.
//   THE DISPATCH IS NOT UNREACHABLE, and the first draft of this paragraph
//   said it was ("nothing marks the Stage"). port/stage_lifecycle_map.txt
//   section 5 MEASURED a VS match-end scene request arriving at it, and
//   section 9 traces a live death-plane chain into it. What keeps the ROM body
//   from running is the gate, not the absence of a caller.
//
// So the residency word is written by nobody today and read by nobody today,
// and the choice is between an empty host body and the ROM's own three, with no
// behavioural difference between them. THE PORT'S NORTH STAR SETTLES THAT: the
// port is meant to BE the decomp, and these three TUs are pure table
// bookkeeping over data_02075998 / data_02075804 / data_020758c8 that bottoms
// out on the same two hosted leaves either way. Nothing under them is portable
// and nothing under them is being taken: FS_LoadOverlay and the card chain stay
// exactly where hal/scene_boot.cpp left them.
//
// NAME THOSE TWO LEAVES CORRECTLY. This paragraph, port/slice_gate213.txt,
// port/stage_lifecycle_map.txt 12e and port/tools/romdata.py all said the same
// thing: that both leaves are empty PORT_HOST_ABI bodies in hal/scene_boot.cpp.
// One is; the other is not, and a reader who goes looking for it there does not
// find it (corrected run link100, lane STAGEFIX):
//
//   LoadOverlay      IS an empty PORT_HOST_ABI body -- hal/scene_boot.cpp,
//                    `extern "C" void LoadOverlay(int) {}` in the overlay-loader
//                    block.
//   UnloadOverlay    IS NOT. It is a LOUD-ONCE REFUSAL SEAM at
//                    hal/scene_vs_menu.cpp:992, one of that file's VS_SEAM
//                    family: the first call prints
//                        [vs] refused-subtree seam entered: UnloadOverlay
//                    to stderr, every call answers the idle value, and it is
//                    declared `int UnloadOverlay(void)` there while the ROM's
//                    callers spell `void UnloadOverlay(int)`.
//
// THE NEUTRALITY CONCLUSION SURVIVES, which is why this is a wording fix and
// not a ruling change. Both spellings are __cdecl, so the CALLER cleans the one
// argument the seam ignores, and the int it hands back in eax lands where the
// callers read nothing. And nothing reaches it today: UnloadOverlay is called
// only from LoadOrUnloadObjectOverlays, which is called only from
// LoadLevelOverlays (gated, SM64DS_SLOT0_ROM) and UnloadLevelOverlays (called
// only from Stage::CleanupResources, gated, SM64DS_STAGE_SLOT3_ROM). What
// changes is that whoever opens either gate should expect that stderr line, and
// should look for it in scene_vs_menu.cpp rather than in scene_boot.cpp.
//
// ONE HAZARD, NAMED RATHER THAN DISCOVERED LATER, and it belongs to PART 4's
// probe. The ROM's LoadLevelOverlays opens with
//
//     if (data_0209f278) { if (data_02092130 != data_020758c8[level]) Crash(); ... }
//
// and Crash() is a host body that aborts (hal/heap_vtable.cpp:151). src/
// func_0202deac.c is the only writer of data_0209f278 in the whole of src/ and
// it IS in walk_window.map, so the guard is not dead the way the branch in
// hal/w2_dtor_heads.cpp's group 4 is dead -- it is merely unreached, because
// LoadLevelOverlays only runs under SM64DS_SLOT0_ROM. Anyone re-running the
// PART 4 bisect should read this as a FIFTH possible fault ahead of it, on the
// same flag, and it is the ROM's own fault rather than a new one.

// ===========================================================================
// PART 2 -- STORAGE: TWO BSS WORDS AND ONE ALIAS
// ===========================================================================
//
// The rest of slot 0's data is romdata.py's (six ROM-byte rows) and
// hal/ptr_tables.cpp's (data_020756f0, twelve relocated words). What is left
// is arm9 BSS, which is runtime-initialised and never belongs in romdata, and
// one name the port already hosts under its other spelling.
//
// data_020a0f04   func_0203da3c is `return data_020a0f04`, and
//                 Stage::InitResources reads that answer to decide whether to
//                 release the level-specific archive on the way out
//                 (`if (func_0203da3c() != 2 && archiveIdx != 0xBF)`). Its
//                 other readers spell it through the same one-byte accessor.
//                 config/arm9/symbols.txt:4961, kind:bss, and the next symbol
//                 is 0x020a0f08, so the object is one 4-byte word.
//
// data_020a0f30   NOT ON THE HANDOFF'S LIST OF SIXTEEN, and this is where it
//                 came from rather than a correction to that list. The
//                 sixteen were measured with the three unsliced callees still
//                 OUT; one of them, func_0203d81c, reads and clears this word
//                 (`if (data_020a0f30[0]) { data_020a0f30[0] = 0; return 1; }`
//                 -- src/func_0203d81c.c), and enrolling the callee is what
//                 asks for it. So the sixteen was honest when it was taken and
//                 closing three of its rows opened a seventeenth. Measured the
//                 same way, by port/tools/closure.py over
//                 port/slice_s4_slot0.txt against walk_window.map, and then by
//                 the real link. config/arm9/symbols.txt:4972, kind:bss, next
//                 symbol 0x020a0f34, one 4-byte word.
//
// Both go in .dsstate like every other hosted DS global: they are mutable game
// state, and a save-state restore that left them behind would restore a level
// whose archive-release decision and whose func_0203d81c edge came from the
// run before it.
DSSTATE_BEGIN
extern "C" {
unsigned char data_020a0f04[4];
unsigned short data_020a0f30[2];
}
DSSTATE_END

/* data_02075720 IS AN ALIAS, NOT A MOUNT, and the direction is worth stating
   because it runs opposite to every other row in this closure. The port has
   hosted these bytes since the VS star-order fix -- hal/bob_enemy_bridges.cpp
   defines `unsigned char VS_STAR_SPAWN_ORDERS[6][0xC]`, which is the name
   config/arm9/symbols.txt:3142 gives the arm9 symbol at 0x02075720.
   Stage::InitResources is the one caller that spells it by ADDRESS instead
   (`extern char data_02075720[][0xC]`), for its last statement:

       data_0209f344 = &data_02075720[func_0203dad4() % 6];

   which is the line hal/level_boot.cpp:3384 hand-rolls today against the same
   host array (the derivation is the comment block above it at 3367). The 2328
   this note used to cite is an unrelated LoadFile-dedupe paragraph; corrected
   by run rel0215 lane prop15, which read both lines rather than either name.
   So there is one object and two spellings, and the fix is to tie
   the spellings together -- adding the address name to romdata.py's NAMED list
   would emit a SECOND copy of the bytes, and then the ROM's line and the
   port's stand-in would seat data_0209f344 into two different arrays.

   An /alternatename rather than a definition, so that the LHS only binds if
   nothing else defines it; if a later lane ever hosts data_02075720 for real,
   its strong symbol wins and nothing collides. port/tools/alternatename_guard.py
   is the check that the LHS has not become a defined symbol behind this.

   AND THE ALIAS MADE A SECOND GUARD SPEAK, which is worth recording because it
   looks like a regression and is not. VS_STAR_SPAWN_ORDERS has always lived
   outside .dsstate and port/tools/dsstate_guard.py never looked at it: that
   name matches none of its HOSTED patterns. The alias publishes the same
   address under a data_ name, so the guard saw the array for the first time
   and said a DS global is not captured by the save state. True, and harmless:
   nothing writes it. It is on that tool's BOOT_CONSTANT list now, with the
   reasoning beside the entry. Measured, not predicted -- the first link with
   this alias in it failed exactly there. */
#pragma comment(linker, "/alternatename:_data_02075720=_VS_STAR_SPAWN_ORDERS")

// ===========================================================================
// PART 3 -- THE FOUR C-NAME FACES
// ===========================================================================
//
// READ THE HEAD OF hal/method_faces.cpp BEFORE REVIEWING THESE. Its three
// failure modes -- wrong target, dropped receiver, mis-bridged receiver -- are
// the whole risk surface of this part, and two of the four faces below are the
// receiver-carrying kind. The per-face checklist is (a) TARGET, (b) ARITY,
// (c) RECEIVER, verified from the DEFINITION rather than from a signature that
// happens to compile.
//
// WHY THE CLASSES ARE DECLARED LOCALLY AND Stage.h IS NOT INCLUDED. MSVC's
// mangling of a member function depends on the class NAME and the signature,
// never on the class BODY, so a local declaration produces the identical
// symbol -- and if the signature is wrong the link fails loudly with LNK2019
// rather than calling something plausible. That failure mode is the reason to
// prefer it here. It is also the only way to reach two of these four:
// include/Stage.h is byte gate input and declares neither InitResources nor
// LoadGraphics2D, and this lane does not edit include/. The same local
// non-virtual declaration trick is already in hal/stage_bridges.cpp (its
// ActorBase) and hal/scene_boot.cpp, for the same reason.
//
// THE PLAUSIBLE-SIBLING CHECK, run on all four against walk_window.map before
// a line of this was written. Each row is the mangled symbol the face calls,
// the object that defines it, and the definition that fixes the signature:
//
//   ?LoadGraphics2D@Stage@@SAX_NH@Z          _ZN5Stage14LoadGraphics2DEbi.cpp.obj
//       src/_ZN5Stage14LoadGraphics2DEbi.cpp:32 declares it STATIC in its own
//       local class and :41 defines `void Stage::LoadGraphics2D(bool b, int i)`.
//       SA = public static __cdecl, so there is NO receiver to deliver and the
//       ROM caller is right to pass two arguments and no `this`
//       (src/_ZN5Stage13InitResourcesEv.cpp:332). No sibling: this is the only
//       LoadGraphics2D in src/.
//   ?LoadFog@Stage@@QAEXXZ                   _ZN5Stage7LoadFogEv.cpp
//       QAE = public __thiscall. The ROM caller declares
//       `void _ZN5Stage7LoadFogEv(void *thiz)` and passes the Stage as its
//       first STACK argument, so this face is the receiver-bridging kind: the
//       qualified call moves thiz into ECX. Sibling check: RenderFog is the
//       near neighbour and is a different name and a different slot; nothing
//       else in src/ is spelled LoadFog.
//   ?LoadTextureTransformers@Stage@@QAEXXZ   _ZN5Stage23LoadTextureTransformersEv.cpp
//       Same shape as LoadFog, same bridge. ALREADY IN THE IMAGE and already
//       called -- hal/stage_bridges.cpp:588 reaches it as a method today. The
//       face adds the C spelling, not the body.
//   ?CleanAll@ShadowModel@@SAXXZ             _ZN11ShadowModel8CleanAllEv.cpp.obj
//       SA = public static __cdecl, no arguments. include/ShadowModel.h:66
//       declares it under an explicit `/* --- static --- */` and the body
//       touches only data_0209cef4 and data_0209ceec, never a receiver. The
//       plausible sibling here is RenderAll, declared one line above it at :65
//       and static too; the face calls CleanAll and the two do opposite things
//       (RenderAll walks the list, CleanAll empties it), so getting this one
//       wrong would be silent. It is spelled out for that reason.
//
// Three of the four bodies are ALREADY LINKED under the MSVC mangling (all but
// LoadFog, which arrives with port/slice_s4_slot0.txt), so three of these
// faces add a spelling and not a body. That is why they are inert until
// something calls the C name, which is the seat -- see the T4 note in
// port/stage_lifecycle_map.txt section 7 on why landing faces alone moves no
// number.

struct Stage {
    /* Signatures fixed by the definitions named above. A mismatch here is an
       LNK2019 on a name that does not exist, never a call to a sibling. */
    static void LoadGraphics2D(bool b, int i);
    void LoadFog();
    void LoadTextureTransformers();
    /* src/_ZN5Stage13InitResourcesEv.cpp:15, the same one-line declaration
       that TU makes. ?InitResources@Stage@@QAEHXZ. */
    int InitResources();
};

struct ShadowModel {
    static void CleanAll();
};

extern "C" {

/* No receiver: static, and the ROM caller passes two arguments. `bool` is the
   low byte of the 4-byte stack slot the caller's `int` fills, and the caller's
   value is the 0/1 of a `== 1` comparison. */
void _ZN5Stage14LoadGraphics2DEbi(int b, int level)
{ Stage::LoadGraphics2D(b != 0, level); }

/* Receiver delivered: the ROM caller pushes the Stage, the qualified call puts
   it in ECX. Qualified so nothing re-dispatches through a vtable. */
void _ZN5Stage7LoadFogEv(void *thiz)
{ ((Stage *)thiz)->Stage::LoadFog(); }

void _ZN5Stage23LoadTextureTransformersEv(void *thiz)
{ ((Stage *)thiz)->Stage::LoadTextureTransformers(); }

/* No receiver: static, no arguments. CleanAll, not RenderAll. */
void _ZN11ShadowModel8CleanAllEv(void)
{ ShadowModel::CleanAll(); }

}  /* extern "C" */

// ===========================================================================
// PART 4 -- THE SLOT-0 SWAP, ENV-GATED SO THE TWO WALLS CAN BE BISECTED
// ===========================================================================
//
// port/stage_lifecycle_map.txt section 7 hands this lane the swap: route slot 0
// to the ROM's Stage::InitResources through the seated dispatch. The closure is
// closed and the link measured it, so what is left is BEHAVIOUR, which the map
// is explicit nobody has ever asked.
//
// This is the shape that asks it. SM64DS_SLOT0_ROM selects what
// hal/stage_bridges.cpp's slot-0 thunk does after the host boot body:
//
//   unset / 0   nothing. The port behaves exactly as it did and this part of
//               the file is inert at run time.
//   1           call the ROM's Stage::InitResources on the Stage.
//   2           the same, with data_02092208[level] SEEDED to the host mount
//               first. See port_slot0_seed_lvl_table.
//   3           mode 2 plus data_02092110 put back to the booted level, so
//               InitResources' own re-latch lands on it. See
//               port_slot0_seed_pending_level.
//
// GATED RATHER THAN UNCONDITIONAL, and not from timidity: the two walls below
// are independent, and an unconditional swap answers neither because a run that
// dies has one stack and two candidate causes. Gated, mode 0 leaves the shipped
// dispatch byte for byte as it was and each wall gets its own run.
//
// ---------------------------------------------------------------------------
// SM64DS_STAGE_SLOT3_ROM, THE SIBLING GATE, documented here because this is
// where a reader looking for "which Stage slots run the ROM's body and under
// what" will come (run link100, lane STAGEFIX).
//
//   WHERE      hal/stage_bridges.cpp, the st_clean thunk on _ZTV5Stage slot 3.
//   SHAPE      PRESENCE, not a number: set to anything (0 included) and the
//              slot runs the ROM's Stage::CleanupResources; unset and it runs
//              the named abort the slot carried before it was seated, same
//              reporter, same message, same abort. Read once. Slot 0's gate
//              parses a number because it has four modes to select between;
//              slot 3 has one, so there is nothing to parse.
//   WHY GATED  for a DIFFERENT reason than this file's own gate, and the two
//              are worth telling apart. Slot 0's gate asks whether the ROM's
//              body WORKS on the host -- four measured walls, below -- on a
//              slot the port dispatches on every level entry. Slot 3's asks
//              whether the ROM's body SHOULD run at all, on a slot the port
//              reaches only when something has gone wrong. Seating it
//              unconditionally removed a deliberate safety stop: the port keeps
//              ONE Stage across every level change and never rebuilds it, so a
//              cleanup Process dispatch runs the ROM's real teardown -- twelve
//              file handles released, the area table and the Camera zeroed,
//              Deallocate called -- and then the game keeps running on the
//              torn-down level, where before it aborted by name. The four
//              places in this repo that reach the dispatch are listed at
//              st_clean and in port/stage_lifecycle_map.txt sections 5, 9 and
//              12a.
//   PROOF      port/tools/stage_seat_proof.py rungs 6 and 7 dispatch the slot
//              on the same binary with the variable unset and set, and check
//              that the first run aborts by name and the second reaches the ROM
//              body and comes back with the ROM's own 1.
//   COST       none in linkage. The thunk names port_stage_cleanup_resources on
//              a branch the linker cannot prove dead, so
//              ?CleanupResources@Stage@@QAEHXZ stays in walk_window.map from
//              its own object -- exactly the way
//              ?InitResources@Stage@@QAEHXZ stays in it under THIS gate.
// ---------------------------------------------------------------------------
//
// AND THE HOST BOOT BODY STILL RUNS IN FRONT OF IT, which makes modes 1 and 2 a
// PROBE and not a candidate seat. port_stage_a_boot is not a pure subset of
// InitResources: section 2e lists what is genuinely host in it -- the mount,
// port_level_boot_target, the ov009 static initialisers, the data_0209caa0 bit
// -- and none of that has a ROM counterpart to inherit it. A real seat keeps
// that prologue and drops only the duplicated middle, which is surgery inside
// hal/level_boot.cpp and is not this lane's file. So these modes run the ROM
// body ON TOP OF the host boot. Anything they reach is a real reading of what
// InitResources does to the port's state; anything that breaks because a step
// ran twice (LoadClsnAndObjects spawning a level's objects a second time) is an
// artefact of the probe. The two are told apart by WHERE the run stops, which
// is why the probe prints its position rather than only its verdict.

extern "C" {
extern unsigned char *data_0209f340;      /* the LVL_Overlay the boot mounted */
extern signed char data_0209f2f8;         /* the current sublevel */
extern signed char data_02092110;         /* the PENDING sublevel */
extern unsigned char data_02092208[];     /* level -> LVL_Overlay, ROM bytes */
extern void *data_0209f324;               /* WIPES */
extern void *data_0209f5bc;               /* the installed fader */
}

namespace {

int slot0_mode(void)
{
    static int mode = -1;
    if (mode < 0) {
        const char *e = std::getenv("SM64DS_SLOT0_ROM");
        mode = e ? std::atoi(e) : 0;
    }
    return mode;
}

/* THE WALL THE MAP DOES NOT NAME, and it is the bigger of the two.
   Stage::InitResources' own line, four statements before LoadClsnAndObjects:

       data_0209f340 = data_02092208[data_0209f2f8];

   data_02092208 is hosted by port/tools/romdata.py as the ROM's OWN BYTES, and
   all 52 of its words are relocated DS addresses. Not an inference: it is the
   TOP ROW of port/tools/ptr_audit.py's report, 52 relocations with 12 into
   code, and the only reason that audit passes on it is its "seated by host TU"
   column, which asks whether a file under port/ NAMES the symbol and not
   whether anybody dereferences it. Nobody does. Seating slot 0 is what makes
   somebody dereference it, and Stage::LoadClsnAndObjects is handed the result.

   hal/level_change.cpp said this before this lane existed, in its own words:
   every level overlay is linked over the same 0x0211xxxx window, only one is
   resident at a time on the DS, so the table holds addresses in different
   files that happen to overlap and a host cannot use the VALUE. The port's
   answer is a REGISTRY -- port_level_overlay(level) returns where the host
   mounted that level -- and port_stage_a_boot assigns its result to
   data_0209f340 directly, bypassing the table.

   Mode 2 is the smallest faithful reconciliation: write the host mount address
   into the ROM's own table for the level that is booting, so InitResources'
   line evaluates to what the host boot assigns. Same translation
   hal/ptr_tables.cpp makes for every other relocated table, done LAZILY
   because host mounts are lazy: on the DS the entry is only meaningful while
   that overlay is resident, and on the host resident means mounted.
   A probe and not a proposal. Doing it for real means romdata.py stops
   emitting ROM bytes for this symbol and port_level_ds_overlay, which reads
   the same array for the mount disagreement check, gets its DS answer from
   somewhere else. Both are other lanes' files. */
void port_slot0_seed_lvl_table(void)
{
    const int lvl = (int)data_0209f2f8;
    if (lvl < 0 || lvl >= 52 || !data_0209f340)
        return;
    unsigned *row = (unsigned *)(data_02092208 + lvl * 4);
    std::fprintf(stderr, "[slot0] data_02092208[%d]: %08x (ROM, a DS address) "
                 "-> %08x (the host mount)\n", lvl, *row,
                 (unsigned)(size_t)data_0209f340);
    *row = (unsigned)(size_t)data_0209f340;
}

/* A THIRD SEED, AND IT IS WHY MODE 2 ALONE CHANGES NOTHING. Measured, not
   reasoned: with data_02092208[1] seeded to the host mount, the run still
   faulted at the identical address, because InitResources does not index the
   table with the level the host boot settled on. Its own line, 150 statements
   earlier, is

       data_0209f2f8 = data_02092110;

   -- it RE-LATCHES the current sublevel from the PENDING one before it uses
   it. On the DS data_02092110 is what the title screen, the warp or the level
   select put there. On the port it is -1 by the time slot 0 runs, and
   deliberately: hal/level_change.cpp:1071 consumes it (`data_02092110 = -1;`)
   the moment the handoff accepts a change, so a stuck request cannot re-fire.
   port/stage_lifecycle_map.txt section 2a already calls this substitution
   GENUINELY HOST -- "the port has no title screen to latch it" -- and this is
   what that costs at the moment slot 0 runs the ROM's line instead of the
   port's.
   Mode 3 puts the level back in the pending word first, so the ROM's own latch
   evaluates to the level the host boot actually mounted. Also a probe: doing
   it for real means the port stops consuming data_02092110 at handoff time and
   starts consuming it where the ROM does, inside InitResources, which is
   hal/level_change.cpp's decision and not this lane's. */
void port_slot0_seed_pending_level(void)
{
    std::fprintf(stderr, "[slot0] data_02092110: %d (consumed by the handoff) "
                 "-> %d (the level the boot mounted)\n",
                 (int)data_02092110, (int)data_0209f2f8);
    data_02092110 = data_0209f2f8;
}

/* THE OTHER WALL, AND IT IS SMALLER THAN THE MAP RECORDS.
   The map calls data_0209f324 a second-writer collision "with two different
   object layouts". The layouts CONVERGED when lane SL0 landed
   /alternatename:__ZTV9FaderWipe=??_7HalFaderWipe@@6B@, and this reports it at
   run time rather than arguing it:

     * HalFaderWipe (hal/fader_wipes.cpp) is vptr 0x00, currInterp 0x04, speed
       0x08, color 0x0c, unk0e 0x0e, model[0x50] 0x10 -- 0x60 bytes, the exact
       stride Stage::InitResources passes func_02073470(7, 0x60, 8, ...), and
       src/engine/fader/_ZN9FaderWipeC1Ev.c declares the identical struct.
     * that ctor's last store is `this->vtable = _ZTV9FaderWipe`, and after
       SL0's alias _ZTV9FaderWipe IS ??_7HalFaderWipe@@6B@. A FaderWipe the ROM
       constructs on the game heap carries the SAME TABLE as a static
       hal_wipes[] entry, in the same ROM slot order.
     * data_0209f5bc keeps pointing at hal_wipes[0] across the handover and
       that object stays constructed and valid, so the frame-zero guarantee
       Scene::BeforeBehavior depends on is not disturbed by the second write.

   So the handover is a second WRITE and not a second DEFINITION: no LNK2005,
   and C++ static init is ordered before main. The host array does not retire,
   it demotes, from "the wipes" to "the wipes before a Stage exists" -- still
   needed, because a scene run never boots one.
   What none of that settles is whether the seven FaderWipe::LoadAndSetFile
   calls succeed. The file ids are real: data_020755e0 is {2053, 2052, 2055,
   2056, 2051, 2054, 0x80c5}, six plain NitroFS handles and one
   ARCHIVE-INTERIOR id that hal/fs.cpp resolves lazily through
   port_archive_map. "The port has no BMD for the wipes" is a claim about
   nobody having asked, not about the file system. This reports the answer. */
void port_slot0_check_wipes(void *before)
{
    unsigned char *w = (unsigned char *)data_0209f324;
    if (!w) {
        std::fprintf(stderr, "[slot0] WIPES is null after InitResources\n");
        return;
    }
    void *vt0 = *(void **)before;
    int same = 0, loaded = 0;
    for (int i = 0; i < 7; ++i) {
        unsigned char *o = w + i * 0x60;
        if (*(void **)o == vt0) ++same;
        if (*(void **)(o + 0x14)) ++loaded;   /* Model::modelFile, +0x10 + 4 */
    }
    std::fprintf(stderr, "[slot0] WIPES %p -> %p, %d/7 carry the frame-zero "
                 "vtable, %d/7 have a model file; installed fader %p %s\n",
                 before, (void *)w, same, loaded, data_0209f5bc,
                 data_0209f5bc == before ? "(unmoved, still valid)" : "(moved)");
}

}  /* anonymous namespace */

/* WHAT THE THREE MODES MEASURED, 2026-08-16, walk_window level 1,
   SM64DS_WINDOW_SELFTEST=60 SM64DS_FAULTS_FATAL=1. Every address below was
   resolved against walk_window.map. Read this before re-running: the point of
   the modes is that each one moves the failure, which is what makes them
   walls rather than one wall seen four times.

   MODE 0   selftest 60 frames, pos=(-4915200, 1040384, 26968144), exit 0.
            The control, and the shipped behaviour.

   MODE 1   FAULT c0000005 at Stage::LoadModel+0xd, accessing 0x0000006d,
            from Stage::InitResources+0x51c. WALL ONE, exactly as predicted
            above and for exactly the predicted reason. LoadModel's first line
            is
                Model::LoadAndSetFile(self + 0x86c,
                                      *(u16 *)(*(char **)data_0209f340 + 8), ...)
            and data_0209f340 had just been assigned
            data_02092208[data_0209f2f8].

            NAME THE INDEX CAREFULLY, because the first draft of this paragraph
            did not and mode 2 below disproves it. The index is NOT 1 and the
            word read is NOT the [1] entry 0x02112bdc: InitResources' own line
            208 does `data_0209f2f8 = data_02092110` unconditionally, 150
            statements before it indexes the table, so by then the sublevel is
            the PENDING one, which is -1 on the port. Both mode 1 and mode 2
            therefore read data_02092208[-1] -- the four bytes BEFORE the host
            array -- and 0x02112bdc is only what mode 2's seed print shows the
            [1] entry holding. The wall is unchanged either way: whichever
            entry it lands on, the port cannot use the value. What changes is
            which word to look at when re-running.

            The port maps main RAM (kRegions' MAIN_BASE, non-fatal), so the
            read does not fault -- it returns garbage, 0x65, and the fault
            lands at 0x65 + 8 = 0x6d. A mapped window makes this wall QUIETER
            than an unmapped one would, not safer.

   MODE 2   THE IDENTICAL FAULT, same address. Seeding data_02092208[1] alone
            changes nothing, and that null result is the finding: the re-latch
            above means the index is -1, so seeding row 1 seeds a row nothing
            reads. Hence mode 3.

   MODE 3   THE FAULT MOVES, to Heap::Allocate+0xd accessing 0x00000005, under
            SharedFilePtr::Load -> fs_hand_out -> Memory::Allocate with a heap
            pointer of 5. WALL THREE, AND IT IS NOT THIS SWAP'S BUG. It is a
            mis-bridged receiver of the method_faces.cpp class 3, sitting in
            hal/lk4_solidheap_seat.cpp:45:

                /alternatename:__ZN4Heap10SetDefaultEv=?SetDefault@Heap@@QAEHXZ

            The RHS is __thiscall and takes its receiver in ECX. EVERY C caller
            of the LHS passes it on the stack instead --
            src/_ZN4Heap20RestoreFromTemporaryEv.c:11,
            src/_ZN4Heap23SetupSolidHeapAsDefaultEjPS_i.c:18,
            src/func_ov007_020cc2cc.c:49 and :53, and
            src/_ZN5Stage13InitResourcesEv.cpp:234 -- so the method runs with a
            garbage `this` and its body is `int old = G; G = (int)this;
            return old`. It writes the default-heap pointer from a register
            nobody set. InitResources hits it twice, on the level-1
            LoadArchive(7) pair, and the RESTORE half is mis-bridged too, so
            the default heap is left garbage for the rest of the boot.
            THREE OF THOSE FOUR CALLERS ARE ALREADY COMPILED AND LINKED. This
            is a live latent bug in the shipped port that slot 0 merely
            reaches; the battery is green today because nothing on a selftest
            path calls them. Fixing it is a one-line face plus deleting the
            dead directive, and it belongs to whoever owns
            hal/lk4_solidheap_seat.cpp, with its own battery, because it
            changes behaviour for three callers that have nothing to do with
            slot 0.

   AND WALL FOUR EXISTS, measured with a THROWAWAY correct face for
   Heap::SetDefault that was built, run and then reverted rather than kept
   (keeping it is what the paragraph above declines). With the heap right, the
   run reaches Model::UpdateFileOffsets+0x25 accessing 0x600b8ca8, under
   Stage::LoadModel -> Model::LoadAndSetFile -> func_02016ff4. That one is NOT
   cleanly attributable: LoadModel is one of the steps the host boot body has
   already run in front of the probe, so a second load of the same stage model
   is exactly the artefact this header warns modes 1-3 can produce. It is
   recorded as "there is something behind wall three and it needs a real seat,
   not a probe, to name it".
   The throwaway face also proved the alias is live rather than dormant:
   defining the LHS strongly made port/tools/alternatename_guard.py fail the
   build with a NEW DEFEAT naming that exact directive, LHS and RHS addresses
   included. That is the guard doing its job and it is the reason the fix is
   somebody's deliberate change and not a quiet one.

   Returns InitResources' own result, or -2 for "declined, keep the host
   answer". hal/stage_bridges.cpp's slot-0 thunk is the only caller. */
extern "C" int port_slot0_rom_init(void *self)
{
    const int mode = slot0_mode();
    if (mode <= 0)
        return -2;
    void *wipes_before = data_0209f324;
    if (mode >= 3)
        port_slot0_seed_pending_level();
    if (mode >= 2)
        port_slot0_seed_lvl_table();
    std::fprintf(stderr, "[slot0] mode %d: calling the ROM's "
                 "Stage::InitResources on %p (level %d, LVL_Overlay %p)\n",
                 mode, self, (int)data_0209f2f8, (void *)data_0209f340);
    const int r = ((Stage *)self)->Stage::InitResources();
    std::fprintf(stderr, "[slot0] InitResources returned %d; LVL_Overlay is "
                 "now %p\n", r, (void *)data_0209f340);
    port_slot0_check_wipes(wipes_before);
    return r;
}

