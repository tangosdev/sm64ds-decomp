/* ---- link100 SCENE: dScBoot_c / BootScene, ACTOR ID 0 ----------------------
 *
 * THE ROM'S FIRST SCENE. Not a menu and not an overlay: an arm9-resident Scene
 * subclass the game spawns before anything else exists, which puts the health
 * and safety card up and then hands off. Everything below is read out of the
 * ROM by address, and every address is quoted where it is used so a reviewer
 * can re-derive it from config/ without this file.
 *
 * ---- 1. THE IDENTITY ------------------------------------------------------
 *
 * The id is the ARM9 spawn table's own index, the rule every scene row in
 * hal/scene_boot.cpp uses:
 *
 *   config/arm9/relocs.txt  from:0x02090864 kind:load to:0x020914a8 module:main
 *   and (0x02090864 - 0x02090864) / 4 = 0, so the record at 0x020914a8 is
 *   actor id 0.
 *
 * The class name comes from the ROM's own RTTI rather than from a filename.
 * The word BEFORE the vtable is the type_info pointer (read out of
 * extracted/arm9_dec.bin, which is flat at base 0x02004000 -- the base
 * port/tools/romdata.py pins and anchors on Copy36Bytes):
 *
 *   0x02091524 -> 0x020914c8              the type_info record
 *   0x020914c8 + 8 -> the name at 0x020914bc, which reads "9dScBoot_c"
 *
 * and config/arm9/symbols.txt names slot 17 of the table _ZN9BootSceneD0Ev, so
 * the two spellings this tree carries for the class -- dScBoot_c (the ROM's
 * mangled RTTI name) and BootScene (the decomp's) -- are one object.
 *
 * src/GetSceneOverlayID.c has NO case for 0 and returns -1, so this scene
 * loads no overlay: arm9 code and arm9 data end to end. That is why this seat
 * needs no mount, and why its vtable and its SpawnInfo are host arrays below
 * rather than mounted bytes.
 *
 * ---- 2. THE EIGHTEEN SLOTS, READ FROM config/arm9/relocs.txt --------------
 *
 * Every word is a `from:0x02091528 + 4*slot` row quoted as its `to:`; nothing
 * here is inferred from a slot number and no slot is a trap -- all eighteen
 * have a body, and the four the class OWNS are the ones this file thunks.
 *
 *    0  InitResources          0x02005a58  func_02005a58        OWN
 *    1  BeforeInitResources    0x0202e638  Scene::
 *    2  AfterInitResources     0x0202e62c  Scene::   VENEER
 *    3  CleanupResources       0x02043bf0  ActorBase::
 *    4  BeforeCleanupResources 0x0202e5f0  Scene::
 *    5  AfterCleanupResources  0x0202e5d0  Scene::
 *    6  Behavior               0x02005418  func_02005418        OWN
 *    7  BeforeBehavior         0x0202e3d4  Scene::
 *    8  AfterBehavior          0x0202e3c8  Scene::   VENEER
 *    9  Render                 0x02043af0  ActorBase::
 *   10  BeforeRender           0x0202e3a4  Scene::
 *   11  AfterRender            0x0202e398  Scene::   VENEER
 *   12  OnPendingDestroy       0x02043ac0  ActorBase::
 *   13  Virtual34              0x0204357c  ActorBase::
 *   14  Virtual38              0x0204349c  ActorBase::
 *   15  OnHeapCreated          0x02043494  ActorBase::
 *   16  D2                     0x02023598  _ZN5SceneD2Ev        OWN (see 3)
 *   17  D0                     0x020235d4  _ZN9BootSceneD0Ev    OWN
 *
 * THE WIDTH IS EIGHTEEN AND THE ROM SAYS SO TWICE. The next named symbol after
 * data_02091528 in config/arm9/symbols.txt is data_02091570, a delta of 0x48 =
 * 18 words; and src/func_02005a58.c reads data_02091570 as an LZ blob, so that
 * boundary is a real object rather than a padding artefact.
 *
 * ---- 3. SLOT 16 IS Scene::~Scene AND THAT IS NOT A MISTAKE ---------------
 *
 * config/arm9/symbols.txt names 0x02023598 _ZN5SceneD2Ev, and that address is
 * what this class's slot 16 holds. The body settles it: src/_ZN5SceneD2Ev.c's
 * FIRST vptr store is data_02091528 -- this table. CodeWarrior folded the two
 * complete-object destructors, because BootScene adds no member that needs
 * destroying and its D2 is instruction for instruction Scene's, and the linker
 * kept one copy under the base class's name. The port seats the body the ROM's
 * table points at, which is the only rule this file follows.
 *
 * ---- 4. THE TWO ADJUDICATED BODIES ---------------------------------------
 *
 * src/func_02005a58.c and src/func_02005418.c carry the "recovered from vtable
 * slot identity" marker, so port/tools/inferred_stub_guard.py would refuse
 * this seat on its own. Both are ruled REAL_DECOMP in
 * port/tools/inferred_stub_adjudicated.txt off the BYTE GATE (run link100 lane
 * MATCH): compiled with the pinned mwccarm 2004/b56 and compared against the
 * ROM by tools/match.py --strict-relocs, 0x33c and 0x640 bytes reproduced
 * exactly with every relocation landing where config says it should. The
 * marker records how the NAME was recovered and says nothing about the body.
 *
 * ---- 5. WHAT THE SCENE ASKS FOR NEXT -------------------------------------
 *
 * src/func_02005418.c (Behavior) is a small state machine over self+0x50 that
 * ends in
 *
 *     Scene::StartSceneFade(func_0203da3c() != 0 ? 6 : 1, 0, 0)
 *
 * -- scene 6, the VS / wireless entry menu, when the wireless boot flag is
 * set, and otherwise SCENE 1, THE TITLE. So on the ROM the boot scene is the
 * step BEFORE the title, and the port's default route (title first,
 * hal/title_entry.cpp) is one step SHORT of the ROM's rather than different
 * from it. THIS LANE DOES NOT MOVE THE DEFAULT ROUTE. What moving it would
 * take is written up in the lane report rather than tried here.
 *
 * ---- 6. THE SUBLEVEL ANSWER, MEASURED ------------------------------------
 *
 * reads_sublevel is 0 in the registry row, measured rather than assumed: the
 * ROM reloc-graph closure of this class is 873 arm9 functions (everything
 * transitively reachable from the factory and the four own slots through
 * config/arm9/relocs.txt) and NOT ONE of their src/ TUs names data_02092110.
 */

