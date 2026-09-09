/* ---------------------------------------------------------------------------
 * ctor_runner.cpp -- THE ROM'S OWN STATIC-INITIALISER RUNNER, AT ENTRY'S POINT.
 *
 * Run link100, lane CTOR, rung C1a. Lane CWSCOUT measured the root; this file
 * lands it.
 *
 * WHAT THE ROM DOES. Entry (arm9 0x02004800) ends with three instructions and
 * a tail jump, read out of extracted/arm9_dec.bin at base 0x02004000:
 *
 *     020048a0  bl 0x1ffafd4
 *     020048a4  bl 0x2019780        func_02019780()
 *     020048a8  bl 0x2072f94        func_02072f94()      <-- THIS ONE
 *     020048ac  ldr r1, [pc, #0x1c]     -> 0x02007000 = main
 *     020048b0  ldr lr, [pc, #0x1c]     -> 0xffff0000
 *     020048b4  bx  r1
 *
 * func_02072f94 is CodeWarrior's __init_cpp / static-initialiser runner and
 * src/func_02072f94.c is the whole of it:
 *
 *     fn_t *p = (fn_t *)0x2086b60;
 *     while (p != 0 && (f = *p) != 0) { f(); p++; }
 *
 * 0x02086b60 is the .ctor table: 23 __sinit_* addresses and a NULL terminator
 * at 0x02086bbc, confirmed both ways -- config/arm9/symbols.txt:3210-3232 names
 * the 23 words .p__sinit_<addr>, and the image itself reads:
 *
 *     02086b60: 02073a24   02086b7c: 02074e0c   02086b98: 02074fe4
 *     02086b64: 02073e6c   02086b80: 02074e44   02086b9c: 0207501c
 *     02086b68: 02074d90   02086b84: 02074e80   02086ba0: 02075054
 *     02086b6c: 02074da8   02086b88: 02074e84   02086ba4: 020750b4
 *     02086b70: 02074dbc   02086b8c: 02074edc   02086ba8: 020750b8
 *     02086b74: 02074dc0   02086b90: 02074f80   02086bac: 020750ec
 *     02086b78: 02074dc4   02086b94: 02074fb8   02086bb0: 0207511c
 *                                               02086bb4: 02075150
 *                                               02086bb8: 02075154
 *                                               02086bbc: 00000000
 *
 * WHERE THE TABLE LIVES IN THIS PORT, and why it is not just a host array.
 * The runner is a MATCHED TU and it reads a LITERAL DS ADDRESS. The port's
 * hosted DS globals are ordinary host symbols wherever the linker puts them,
 * so a host array called data_02086b60 would be invisible to that load. What
 * IS at 0x02086b60 in this process is DS main RAM: ntr/io.cpp reserves
 * 0x02000000..0x02400000 zero-filled from a TLS callback before the CRT runs,
 * which is the same mapping hal/level_boot.cpp leans on when it says a raw DS
 * pointer read "does not fault, because ntr/io.cpp reserves the DS address
 * space zero-filled". So the table is PUBLISHED there: kCtorTable below is the
 * ROM's 23 words plus the terminator with every word BOUND to the host body
 * that answers for that initialiser, and port_rom_entry_ctors() copies it to
 * 0x02086b60 one instruction before the ROM's own runner walks it.
 *
 * That is the port's romdata pointer-table contract, applied to a table the
 * port had never mounted: a relocated word carries a DS address, and a DS
 * address the port cannot execute must be bound to the host body before
 * anything dispatches through it. 0x02086b60 is arm9 .rodata (the ROM's own
 * .data does not start until 0x02086bc0), nothing else in this process writes
 * that page, and no overlay mounts anywhere near it -- the level and minigame
 * overlays start at 0x0210xxxx.
 *
 * MAIN RAM IS THE ONE RESERVED RANGE ntr/io.cpp CALLS SURVIVABLE TO LOSE, so
 * the publish is checked rather than assumed. If the page is not committed and
 * writable the runner is NOT called (a walk from an unmapped literal is an
 * access violation with no message), the table is run directly instead, and
 * the [ctor] line says so. That branch is a host safety net for a mapping the
 * port already documents as raceable; it is not the normal path and it says
 * when it is taken.
 *
 * WHERE IT IS CALLED FROM. hal/rom_main.cpp's port_rom_main_run(), at its
 * head, which tests/walk_window.cpp calls immediately after
 * port_boot_rom_pre_main() and Heap::InitializeRootHeap() -- func_02019780's
 * own body and its fourth call. So the port's order is now Entry's order:
 * func_02019780, then func_02072f94, then main. Nothing was moved to make room.
 *
 * WHAT RUNS AND WHAT DOES NOT. After rungs C1a, C1b, C1d, C1e, C1f and C1g,
 * and after run link100 lane CTOR3's rung 0 took C1c back out, TWENTY of the
 * 23 words are bound to the ROM's own initialiser and THREE are bound to a
 * FACE that names the initialiser it stands for and returns. The faces do not
 * abort. Word 0 was a face until rung C1e and an abort there would have killed
 * every boot before the first frame, which is where the rule came from; it
 * still holds for the five that are left, because a face that aborts turns a
 * named gap into a dead build and tells nobody anything the message did not
 * already say. They report once each, by name, and the
 * summary line counts them, which is what makes the walk complete AND honest:
 * the runner really visits all 23 words, and the run says exactly which ones
 * the port cannot answer for yet.
 *
 * THE TWO HAND CALLS ARE GONE IN THIS COMMIT. __sinit_02073e6c was called from
 * hal/level_boot.cpp's port_intro_seat_scripts() and __sinit_02074f80 from a
 * C++ static object in hal/fdr_arm9_fader_seat.cpp. Both are in the link
 * already (slice_intro.txt and slice_fdr.txt) and both are now words in this
 * table, so leaving the hand calls would have run them TWICE. Neither is
 * idempotent in the interesting direction -- __sinit_02074f80 pushes a
 * destructor node onto data_020aa3f0 through func_020731dc, so a second run
 * pushes a second node for the same object -- which is why the two words go
 * through counting shims and the [ctor] line prints both counters. One each is
 * the pass; anything else is a defect this file will say out loud.
 *
 * ORDER, stated because it is the whole risk of these rungs:
 *   * The eight initialisers rung C1a adds allocate nothing. Five have empty
 *     bodies; __sinit_02074d90 zeroes two words of data_0209b468;
 *     __sinit_02074da8 calls func_02011a5c, which zeroes data_0209b53c;
 *     __sinit_02074fb8 stores two pointers into data_020a0c68. No call to
 *     Heap::, Memory::, operator new or malloc appears anywhere in the eight
 *     or in func_02011a5c, so none of them can touch a game heap that Entry
 *     has not built yet. (The root heap DOES exist here -- walk_window calls
 *     Heap::InitializeRootHeap one line above -- and the game heap does not;
 *     neither is needed.)
 *   * All three globals are zeroed host BSS at this point in the boot and no
 *     host seat writes any of them before Entry: data_0209b468 and
 *     data_0209b53c are hal/actor_vtables.cpp storage that only actor and
 *     sound code fills, later; data_020a0c68 is hal/scene_link100_boot.cpp
 *     storage whose only ROM reader (func_0203506c) is not in this link.
 *   * The ov002 static initialisers tests/walk_window.cpp calls by hand keep
 *     their place AFTER this call, unchanged. That is the ROM's order too:
 *     ov002 is an overlay and is not resident at Entry, so its .ctor work
 *     cannot precede the arm9 table's. Nothing in the eight names an ov002
 *     symbol.
 *   * hal/level_boot.cpp still seats data_02089dcc's one code pointer at intro
 *     time. __sinit_02073e6c writes bytes 0x22..0x25, 0x3e..0x41 and
 *     0x30d..0x310 of that blob and the seat writes the word at 0x180, so the
 *     two are disjoint and the order between them cannot change a byte.
 *
 * RUNG C1b ADDS SIX MORE, and they allocate nothing either: five of the six are
 * a handful of field stores plus one func_020731dc push, and the sixth
 * (__sinit_02074dc4) additionally calls func_0203b9b4 on data_0209d4b8 and
 * Timer's constructor func_0201964c on data_0209d4c8, both of which are field
 * stores into storage hal/auto_bss.cpp already hosts. The same grep over the
 * six and over func_0201964c returns nothing for Heap, Memory, operator new or
 * malloc.
 *
 * TWO OF C1b's NINE ARE DELIBERATELY LEFT FACED and the reasons are in their
 * face messages:
 *   __sinit_02074e0c calls func_0201aa18, whose chain is
 *   func_0201aa18 -> func_0201aad4 -> func_0201aac8 -> func_02059ba0, and
 *   src/func_0201aac8.c is a tail-call veneer decompiled as `void(void)` that
 *   its caller invokes WITH an argument. On ARM the value rides r0 through the
 *   veneer; on the host it does not. Linking that at Entry would hand
 *   func_02059ba0 whatever the host left in a register. That is a PORT_HOST_ABI
 *   question, not a linkage one.
 *   __sinit_0207501c CONSTRUCTS a RaycastLine into data_020a0d0c, and
 *   hal/clsn_vtable.cpp hosts that as a 0x10 grouped-section block whose
 *   neighbours hal/mmc_vtable.cpp:148 asserts at ROM spacing. The constructor
 *   writes past +0x64. Whether that stays inside the band is a layout question
 *   owned by those two files.
 *
 * RUNG C1d ADDS __sinit_02074e84, the arm9 Clipper's own initialiser. It is
 * four statements: two pointer stores into data_0209f3c4 (the second wins; the
 * first is dead in the ROM itself), Clipper's constructor on data_0209f43c, and
 * one func_020731dc push. It allocates nothing -- _ZN7ClipperC1Ev writes its
 * vptr and calls Clipper::Func_020156DC, which is four field stores and a call
 * to Func_0201559C, and that one is fixed-point arithmetic over the object's
 * own words. The same grep for Heap, Memory, operator new and malloc over the
 * three bodies returns nothing.
 *
 * WHAT C1d HAD TO HOST, and why each one was safe to host here:
 *   _ZTV7Clipper   0x0208e730..0x0208e738, TWO words, both relocated
 *                  (config/arm9/relocs.txt 0x0208e730 -> 0x02015720 = D1,
 *                  0x0208e734 -> 0x020156fc = D0). Nothing in the port hosted
 *                  it and both readers -- src/_ZN7ClipperC1Ev.c and
 *                  src/_ZN7ClipperD1Ev.c -- are this rung's own TUs. Word 0 is
 *                  bound to the ROM's D1 body. WORD 1 IS A NAMED TRAP, not a
 *                  body: src/_ZN7ClipperD0Ev.c calls `base_dtor_Clipper`, an
 *                  INVENTED name for 0x0203cbcc that nothing in this tree
 *                  defines, so the deleting destructor is not linkable today.
 *                  It cannot fire either -- the only Clipper in this image is
 *                  the static data_0209f43c and nothing deletes it -- and it
 *                  says so out loud if it ever does, the shape
 *                  hal/arm9_tables_link100.cpp's a9t_trap rows take.
 *   data_0209f388  0x0209f388..0x0209f394, 12 bytes, the destruct-node cell of
 *                  this initialiser's func_020731dc push. Hosted nowhere. It
 *                  falls inside the nominal span of hal/auto_bss.cpp's generic
 *                  `int data_0209f37c[8]`, the same slack the C1b cells sit
 *                  beside: data_0209f37c's own ROM extent is 12 bytes and its
 *                  three readers (SetStarMarker, Minimap::Render,
 *                  Minimap::Behavior) index it as a byte array [0..7], so no
 *                  reader crosses +12 and the generosity is slack rather than
 *                  aliasing.
 *
 * AND FIVE MSVC NAMES, because src/__sinit_02074e84.cpp is a //cpp TU that
 * declares its five data names WITHOUT extern "C". Every one of the five LHS
 * strings below was transcribed from `dumpbin /symbols` on this tree's own
 * compile of that TU (tmp/probe), the rule hal/w8a_stage_storage.cpp's alias
 * block sets out: a hand-built mangling that is one letter off is a directive
 * that never fires and never says so.
 *
 * NOTHING IN THIS BUILD DISPATCHES WHAT C1d WRITES. data_0209f3c4's first word
 * becomes &data_02092188 (the Stage's graph-callback table,
 * hal/arm9_tables_link100.cpp section 3, all four slots seated). The only
 * reader of that vptr is func_02019144, through data_0209d4a8 -- and
 * data_0209d4a8 is null in this port: hal/w8a_stage_storage.cpp hosts it and
 * says nothing seats it, src/func_02019144.c guards the dispatch with
 * `if (p != 0)`, and the two TUs that do write it (func_0203506c,
 * func_ov004_020b265c) write a DIFFERENT object. data_0209f43c gets the ROM's
 * own 4:3 seed (0x1555, 0xe38, 0x1000, 0x01388000) at Entry, which
 * Camera::Render re-seeds through the same Clipper::Func_020156DC on every
 * frame anyway -- so the seed is the ROM's own value at the ROM's own point,
 * not a new one.
 *
 * RUNG C1e ADDS __sinit_02073a24, WORD 0 of the table and the last of the
 * campaign's clean rungs. It is 38 struct copies: the ROM's 19 camera State
 * objects at 0x0209b008..0x0209b138, step 0x10, each {onEnter PMF, main PMF},
 * filled from 38 relocated eight-byte pairs in arm9 .rodata at
 * 0x02086bc8..0x02086e48. It calls nothing and allocates nothing.
 *
 * THE DESTINATIONS ARE hal/camera_states.cpp's AND THEY STAY ITS. That file
 * hosts all nineteen at their ROM names and BAKES THE SAME VALUES IN, and says
 * so in its own header: "That sinit is not run on host (its 38 source pairs are
 * ROM pointer data that would each need its own symbol); the objects are
 * synthesized here with the same addresses, read out of the arm9 image once and
 * baked in below." This rung supplies exactly the thing that sentence says was
 * missing -- the 38 symbols -- and runs the ROM's own copy instead.
 *
 * SO THE WRITE IS PROVED REDUNDANT BEFORE IT IS ADDED, not assumed to be. All
 * 19 x 4 words were read out of extracted/arm9_dec.bin through the sinit's own
 * source-to-destination mapping and compared against camera_states.cpp's
 * nineteen CAMSTATE lines: 19 of 19 identical, both function words and both
 * zero this-deltas. The full table is out/CTOR2/c1e_equality.txt. The bake-in
 * is compile-time initialised data, so the values are already right before
 * Entry and the ROM's initialiser then writes the same bytes over them -- an
 * idempotent second write, not a second address for the same bytes and not a
 * host seat that decides the outcome. hal/camera_states.cpp is NOT this lane's
 * file: its bake-in and the two sentences above it are now redundant rather
 * than wrong, and retiring them is a proposal in this lane's report, not an
 * edit here.
 *
 * THE 38 KEEP THEIR DS ADDRESSES, and that is required rather than tolerated.
 * They are mwcc pointers to member -- {code address, this-delta} -- and nothing
 * calls through them: func_0200cae4 and func_0200ca50 read the word and hand it
 * to hal/camera_states.cpp's hal_call_camera_state_fn, which translates the DS
 * address to a host call through camera_states.inc. Binding these words to host
 * bodies the way data_0208ee14 below is bound would break that translation.
 * That is the port's standing mwcc pointer-to-member exception, at the storage
 * this time rather than at a call.
 *
 * Every word below was read twice and the two had to agree: the raw image at
 * extracted/arm9_dec.bin base 0x02004000, and config/arm9/relocs.txt, which
 * carries a `kind:load` row at each pair's FIRST word and none at its second --
 * so the code half is a relocation and the this-delta half is a literal zero,
 * in all 38.
 *
 * RUNG C1c IS OUT, AND THIS IS WHY, run link100 lane CTOR3 rung 0.
 * __sinit_02074edc constructs the two arm9 fader statics -- a FaderColor into
 * data_0209f5e8 and a FaderBrightness into data_0209f5d0, three vptr stores
 * each in base-to-derived order (data_0208eafc = Fader, data_0208eacc =
 * FaderBrightness, and for the first one data_0208eb2c = FaderColor), two
 * field stores each, and one func_020731dc push each. Lane CTOR2 landed it,
 * and it is the one rung of this campaign that changes what a LIVE object
 * dispatches: hal/fader_wipes.cpp placement-news a host HalFaderWipe over
 * data_0209f5e8 before main, so the ROM's initialiser at Entry becomes the
 * last writer of that vptr and the installed colour fader dispatches
 * hal/scene_boot.cpp's data_0208eb2c instead of the host class's table.
 *
 * THE TABLE IS NOT LIVE YET WHEN THAT HAPPENS. hal/scene_boot.cpp defines
 * data_0208eb2c as ten words of zeroed host storage and fills them in
 * l2_fill_0208ea6c(), which runs from scene_fill_title() -- one row of
 * port_scene_registry_install()'s walk, late in the boot. Between Entry and
 * that fill every one of the ten words is 0, so a dispatch through the vptr
 * this initialiser installs is a call to address 0.
 *
 * A LOOPBACK PAIR REACHES EXACTLY THAT, and run link100 lane WM9 caught it on
 * the integration tip: both windows died rc=0xC0000005 in
 * func_ov075_02116c8c -> LoadLevelNoReturn -> LoadLevel ->
 * Scene::SetAndStopColorFader -> Scene::SetFaders+0x27, whose faulting frame
 * is EIP 0 with eax = data_0208eb2c and ecx = data_0209f5e8, both read out of
 * that build's own walk_window.map. Scene::SetFaders asks the INSTALLED fader
 * `vt->f14` (ROM byte +0x14, FaderBrightness::IsAtStart) before replacing it,
 * and that word was still zero. The single-player gates never see it: the
 * title comes up first there and the fill has run by the time any level
 * loads.
 *
 * SO THE WORD GOES BACK TO ITS FACE HERE and the fix is the seat's, not this
 * file's: the fader vtables have to be live from before the ROM's own .ctor
 * walk, the way the cartridge's are. That is rung 1 of lane CTOR3, in
 * hal/scene_boot.cpp. The [ctor] census below stays either way, because it is
 * the line that names the table each vptr landed on and it is what makes this
 * visible in a captured run instead of only in prose.
 *
 * RUNGS C1f AND C1g ARE THE TWO WORDS LANE CTOR REFUSED THAT RE-MEASURED AS
 * RUNGS. Both refusals rested on a reading of another file's hosting, and both
 * readings were checked again against this tree's own map and source.
 *
 * C1f, __sinit_0207501c. CTOR refused it because RaycastLine's constructor
 * writes past +0x64 of data_020a0d0c and that symbol's host block is 0x10.
 * THE BLOCK IS 0x78. hal/clsn_vtable.cpp hosts the whole run contiguously in
 * ordered sections at ROM spacing -- data_020a0d0c 0x10, data_020a0d1c 0x44,
 * data_020a0d60 0x24, ending at 0x020a0d84 which is the next dsd symbol -- and
 * hal/mmc_vtable.cpp:148 asserts both deltas at run time on every boot,
 * precisely so a linker that stopped packing them would say so. +0x64 is 0x14
 * bytes inside that band. The three moving-collider DetectClsn bodies already
 * fill the same object every time they run, so a construction at Entry is the
 * ROM's own first fill and not a new writer. Both bodies it names were already
 * in the link; the only name it wanted is the destruct-node cell
 * data_020a0d00, below. Nothing in the constructor's chain (func_02035514,
 * func_0203819c, func_0203ac60) names Heap, Memory, operator new or malloc.
 *
 * C1g, __sinit_02075054. CTOR called it a CONFLICT because hal/auto_bss.cpp's
 * generic `int data_020a0db0[8]` is 32 bytes and covers data_020a0db8,
 * data_020a0dc0 and data_020a0dcc. IT IS SLACK, and that is measured rather
 * than argued: data_020a0db0's own ROM extent is 4 bytes and every reader in
 * the tree treats it as ONE WORD -- a bitmask tested with &1, &8, &0x10 and
 * &0x18 in MgBounceAndPounce::BeforeBehavior, HUD::RenderStarCount,
 * Stage::RenderBouncingArrows and Message::Update -- so nothing crosses +4 and
 * a byte-exact host beside it is the same shape as every other cell in this
 * file. The fourth name, data_020a0dbc, is ALREADY hosted byte-exact by
 * hal/auto_bss.cpp at its 4-byte ROM span, and that is the one the three ROM
 * readers use, so the initialiser writes the same storage they read.
 *
 * ONE DEFECT IS NAMED AND NOT FIXED HERE, because it is older than this rung
 * and it is another file's: the ROM bytes at 0x020a0dbc already have TWO host
 * addresses today, hal/auto_bss.cpp's byte-exact short data_020a0dbc[2] and
 * the interior of its own int data_020a0db0[8]. Linking this initialiser does
 * not add to that and does not depend on it -- it writes the byte-exact
 * symbol -- but narrowing data_020a0db0 to its ROM span is a one-line fix that
 * file should take whether or not this word is ever linked.
 *
 * ONE ROM DATA OBJECT IS HOSTED HERE, because __sinit_02074fb8 names it and
 * nothing else in the port does: data_0208ee14, 16 bytes at arm9 .data, the
 * four-slot Scene graph-callback table. Its words are code addresses and
 * config/arm9/relocs.txt carries the relocations, so it gets the same bind
 * treatment as the .ctor table itself. Scene::GraphCallback0/1/3 are already
 * in the link; GraphCallback2 joins them because this table's third word names
 * it. Every one of the four is an eight-byte `return 1`.
 * ---------------------------------------------------------------------------
 */

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include "dsstate_seg.h"

