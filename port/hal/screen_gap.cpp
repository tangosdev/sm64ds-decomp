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
int g_shift = -1;            /* the OBJ display shift the last layout used */
/* G_rom: the G THE SCENE'S OWN InitResources WROTE, captured by the gapless
   latch below at the instant before it stores zero over it, because that is the
   only moment the number exists. Everything after the latch reads zero, and the
   headroom is exactly this many DS rows -- the world's top rows the zeroed G
   pushed off the top of the top screen. Zero until a latch engages. */
int g_gapless_head_ds;
unsigned g_gen;              /* steps whenever the layout's shape changes */
ntr::StackLayout g_lay;

int obj_shift_ds(void);

int read_raw(void)
{
    return (int)((unsigned)data_ov004_020beb6c[0] |
                 ((unsigned)data_ov004_020beb6c[1] << 8) |
                 ((unsigned)data_ov004_020beb6c[2] << 16) |
                 ((unsigned)data_ov004_020beb6c[3] << 24));
}

/* THE HEADROOM THE LAYOUT SHOULD CARRY RIGHT NOW, in DS rows.
 *
 * IT IS ZERO, AND IT IS ZERO BY DEFAULT BECAUSE THE FEATURE IS WRONG. The
 * strip above the top screen was built on a premise that has since been
 * measured and does not hold, so it is opt-in behind
 * SM64DS_GAPLESS_HEADROOM=1 and nothing turns it on. What follows is the
 * measurement, because the next lane to look at this needs the numbers and not
 * the story.
 *
 * THE PREMISE. Gapless zeroes G, so the top engine takes world y to y + 192
 * instead of y + 224, and the world's top 32 rows -- -224..-193 -- fall above
 * the top screen's first row. The strip was meant to give those rows back.
 *
 * WHAT IS ACTUALLY IN THEM. Nothing. Measured on scene 368 over 400 frames in
 * BOTH arms with SM64DS_ENGA_LAYERS (hal/message_compositor.cpp), sampling each
 * enabled engine-A background through the game's own sampler:
 *
 *   BG3  32x32 tiles, vofs 0    engine rows -32..-1 are map rows 224..255,
 *                               0 of 8192 texels opaque
 *                               engine rows 0..31 are map rows 0..31,
 *                               8192 of 8192 texels opaque
 *   BG2  64x32 tiles, ofs(256,64)  engine rows -32..-1 are map rows 32..63,
 *                               0 of 8192 opaque, and 0 of 8192 opaque on the
 *                               screen as well: BG2 is enabled and contributes
 *                               no pixel anywhere on any frame
 *
 * and the scroll registers are IDENTICAL in the two arms on every frame, so the
 * background does not follow G and there is no other window to look in.
 *
 * SO THE ARTWORK THE STRIP WAS SUPPOSED TO SHOW IS NOT ABOVE THE SCREEN. It is
 * ON the screen, at engine rows 0..31, fully opaque, and it is the same picture
 * in both arms: with the OBJ layer masked off, the gapped image's rows 0..63
 * and the gapless-plus-headroom image's rows 64..127 differ in 0 of 32768
 * pixels. The strip's own 64 rows differ from that artwork in 24240 of 32768.
 * A player sees a band of invented fill sitting on top of the picture's own top
 * edge, which is what this was reported as.
 *
 * WHAT THE REAL DEFECT IS, since the strip is not it. Zeroing G moves the
 * OBJECTS 32 rows up the screen and leaves the BACKGROUND where it is, so
 * everything the game draws is 32 rows out of register with the artwork it was
 * drawn against: Lakitu, which the ROM puts 32 rows below the beam, ends up
 * flat against the top edge and loses rows off it. Adding image ABOVE the
 * artwork cannot fix that, because the artwork is the top of the picture.
 *
 * THAT WAS THE OBJECT SHIFT, and it is built and it is ALSO OFF. obj_shift_ds
 * below carries its own measurement: only ONE sprite identity of the twelve on
 * this scene follows G, and moving the whole OBJ layer puts the other eleven,
 * the score rows and the top screen's own artwork, 32 rows down a screen they
 * were never displaced from. Both fixes for this report are now in the tree
 * behind switches, because both are measurements and neither is an
 * improvement. Read obj_shift_ds for what would be.
 *
 * WHEN IT IS ON (the opt-in), the value is g_gapless_head_ds, the G the running
 * scene's own InitResources wrote, captured by the latch below before it stored
 * zero over it, gated on the mod being engaged FOR THE SCENE NOW RUNNING --
 * hal_gapless_engaged's own test, because the flag alone keeps reading 1 after
 * the minigame that set it has ended and a headroom that outlived its scene
 * would size the next one's window wrong. Read once.
 */