#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

extern "C" {

/* ---- the class's own four, plus the factory. Flat C names out of src/, the
   way every arm9 method in this tree is spelled. */
int   func_02005a58(char *self);        /* slot  0  InitResources          */
int   func_02005418(void *self);        /* slot  6  Behavior               */
void *_ZN5SceneD2Ev(void *self);        /* slot 16  D2  (folded, see 3)    */
void *_ZN9BootSceneD0Ev(void *self);    /* slot 17  D0                     */
void *func_02023624(void);              /* the factory, 0x02023624         */

/* the shared halves (hal/scene_boot.cpp, hal/scene_link100_base_faces.cpp) */
unsigned port_scene_fill_rom(void **vt, unsigned n);
int  port_scene_env_want(void);
int  port_scene_link100_base_render(void *self);
void port_scene_link100_base_pending_destroy(void *self);

void port_scene_boot_report(void);

/* ---- THE VTABLE, a host array under the ROM'S OWN DATA NAME ---------------
 *
 * data_02091528 is what the factory writes into the object's +0 word
 * (src/func_02023624.c) and what both destructor bodies write back on the way
 * down (src/_ZN5SceneD2Ev.c, src/_ZN9BootSceneD0Ev.c), so the spelling has to
 * be exactly this and at C linkage. arm9 .data is NOT mounted -- there is no
 * arm9 equivalent of port/ovNNN_syms.txt -- so this is a FRESH HOST ARRAY, the
 * shape hal/actor_vtables.cpp's data_0208e4b8 and hal/scene_boot.cpp's
 * data_ov003_020b1704 already have.
 *
 * IT IS INITIALISED WITH THE ROM'S OWN EIGHTEEN WORDS, transcribed from the
 * relocation rows in section 2, and then rewritten by the fill below. Two
 * reasons, neither decoration. It lets the fill be port_scene_fill_rom -- the
 * KEYED fill, which replaces a slot by the ROM address it holds rather than by
 * its index, and therefore cannot quietly write a slot this class overrides.
 * And it makes the fill SELF-CHECKING: the keyed pass returns how many slots
 * it left holding a raw DS word, and for this class that has to be exactly
 * six, which is exactly what the six writes below account for.
 *
 * NOTHING DISPATCHES THROUGH IT BEFORE THE FILL. port_scene_registry_install
 * runs the fill on every boot, level runs included, before any actor spawns,
 * so the raw words live for the length of one function call and the count
 * check is what proves the transcription right.
 *
 * BOTH THIS AND THE SpawnInfo RECORD BELOW ARE BRACKETED IN .dsstate, the
 * dScStarSel_c precedent (hal/scene_boot.cpp's data_ov003_020b1704): a fresh
 * host array outside any mount is not swept by an overlay's own bracket, and
 * port/tools/dsstate_guard.py refuses a hosted DS symbol left outside
 * [dsstate_lo, dsstate_hi). Both arrays are written once (the fill; the
 * registry install) and never mutated again, but the guard's rule is the
 * address range, not the mutation rate -- see hal/dsstate_seg.h. */
DSSTATE_BEGIN
void *data_02091528[18] = {
    (void *)0x02005a58u, (void *)0x0202e638u, (void *)0x0202e62cu,
    (void *)0x02043bf0u, (void *)0x0202e5f0u, (void *)0x0202e5d0u,
    (void *)0x02005418u, (void *)0x0202e3d4u, (void *)0x0202e3c8u,
    (void *)0x02043af0u, (void *)0x0202e3a4u, (void *)0x0202e398u,
    (void *)0x02043ac0u, (void *)0x0204357cu, (void *)0x0204349cu,
    (void *)0x02043494u, (void *)0x02023598u, (void *)0x020235d4u,
};

/* ---- THE SpawnInfo RECORD -------------------------------------------------
 *
 * The ROM's record is eight bytes at 0x020914a8: a factory pointer at +0 (a
 * relocation -- `from:0x020914a8 kind:load to:0x02023624 module:main`) and the
 * word 0x00020000 at +4, whose LOW halfword is the actor id the registry
 * cross-checks. Read out of extracted/arm9_dec.bin at 0x020914a8 - 0x02004000.
 *
 * A HOST ARRAY rather than a romdata blob, for the reason the vtable above is
 * one: the +0 word is a relocation carrying a DS code address, and
 * port/tools/romdata.py's contract is that a NAMED span is reloc-free. The
 * factory word is left zero because port_scene_registry_install writes it --
 * the same word it writes for every mounted row -- and the id halfword is the
 * ROM's, so the registry's "record says id N, the spawn table says M" check
 * still reads the ROM's own number. */
unsigned char data_020914a8[8] = { 0, 0, 0, 0,   0x00, 0x00, 0x02, 0x00 };
DSSTATE_END

/* ---- THE TWO BSS OBJECTS THE PAIR'S OWN BODIES NAME ----------------------
 *
 * Neither is hosted anywhere else in the port (checked across port/hal and
 * port/ntr). Both are arm9 .bss by config/arm9/symbols.txt -- kind:bss, above
 * the 0x0209b000 boundary port/tools/romdata.py pins -- so they are zeroed
 * storage, and they go into .dsstate with the rest of the port's hosted DS bss
 * so a save state rolls them back with what they belong to.
 *
 * data_020a0c68 is the 0x10-byte record src/func_0203506c.c hands to
 * func_02034b1c and then publishes through data_0209d4a8; data_020a0c64 is the
 * four-byte handle the same body stores func_0201a244's result in. Both belong
 * to dScMB_c and live here only because this file is the first of the pair. */
DSSTATE_BEGIN
unsigned char data_020a0c68[0x10];
unsigned char data_020a0c64[4];
DSSTATE_END

/* ---- THE REST OF THE SLICE'S BSS, six four-byte objects -------------------
 *
 * Every one is kind:bss in config/arm9/symbols.txt, four bytes by the
 * delta-to-next-symbol rule, and none is defined anywhere else in the port
 * (checked against the pre-seat walk_window.map: not one of the six is in it).
 * They are zeroed storage and they go into .dsstate with the rest of the
 * port's hosted DS bss.
 *
 *   data_0209d4e0  "the id-6 wireless overlay is resident" latch, written by
 *                  src/func_0201a694.c and cleared by src/func_0201a614.c.
 *   data_0209d520  the root heap Heap::CreateRootHeap returns
 *                  (src/func_0201a3e4.c); zero means "not created yet",
 *                  which is what the DS boots with.
 *   data_0209d674  the message renderer's BG2 character-row base
 *                  (src/func_0201b100.c).
 *   data_020a0c5c  the heap Heap::SetDefault displaced, saved and restored by
 *                  src/func_02034fbc.c.
 *   data_020a0c60  that body's own once-only bit-0 gate.
 *   data_0209f1e8  where src/func_02005a58.c parks func_0201a244's answer.
 *
 * NONE OF THE SIX IS A ROMDATA CANDIDATE, and the distinction is the one
 * port/tools/romdata.py's header draws: an address at or past 0x0209b000 is
 * runtime-initialised and belongs in the HAL. The one sentinel in this seat's
 * closure that is NOT bss -- data_0208ee4c, whose ROM value is -1 -- is in
 * that file's NAMED list instead, for exactly that reason. */
DSSTATE_BEGIN
extern unsigned char data_0209d4e0[4];   /* hosted by hal/boot_globals.cpp */
void *data_0209d520;
extern unsigned char data_0209d674[4];   /* hosted by hal/stage_globals.cpp */
void *data_020a0c5c;
unsigned data_020a0c60;
unsigned char data_0209f1e8[4];
DSSTATE_END

/* ---- THE SEVEN OVERLAY-ID SYMBOLS THIS CLOSURE NAMES ----------------------
 *
 * The same mechanism, and the same honest limitation, that hal/scene_boot.cpp
 * section 2 already spells out for overlay_64/66/100/102: on the DS these are
 * LINKER symbols whose ADDRESS is the overlay id, src spells the argument
 * `(int)&overlay_5`, and MSVC cannot give a C++ global an absolute address --
 * so on the host `&overlay_5` is an ordinary host address and NOT 5. Stated
 * rather than papered over, exactly as it is there.
 *
 * It costs nothing here for the same reason it costs nothing there. The two
 * consumers this closure brings in are src/func_0201a754.c and
 * src/func_0201a798.c (unload and load), and both bottom out in the port's
 * faced LoadOverlay / UnloadOverlay -- every overlay the port hosts is a
 * static build-time mount, so there is no loader for a correct id to reach.
 * The seven are here so src/GetSceneOverlayID.c and src/func_0201a694.c
 * resolve; the day an overlay loader exists they become its customers. */
extern int overlay_2, overlay_3, overlay_4, overlay_5, overlay_6, overlay_7,
    overlay_75;   /* hosted by hal/boot_hw.cpp (lane BOOT), one definition */

}  /* extern "C" */