extern "C" {

/* The runner itself: src/func_02072f94.c, arm9 0x02072f94. */
void func_02072f94(void);

/* The ten initialisers this build answers for. */
void __sinit_02073e6c(void);   /* was hand-called from hal/level_boot.cpp */
void __sinit_02073a24(void);   /* C1e */
void __sinit_02074d90(void);
void __sinit_02074da8(void);
void __sinit_02074dbc(void);
void __sinit_02074dc0(void);
void __sinit_02074dc4(void);   /* C1b */
void __sinit_02074e44(void);   /* C1b */
void __sinit_02074e80(void);
void __sinit_02074e84(void);   /* C1d */
void __sinit_02074fe4(void);   /* C1b */
void __sinit_0207501c(void);   /* C1f */
void __sinit_02075054(void);   /* C1g */
void __sinit_02074f80(void);   /* was hand-called from hal/fdr_arm9_fader_seat.cpp */
void __sinit_02074fb8(void);
void __sinit_020750b4(void);
void __sinit_020750b8(void);   /* C1b */
void __sinit_020750ec(void);   /* C1b */
void __sinit_0207511c(void);   /* C1b */
void __sinit_02075150(void);

/* The two arm9 fader statics rung C1c's initialiser constructs, hosted by
   hal/auto_bss.cpp (data_0209f5d0, 12 bytes at the ROM span) and by
   hal/fader_wipes.cpp (data_0209f5e8, the gate-31 placement-new). Read here
   only, to report which vtable each one's vptr word ended up naming. */
extern int data_0209f5d0[3];
extern unsigned char data_0209f5e8[];

/* The three fader vtables hal/scene_boot.cpp hosts, for the same report. */
extern void *data_0208eafc[12];
extern void *data_0208eacc[12];
extern void *data_0208eb2c[10];

/* The nineteen camera State objects rung C1e's initialiser fills, hosted by
   hal/camera_states.cpp. Read here and never written: port_rom_entry_ctors
   snapshots their 76 words before the walk and compares them after, so the
   claim that __sinit_02073a24's fill is byte-for-byte redundant against that
   file's compile-time bake-in is CHECKED on every boot instead of trusted. */
extern unsigned int data_0209b008[4], data_0209b018[4], data_0209b028[4],
                    data_0209b038[4], data_0209b048[4], data_0209b058[4],
                    data_0209b068[4], data_0209b078[4], data_0209b088[4],
                    data_0209b098[4], data_0209b0a8[4], data_0209b0b8[4],
                    data_0209b0c8[4], data_0209b0d8[4], data_0209b0e8[4],
                    data_0209b0f8[4], data_0209b108[4], data_0209b118[4],
                    data_0209b128[4];

/* The MSL global-destructor chain head, hosted in hal/cxx_aliases.cpp. Every
   initialiser that constructs a static pushes a three-word node onto it
   through func_020731dc; nothing ever pops it. Its length is part of the
   [ctor] line because it is the one shape this rung changes that outlives the
   call. */
extern void *data_020aa3f0;

/* Clipper's destructor, the first word of _ZTV7Clipper below (rung C1d).
   src/_ZN7ClipperD1Ev.c, arm9 0x02015720. */
void _ZN7ClipperD1Ev(void *self);

/* Scene's four graph callbacks, the words of data_0208ee14 below. */
int _ZN5Scene14GraphCallback0Ev(void);
int _ZN5Scene14GraphCallback1Ev(void);
int _ZN5Scene14GraphCallback2Ev(void);
int _ZN5Scene14GraphCallback3Ev(void);

}  /* extern "C" */