int headroom_ds(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAPLESS_HEADROOM");
        on = s && *s && *s != '0';
    }
    if (!on || !hal_gapless_engaged()) return 0;
    /* AND NEVER WITH THE OBJECT SHIFT ON. Both are opt-in and both are wrong,
       so nothing turns either on; what this refuses is the ONE combination
       whose picture could not be attributed afterwards. The strip fills rows
       above the artwork from the top screen's own first row, and under the
       shift that row is a different row, so a capture of the two together would
       carry one feature's error smeared through the other's and no way to read
       which was which. A measurement kept for its numbers has to stay
       separable. */
    if (obj_shift_ds()) return 0;
    return g_gapless_head_ds;
}

/* THE TOP ENGINE'S OBJ DISPLAY SHIFT, in DS rows.
 *
 * IT IS ZERO, AND IT IS ZERO BY DEFAULT BECAUSE THE FEATURE IS WRONG, which is
 * the second time that sentence has been written in this file about a fix for
 * the same report. Read headroom_ds above for the first. The mechanism is whole
 * and it works exactly as designed; the design rests on a premise that has now
 * been measured and does not hold. What follows is the measurement, because the
 * next lane needs the numbers and not the story.
 *
 * THE IDEA. Zeroing G takes the top engine's submission from world y + 0xc0 + G
 * to world y + 0xc0, so a sprite lands G rows higher up a background that did
 * not move with it. Adding G back at the raster would put every sprite on the
 * row the ROM draws it on, and the image would gain G rows below the top screen
 * for the sprites that then run off its bottom edge. That is what this builds:
 * hal/message_compositor.cpp adds it to the entry's y, ntr/ppu_sub.cpp's
 * hinge_paint draws the overflow, and ntr/ppu.h carries the geometry.
 *
 * THE PREMISE IS THAT THE WHOLE OBJ LAYER MOVES WITH G. It does not. Only the
 * submissions that go through the framework's own OAM router do; the rest are
 * placed in SCREEN space and are at the same engine row in both arms. Measured
 * on scene 368 over 300 frames, one headless run per arm, with
 * SM64DS_OBJSHIFT_TRACE (port/tools/objshift.py register), reporting gap-on's
 * engine row minus gapless's for every entry the two arms share:
 *
 *   a2=1010  32x32 dbl    116 sightings   +32 on 116 of them
 *   a2=211c  32x64 dbl    699 sightings     0 on 699 of them
 *   a2=211c  32x64          4 sightings     0 on 4
 *   a2=e2d7/e2db/e2dc     299 each          0 on every one
 *   a2=d280/e2df          598 / 299         0 on every one
 *   a2=8256/825a/825e/a254  226 each        0 on every one
 *
 * ONE identity of twelve follows G. The other eleven are the top screen's own
 * furniture: the score and timer rows parked at engine y 4 and y 168, and the
 * three 32x64 double-size entries at x 0, 64 and 128 that sit at engine y -32
 * in BOTH arms. A blanket shift moves all twelve, so it puts the score readout
 * into the band and the artwork 32 rows down its own screen.
 *
 * AND THE PICTURE SAYS THE SAME THING. Top screen, 512x384, gap-on against each
 * gapless arm at frame 300:
 *
 *   gap-on vs gapless, shift OFF    2940 of 196608 pixels differ, rows 0..99
 *   gap-on vs gapless, shift ON    28152 of 196608 pixels differ, rows 0..177
 *                                  and 336..367
 *
 * The unshifted gapless top screen is ALREADY the ROM's picture almost
 * everywhere; what is wrong with it is 1.5% of one screen, and it is the routed
 * sprite alone. Turning the shift on multiplies the disagreement by ten.
 *
 * THERE IS A SECOND COST THE SHIFT CANNOT TOUCH, found in the same run.
 * OAM::Render culls at submission with `if (y + h < 0) return`, and a routed
 * sprite's y is G lower in this arm, so 215 submissions over those 300 frames
 * reached OAM in gap-on and reached it in NO form under the mod. A display
 * shift can only move what was submitted.
 *
 * WHAT WOULD BE RIGHT, stated precisely because it is buildable and this is
 * not. The shift has to be per entry, applied to the submissions the
 * framework's G-adding routers made and to no others. Those are
 * func_ov004_020aff38, func_ov004_020b023c, func_ov004_020b0380,
 * func_ov004_020afdd0 and RenderOamBothScreens, all of which reach
 * OAM::Render with `a2 + 0xc0 + G` while every other submitter passes a plain
 * engine row. Nothing in the finished OAM distinguishes the two, so the port
 * cannot tell them apart at the raster: the routed set has to be recorded AT
 * THE CALL, and that is a hook at those five bodies rather than anything the
 * display side can derive. A table of attribute words would be a guess with a
 * measurement standing behind it today and nothing standing behind it the first
 * time a sprite changes tile.
 *
 * SO IT IS OPT-IN, behind SM64DS_GAPLESS_OBJ_SHIFT=1, and nothing turns it on.
 * The switch stays so an A/B of the picture is one binary, which is what
 * notes/port-selftest-bmp-gate.md requires before two BMPs may be compared at
 * all, and so the next lane can put the per-entry version behind the same
 * geometry without rebuilding the band, the layout or the proofs.
 *
 * WHEN IT IS ON, the value is g_gapless_head_ds: the G the running scene's own
 * InitResources wrote, captured by the latch below one statement before it
 * stores zero over it, gated on the mod being engaged FOR THE SCENE NOW RUNNING
 * because the flag alone keeps reading 1 after that minigame has ended.
 *
 * READ FROM TWO PLACES and it must give both the same answer within a frame:
 * the layout below sizes the band from it and the engine A compositor shifts
 * its raster by it, so this reads the captured word and the engagement test
 * rather than caching an answer of its own. */
