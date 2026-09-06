/* ---- link100 SCENE: dScMB_c / MultiBootScene, ACTOR ID 360 (0x168) --------
 *
 * NOT THE MAIN MENU. dScMB_c is the DS DOWNLOAD PLAY boot scene: the screen a
 * cartridge-less DS shows while it pulls the multiboot image, and the ROM's
 * OTHER first scene. src/_ZN5Scene18PrepareToSpawnBootEv.c is the whole of
 * that decision and it is two lines --
 *
 *     if (func_0203d9b4()) data_02092664 = 0;      /- dScBoot_c,  id 0
 *     else                 data_02092664 = 0x168;  /- dScMB_c,    id 360
 *
 * -- so the pair this lane seats is one fork, not a sequence, and neither half
 * is downstream of the title. The brief that opened this lane called 360 the
 * main menu; the ROM does not, and the seat follows the ROM.
 *
 * ---- 1. THE IDENTITY ------------------------------------------------------
 *
 * The id is the ARM9 spawn table's own index:
 *
 *   config/arm9/relocs.txt  from:0x02090e04 kind:load to:0x0209435c module:main
 *   and (0x02090e04 - 0x02090864) / 4 = 360 = 0x168.
 *
 * The class name is the ROM's own RTTI, reached through the word before the
 * vtable (extracted/arm9_dec.bin, flat at base 0x02004000):
 *
 *   0x020943c0 -> 0x02094370               the type_info record
 *   0x02094370 + 4 -> the name at 0x02094364, which reads "7dScMB_c"
 *   0x02094370 + 8 -> 0x020914d4, the BASE type_info -- Scene's, the same
 *                     record hal/scene_vs_menu.cpp already hosts, so the ROM
 *                     itself says this is a DIRECT Scene subclass.
 *
 * src/GetSceneOverlayID.c's `case 0x168:` FALLS THROUGH TO -1, which is the
 * ROM saying in its own code that this scene has no overlay: arm9 end to end,
 * so this seat needs no mount and its vtable and SpawnInfo are host arrays.
 *
 * ---- 2. THE EIGHTEEN SLOTS, READ FROM config/arm9/relocs.txt --------------
 *
 * Every word is a `from:0x020943c4 + 4*slot` row quoted as its `to:`.
 *
 *    0  InitResources          0x0203506c  func_0203506c        OWN
 *    1  BeforeInitResources    0x0202e638  Scene::
 *    2  AfterInitResources     0x0202e62c  Scene::   VENEER
 *    3  CleanupResources       0x02034d70  func_02034d70        OWN
 *    4  BeforeCleanupResources 0x0202e5f0  Scene::
 *    5  AfterCleanupResources  0x0202e5d0  Scene::
 *    6  Behavior               0x02034da4  func_02034da4        OWN
 *    7  BeforeBehavior         0x0202e3d4  Scene::
 *    8  AfterBehavior          0x0202e3c8  Scene::   VENEER
 *    9  Render                 0x02034d9c  func_02034d9c        OWN
 *   10  BeforeRender           0x0202e3a4  Scene::
 *   11  AfterRender            0x0202e398  Scene::   VENEER
 *   12  OnPendingDestroy       0x02043ac0  ActorBase::
 *   13  Virtual34              0x0204357c  ActorBase::
 *   14  Virtual38              0x0204349c  ActorBase::
 *   15  OnHeapCreated          0x02043494  ActorBase::
 *   16  D2                     0x02034a78  func_02034a78        OWN
 *   17  D0                     0x02034ac0  func_02034ac0        OWN
 *
 * THE WIDTH IS EIGHTEEN by the ROM's own spacing: the next relocated word
 * after slot 17 is 0x0209440c and it does not belong to this table (there is
 * no `from:0x0209440c` row at all), and the class holds the same eleven shared
 * Scene:: / ActorBase:: addresses in the same arrangement dScStarSel_c,
 * dScMiniGm_c and dScEntry_c hold -- which a 36-slot dScMgBase_c subclass does
 * not, because that base overrides five of them.
 *
 * ---- 3. FIVE ADJUDICATED BODIES ------------------------------------------
 *
 * func_0203506c, func_02034d70, func_02034da4, func_02034d9c and func_02034ac0
 * carry the "recovered from vtable slot identity" marker, so
 * port/tools/inferred_stub_guard.py would refuse this seat on its own. All
 * five are ruled REAL_DECOMP in port/tools/inferred_stub_adjudicated.txt off
 * the BYTE GATE (run link100 lane MATCH): compiled with the pinned mwccarm
 * 2004/b56 and compared to the ROM by tools/match.py --strict-relocs, every
 * one reproducing its ROM bytes exactly with every relocation landing where
 * config says. The marker records how the NAME was recovered, not where the
 * BODY came from -- and for func_02034ac0 the name is provably wrong: it is
 * spelled dScMB_c_OnYoshiTryEat in src/ and the body is the deleting
 * destructor D0, which is what slot 17 holds.
 *
 * ---- 4. WHAT THE SCENE DOES ----------------------------------------------
 *
 * src/func_02034da4.c (Behavior) is an eight-state machine over self+0x60. It
 * decompresses the per-language multiboot artwork out of data_0208a0e4 --
 * a five-entry pointer table indexed by func_0200f0bc(), ALREADY HOSTED with
 * host pointers in hal/scene_vs_menu.cpp for the VS lobby, which reads the
 * same five blobs -- runs the wireless download conversation, and ends in
 *
 *     Scene::StartSceneFade(6, 0, 0x7fff)
 *
 * handing off to scene 6, the VS entry menu. State 0 also arms a watchdog on
 * self+0x64: at 0x1518 frames (about 90 seconds) it sets data_0209fc54, the
 * "give up" flag. A headless proof run that sits past that frame count is
 * therefore expected to trip it, and that is the ROM's behaviour rather than a
 * port fault.
 *
 * ---- 5. THE SUBLEVEL ANSWER, MEASURED ------------------------------------
 *
 * reads_sublevel is 0 in the registry row, and it is measured the way the
 * dScBoot_c row's is: the ROM reloc-graph closure of this class is 886 arm9
 * functions and not one of their src/ TUs names data_02092110.
 */

