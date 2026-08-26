/* ---- run mg15 lane MENU: dScMiniGm_c, the MINIGAME SELECTION MENU ---------
 *
 * SCENE ID 5. The Rec Room grid the player picks a minigame off, and the one
 * scene the port's whole minigame campaign has been reaching AROUND: the
 * launcher's F5 boots SM64DS_SCENE=<id> straight into an ov006 minigame, so
 * dScMiniGm_c had never run a frame in this tree. hal/scene_mg.cpp's RNG
 * seeder exists precisely because of that -- it replays this scene's per-frame
 * draws because the scene itself was absent.
 *
 * ---- 1. THE IDENTITY, DERIVED FROM THE ROM --------------------------------
 *
 * The id is read two independent ways and they agree:
 *
 *   src/GetSceneOverlayID.c   `case 5: return (int)&overlay_5;`
 *   config/arm9/relocs.txt    from:0x02090878 kind:load to:0x020c2440,
 *                             and (0x02090878 - 0x02090864) / 4 = 5
 *
 * Everything below came out of extracted/overlays/overlay_0005.bin at base
 * 0x020bfec0 -- the shipped image, not a dsd export, per the standing rule --
 * with the base proven on a symbol before anything was read off it:
 * func_ov005_020c21ec's first word is e92d4010 (push {r4, lr}) and
 * func_ov005_020c14a0's is the same.
 *
 *   SpawnInfo   0x020c2440   { 0x020c21ec, 0x00080005 }
 *   RTTI        0x020c2448   { 0x0209a764, 0x020c2454, 0x020914d4 }, and the
 *                            name at 0x020c2454 reads "11dScMiniGm_c"
 *   vtable      0x020c2490   EIGHTEEN slots
 *   factory     0x020c21ec   operator new(0xb0), ActorBase::ActorBase(), the
 *                            ActorDerived and Scene vptrs, |= 1 and |= 4 on
 *                            the flag byte at +0x13, then the class vptr
 *
 * ---- 2. EIGHTEEN SLOTS, AND WHY THAT IS THE INTERESTING NUMBER ------------
 *
 * Every minigame class this port has seated is THIRTY-SIX slots, because they
 * all derive from dScMgBase_c. THIS ONE IS NOT ONE OF THEM. dScMiniGm_c is a
 * direct Scene subclass -- the same shape dScStarSel_c and dScDSMT_c have --
 * and the width is 18 by the ROM's own spacing: the next named symbol after
 * data_ov005_020c2490 is data_ov005_020c24d8, a delta of 0x48 = 18 words.
 * Confirmed a second way by the slot contents: slots 1/2/4/5/7/8/10/11/13/14/15
 * hold the ELEVEN arm9 Scene:: and ActorBase:: addresses
 * hal/scene_boot.cpp's index fill writes for ov003 and ov007, in that exact
 * arrangement, which a 36-slot dScMgBase_c table does not have (it overrides
 * five of them). So this class belongs with the star select and the title
 * screen, not with the games it launches.
 *
 * ---- 3. THE FILL IS THE KEYED ONE, NOT THE INDEX ONE ----------------------
 *
 * scene_boot.cpp's scene_fill_shared() writes eleven fixed indices and its own
 * header block says it stays "for ov003 and ov007 only". This seat uses
 * port_scene_fill_rom() instead -- the generic fill that keys on the ROM WORD
 * each slot holds -- for the reason that function's header gives: the table is
 * INSIDE A MOUNTED SPAN (port/ov005_syms.txt keeps data_ov005_020c2490 in the
 * mount), so the raw DS words are still there at fill time to key on, and a
 * fill that cannot write a slot the ROM did not park a shared body in is the
 * safer instrument.
 *
 * IT IS NOT THAT FUNCTION'S FIRST CALLER, and an earlier revision of this
 * banner said it was. hal/scene_mg.cpp already calls port_scene_fill_rom at
 * SEVENTEEN sites and did so at this branch's base (5bee54dfa), and that file
 * is compiled -- port/CMakeLists.txt's SCENE_MG_SOURCES names it. The false
 * claim was inherited from the stale "NO CALLER TODAY" paragraph that still
 * stands above the definition in hal/scene_boot.cpp; that comment is another
 * lane's file and is flagged rather than edited here. The correction runs in
 * the reassuring direction -- the function this seat leans on is far better
 * exercised than the seat claimed -- but a banner that invents a first is a
 * banner nobody can trust about a second thing either. What IS first here is
 * narrower and still worth saying: this is the first caller that is not an
 * ov006 minigame class, and the first to hand it an EIGHTEEN-slot table.
 *
 * It returns the count of slots left holding a raw DS word. For this class the
 * answer must be exactly SEVEN -- the class's own overrides -- and the seven
 * writes below account for every one. Anything else is reported loudly: a
 * scene table with a raw DS word left in a dispatched slot is a wild call.
 *
 * ---- 4. THE CALLING CONVENTION SEAM ---------------------------------------
 *
 * Every slot written here goes through a __fastcall (void *self, void *)
 * thunk, the shape hal/scene_boot.cpp's ti_* thunks use. The bodies are plain
 * cdecl C functions and the dispatchers are MSVC C++ virtual calls, so putting
 * a bare cdecl body into a slot is the defect class run mg13 lane BNC found
 * the hard way (a seat that dispatched a hosted vtable through the wrong
 * convention, invisible to every byte gate). There is no bare body in this
 * file's fill.
 *
 * ---- 5. WHAT THIS SEAT DOES NOT FIX ---------------------------------------
 *
 * func_ov005_020c0b04, the Render slot, carries
 *
 *     NONMATCHING: missing logic (ROM does more) (div=30)
 *
 * so the body is decompiled but INCOMPLETE against the ROM. It is seated
 * anyway because the alternative is a no-op slot that draws nothing, but it is
 * named here rather than left to be discovered: anything the menu fails to
 * DRAW has this body as its first suspect, and no drawing claim made off this
 * seat should be read as a claim about the ROM's Render.
 *
 * The other two NONMATCHING bodies in the overlay are func_ov005_020c0378 (the
 * touch selector, a regalloc/schedule divergence at matching SIZE) and
 * func_ov005_020c16e4 (the tile blitter, 0x338 vs 0x33c). Both are logic-
 * complete; neither is a behaviour floor the way Render's is.
 */

