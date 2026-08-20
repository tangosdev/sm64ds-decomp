/* The screen gap's layout owner. See port/hal/screen_gap.h for what the gap is
   and why the layout is one struct rather than five copies of a subtraction. */

#include "hal/screen_gap.h"

#include "hal/gap_art.h"
#include "hal/gap_continuity.h"
#include "hal/host_settings.h"

#include <cstdio>
#include <cstdlib>

/* THE GAME'S OWN WORD, and it is read through the mount rather than hosted a
   second time. build/port/host-src/ov004_syms.c defines data_ov004_020beb6c as
   four bytes of real packed ov004 storage at DS 0x020beb6c, with a PACK BROKEN
   check on its offset, and every ROM body that touches the gap -- the setter
   func_ov004_020b04d0, the reader func_ov004_020b04c0, RenderOamBothScreens,
   func_ov004_020ae3b4 -- reaches it there. Hosting a second copy here would put
   this file's idea of G beside the game's and let them drift apart silently,
   which is the exact defect hal/scene_mg_flower.cpp's note records for the two
   words either side of it.

   AS BYTES, NOT AS AN int. The mount declares it `u8 [4]` and a C++ `extern int`
   of the same name is a different type for the same address; MSVC's C linkage
   makes that link, but the declaration would be a second opinion about the
   storage rather than a reader of it. Four bytes, little endian, assembled
   here. */
extern "C" unsigned char data_ov004_020beb6c[4];

namespace {

int g_have;
int g_raw = -1;              /* the last G read out of the game */
int g_scene = -2;            /* the scene the last latch was for */
int g_head = -1;             /* the headroom the last layout was built with */
/* G_rom: the G THE SCENE'S OWN InitResources WROTE, captured by the gapless
   latch below at the instant before it stores zero over it, because that is the
   only moment the number exists. Everything after the latch reads zero, and the
   headroom is exactly this many DS rows -- the world's top rows the zeroed G
   pushed off the top of the top screen. Zero until a latch engages. */
int g_gapless_head_ds;
unsigned g_gen;              /* steps whenever the layout's shape changes */
ntr::StackLayout g_lay;

int read_raw(void)
{
    return (int)((unsigned)data_ov004_020beb6c[0] |
                 ((unsigned)data_ov004_020beb6c[1] << 8) |
                 ((unsigned)data_ov004_020beb6c[2] << 16) |
                 ((unsigned)data_ov004_020beb6c[3] << 24));
}

/* THE HEADROOM THE LAYOUT SHOULD CARRY RIGHT NOW, in DS rows, and zero is the
   answer for everything that is not a gapless minigame in progress.

   It is g_gapless_head_ds -- the G the running scene's own InitResources wrote,
   captured by the latch below before it stored zero over it -- gated on the mod
   being engaged FOR THE SCENE NOW RUNNING, which is the same test
   hal_gapless_engaged makes and for the same reason: the flag alone keeps
   reading 1 after the minigame that set it has ended, and a headroom that
   outlived its own scene would make the next scene's window the wrong height.

   SM64DS_GAPLESS_HEADROOM=0 is the A/B arm, on the same binary. It suppresses
   the headroom and nothing else, so the run composes the 512x768 image gapless
   produced before this feature, from the build that also produces the 512x832
   one. Read once. */
int headroom_ds(void)
{
    static int off = -1;
    if (off < 0) {
        const char *s = std::getenv("SM64DS_GAPLESS_HEADROOM");
        off = s && *s && *s == '0';
    }
    if (off || !hal_gapless_engaged()) return 0;
    return g_gapless_head_ds;
}

}  // namespace

int hal_screen_gap_raw(void) { return read_raw(); }