/* ---- THE ONE REFUSED SUBTREE: func_0201a2f8, THE BOOT WORKER THREAD -------
 *
 * BOTH classes' InitResources take this body's ADDRESS and hand it to
 * func_0201a244 (src/func_02005a58.c and src/func_0203506c.c), so the symbol
 * has to resolve or neither class links. It is faced here rather than
 * compiled, and this is the one place this seat traded linkage for honesty on
 * purpose -- the same trade, in the same direction, that hal/scene_boot.cpp's
 * LoadOverlay block and hal/scene_vs_menu.cpp's VS_SEAM family already make.
 *
 * WHAT IT IS. src/func_0201a2f8.c is the ROM's boot worker: it loads
 * overlay_0, calls that overlay's entry at 0x020aa420, unloads it, loads
 * overlay_1, then sets the sound mode, the initial sound group and font 0.
 *
 * WHY IT IS NOT LINKED, in the order the evidence lands:
 *
 *   1. IT NEVER RUNS. func_0201a244 (src/func_0201a244.c) either calls the
 *      entry inline -- only when data_0209d4f8 is already non-zero, and that
 *      byte is bss and zero at boot -- or creates a DS thread through
 *      func_02058200. The port runs no DS threads: func_02058200 is faced to
 *      0 in hal/scene_vs_menu.cpp, whose OS_InitContext note states the floor
 *      ("a context initialised here is never context-switched INTO"). On a
 *      scene run this is the FIRST call, so the thread branch is the one
 *      taken and the body is never entered.
 *   2. ITS FIRST ACT IS AN OVERLAY LOAD the port refuses by design. The entry
 *      it calls, func_020aa420, is func_ov000_020aa420 -- and ov000 HAS NO
 *      MOUNT AND MUST NOT GET ONE: hal/scene_mg_faces.cpp records that its
 *      footprint 0x020aa420..0x020bf4e0 covers ov004's, which is permanently
 *      mounted. So linking the ROM's body would put a call to an address the
 *      port cannot host on a path the port cannot take.
 *   3. THE PRICE, MEASURED AND NAMED. Taking it means taking func_0201fec8,
 *      func_02018aa4, func_020134c8, func_0205d94c and func_0205c91c under
 *      it, plus hosting FIVE pointer-bearing arm9 tables (data_020752cc,
 *      data_020752e8, data_02075304, data_02075320, data_0207533c -- four
 *      relocated words each, into eight further arm9 data spans none of which
 *      the port hosts today) and a host body for the ov000 entry. Roughly
 *      twenty matched TUs and fourteen audited data spans, to feed a body
 *      nothing calls. That is the cost of this refusal, recorded rather than
 *      left to be discovered.
 *
 * THE FACE IS LOUD-ONCE AND IT ANSWERS NOTHING. If a future lane ever runs a
 * DS thread, the first entry prints and the run keeps going, which is what
 * separates a refusal from a silent stub. */