#include <cstdio>
#include <cstdlib>

/* ---- THE ONE NAME-SPELLING REPAIR ----------------------------------------
 *
 * src/func_ov005_020bff4c.cpp is a genuine .cpp and it declares its callee the
 * way the ROM's own source did:
 *
 *     namespace SaveData { void SaveMinigames(MinigameSaveData *); }
 *
 * so MSVC emits a reference to ?SaveMinigames@SaveData@@YAXPAUMinigameSaveData@@@Z.
 * The port's copy of that arm9 body is src/_ZN8SaveData13SaveMinigamesEP16
 * MinigameSaveData.c -- a C TU spelling the Itanium-mangled name FLAT, the way
 * every arm9 method in this tree is spelled -- so it defines
 * _ZN8SaveData13SaveMinigamesEP16MinigameSaveData at C linkage and the two
 * never meet. Measured, not predicted: the first link of this seat failed with
 * exactly that one unresolved external and nothing else.
 *
 * The alias is exact rather than convenient. Both names mean ONE arm9
 * function at 0x02013be0 (config/arm9/symbols.txt:
 * `_ZN8SaveData13SaveMinigamesEP16MinigameSaveData kind:function(arm,size=0x2c)
 * addr:0x02013be0`), the C++ spelling is unique to this one TU, and the flat
 * spelling is already in the link through port/slice_ov007.txt. The MSVC name
 * returns void where the flat one returns int; the caller in
 * func_ov005_020bff4c.cpp discards the result, which is what the ROM does too
 * (its `bl` is followed by no use of r0), so no value crosses the seam.
 *
 * It lives HERE and not in hal/cxx_aliases.cpp because the only TU that needs
 * it is this seat's, and an alias in the shared file would be live in targets
 * that carry neither side of it.
 */
/* One physical line: #pragma comment is a preprocessor directive and MSVC's
   parser wants a single string literal, not the adjacent-literal concatenation
   the rest of this file's long strings use (measured -- the split spelling is
   C2059). */
#pragma comment(linker, "/alternatename:?SaveMinigames@SaveData@@YAXPAUMinigameSaveData@@@Z=__ZN8SaveData13SaveMinigamesEP16MinigameSaveData")

extern "C" {

/* the mount (port/ov005_syms.txt, generated by tools/ovdata.py) */
extern unsigned char data_ov005_020c2440[];   /* SpawnInfo, 8 bytes           */
extern unsigned char data_ov005_020c2490[];   /* _ZTV11dScMiniGm_c, 18 words  */
extern unsigned char data_ov005_020c24d8[];   /* the 36-row launch table      */
extern unsigned char data_ov005_020c2c28[];   /* the row after it             */
void port_ov005_pack_check(void);
void port_ov005_syms_patch(void);

/* the generic keyed fill and the run's scene id (hal/scene_boot.cpp) */
unsigned port_scene_fill_rom(void **vt, unsigned n);
int port_scene_env_want(void);
void port_scene_mgm_report(void);

/* the class's own seven, all cdecl bodies out of src/ */
int   func_ov005_020c1a20(void *self);        /* slot  0  InitResources     */
int   func_ov005_020c1654(void *self);        /* slot  3  CleanupResources  */
int   func_ov005_020c14a0(char *self);        /* slot  6  Behavior          */
int   func_ov005_020c0b04(void *self);        /* slot  9  Render            */
void  func_ov005_020c0b00(void);              /* slot 12  OnPendingDestroy  */
int  *func_ov005_020bfec0(int *self);         /* slot 16  D2                */
void *func_ov005_020bfefc(void *self);        /* slot 17  D0                */

/* the factory (src/func_ov005_020c21ec.c) */
void *func_ov005_020c21ec(void);

/* the two decision words the menu writes, for the report below */
extern int data_0208a170;                     /* the highlighted row base   */
extern int data_0208a174;                     /* the SELECTED row, -1 = none*/
extern unsigned char data_0209b2fc[];         /* "a launch was requested"   */
extern unsigned char data_0209b308[];         /* the picked minigame's row  */
extern unsigned short data_02092664;          /* THE PENDING SCENE ID       */

}