const ntr::StackLayout *hal_screen_layout(void)
{
    const int raw = read_raw();
    /* MinigameGap false means G is treated as ZERO FOR LAYOUT, and only for
       layout: the word itself is not written, so the game's sprite submission
       still runs at a2 + 0xc0 + G and objects still vanish at the seam. That is
       precisely what the setting promises -- "remove the gap" gives back the
       picture the port had before this feature, jump and all. */
    const int want = host_setting_minigame_gap() ? raw : 0;
    /* THE HEADROOM, and it is a DISPLAY answer to a SIMULATION cost. See the
       StackLayout note in ntr/ppu.h: gapless zeroes G, the game's own fixed
       constants keep placing actors in a world 192 + G_rom + 192 rows tall, and
       the top G_rom rows of it fall above the top screen. Giving the IMAGE
       those rows back shows them without touching a constant or the zeroed
       word. It is asked for ONLY while the mod is engaged for the scene now
       running, so every other layout in the program carries a zero.

       SM64DS_GAPLESS_HEADROOM=0 turns it off on the same binary, which is what
       an A/B of the picture needs: notes/port-selftest-bmp-gate.md is explicit
       that two BMPs may only be compared out of one build. */
    const int head = headroom_ds();
    /* THE SCENE IS PART OF THE LATCH, and it has to be: two minigames with the
       same G give the same LAYOUT and not the same BAND. 368 and 374 are both
       G = 32, and the art and the continuity reader are per scene, so latching
       on G alone would have left Curling wearing Bob-omb Squad's picture. */
    const int scene = hal_gap_scene_id();
    /* AND THE HEADROOM IS PART OF THE LATCH TOO. It cannot move without `want`
       moving today -- gapless zeroes G, so engaging changes both -- and it is
       in the key anyway, because a cache whose key is a subset of its inputs is
       one edit away from serving a stale answer. */
    /* THE SEAM FLAG IS NOT LATCHED, and that is the whole of why it is set on
       both paths out of here rather than inside the rebuild below. The latch
       fires when the scene or its G changes; GaplessMinigames engages from the
       running minigame's InitResources, which is after the scene has changed
       and can be after this has already latched a layout for it. A cached zero
       would leave the seam pass off for the entire game, which is exactly the
       shape of defect a "latch it with everything else" reading produces and
       nothing downstream would report. It is one int and it costs a call. */
    const int seam = hal_gapless_engaged() ? 1 : 0;
    if (g_have && want == g_raw && scene == g_scene && head == g_head) {
        g_lay.seam = seam;
        return &g_lay;
    }

    const int was_h = g_have ? g_lay.h : 0;
    g_raw = want;
    g_scene = scene;
    g_head = head;

    const int mode = host_setting_gap_fill_mode();
    const int peek = host_setting_gap_peek();
    /* THE ART IS LOADED HERE, ONCE, and this is the only place that asks for
       it. Both askers are here: the "custom" fill mode wants it with peek off,
       and peek wants it as its backdrop whatever the fill mode says. Asking at
       the latch rather than per frame is what keeps a file read off the frame
       path -- the loader caches on the same (scene, G) pair this does, so the
       second ask in a scene is a pointer compare. */
    const uint32_t *art = 0;
    if (want && (peek || mode == ntr::GAP_FILL_CUSTOM))
        art = hal_gap_art(scene, want, mode == ntr::GAP_FILL_CUSTOM);

    g_lay = ntr::stack_layout(want, head, mode, host_setting_gap_color(), peek,
                              art);
    g_lay.seam = seam;
    /* and the band's per-scene continuity reader, installed at the same moment
       for the same reason: it is per scene, and installing clears the cached
       OAM attributes so nothing crosses from the last minigame into this one */
    hal_band_continuity_latch(scene);
    /* and the ambient fill's memory, dropped here for the third time in the
       same sentence: the band now FOLLOWS the two edge rows rather than reading
       them cold, and a decaying glow that outlived its own minigame would light
       the next one's first frames off the last one's picture. */
    ntr::ppu_band_ambient_reset();
    g_have = 1;
    /* THE GENERATION IS THE SHAPE'S, not the latch's. walk_window re-sizes the
       window and re-shapes the DIB header off this counter, and a scene change
       that keeps the same G changes the band's CONTENT and not its size, so
       stepping it there would ask for a resize to the size it already is. */
    if (g_lay.h != was_h) ++g_gen;
    {
        /* SAID ONCE PER LATCH, and it is the line a report about a wrong-looking
           gap is answered from: which minigame, its own G, what the setting did
           to it, the band's height in host rows, the image the window now has to
           carry, and whether the scene's art was found. A capture with no such
           line is a capture with no gap. */
        if (g_lay.head_ds)
            std::fprintf(stderr, "[gapless] scene %d: HEADROOM %d DS rows "
                         "(%d host rows) above the top screen, image %dx%d. "
                         "The world's rows -%d..-%d -- the ones zeroing G "
                         "pushed off the top -- are back in the picture; the "
                         "simulation is untouched.\n", scene, g_lay.head_ds,
                         g_lay.head_h, g_lay.w, g_lay.h,
                         192 + g_lay.head_ds, 193);
        if (raw)
            std::fprintf(stderr, "[gap] scene %d, G %d DS rows%s -> band %d "
                         "host rows, image %dx%d, fill %s, peek %s, art %s\n",
                         scene, raw,
                         want == raw ? "" : " (MinigameGap off: layout uses 0)",
                         g_lay.band_h, g_lay.w, g_lay.h,
                         g_lay.fill_mode == ntr::GAP_FILL_SOLID  ? "solid"
                         : g_lay.fill_mode == ntr::GAP_FILL_CUSTOM
                             ? "custom"
                             : "ambient",
                         g_lay.peek ? "ON" : "off",
                         g_lay.art ? "loaded" : "none");
    }
    return &g_lay;
}