/* ---- data_0208ee14, the four-slot Scene graph-callback table --------------
 *
 * ROM span 0x0208ee14..0x0208ee24 (next symbol in config/arm9/symbols.txt is
 * data_0208ee24), four words, every one of them a relocation into arm9 code:
 *
 *     0208ee14  02018eb8  _ZN5Scene14GraphCallback0Ev
 *     0208ee18  02018eb0  _ZN5Scene14GraphCallback1Ev
 *     0208ee1c  02018ea8  _ZN5Scene14GraphCallback2Ev
 *     0208ee20  02018ea0  _ZN5Scene14GraphCallback3Ev
 *
 * It is hosted here and not in hal/arm9_tables_link100.cpp because the reason
 * it is needed is this rung's: src/__sinit_02074fb8.c names it, and that
 * initialiser is one of the eight. The store it appears in is dead in the ROM
 * itself (`data_020a0c68[0] = &data_0208ee14;` is overwritten by the next
 * statement with &data_02094390), so nothing in this build reads a word back
 * out of this table -- but a table of code addresses is bound to the bodies
 * whether or not this build dispatches it, because that is the contract and
 * because the next lane to link func_0203506c inherits it. */
DSSTATE_BEGIN
extern "C" void *data_0208ee14[4] = {
    (void *)&_ZN5Scene14GraphCallback0Ev,
    (void *)&_ZN5Scene14GraphCallback1Ev,
    (void *)&_ZN5Scene14GraphCallback2Ev,
    (void *)&_ZN5Scene14GraphCallback3Ev,
};
DSSTATE_END