/* ---- the seven thunks -----------------------------------------------------
   Hit counters for the same reason hal/scene_boot.cpp's g_ti_hits exist: a
   scene that "runs" with a slot never entered is a different claim from one
   that runs, and only a counter can tell them apart. */
static unsigned g_mgm_hits[18];

static int  __fastcall mgm_init(void *s, void *)
{ ++g_mgm_hits[0];  return func_ov005_020c1a20(s); }
static int  __fastcall mgm_clean(void *s, void *)
{ ++g_mgm_hits[3];  return func_ov005_020c1654(s); }
static int  __fastcall mgm_beh(void *s, void *)
{ ++g_mgm_hits[6];  return func_ov005_020c14a0((char *)s); }
static int  __fastcall mgm_render(void *s, void *)
{ ++g_mgm_hits[9];  return func_ov005_020c0b04(s); }
static int  __fastcall mgm_render_noop(void *, void *)
{ ++g_mgm_hits[9];  return 1; }
static void __fastcall mgm_pdes(void *, void *)
{ ++g_mgm_hits[12]; func_ov005_020c0b00(); }
static void *__fastcall mgm_d2(void *s, void *)
{ ++g_mgm_hits[16]; return (void *)func_ov005_020bfec0((int *)s); }
static void *__fastcall mgm_d0(void *s, void *)
{ ++g_mgm_hits[17]; return func_ov005_020bfefc(s); }

/* ---- the pack check -------------------------------------------------------
   The mount's own spacing, asserted at run time rather than trusted. Two
   facts, both read off config/arm9/overlays/ov005/symbols.txt:
     - the vtable is 0x48 bytes (18 words) before the launch table
     - the launch table is 0x750 bytes (36 rows of 0x34) before the next symbol
   ovdata.py's --pack lays the symbols contiguously and its own generated
   port_ov005_pack_check() checks every gap; this checks the two THIS SEAT
   depends on, in the terms the seat depends on them. */
static void mgm_pack_check(void)
{
    const long dv = (long)(data_ov005_020c24d8 - data_ov005_020c2490);
    const long dt = (long)(data_ov005_020c2c28 - data_ov005_020c24d8);
    if (dv != 0x48)
        std::fprintf(stderr, "  [mgm] PACK BROKEN: the vtable is %ld bytes "
                     "before the launch table, the ROM says 0x48 (18 slots)\n",
                     dv);
    if (dt != 36 * 0x34)
        std::fprintf(stderr, "  [mgm] PACK BROKEN: the launch table is %ld "
                     "bytes, the ROM says %d (36 rows of 0x34)\n",
                     dt, 36 * 0x34);
}

/* ---- the fill -------------------------------------------------------------
   Mount first, fill second, the order hal/scene_boot.cpp's scene_fill_title
   establishes: port_ov005_syms_patch() rebases the mount's own in-span pointer
   words, and every word this fill writes is a CODE address (outside the
   patch's coverage), so the patch cannot undo the fill -- but the reverse
   order could, if a patch ever grew to cover a slot. */