int obj_shift_ds(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAPLESS_OBJ_SHIFT");
        on = s && *s && *s != '0';
    }
    if (!on || !hal_gapless_engaged()) return 0;
    return g_gapless_head_ds;
}

}  // namespace

int hal_gapless_obj_shift_ds(void) { return obj_shift_ds(); }

int hal_screen_gap_raw(void) { return read_raw(); }

const ntr::StackLayout *hal_screen_layout(void)
{
    const int raw = read_raw();
    /* MinigameGap false means G is treated as ZERO FOR LAYOUT, and only for
       layout: the word itself is not written, so the game's sprite submission
       still runs at a2 + 0xc0 + G and objects still vanish at the seam. That is
       precisely what the setting promises -- "remove the gap" gives back the
       picture the port had before this feature, jump and all. */
    const int want_gap = host_setting_minigame_gap() ? raw : 0;
    /* THE SHIFT, AND THE BAND IT NEEDS. See obj_shift_ds above. With the mode
       engaged the game's own G reads zero, so `want_gap` is zero and there is
       no band; the shifted objects need one, because the rows they are pushed
       into past the top screen's bottom edge are world -G_rom..-1 and those
       rows have to be somewhere in the image. They go where the hinge goes,
       between the halves, so the band's height is the shift and the image is
       192 + G_rom + 192 DS rows, which is gap-on's own shape.

       THE TWO CANNOT BOTH BE NON-ZERO. `want_gap` is the game's G, which the
       mod has zeroed before anything can engage the shift, so this is a choice
       between a real gap and a shifted band and never a sum. */
    const int shift = obj_shift_ds();
    const int want = shift ? shift : want_gap;
    /* THE HEADROOM, and it is a DISPLAY answer to a SIMULATION cost. See the
       StackLayout note in ntr/ppu.h: gapless zeroes G, the game's own fixed
       constants keep placing actors in a world 192 + G_rom + 192 rows tall, and
       the top G_rom rows of it fall above the top screen. Giving the IMAGE
       those rows back shows them without touching a constant or the zeroed
       word. It is asked for ONLY while the mod is engaged for the scene now
       running, so every other layout in the program carries a zero.

       IT IS OFF UNLESS SM64DS_GAPLESS_HEADROOM=1 ASKS FOR IT, and headroom_ds
       above carries the measurement that made it opt-in: there is nothing above
       the top screen to draw, in either arm, on any frame. The switch stays so
       an A/B of the picture is one binary, which is what
       notes/port-selftest-bmp-gate.md requires before two BMPs may be compared
       at all. */
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
    /* AND THE SHIFT IS PART OF THE LATCH, for the reason the headroom is: it
       cannot move without `want` moving today, and a cache whose key is a
       subset of its inputs is one edit away from serving a stale answer. */
    if (g_have && want == g_raw && scene == g_scene && head == g_head &&
        shift == g_shift) {
        g_lay.seam = seam;
        return &g_lay;
    }

    const int was_h = g_have ? g_lay.h : 0;
    g_raw = want;
    g_scene = scene;
    g_head = head;
    g_shift = shift;

    const int mode = host_setting_gap_fill_mode();
    /* PEEK IS OFF IN THE SHIFTED MODE, decided here so that the layout every
       consumer reads already carries the answer and no pass has to ask the
       question a second time. Peek shows what the engines put in rows nobody
       displays; under the shift the top engine displays those rows itself, so
       there is nothing left for peek to reveal and a second rendering of the
       same objects is all it could add. It also drops the band to a black
       backdrop, which would replace the fill the picture is supposed to share
       with gap-on mode. */
    const int peek = shift ? 0 : host_setting_gap_peek();
    /* THE ART IS LOADED HERE, ONCE, and this is the only place that asks for
       it. Both askers are here: the "custom" fill mode wants it with peek off,
       and peek wants it as its backdrop whatever the fill mode says. Asking at
       the latch rather than per frame is what keeps a file read off the frame
       path -- the loader caches on the same (scene, G) pair this does, so the
       second ask in a scene is a pointer compare. */
    const uint32_t *art = 0;
    if (want && (peek || mode == ntr::GAP_FILL_CUSTOM))
        art = hal_gap_art(scene, want, mode == ntr::GAP_FILL_CUSTOM);

    g_lay = ntr::stack_layout(want, head, shift, mode,
                              host_setting_gap_color(), peek, art);
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
        /* SAID ONCE PER LATCH IN THE SHIFTED MODE, and it is the line that
           tells a capture from this mode apart from a gap-on capture of the
           same scene. The two images are the same size and the same shape by
           design, so the size cannot do it and a run report with no such line
           is a run report about a different picture. */
        if (g_lay.obj_shift_ds)
            std::fprintf(stderr, "[gapless] scene %d: OBJ SHIFT %d DS rows "
                         "(%d host rows) -- the top engine's sprites are drawn "
                         "at the rows the ROM submits them at, and the band "
                         "below the top screen is world -%d..-1 with the top "
                         "engine drawing into it. Image %dx%d, the same shape "
                         "gap-on composes. The simulation is untouched and is "
                         "still gapless: nothing crosses a hinge.\n", scene,
                         g_lay.obj_shift_ds, g_lay.band_h, g_lay.obj_shift_ds,
                         g_lay.w, g_lay.h);
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
                 "screen gap is now gone from the SIMULATION: objects cross the "
                 "seam directly and arrive %d rows sooner than a DS delivers "
                 "them. THIS IS NOT THE ROM'S BEHAVIOUR. The PICTURE keeps the "
                 "%d rows: %s\n", scene, row->what, was, was, was,
                 obj_shift_ds()
                     ? "the OBJ display shift is ON, which is opt-in and "
                       "MEASURED WRONG: it moves the whole top engine OBJ "
                       "layer, and only the framework-routed sprites are "
                       "displaced. Read obj_shift_ds in hal/screen_gap.cpp"
                     : "the halves are edge to edge, and the sprites the "
                       "framework's OAM router placed sit G rows above the "
                       "artwork they were drawn against");
}

/* THE SCENE IS PART OF THE ANSWER. The flag alone would keep reading 1 after
   the minigame that engaged it has ended, and a run report that says "gapless"
   about a scene this never touched is worse than no line at all. */
int hal_gapless_engaged(void)
{
    return g_gapless_on && hal_gap_scene_id() == g_gapless_scene;
}