/* ---- rung C1b: the seven destruct-node cells --------------------------
 *
 * Every one of them is the third argument of a func_020731dc call in the
 * initialiser that names it -- the three-word node src/func_020731dc.c pushes
 * onto data_020aa3f0 (node[0] = previous head, node[1] = the destructor,
 * node[2] = the object). Nothing in this port hosted any of them, and nothing
 * in this port reads one back: the ROM's only reader of the chain is
 * func_02072f3c, the exit-time walker behind MSL's exit(), which is not in the
 * link.
 *
 * SIZED BY ROM SPAN, not by the three words the writer uses, per the port's
 * undersized-globals rule: each is the delta to the next symbol in
 * config/arm9/symbols.txt.
 *
 *     data_0209d4bc  0x0209d4bc..0x0209d4c8   12   __sinit_02074dc4
 *     data_0209e654  0x0209e654..0x0209e660   12   __sinit_02074e44
 *     data_020a0ce0  0x020a0ce0..0x020a0cec   12   __sinit_02074fe4
 *     data_020a0eb0  0x020a0eb0..0x020a0ebc   12   __sinit_020750b8
 *     data_020a0ec8  0x020a0ec8..0x020a0ed0    8   __sinit_020750ec (the object)
 *     data_020a0ed0  0x020a0ed0..0x020a0edc   12   __sinit_020750ec (the node)
 *     data_020a0ee4  0x020a0ee4..0x020a0ef0   12   __sinit_0207511c
 *
 * FOUR OF THE SEVEN ARE NOT COVERED BY ANY OTHER HOST OBJECT AT ALL. The other
 * three (data_0209d4bc, data_0209e654, data_020a0ed0) fall inside the nominal
 * span of one of hal/auto_bss.cpp's generic `int[8]` neighbours -- that file's
 * generous default, which it itself narrows to the ROM span wherever a ROM
 * reader cross-reads a block (see its data_020a0dbc note). Nothing cross-reads
 * these: the writer is func_020731dc and the reader is not in the link, so the
 * generosity is slack rather than aliasing and a byte-exact host beside it is
 * the honest shape rather than a second name for live storage. */