unsigned hal_screen_layout_generation(void)
{
    hal_screen_layout();
    return g_gen;
}

/* ---- GaplessMinigames: THE MOD, its table, and the audit behind it ---------
 *
 * WHAT IT DOES. One store of zero into the game's own word, from the running
 * minigame's InitResources thunk, after the real body has finished. Nothing
 * else in this program or in the ROM is patched, redirected or special-cased:
 * every consumer of G below simply reads a zero where it used to read 32, and
 * the ROM's own arithmetic does the rest.
 *
 * WHY THAT IS ENOUGH, and this is the analysis rather than a hope. G is not a
 * flag, it is the OFFSET BETWEEN TWO COORDINATE FRAMES. The world a minigame
 * simulates runs from the top screen's top row to the bottom screen's bottom
 * row with its origin at the BOTTOM screen's top row, so world row y lands on
 * the top engine at y + 0xc0 + G and on the bottom engine at y, and the G rows
 * at [-G, -1] land on neither. Set G to zero and the two frames become
 * adjacent: world -1 is the top engine's last row, world 0 is the bottom
 * engine's first, and there are no rows in between.
 *
 * SO THE ONLY WAY TO GET THIS WRONG is to have one party compute a coordinate
 * with one G while another submits it with a different one. That makes the
 * question a lifetime question rather than a maths question, and the answer
 * had to be measured. THE READERS LINKED INTO THIS PROGRAM, all of them ov004,
 * and what each does with the value:
 *
 *   PER FRAME, so they adapt the instant the word changes and cannot go stale:
 *     RenderOamBothScreens 0x020b0104   both engines, y + 0xc0 + G on the top
 *     func_ov004_020afdd0  0x020afdd0   top engine + RenderSub
 *     func_ov004_020aff38  0x020aff38   THE ROUTER. Picks ONE engine by a band
 *                                       test on world y and returns; this is
 *                                       the body that makes a crossing object
 *                                       disappear, and the one this mode is
 *                                       really aimed at. Bob-omb Squad's balls
 *                                       go through it.
 *     func_ov004_020b023c  0x020b023c   top engine, matrix form. The falling
 *                                       bob-ombs go through it.
 *     func_ov004_020b0380  0x020b0380   top engine, matrix form
 *     func_ov004_020ae3b4  0x020ae3b4   the BG pixel plotter, dScMgBase_c slot
 *                                       34: picks a char base by the same band
 *                                       test and adds G + 0xc0 for the top
 *     func_ov004_020b04c0  0x020b04c0   the getter itself
 *
 *   WRITERS:
 *     func_ov004_020b04d0  0x020b04d0   the setter, called once per minigame
 *                                       from its InitResources
 *     func_ov004_020b265c  0x020b265c   slot 33, stores 0 during scene setup,
 *                                       which is why G already reads 0 on
 *                                       entry to any InitResources
 *
 *   READ ONCE AND CACHED -- the stale-G class, and the reason the write is
 *   placed where it is:
 *     func_ov004_020b7854  0x020b7854   passes data_ov004_020bc8ac - (G + 0xc0)
 *     func_ov004_020b6ddc  0x020b6ddc   passes data_ov004_020bc864 - G
 *       both through func_ov004_020b0cac -> func_ov004_020b3278, which STORES
 *       the y into the banner element at +0x12 and draws from the stored copy
 *       every frame afterwards. Both are dScMgBase_c state bodies that run
 *       DURING a minigame, long after InitResources, so with the write placed
 *       at the end of init they read the zero and place against the same frame
 *       they are submitted in. Neither is a hazard here; both WOULD be if the
 *       write moved later.
 *
 * AND THE ONES THE ROM HAS THAT THIS PROGRAM DOES NOT. The score and
 * name-entry HUD parks -- func_ov004_020b3888 (which is the worst of them:
 * *(short *)(r0 + 0x12) = -(G + 0x60), a cached park), _020b4e78, _020b53f0,
 * _020b556c, _020b5f6c, _020b612c, _020b682c -- and ov006's own G readers,
 * including the player clamp func_ov006_020eee3c at -(184 + G) << 12 and the
 * BG2 scroller func_ov006_021211e0, are in src/ but appear in no
 * port/slice_*.txt, so nothing compiles them and nothing can run them. They
 * are named here so a later lane that seats one of them knows this audit has
 * to be re-run rather than inherited.
 *
 * BOB-OMB SQUAD'S OWN CODE READS G NOWHERE. Every ov006 body in
 * port/slice_pch.txt was checked: the only mention of the word or the getter
 * in the whole class is the setter call at the tail of its InitResources. The
 * ball physics, the plunger, the collision scan and the spawner are all in
 * world coordinates with fixed constants -- the parachute spawn is a flat
 * -0x100000, world row -256 -- so the SIMULATION is bit-for-bit what it was
 * and only the mapping onto the two engines changes.
 *
 * WHAT IT COSTS, stated because it is real and a player should be told. The
 * world is 192 + G + 192 rows tall and the two screens are 384. With G at its
 * own value the 32 rows the screens cannot show are the ones behind the hinge,
 * in the middle. With G forced to zero they are the TOP 32 -- world rows -224
 * to -193 -- which fall off the top of the top screen, and the top screen's
 * background art, which is a fixed picture and not a window onto the world,
 * stays where it is while the sprites over it move up by 32. In Bob-omb Squad
 * that background is sky, so the cost is that a parachuting bob-omb becomes
 * visible 32 rows later than it does on hardware. In a minigame whose top
 * screen art lines up with its playfield it would be a visible misregistration,
 * which is the other half of why this is scene-gated.
 *
 * THE TABLE IS THE "WHEN ABLE" IN THE LAUNCHER'S LABEL, and it is a table
 * rather than a policy because the audit above is the only thing that makes
 * this safe and the audit is per game. A scene that is not a row keeps its
 * full gap simulation with the setting on, and says "unsupported" rather than
 * failing quietly. ADDING A GAME IS: one row here, the same call in that
 * scene's InitResources thunk, and the same walk of its own bodies for G
 * reads and for cached G-derived coordinates -- plus the crossing trace that
 * shows the object arriving on the adjacent row. */