extern "C" void port_scene_fill_mgm(void)
{
    static int done;
    if (!done) {
        done = 1;
        port_ov005_pack_check();
        port_ov005_syms_patch();
        mgm_pack_check();
    }

    void **vt = (void **)data_ov005_020c2490;

    /* THE ELEVEN SHARED SLOTS, keyed on the ROM word each holds. */
    const unsigned left = port_scene_fill_rom(vt, 18);

    /* THE SEVEN THE CLASS OWNS. */
    vt[0]  = (void *)mgm_init;
    vt[3]  = (void *)mgm_clean;
    vt[6]  = (void *)mgm_beh;
    {
        /* SM64DS_SCENE_SLOT9=0 leaves Render on a no-op. The same diagnostic
           knob scene 1 carries, and for the same reason: it separates a
           Behavior fault from a Render one, and this class's Render is the
           one body in the overlay whose banner says the ROM does more. */
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        vt[9] = (s9 && s9[0] == '0') ? (void *)mgm_render_noop
                                     : (void *)mgm_render;
    }
    vt[12] = (void *)mgm_pdes;
    vt[16] = (void *)mgm_d2;
    vt[17] = (void *)mgm_d0;

    /* THE ACCOUNTING. port_scene_fill_rom returns how many slots it left
       holding a raw DS word; for this class that has to be exactly the seven
       written above, and if it is not, one of two things is wrong -- the
       width, or the ROM's arrangement -- and both are worth a line on stderr
       rather than a wild call three frames later. */
    if (left != 7)
        std::fprintf(stderr, "  [mgm] VTABLE SHAPE: port_scene_fill_rom left "
                     "%u raw DS word(s) in the 18-slot table, expected 7 (the "
                     "class's own overrides)\n", left);

    /* THE WITNESS REPORTS ITSELF, through std::atexit, for the reason
       hal/scene_mg.cpp's block gives for doing the same: scene_boot.cpp's
       end-of-run block chooses between ov003's and ov007's counters on
       `scene == 1` and has no third branch, and widening it is not this
       lane's region to widen. atexit puts the line after the run's own last
       one, which is a worse place but an honest one.

       ARMED ONLY WHEN THE MENU IS THE SCENE BEING RUN. This fill is called on
       EVERY boot -- port_scene_registry_install walks the whole table -- so an
       unguarded atexit here would print a row of zeroes at the end of every
       level walk and every other scene run. */
    if (port_scene_env_want() == 5) {
        static int armed;
        if (!armed) { armed = 1; std::atexit(port_scene_mgm_report); }
    }
}

/* The registry's factory column is void *(*)(void). The matched factory
   already returns void *, so this forwarder exists only so the row names a
   symbol this file owns; /OPT:REF follows it to the real body. */
extern "C" void *port_mgm_spawn(void) { return func_ov005_020c21ec(); }

/* ---- the report -----------------------------------------------------------
 *
 * WHAT A TAP ON THE MENU IS SUPPOSED TO MOVE, named from the ROM before any
 * run rather than read off one afterwards. src/func_ov005_020c0378.c is the
 * selector and it is the only writer of both words:
 *
 *   data_0208a170   the highlighted row base. func_ov005_020c0878 advances it
 *                   as the player moves across the grid; the selector reads it
 *                   as the base of the hit-test's index arithmetic.
 *   data_0208a174   THE SELECTION. -1 at boot (the ROM's own .data value, see
 *                   port/tools/romdata.py), and set to the picked row the
 *                   frame a touch lands inside a cell.
 *   data_0209b2fc   set to 1 the same frame, the "a launch was requested"
 *                   latch, alongside self+0xac.
 *   data_0209b308   the picked minigame's 0x34-byte row, copied out of
 *                   data_ov005_020c24d8. Its first halfword is the SCENE ID
 *                   the menu is about to ask Scene::StartSceneFade for.
 *
 * So "the menu took a touch" is a statement about data_0208a174 moving off -1,
 * and "the menu launched a game" is a statement about data_0209b308's first
 * halfword. Neither is a screenshot.
 */
extern "C" void port_scene_mgm_report(void)
{
    std::printf("[mgm] slots entered: init %u clean %u beh %u render %u "
                "pdes %u d2 %u d0 %u\n",
                g_mgm_hits[0], g_mgm_hits[3], g_mgm_hits[6], g_mgm_hits[9],
                g_mgm_hits[12], g_mgm_hits[16], g_mgm_hits[17]);
    std::printf("[mgm] selection: data_0208a174 = %d (-1 = nothing picked), "
                "row base data_0208a170 = %d, launch latch data_0209b2fc = %u, "
                "picked scene id = %u\n",
                data_0208a174, data_0208a170, (unsigned)data_0209b2fc[0],
                (unsigned)(data_0209b308[0] | (data_0209b308[1] << 8)));
    /* AND THE REQUEST ITSELF. Scene::StartSceneFade's whole body is
       `if (Scene::SetSceneToSpawn(actorID, param)) data_0209f5e8.color = ...`
       (src/_ZN5Scene14StartSceneFadeEjjt.c), and SetSceneToSpawn parks the id
       in data_02092664. So this word IS the launch request: a menu that took a
       touch and a menu that ASKED THE GAME FOR A SCENE are two different
       claims, and only this one is the second. At boot the harness leaves it
       at the id it spawned the menu with. */
    std::printf("[mgm] pending scene id data_02092664 = %u (the scene the menu "
                "asked Scene::SetSceneToSpawn for)\n",
                (unsigned)data_02092664);
    std::fflush(stdout);
}