DSSTATE_BEGIN
extern "C" {
unsigned char data_0209d4bc[12];
unsigned char data_0209e654[12];
unsigned char data_020a0ce0[12];
unsigned char data_020a0eb0[12];
unsigned char data_020a0ec8[8];
unsigned char data_020a0ed0[12];
unsigned char data_020a0ee4[12];
}
DSSTATE_END

/* ---- rungs C1f and C1g: three more cells and one four-byte word ----------
 *
 *     data_020a0d00  0x020a0d00..0x020a0d0c  12  C1f, the RaycastLine node
 *     data_020a0db8  0x020a0db8..0x020a0dbc   4  C1g, the two flag halfwords
 *     data_020a0dc0  0x020a0dc0..0x020a0dcc  12  C1g, the first node
 *     data_020a0dcc  0x020a0dcc..0x020a0dd8  12  C1g, the second node
 *
 * All four sized by the delta to the next symbol in config/arm9/symbols.txt
 * and hosted nowhere before this. The header carries why the three that fall
 * inside hal/auto_bss.cpp's int data_020a0db0[8] are slack rather than
 * aliasing. data_020a0db8 is a short pair because that is how
 * src/__sinit_02075054.c writes it. */
DSSTATE_BEGIN
extern "C" {
unsigned char data_020a0d00[12];
short data_020a0db8[2];
unsigned char data_020a0dc0[12];
unsigned char data_020a0dcc[12];
}
DSSTATE_END

/* ---- rung C1c: the two fader destruct-node cells -------------------------
 *
 *     data_0209f5c4  0x0209f5c4..0x0209f5d0  12  the FaderBrightness node
 *     data_0209f5dc  0x0209f5dc..0x0209f5e8  12  the FaderColor node
 *
 * Both sized by the delta to the next symbol in config/arm9/symbols.txt, and
 * hosted nowhere before this. data_0209f5c4 falls inside the nominal span of
 * hal/auto_bss.cpp's generic `int data_0209f5b8[8]`, which is the same slack
 * the C1b and C1d cells sit beside: data_0209f5b8's own ROM extent is 4 bytes
 * and both of its readers (Scene::SetSceneToSpawn writes it, and
 * Scene::SpawnIfNecessary passes it to func_02013edc) touch exactly that one
 * word, so nothing crosses +4 and the generosity is slack rather than
 * aliasing. data_0209f5dc sits between hal/auto_bss.cpp's byte-exact
 * data_0209f5d0[0xc] and hal/fader_wipes.cpp's data_0209f5e8 and overlaps
 * neither. */
DSSTATE_BEGIN
extern "C" {
unsigned char data_0209f5c4[12];
unsigned char data_0209f5dc[12];
}
DSSTATE_END