// PORT_HOST_ABI: refused subtree -- the ROM's boot worker thread. Its own first
// act is loading ov000, which the port cannot mount (its footprint covers
// ov004's), and it is only ever reached through a DS thread the port never
// runs (func_02058200 is faced idle in hal/scene_vs_menu.cpp).
extern "C" void func_0201a2f8(void)
{
    static int said;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [boot] refused subtree entered: func_0201a2f8, "
                     "the ROM boot worker thread (ov000 load + font/sound "
                     "bring-up). The port has no ov000 mount and runs no DS "
                     "threads; nothing was done.\n");
        std::fflush(stderr);
    }
}

/* ---- the six thunks -------------------------------------------------------
 *
 * __fastcall (void *self, void *) is the port's dead-edx face: the port's own
 * dispatchers make MSVC __thiscall virtual calls (this in ecx, the rest on the
 * stack), and a bare cdecl body in a slot is the defect class run mg13 lane
 * BNC found the hard way. The counters exist for the reason
 * hal/scene_mg_menu.cpp's g_mgm_hits do: a scene that BOOTS with a slot never
 * entered is a different claim from one that RUNS. */
static unsigned g_boot_hits[18];

static int  __fastcall bt_init(void *s, void *)
{ ++g_boot_hits[0];  return func_02005a58((char *)s); }
static int  __fastcall bt_beh(void *s, void *)
{ ++g_boot_hits[6];  return func_02005418(s); }
static int  __fastcall bt_render(void *s, void *)
{ ++g_boot_hits[9];  return port_scene_link100_base_render(s); }
static void __fastcall bt_pdes(void *s, void *)
{ ++g_boot_hits[12]; port_scene_link100_base_pending_destroy(s); }
static void *__fastcall bt_d2(void *s, void *)
{ ++g_boot_hits[16]; return _ZN5SceneD2Ev(s); }
static void *__fastcall bt_d0(void *s, void *)
{ ++g_boot_hits[17]; return _ZN9BootSceneD0Ev(s); }