#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

extern "C" {

/* ---- the class's own six, plus the factory. Flat C names out of src/.
   func_02034d70 and func_02034d9c take NO argument: the ROM bodies do not
   touch the receiver, and the decomp spells them the way the ROM compiled
   them. The thunks below drop self for exactly those two and no others. */
int   func_0203506c(void *self);        /* slot  0  InitResources          */
int   func_02034d70(void);              /* slot  3  CleanupResources       */
int   func_02034da4(void *self);        /* slot  6  Behavior               */
int   func_02034d9c(void);              /* slot  9  Render (mov r0,#1)     */
void *func_02034a78(void *self);        /* slot 16  D2                     */
void *func_02034ac0(void *self);        /* slot 17  D0                     */
void *func_020352b4(void);              /* the factory, 0x020352b4         */

/* the shared halves (hal/scene_boot.cpp, hal/scene_link100_base_faces.cpp) */
unsigned port_scene_fill_rom(void **vt, unsigned n);
int  port_scene_env_want(void);
void port_scene_link100_base_pending_destroy(void *self);

void port_scene_mb_report(void);

/* the two words the report reads: the scene's own state counter lives in the
   object, but the hand-off request lands in the ROM's pending-scene word. */
extern unsigned short data_02092664;    /* the pending scene id            */

/* ---- THE VTABLE, a host array under the ROM'S OWN DATA NAME ---------------
 * Same shape and same reasoning as hal/scene_link100_boot.cpp's: the factory
 * (src/func_020352b4.c) and both destructor bodies (src/func_02034a78.c,
 * src/func_02034ac0.c) spell this name, arm9 .data is not mounted, so the
 * table is a fresh host array seeded with the ROM's eighteen words and then
 * rewritten by the keyed fill. The seed is what makes the fill self-checking:
 * it has to leave exactly SEVEN raw words and this file writes exactly seven.
 *
 * BOTH THIS AND THE SpawnInfo RECORD BELOW ARE BRACKETED IN .dsstate, the same
 * dScStarSel_c precedent hal/scene_link100_boot.cpp cites: a fresh host array
 * outside any mount needs the explicit bracket or port/tools/dsstate_guard.py
 * refuses it as a hosted DS symbol left outside [dsstate_lo, dsstate_hi). See
 * hal/dsstate_seg.h. */
DSSTATE_BEGIN
void *data_020943c4[18] = {
    (void *)0x0203506cu, (void *)0x0202e638u, (void *)0x0202e62cu,
    (void *)0x02034d70u, (void *)0x0202e5f0u, (void *)0x0202e5d0u,
    (void *)0x02034da4u, (void *)0x0202e3d4u, (void *)0x0202e3c8u,
    (void *)0x02034d9cu, (void *)0x0202e3a4u, (void *)0x0202e398u,
    (void *)0x02043ac0u, (void *)0x0204357cu, (void *)0x0204349cu,
    (void *)0x02043494u, (void *)0x02034a78u, (void *)0x02034ac0u,
};

/* ---- THE SpawnInfo RECORD -------------------------------------------------
 * Eight bytes at 0x0209435c: the factory pointer at +0 (the relocation
 * `from:0x0209435c kind:load to:0x020352b4 module:main`) and the word
 * 0x01680168 at +4, whose LOW halfword 0x0168 is the actor id the registry
 * cross-checks against the spawn table's index. Read out of
 * extracted/arm9_dec.bin at 0x0209435c - 0x02004000. A host array rather than
 * a romdata blob because its +0 word is a relocation; the factory word is left
 * zero because port_scene_registry_install writes it. */
unsigned char data_0209435c[8] = { 0, 0, 0, 0,   0x68, 0x01, 0x68, 0x01 };
DSSTATE_END

}  /* extern "C" */