/* ---- rung C1d: _ZTV7Clipper and one more destruct-node cell ---------------
 *
 * The header carries the derivation. The trap is defined before the table
 * because the table takes its address. */

namespace {
void ctor_trap_clipper_d0(void)
{
    std::fprintf(stderr, "  UNHOSTED: _ZTV7Clipper slot 1, _ZN7ClipperD0Ev at "
                         "0x020156fc -- src/_ZN7ClipperD0Ev.c calls "
                         "base_dtor_Clipper, an invented name for 0x0203cbcc "
                         "that nothing in this tree defines. The only Clipper "
                         "in this image is the static data_0209f43c and "
                         "nothing deletes it, so this slot is unreachable.\n");
    std::fflush(stderr);
}
}  /* anonymous namespace */

DSSTATE_BEGIN
extern "C" {
void *_ZTV7Clipper[2] = {
    (void *)&_ZN7ClipperD1Ev,
    (void *)&ctor_trap_clipper_d0,
};
unsigned char data_0209f388[12];
}
DSSTATE_END

/* ---- rung C1e: the 38 camera-state pairs ---------------------------------
 *
 * arm9 .rodata 0x02086bc8..0x02086e48, eight bytes each, sorted by address.
 * The comment on each line names the State object and the half it fills. The
 * derivation, the equality proof against hal/camera_states.cpp and the reason
 * these words stay DS addresses are all in the header. */
DSSTATE_BEGIN
extern "C" {
unsigned int data_02086bc8[2] = { 0x02009e70u, 0u };   /* data_0209b0b8 main */
unsigned int data_02086bd0[2] = { 0x02005324u, 0u };   /* data_0209b0d8 onEnter */
unsigned int data_02086be0[2] = { 0x02009aa8u, 0u };   /* data_0209b098 main */
unsigned int data_02086be8[2] = { 0x02009d30u, 0u };   /* data_0209b098 onEnter */
unsigned int data_02086bf0[2] = { 0x02009e70u, 0u };   /* data_0209b068 main */
unsigned int data_02086bf8[2] = { 0x02009e70u, 0u };   /* data_0209b048 main */
unsigned int data_02086c00[2] = { 0x0200987cu, 0u };   /* data_0209b0a8 main */
unsigned int data_02086c08[2] = { 0x0200985cu, 0u };   /* data_0209b0b8 onEnter */
unsigned int data_02086c10[2] = { 0x02009d70u, 0u };   /* data_0209b088 onEnter */
unsigned int data_02086c18[2] = { 0x020097ecu, 0u };   /* data_0209b0c8 onEnter */
unsigned int data_02086c28[2] = { 0x02009e70u, 0u };   /* data_0209b008 main */
unsigned int data_02086c30[2] = { 0x02009e70u, 0u };   /* data_0209b018 main */
unsigned int data_02086c40[2] = { 0x02009e70u, 0u };   /* data_0209b028 main */
unsigned int data_02086c48[2] = { 0x02009e30u, 0u };   /* data_0209b028 onEnter */
unsigned int data_02086c58[2] = { 0x02009e70u, 0u };   /* data_0209b038 main */
unsigned int data_02086c68[2] = { 0x02009df0u, 0u };   /* data_0209b048 onEnter */
unsigned int data_02086c70[2] = { 0x02009db0u, 0u };   /* data_0209b068 onEnter */
unsigned int data_02086c98[2] = { 0x02009e10u, 0u };   /* data_0209b038 onEnter */
unsigned int data_02086ce0[2] = { 0x020050dcu, 0u };   /* data_0209b0e8 onEnter */
unsigned int data_02086d10[2] = { 0x020098b8u, 0u };   /* data_0209b0a8 onEnter */
unsigned int data_02086d28[2] = { 0x0200af0cu, 0u };   /* data_0209b008 onEnter */
unsigned int data_02086d30[2] = { 0x02009e70u, 0u };   /* data_0209b078 main */
unsigned int data_02086d48[2] = { 0x02009e70u, 0u };   /* data_0209b058 main */
unsigned int data_02086d58[2] = { 0x02009dd0u, 0u };   /* data_0209b058 onEnter */
unsigned int data_02086d90[2] = { 0x02009e70u, 0u };   /* data_0209b088 main */
unsigned int data_02086da8[2] = { 0x02009e50u, 0u };   /* data_0209b018 onEnter */
unsigned int data_02086db0[2] = { 0x020095e4u, 0u };   /* data_0209b0c8 main */
unsigned int data_02086db8[2] = { 0x02009d90u, 0u };   /* data_0209b078 onEnter */
unsigned int data_02086dc8[2] = { 0x02005000u, 0u };   /* data_0209b128 main */
unsigned int data_02086dd0[2] = { 0x02005060u, 0u };   /* data_0209b128 onEnter */
unsigned int data_02086de0[2] = { 0x020094c0u, 0u };   /* data_0209b118 main */
unsigned int data_02086de8[2] = { 0x020094e4u, 0u };   /* data_0209b118 onEnter */
unsigned int data_02086df8[2] = { 0x020098e0u, 0u };   /* data_0209b108 main */
unsigned int data_02086e00[2] = { 0x02009a8cu, 0u };   /* data_0209b108 onEnter */
unsigned int data_02086e10[2] = { 0x02009540u, 0u };   /* data_0209b0f8 main */
unsigned int data_02086e18[2] = { 0x020095c4u, 0u };   /* data_0209b0f8 onEnter */
unsigned int data_02086e28[2] = { 0x02005098u, 0u };   /* data_0209b0e8 main */
unsigned int data_02086e40[2] = { 0x02005110u, 0u };   /* data_0209b0d8 main */
}
DSSTATE_END

/* The five MSVC spellings src/__sinit_02074e84.cpp emits, transcribed from
   dumpbin /symbols on this tree's own compile of it. */
#pragma comment(linker, "/alternatename:?data_0208ee14@@3DA=_data_0208ee14")
#pragma comment(linker, "/alternatename:?data_02092188@@3DA=_data_02092188")
#pragma comment(linker, "/alternatename:?data_0209f388@@3DA=_data_0209f388")
#pragma comment(linker, "/alternatename:?data_0209f3c4@@3RAXA=_data_0209f3c4")
#pragma comment(linker, "/alternatename:?data_0209f43c@@3DA=_data_0209f43c")