extern "C" void port_scene_fill_boot(void)
{
    void **vt = data_02091528;

    /* the eleven shared slots plus ActorBase::CleanupResources, keyed on the
       ROM word each holds rather than on its index */
    const unsigned left = port_scene_fill_rom(vt, 18);

    vt[0]  = (void *)bt_init;
    vt[6]  = (void *)bt_beh;
    vt[9]  = (void *)bt_render;
    vt[12] = (void *)bt_pdes;
    vt[16] = (void *)bt_d2;
    vt[17] = (void *)bt_d0;

    /* THE ACCOUNTING, and it is the transcription's only proof. Six is the
       four this class overrides (0/6/16/17) plus the two ActorBase halves
       hal/scene_boot.cpp's kSceneRomFaces does not carry (9 Render and 12
       OnPendingDestroy). Anything else means either the transcription above or
       the ROM's arrangement is not what this file says, and both are worth a
       line on stderr rather than a wild call three frames later. */
    static int first = 1;
    if (first && left != 6)
        std::fprintf(stderr, "  [boot] VTABLE SHAPE: port_scene_fill_rom left "
                     "%u raw DS word(s) in the 18-slot table, expected 6\n",
                     left);
    /* ONLY THE FIRST FILL IS CHECKED, and the reason is that the check is
       about the TRANSCRIPTION rather than about the table. The array is seeded
       with the ROM's eighteen words and the keyed pass reads those words, so
       the count is only meaningful while they are still there; a second call
       (port_scene_registry_install runs once per process today, but nothing
       structural stops a second) would see eighteen host addresses, key on
       none of them, and report 18 -- a line that says nothing about the ROM
       and would read as a defect. The writes below are idempotent either
       way. */
    first = 0;

    /* ARMED ONLY WHEN THE BOOT SCENE IS THE SCENE BEING RUN. This fill runs on
       every boot -- port_scene_registry_install walks the whole table -- so an
       unguarded atexit would print a row of zeroes at the end of every level
       walk and every other scene run. */
    if (port_scene_env_want() == 0) {
        static int armed;
        if (!armed) { armed = 1; std::atexit(port_scene_boot_report); }
    }
}

/* The registry's factory column is void *(*)(void) and the matched factory
   already is one; this forwarder exists so the row names a symbol this file
   owns, and /OPT:REF follows it to src/func_02023624.c. */
extern "C" void *port_boot_scene_spawn(void) { return func_02023624(); }

extern "C" void port_scene_boot_report(void)
{
    std::printf("[boot] slots entered: init %u beh %u render %u pdes %u "
                "d2 %u d0 %u\n",
                g_boot_hits[0], g_boot_hits[6], g_boot_hits[9],
                g_boot_hits[12], g_boot_hits[16], g_boot_hits[17]);
    std::fflush(stdout);
}