/* ---- the seven thunks. __fastcall (void *self, void *) is the dead-edx face
   every scene fill in this port uses; see hal/scene_link100_boot.cpp. */
static unsigned g_mb_hits[18];

static int  __fastcall mb_init(void *s, void *)
{ ++g_mb_hits[0];  return func_0203506c(s); }
static int  __fastcall mb_clean(void *, void *)
{ ++g_mb_hits[3];  return func_02034d70(); }
static int  __fastcall mb_beh(void *s, void *)
{ ++g_mb_hits[6];  return func_02034da4(s); }
static int  __fastcall mb_render(void *, void *)
{ ++g_mb_hits[9];  return func_02034d9c(); }
static void __fastcall mb_pdes(void *s, void *)
{ ++g_mb_hits[12]; port_scene_link100_base_pending_destroy(s); }
static void *__fastcall mb_d2(void *s, void *)
{ ++g_mb_hits[16]; return func_02034a78(s); }
static void *__fastcall mb_d0(void *s, void *)
{ ++g_mb_hits[17]; return func_02034ac0(s); }

extern "C" void port_scene_fill_mb(void)
{
    void **vt = data_020943c4;

    const unsigned left = port_scene_fill_rom(vt, 18);

    vt[0]  = (void *)mb_init;
    vt[3]  = (void *)mb_clean;
    vt[6]  = (void *)mb_beh;
    vt[9]  = (void *)mb_render;
    vt[12] = (void *)mb_pdes;
    vt[16] = (void *)mb_d2;
    vt[17] = (void *)mb_d0;

    /* Seven: the six this class overrides (0/3/6/9/16/17) plus slot 12,
       ActorBase::OnPendingDestroy, which hal/scene_boot.cpp's kSceneRomFaces
       does not carry. */
    /* first-call only, for the reason hal/scene_link100_boot.cpp's fill gives:
       the count is a statement about the ROM-word seed, which only the first
       keyed pass can see. */
    static int first = 1;
    if (first && left != 7)
        std::fprintf(stderr, "  [mb] VTABLE SHAPE: port_scene_fill_rom left "
                     "%u raw DS word(s) in the 18-slot table, expected 7\n",
                     left);
    first = 0;

    if (port_scene_env_want() == 360) {
        static int armed;
        if (!armed) { armed = 1; std::atexit(port_scene_mb_report); }
    }
}

/* the registry's factory column; /OPT:REF follows it to src/func_020352b4.c */
extern "C" void *port_mb_scene_spawn(void) { return func_020352b4(); }

extern "C" void port_scene_mb_report(void)
{
    std::printf("[mb] slots entered: init %u clean %u beh %u render %u "
                "pdes %u d2 %u d0 %u\n",
                g_mb_hits[0], g_mb_hits[3], g_mb_hits[6], g_mb_hits[9],
                g_mb_hits[12], g_mb_hits[16], g_mb_hits[17]);
    /* Scene::StartSceneFade parks the requested id in data_02092664
       (src/_ZN5Scene14StartSceneFadeEjjt.c -> Scene::SetSceneToSpawn), so this
       word IS the hand-off request. At boot the harness leaves it at the id it
       spawned this scene with; 6 means the download conversation reached its
       end and asked for the VS entry menu. */
    std::printf("[mb] pending scene id data_02092664 = %u\n",
                (unsigned)data_02092664);
    std::fflush(stdout);
}