namespace {

/* ---- the counters -------------------------------------------------------- */

int g_hits_02073e6c;
int g_hits_02074f80;
int g_faces_reported;
int g_rom_bodies_run;

/* ---- the two counting shims ---------------------------------------------- */

void ctor_02073e6c(void)
{
    ++g_hits_02073e6c;
    __sinit_02073e6c();
}

void ctor_02074f80(void)
{
    ++g_hits_02074f80;
    __sinit_02074f80();
}

/* ---- the thirteen faces --------------------------------------------------
 *
 * One function per initialiser so the name in the message is the name of the
 * word that reached it, not a shared string with an argument. Each reports
 * once. They RETURN: see the header for why an abort at word 0 would be a
 * boot that never starts rather than a fact anybody could act on. */

void ctor_face(const char *name, const char *why)
{
    ++g_faces_reported;
    std::fprintf(stderr, "  [ctor] NOT LINKED: %s -- %s\n", name, why);
}

#define CTOR_FACE(sym, why)                                                    \
    void ctor_face_##sym(void)                                                 \
    {                                                                          \
        static int said;                                                       \
        if (said) { ++g_faces_reported; return; }                              \
        said = 1;                                                              \
        ctor_face("__sinit_" #sym, why);                                       \
    }

CTOR_FACE(02074e0c, "func_0201aa18 -> func_0201aad4 -> func_0201aac8 is an argument-dropping tail-call veneer chain into func_02059ba0; a PORT_HOST_ABI question, not a linkage one")
CTOR_FACE(02074edc, "it constructs the two arm9 fader statics and makes data_0208eb2c the live vptr of data_0209f5e8 -- but hal/scene_boot.cpp fills that table from scene_fill_title, late in the boot, so between Entry and the fill its ten words are zero. Run link100 lane WM9's loopback pair died on it: Scene::SetFaders read byte +0x14 as 0 and called address 0 on the VS menu -> level transition. Rung 1 of lane CTOR3 makes the seat unconditional; this word is faced until then")
CTOR_FACE(02075154, "data_02099f48..data_02099f70 are hosted nowhere -- five mwcc pointer-to-member pairs -- and hal/actor_registry.cpp:412 already seats the same five list callbacks with host wrappers, LATER in the boot, so linking this would write the heads at Entry and have every word overwritten")

#undef CTOR_FACE

/* ---- the table -----------------------------------------------------------
 *
 * The ROM's 23 words in the ROM's order, plus the ROM's NULL terminator. Word
 * N here IS the word at 0x02086b60 + 4*N, and the comment on each line is the
 * DS address that word holds in the cartridge. */

typedef void (*ctor_fn)(void);

struct CtorWord {
    unsigned    ds;       /* what the ROM's word holds */
    ctor_fn     host;     /* what this build binds it to */
    const char *name;     /* the initialiser's name, for the log */
    int         rom;      /* 1 = the ROM's own body runs, 0 = a face */
};

const CtorWord kCtorTable[] = {
    { 0x02073a24, __sinit_02073a24, "__sinit_02073a24", 1 },
    { 0x02073e6c, ctor_02073e6c,      "__sinit_02073e6c", 1 },
    { 0x02074d90, __sinit_02074d90,   "__sinit_02074d90", 1 },
    { 0x02074da8, __sinit_02074da8,   "__sinit_02074da8", 1 },
    { 0x02074dbc, __sinit_02074dbc,   "__sinit_02074dbc", 1 },
    { 0x02074dc0, __sinit_02074dc0,   "__sinit_02074dc0", 1 },
    { 0x02074dc4, __sinit_02074dc4, "__sinit_02074dc4", 1 },
    { 0x02074e0c, ctor_face_02074e0c, "__sinit_02074e0c", 0 },
    { 0x02074e44, __sinit_02074e44, "__sinit_02074e44", 1 },
    { 0x02074e80, __sinit_02074e80,   "__sinit_02074e80", 1 },
    { 0x02074e84, __sinit_02074e84, "__sinit_02074e84", 1 },
    { 0x02074edc, ctor_face_02074edc, "__sinit_02074edc", 0 },
    { 0x02074f80, ctor_02074f80,      "__sinit_02074f80", 1 },
    { 0x02074fb8, __sinit_02074fb8,   "__sinit_02074fb8", 1 },
    { 0x02074fe4, __sinit_02074fe4, "__sinit_02074fe4", 1 },
    { 0x0207501c, __sinit_0207501c, "__sinit_0207501c", 1 },
    { 0x02075054, __sinit_02075054, "__sinit_02075054", 1 },
    { 0x020750b4, __sinit_020750b4,   "__sinit_020750b4", 1 },
    { 0x020750b8, __sinit_020750b8, "__sinit_020750b8", 1 },
    { 0x020750ec, __sinit_020750ec, "__sinit_020750ec", 1 },
    { 0x0207511c, __sinit_0207511c, "__sinit_0207511c", 1 },
    { 0x02075150, __sinit_02075150,   "__sinit_02075150", 1 },
    { 0x02075154, ctor_face_02075154, "__sinit_02075154", 0 },
};
const unsigned kCtorWords = sizeof kCtorTable / sizeof kCtorTable[0];

/* The ROM's own address of the table and of its terminator word. */
const unsigned kCtorBase = 0x02086b60u;

/* How long the MSL global-destructor chain is. Nodes are three words and
   node[0] is the previous head (src/func_020731dc.c), so this is a walk, not a
   counter somebody has to keep in step. The cap is a cycle guard: a corrupt
   chain must not hang the boot. */
int dtor_chain_len(void)
{
    int n = 0;
    void **p = (void **)data_020aa3f0;
    while (p && n < 4096) { p = (void **)p[0]; ++n; }
    return n;
}

/* The nineteen camera State objects, in ROM order, for the C1e redundancy
   check. Nineteen named symbols rather than one array walk: those are nineteen
   separate definitions in hal/camera_states.cpp and only the linker decides
   they are adjacent. */
unsigned int *const kCamStates[19] = {
    data_0209b008, data_0209b018, data_0209b028, data_0209b038, data_0209b048,
    data_0209b058, data_0209b068, data_0209b078, data_0209b088, data_0209b098,
    data_0209b0a8, data_0209b0b8, data_0209b0c8, data_0209b0d8, data_0209b0e8,
    data_0209b0f8, data_0209b108, data_0209b118, data_0209b128,
};

void cam_states_snapshot(unsigned int *out)
{
    for (int i = 0; i < 19; ++i)
        for (int j = 0; j < 4; ++j)
            out[i * 4 + j] = kCamStates[i][j];
}

int cam_states_changed(const unsigned int *before)
{
    int n = 0;
    for (int i = 0; i < 19; ++i)
        for (int j = 0; j < 4; ++j)
            if (kCamStates[i][j] != before[i * 4 + j]) ++n;
    return n;
}

/* Is the ROM's table address a committed, writable page in this process? */
bool ctor_page_ok(void)
{
#if defined(_WIN32)
    MEMORY_BASIC_INFORMATION mbi;
    std::memset(&mbi, 0, sizeof mbi);
    if (!VirtualQuery((LPCVOID)(size_t)kCtorBase, &mbi, sizeof mbi))
        return false;
    if (mbi.State != MEM_COMMIT)
        return false;
    const DWORD w = PAGE_READWRITE | PAGE_WRITECOPY |
                    PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    if ((mbi.Protect & w) == 0)
        return false;
    /* The whole table plus the terminator has to be inside the same region. */
    const size_t need = (size_t)(kCtorWords + 1) * 4u;
    const size_t have = (size_t)mbi.RegionSize -
                        ((size_t)kCtorBase - (size_t)mbi.BaseAddress);
    return have >= need;
#else
    return false;
#endif
}

}  /* anonymous namespace */

extern "C" void port_rom_entry_ctors(void)
{
    static int done;
    if (done) {
        std::fprintf(stderr, "  [ctor] port_rom_entry_ctors called twice; the "
                             "second call did nothing\n");
        return;
    }
    done = 1;

    const int dtors_before = dtor_chain_len();
    unsigned int cam_before[19 * 4];
    cam_states_snapshot(cam_before);
    const bool page = ctor_page_ok();

    if (page) {
        unsigned *w = (unsigned *)(size_t)kCtorBase;
        for (unsigned i = 0; i < kCtorWords; ++i)
            w[i] = (unsigned)(size_t)kCtorTable[i].host;
        w[kCtorWords] = 0;                 /* the ROM's own terminator */
        func_02072f94();                   /* Entry's own call, 0x020048a8 */
    } else {
        std::fprintf(stderr, "  [ctor] DS main RAM at %08x is not committed "
                             "and writable, so the ROM's runner cannot walk "
                             "its own table; running the 23 words directly. "
                             "See ntr/io.cpp: main RAM is the one reserved "
                             "range a process may lose.\n", kCtorBase);
        for (unsigned i = 0; i < kCtorWords; ++i)
            kCtorTable[i].host();
    }

    for (unsigned i = 0; i < kCtorWords; ++i)
        if (kCtorTable[i].rom) ++g_rom_bodies_run;

    const int dtors_after = dtor_chain_len();

    std::fprintf(stderr,
                 "[ctor] func_02072f94 walked %u words at %08x (%s): %d ran as "
                 "ROM bodies, %d as faces; __sinit_02073e6c hits %d, "
                 "__sinit_02074f80 hits %d; dtor chain on data_020aa3f0 "
                 "%d -> %d\n",
                 kCtorWords, kCtorBase,
                 page ? "the ROM's own runner" : "host fallback walk",
                 g_rom_bodies_run, (int)kCtorWords - g_rom_bodies_run,
                 g_hits_02073e6c, g_hits_02074f80,
                 dtors_before, dtors_after);

    /* RUNG C1e, CHECKED RATHER THAN CLAIMED. __sinit_02073a24 copies 38 ROM
       constants into hal/camera_states.cpp's nineteen State objects, which
       that file already bakes the same values into at compile time. Zero
       changed words is the pass: it says the ROM's own initialiser wrote
       exactly what was already there, so this rung moved the WRITER and not
       the bytes. */
    {
        const int changed = cam_states_changed(cam_before);
        if (changed == 0)
            std::fprintf(stderr, "  [ctor] camera-state fill check PASS: "
                                 "__sinit_02073a24 wrote 76 words over "
                                 "hal/camera_states.cpp's bake-in and changed "
                                 "0 of them\n");
        else
            std::fprintf(stderr, "  [ctor] CAMERA-STATE FILL CHANGED %d of 76 "
                                 "words: the ROM's constants and "
                                 "hal/camera_states.cpp's bake-in disagree\n",
                         changed);
    }

    /* THE ONE THING A BYTE GATE CANNOT SEE, kept after rung C1c came back out.
       When __sinit_02074edc is bound to the ROM's body it is the last writer
       of both fader vptrs, after hal/fader_wipes.cpp's gate-31 placement-new
       has already run; when it is faced, as it is now, the colour fader keeps
       the host HalFaderWipe's table and data_0209f5d0 keeps a null vptr.
       Naming the table each one landed on is what makes either state visible
       in every captured run instead of only in this file's prose, and it is
       how the regression above was read. */
    {
        const void *cv = *(void *const *)(const void *)data_0209f5e8;
        const void *bv = *(void *const *)(const void *)data_0209f5d0;
        std::fprintf(stderr,
                     "  [ctor] fader vptrs after the walk: data_0209f5e8 -> %s, "
                     "data_0209f5d0 -> %s\n",
                     cv == (const void *)data_0208eb2c ? "data_0208eb2c "
                         "(_ZTV10FaderColor, ten slots seated in "
                         "hal/scene_boot.cpp)"
                     : cv == (const void *)data_0208eacc ? "data_0208eacc"
                     : cv == (const void *)data_0208eafc ? "data_0208eafc"
                     : cv == 0 ? "NULL"
                     : "a table this file does not name (the host "
                       "HalFaderWipe's, or something else)",
                     bv == (const void *)data_0208eacc ? "data_0208eacc "
                         "(_ZTV15FaderBrightness, dtor pair seated and every "
                         "slot from +0x08 up a named trap)"
                     : bv == (const void *)data_0208eafc ? "data_0208eafc"
                     : bv == 0 ? "NULL"
                     : "a table this file does not name");
    }

    if (g_hits_02073e6c != 1 || g_hits_02074f80 != 1)
        std::fprintf(stderr, "  [ctor] ONCE-ONLY CHECK FAILED: the two "
                             "initialisers that used to be hand-called ran "
                             "%d and %d times, not 1 and 1\n",
                     g_hits_02073e6c, g_hits_02074f80);
    else
        std::fprintf(stderr, "  [ctor] once-only check PASS: __sinit_02073e6c "
                             "1, __sinit_02074f80 1\n");
}