namespace {

struct GaplessScene {
    int scene_id;
    const char *what;
};

/* Proven gapless. Nothing goes in here on an argument; it goes in here on a
   measured crossing. */
const GaplessScene kGaplessScenes[] = {
    {368, "dScMgPachinko_c, Bob-omb Squad"},
};

int g_gapless_on;        /* 1 once the write has engaged for the scene running */
int g_gapless_scene = -2;

const GaplessScene *gapless_row(int scene)
{
    for (unsigned i = 0; i < sizeof kGaplessScenes / sizeof *kGaplessScenes; ++i)
        if (kGaplessScenes[i].scene_id == scene) return &kGaplessScenes[i];
    return 0;
}

}  // namespace

void hal_gapless_minigames_latch(void)
{
    const int scene = hal_gap_scene_id();
    const int on = host_setting_gapless_minigames();
    const GaplessScene *row = gapless_row(scene);

    /* A scene must never inherit the last one's answer. */
    g_gapless_on = 0;
    g_gapless_scene = scene;
    /* AND THE HEADROOM IS DROPPED WITH IT, for the reason the two lines above
       exist: a scene must never inherit the last one's answer, and a stale
       G_rom would size the next minigame's window off the last one's hinge. */
    g_gapless_head_ds = 0;

    if (!on) {
        std::fprintf(stderr, "[gapless] scene %d: off (GaplessMinigames is "
                     "false) -- the DS screen gap is simulated, which is the "
                     "game\n", scene);
        return;
    }
    if (!row) {
        /* THE HONEST HALF OF "WHEN ABLE". A player who ticked the box and
           started a game this is not proven for is owed the reason on the
           spot, not a silent nothing. */
        std::fprintf(stderr, "[gapless] scene %d: UNSUPPORTED -- "
                     "GaplessMinigames is on, but this minigame is not one the "
                     "port has proven gapless, so its screen gap is left "
                     "simulated\n", scene);
        return;
    }

    const int was = read_raw();
    /* CAPTURED HERE BECAUSE HERE IS THE ONLY PLACE IT EXISTS. This is the G the
       scene's own InitResources wrote, one statement before the store below
       replaces it with zero, and after that store nothing in the program can
       recover it -- the word reads zero and no other copy is kept. The display
       headroom is exactly this many DS rows. */
    g_gapless_head_ds = was;
    /* WRITTEN AS FOUR BYTES rather than through a declared `int`, for the
       reason the block over data_ov004_020beb6c gives: the mount owns this
       storage as u8[4] and a second declaration of it as an int would be an
       opinion about the storage rather than a use of it. Four zero bytes is
       the integer zero on either endianness, so nothing is being assumed. */
    data_ov004_020beb6c[0] = 0;
    data_ov004_020beb6c[1] = 0;
    data_ov004_020beb6c[2] = 0;
    data_ov004_020beb6c[3] = 0;
    g_gapless_on = 1;

    /* SAID EVERY TIME IT ENGAGES, and said as a warning rather than as a
       status. A capture or a bug report from a run with this on is not a
       report about this game, and the line is how that is told apart later.
       It also states the display consequence, because the layout reads the
       same word this just zeroed: no band, so the fill, the art and the peek
       have nothing to act on for this scene. */
    std::fprintf(stderr, "[gapless] scene %d: ENGAGED for %s -- G %d -> 0. The "
                 "screen gap is now gone from the SIMULATION as well as the "
                 "picture: objects cross the seam directly and arrive %d rows "
                 "sooner than a DS delivers them, and there is no band left "
                 "for the fill, the art or the peek to act on. THIS IS NOT THE "
                 "ROM'S BEHAVIOUR.\n", scene, row->what, was, was);
}

/* THE SCENE IS PART OF THE ANSWER. The flag alone would keep reading 1 after
   the minigame that engaged it has ended, and a run report that says "gapless"
   about a scene this never touched is worse than no line at all. */
int hal_gapless_engaged(void)
{
    return g_gapless_on && hal_gap_scene_id() == g_gapless_scene;
}
