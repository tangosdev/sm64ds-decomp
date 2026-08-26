/* The screen gap's layout owner. See port/hal/screen_gap.h for what the gap is
   and why the layout is one struct rather than five copies of a subtraction. */

#include "hal/screen_gap.h"

#include "hal/gap_art.h"
#include "hal/gap_continuity.h"
#include "hal/host_settings.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

/* the dual-OAM flag and the compositor's seam hook; see the layout below */
extern "C" unsigned char data_0209e660;
extern "C" void hal_oam_source_a_live(unsigned addr);
static inline int data_0209e660_ref(void) { return data_0209e660 != 0; }

namespace {

int g_have;
int g_raw = -1;              /* the last G read out of the game */
int g_scene = -2;            /* the scene the last latch was for */
int g_head = -1;             /* the headroom the last layout was built with */
int g_shift = -1;            /* the OBJ display shift the last layout used */
int g_setgen_l = -1;         /* host_settings_gen at the last latch: the live
                                re-read steps it when fill/colour/peek move,
                                which changes the layout while every other
                                key input stays equal */
/* G_rom: the G THE SCENE'S OWN InitResources WROTE, captured by the gapless
   latch below at the instant before it stores zero over it, because that is the
   only moment the number exists. Everything after the latch reads zero, and the
   headroom is exactly this many DS rows -- the world's top rows the zeroed G
   pushed off the top of the top screen. Zero until a latch engages. */
int g_gapless_head_ds;
unsigned g_gen;              /* steps whenever the layout's shape changes */
ntr::StackLayout g_lay;

int obj_shift_ds(void);
int obj_layer_shift_ds(void);
int headroom_ds(void);
int main_shadow_is_e674(void);
extern int g_gapless_tall;   /* defined beside the mode table below */

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
    /* OPT-IN AGAIN, third flip, and this one is the owner's morning-after
       verdict on the played build rather than a measurement: the strip's
       backdrop smears the top screen's first image row downward (head_paint's
       fill), which on Bob-omb Squad is the airship hull, and every engaged
       game wore some version of that streak. Gapless now means the two
       screens edge to edge and nothing else -- window 512x768 -- and the
       world's top G rows go undisplayed, exactly as they do on a DS whose
       player is not inside the hinge. SM64DS_GAPLESS_HEADROOM=1 brings the
       strip back for measurement. */
    /* ROW-DRIVEN now: a GL_ZERO_G_TALL scene carries the strip, everything
       else does not, and the env is gone with the rest of the arm switches.
       The strip's earlier retirement was about its use as a universal
       answer; as a per-game mode for a scene whose upper strip carries real
       content, it is exactly the owner's ask: 'the screen needs to be
       taller so the score and Wario can sit properly'. */
    if (!g_gapless_tall || !hal_gapless_engaged()) return 0;
    /* AND NEVER WITH THE OBJECT SHIFT ON. Both are opt-in and both are wrong,
       so nothing turns either on; what this refuses is the ONE combination
       whose picture could not be attributed afterwards. The strip fills rows
       above the artwork from the top screen's own first row, and under the
       shift that row is a different row, so a capture of the two together would
       carry one feature's error smeared through the other's and no way to read
       which was which. A measurement kept for its numbers has to stay
       separable.

       THE REFUSAL NAMES THE LAYER ARM DIRECTLY rather than obj_shift_ds:
       obj_shift_ds reads per_entry_ds, and per_entry_ds now stands down for
       THIS function, so asking the combined question from here would be a
       cycle. The layer arm is the only picture that cannot share a frame
       with the strip, and it is asked for by name. */
    if (obj_layer_shift_ds()) return 0;
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
int obj_layer_shift_ds(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAPLESS_OBJ_SHIFT");
        on = s && *s && *s != '0';
    }
    if (!on || !hal_gapless_engaged()) return 0;
    return g_gapless_head_ds;
}

/* THE PER-ENTRY CORRECTION, in DS rows, and THIS ONE IS THE DEFAULT.
 *
 * WHAT IT IS. The same G_rom the layer shift adds, added to the submissions the
 * framework's five G-adding OAM routers make and to NOTHING else. Read the
 * router hook below for how the routed set is recorded; read obj_layer_shift_ds
 * above for the measurement that says the set matters. In one line: eleven of
 * the twelve sprite identities on scene 368 are placed in screen space and are
 * at the same engine row whether the framework word reads 32 or 0, so the fix
 * has to be able to tell the twelfth from them, and the only place that
 * distinction exists is at the call.
 *
 * IT IS ON BY DEFAULT, which the two features before it were not, and the
 * difference is that this one is measured RIGHT rather than measured wrong.
 * SM64DS_GAPLESS_PER_ENTRY=0 turns it off and gives back the picture the mod
 * composed before this lane, on the same binary, which is what
 * notes/port-selftest-bmp-gate.md requires before two BMPs may be compared at
 * all.
 *
 * AND IT STANDS DOWN FOR THE LAYER ARM. Both mechanisms put G_rom back and they
 * would put it back twice; more to the point, an A/B is only readable if it is
 * one mechanism against the other. With SM64DS_GAPLESS_OBJ_SHIFT=1 the layer
 * arm is the whole answer and this is zero, which is the arm lane OBJSHIFT
 * measured and the arm its numbers still describe.
 *
 * READ FROM TWO PLACES within a frame -- the router hook adds it at submission,
 * the layout sizes the band from it -- so it reads the captured word and the
 * engagement test rather than caching an answer of its own. */
int per_entry_ds(void)
{
    /* OPT-IN NOW, like the other two arms. The correction and the band it
       needs were one regime; the owner's verdict retired that regime for the
       plain edge-to-edge picture, so the default carries no correction and
       no band. SM64DS_GAPLESS_PER_ENTRY=1 brings the banded arm back for
       measurement on one binary. */
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_GAPLESS_PER_ENTRY");
        on = s && *s && *s == '1';
    }
    if (!on || !hal_gapless_engaged()) return 0;
    if (obj_layer_shift_ds()) return 0;
    /* AND IT STANDS DOWN FOR THE HEADROOM, which is the default regime now.
       The correction exists to serve the banded picture: it pushes routed
       sprites back to ROM rows so the band between the halves can carry what
       runs off the top screen's bottom edge. With the strip on there is no
       band -- the halves are edge to edge, the G=0 mapping is the consistent
       one for every sprite, and the world's top rows live in the strip. A
       correction on top of that would displace the routed set 32 rows down a
       picture that already shows every world row once. */
    if (headroom_ds()) return 0;
    return g_gapless_head_ds;
}

/* THE BAND'S WORLD ROWS, which both arms need and which is what the LAYOUT is
   told. The two arms differ in where the correction is applied, not in the
   shape of the image it needs: either way the rows a corrected sprite runs off
   the top screen's bottom edge into are world -G_rom..-1, and either way they
   go between the halves. */
int obj_shift_ds(void)
{
    const int pe = per_entry_ds();
    return pe ? pe : obj_layer_shift_ds();
}

}  // namespace

int hal_gapless_obj_shift_ds(void) { return obj_shift_ds(); }

int hal_gapless_obj_raster_shift_ds(void) { return obj_layer_shift_ds(); }

int hal_gapless_per_entry_ds(void) { return per_entry_ds(); }

int hal_screen_gap_raw(void) { return read_raw(); }

int hal_gapless_visual(void);   /* defined beside the latch below */
int hal_gapless_world(void);    /* same */
int hal_gapless_world_rows(void); /* same */

const ntr::StackLayout *hal_screen_layout(void)
{
    const int raw = read_raw();
    /* MinigameGap false means G is treated as ZERO FOR LAYOUT, and only for
       layout: the word itself is not written, so the game's sprite submission
       still runs at a2 + 0xc0 + G and objects still vanish at the seam. That is
       precisely what the setting promises -- "remove the gap" gives back the
       picture the port had before this feature, jump and all. */
    int want_gap = host_setting_minigame_gap() ? raw : 0;
    /* ONE HINGE, OWNER'S RULING (2026-08-26). Games write their own G --
       the dScMgD3DBase_c four write 16, most others 32 -- and this layout
       used to draw each game's number, so the visible gap changed size per
       game. The owner tested it directly: paused Trampoline Time with Mario
       inside the hinge, and with a 16-row band his feet barely cleared the
       bottom while nothing showed at the top -- the 3D crossing hides MORE
       rows than the 2D G word says, so drawing 16 was not faithful to that
       family's own physics either. His order: one gap, 32 DS rows (64 host),
       every game. DISPLAY ONLY -- the game's word is not written, its sprite
       submission still runs at a2 + 0xc0 + its own G, so a 2D sprite crossing
       the seam in a G=16 game lands 16 rows early against this band. If that
       is ever visible in play it is a report for the owner, not a reason to
       quietly revert this. */
    if (want_gap)
        want_gap = 32;
    /* A VISUAL row drops the band the same way MinigameGap false does, and by
       the same mechanism: layout only. The word is untouched, the game still
       submits at a2 + 0xc0 + G, and nothing crosses in these games, so no
       object is ever in the rows that stopped being displayed. */
    if (hal_gapless_visual() || hal_gapless_world()) want_gap = 0;
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
    /* POWCNT1's DISPLAY SWAP, read live and set on both paths out of here for
       exactly the reason the seam flag is: it is not in the latch key, and it
       is not a per-scene fact at all. GBATEK, POWCNT1 (0x04000304) bit 15:
       0 sends display A to the LOWER screen, 1 to the upper one.

       PER FRAME AND NOT PER SCENE. The dScMgD3DBase_c family's slot 24
       (func_ov006_020e6e78) TOGGLES this bit every frame -- one camera view
       rendered live on the screen the bit names, the other showing the display
       capture unit's copy of the previous frame -- so a value cached at the
       G-latch would be one frame's answer serving a whole minigame. Scene 377
       (Snowball Slalom) clears it once at InitResources and leaves it clear;
       both shapes fall out of reading it here.

       EVERY OTHER SCENE LEAVES IT SET, and hal/sub_screen.cpp's bring-up sets
       it, so this reads 0 for them and ntr::ppu_compose_stacked composes
       exactly what it composed before the bit was read at all. */
    const int main_lower =
        (*(volatile unsigned short *)0x04000304 & 0x8000) ? 0 : 1;
    /* THE SEAM'S FRAME ALIGNMENT, set every frame for the same reason the
       seam flag is: it must die with the scene. With the mod engaged the two
       screens are one picture, so the top screen reads the game's WORKING
       shadow -- the block OAM::Load is about to upload -- instead of the
       hardware OAM it uploaded last frame; otherwise a falling object is one
       frame short exactly at the join. Everywhere else this is zero and the
       top screen draws what it always drew. */
    /* AND THE SHIFT IS PART OF THE LATCH, for the reason the headroom is: it
       cannot move without `want` moving today, and a cache whose key is a
       subset of its inputs is one edit away from serving a stale answer. */
    /* THE LAYER TERM, set on both paths out of here for the reason the seam
       flag is: it is not in the latch key, so a cached layout has to be told it
       too. It is clamped to the band stack_layout actually built rather than to
       the number asked for, so the two can never disagree about rows that do
       not exist. Zero in the per-entry default, which is every run that does
       not ask for the falsified arm. */
    const int raster = obj_layer_shift_ds();
    const int setgen = host_settings_gen();
    if (g_have && want == g_raw && scene == g_scene && head == g_head &&
        shift == g_shift && setgen == g_setgen_l) {
        g_lay.seam = seam;
        g_lay.world_band = (hal_gapless_world() || hal_gapless_world_rows()) ? 1 : 0;
        g_lay.obj_raster_ds = raster ? g_lay.obj_shift_ds : 0;
        g_lay.main_lower = main_lower;
        return &g_lay;
    }

    const int was_h = g_have ? g_lay.h : 0;
    g_raw = want;
    g_scene = scene;
    g_head = head;
    g_shift = shift;
    g_setgen_l = setgen;

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
    g_lay.world_band = (hal_gapless_world() || hal_gapless_world_rows()) ? 1 : 0;
    g_lay.obj_raster_ds = raster ? g_lay.obj_shift_ds : 0;
    g_lay.main_lower = main_lower;
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

enum {
    GL_VISUAL = 0,   /* G untouched; the picture just loses the band */
    GL_ZERO_G = 1,   /* G -> 0: the world compresses, crossings continuous */
    GL_WORLD_ROWS = 4, /* G untouched: both screens are the ROM's exact
                        picture -- score line at the top, Wario on his
                        blocks -- and the hidden hinge rows are drawn as
                        real rows between the screens, backdrop continued
                        from the scene's own art, crossers crisp inside
                        them via the peek raster and the continuity reader.
                        The picture is G rows taller and nothing vanishes. */
    GL_ZERO_G_TALL = 3, /* zero-G plus the headroom strip: the picture gains
                        G rows above the top screen showing the world's own
                        top rows. For a game whose upper strip carries real
                        content (Coincentration's Wario and score line) the
                        compressed window cuts it; the taller picture gives
                        it back without touching registration below. */
    GL_SPLICE = 2,   /* G untouched, band gone, objects TELEPORT across the
                        hidden rows (hal_gapless_splice). For a game whose
                        top screen is REGISTERED art: zeroing G lifts every
                        world sprite off the artwork (Slots Shot's star,
                        ramps and peg hitboxes rode 48 rows high), while
                        this keeps the ROM's own registration everywhere
                        and costs one small single-tick hop at the seam. */
};

struct GaplessScene {
    int scene_id;
    int mode;
    const char *what;
};

/* The rows and their modes are the owner's per-game verdicts, played and
   judged one at a time. Each row still owes the audit the table's note
   above describes. */
const GaplessScene kGaplessScenes[] = {
    {368, GL_ZERO_G, "dScMgPachinko_c, Bob-omb Squad"},
    {374, GL_ZERO_G, "dScMgCurling_c, Shuffle Shell"},
    /* 376, Slots Shot, is DELIBERATELY NOT A ROW -- the owner's 2026-08-21
       verdict after playing both candidate modes. Its top screen is a
       registered playfield, so zero-G lifts every world sprite and hitbox
       off the board; and any mode that keeps the ROM's registration leaves
       48 hidden rows at the seam, where crossers visibly shrink out and pop
       back -- the exact artifact the mod exists to remove. The only clean
       geometry is zero-G with the board art extended 48 rows so it can
       follow the world down, and that is ART WORK, not code: the map rows
       below the visible board would have to be painted. Until someone
       paints them, the honest answer is the one the latch already gives an
       unlisted scene: gap simulated, said out loud. */
    {378, GL_WORLD_ROWS, "MgCoincentration, Coincentration"},
    {366, GL_VISUAL, "dScMgLuigi_c, Wanted!"},
    {390, GL_VISUAL, "dScMgFlower_c, Loves Me...?"},
};

int g_gapless_on;        /* 1 once the write has engaged for the scene running */
int g_gapless_visual;    /* 1 for a VISUAL row: band gone, simulation untouched */
int g_gapless_world;     /* 1 for a FULL row: ROM sim, hinge rows drawn as world */
int g_gapless_tall;      /* 1 for a TALL row: the headroom strip is on */
int g_gapless_wrows;     /* 1 for a WORLD_ROWS row: banded, band = world */
int g_gapless_scene = -2;

const GaplessScene *gapless_row(int scene)
{
    /* THE MOD IS ON HOLD, owner's order 2026-08-21: every mode above is
       played, judged and kept, and none of them ships until the seam-band
       art exists (Slots Shot's note has the recipe; 378's WORLD-ROWS wants
       painted band rows for the same reason). Returning no row here sends
       every scene down the honest UNSUPPORTED path -- GaplessMinigames on
       means a message and the ROM's own simulated gap, nothing else. To
       resurrect the mod, delete this return; the table and every mode
       behind it are intact and each row's verdict is written beside it. */
    return 0;

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
    g_gapless_visual = 0;
    g_gapless_world = 0;
    g_gapless_tall = 0;
    g_gapless_wrows = 0;
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

    if (row->mode == GL_VISUAL) {
        /* THE VISUAL HALF OF THE TABLE. The game's G word is not touched:
           nothing in this minigame travels between the screens, so zeroing G
           would change gameplay math for no visible gain, and the owner's
           spec for these two is explicit -- no gameplay changes. All
           "gapless" means here is the picture: the layout reads the flag
           below and builds no band, so the halves sit edge to edge. */
        g_gapless_visual = 1;
        std::fprintf(stderr, "[gapless] scene %d: VISUAL for %s -- the band "
                     "leaves the PICTURE and the halves sit edge to edge. The "
                     "simulation is untouched: G stays %d, exactly the ROM.\n",
                     scene, row->what, read_raw());
        return;
    }

    /* THE FULL HALF, third architecture, and this one starts from the owner's
       sentence rather than from the word's: "a seamless transition from top
       screen to bottom". Zeroing G never delivered that -- it SLIDES the top
       window down G rows to expose the crossing, which detaches every
       world-anchored sprite from the screen-anchored art it stands on
       (Wario off his platform, the ball above its pegs) and pushes the
       world's top G rows off the picture. The owner played every variant of
       that regime and named the defect exactly: "you shifted the whole
       screen down".

       So the game's G word is NOT TOUCHED ANY MORE. The top screen stays the
       ROM's picture to the pixel, and the G rows between the screens -- the
       world rows a DS physically hides inside its hinge -- are rendered as
       REAL ROWS: the scene's own backdrop colour behind, the engines' own
       submissions plus the continuity pass's dead-zone entries in front,
       crisp. An object falls through the seam in one unbroken line because
       every row of its path is now on screen. Nothing is displaced, so
       there is nothing to correct.

       SM64DS_GAPLESS_ZERO_G=1 restores the old G write for measurement. */
    const int was = read_raw();
    g_gapless_head_ds = was;
    /* THE MODE IS THE ROW'S, one game at a time, each played and judged by
       the owner. ZERO-G compresses the world: seamless crossings, at the
       cost of world sprites riding G rows above screen-anchored art --
       right for the sky games, visibly wrong for a registered playfield.
       SPLICE keeps the ROM's own registration on both screens and instead
       teleports crossers over the hidden rows (hal_gapless_splice), costing
       one small single-tick hop at the seam -- right for Slots Shot, whose
       spinning star, ramps and peg hitboxes all rode 48 rows above the
       board under zero-G. Neither is globally correct; that lesson cost a
       whole night. */
    if (row->mode == GL_WORLD_ROWS) {
        g_gapless_wrows = 1;
        std::fprintf(stderr, "[gapless] scene %d: WORLD-ROWS for %s -- both "
                     "screens keep the ROM's exact picture and the %d hidden "
                     "hinge rows are drawn as real world rows between them, "
                     "art-continued backdrop, crossers crisp. The picture is "
                     "taller; nothing is displaced and nothing vanishes.\n",
                     scene, row->what, was);
        return;
    }
    if (row->mode == GL_ZERO_G || row->mode == GL_ZERO_G_TALL) {
        g_gapless_tall = row->mode == GL_ZERO_G_TALL;
        data_ov004_020beb6c[0] = 0;
        data_ov004_020beb6c[1] = 0;
        data_ov004_020beb6c[2] = 0;
        data_ov004_020beb6c[3] = 0;
        g_gapless_on = 1;
        std::fprintf(stderr, "[gapless] scene %d: ENGAGED for %s -- G %d -> 0,"
                     " no band, no strip, no correction. Objects cross the "
                     "seam in one unbroken line; world-anchored sprites sit "
                     "%d rows above the top screen's own art, which is the "
                     "trade this mode ships with.\n",
                     scene, row->what, was, was);
        return;
    }
    g_gapless_world = 1;
    std::fprintf(stderr, "[gapless] scene %d: SPLICE for %s -- no band, both "
                 "screens keep the ROM's own picture, and an object entering "
                 "the %d hidden hinge rows is carried straight across them in "
                 "its direction of travel (hal_gapless_splice). It arrives %d "
                 "rows sooner than a DS delivers it; nothing else in the game "
                 "is touched.\n", scene, row->what, was, was);
}

/* THE VISUAL FLAG, scene-checked the way hal_gapless_engaged is and for the
   same reason: it must stop answering the moment the minigame that set it is
   gone. Only the layout reads it. */
int hal_gapless_visual(void)
{
    return g_gapless_visual && g_gapless_scene == hal_gap_scene_id();
}

/* THE WORLD-BAND FLAG, same scene check. */
int hal_gapless_world(void)
{
    return g_gapless_world && g_gapless_scene == hal_gap_scene_id();
}

/* WORLD_ROWS: the band stays and is painted as world. Only the layout and
   the band painters read it. */
int hal_gapless_world_rows(void)
{
    return g_gapless_wrows && g_gapless_scene == hal_gap_scene_id();
}

/* ---- THE SPLICE: the rows the mod removed, removed from trajectories too ---
 *
 * The mod's whole meaning, in the owner's words: the gap physically does not
 * exist in the minigame any more. The picture side of that is the bandless
 * layout (the halves edge to edge). This is the world side: an object whose
 * anchor enters the G rows a DS hides inside its hinge is carried straight
 * across them, in its direction of travel, so it leaves one screen and
 * continues on the other with no hidden dwell -- it genuinely arrives G rows
 * sooner than a DS delivers it. Nothing else in the game is touched: the G
 * word keeps the ROM's value, every screen keeps the ROM's registration, and
 * an object that never enters the zone never feels the mod at all.
 *
 * PER GAME, PER ARRAY, BY THE SAME OFFSETS THE GAME'S OWN RENDER LOOPS READ
 * (each row cites its loop). Direction comes from the object's own motion --
 * this frame's y against last frame's, tracked here -- because the arrays do
 * not agree on where they keep a velocity, and a derived sign is true for
 * all of them. A slot with no history yet, or one that has not moved, is
 * left alone this frame and spliced the next; at minigame speeds the zone is
 * crossed in a handful of frames and one frame of grace is invisible. */
namespace {

struct SpliceArray {
    int scene;
    int base, stride, count;   /* the object array, bytes off the scene */
    int live1, live2;          /* in-slot flag offsets; -1 = only one flag */
    const char *what;
};

const SpliceArray kSplice[] = {
    /* func_ov006_020fc8c0 renders these 30; live flag abs +0x468d is checked
       there, spawn sets +0x468c -- both must be up before a slot is real */
    {368, 0x4660, 0x38, 30, 0x2c, 0x2d, "Bob-omb Squad parachute drops"},
    /* func_ov006_020fe1d0 renders these 48; hal/gap_continuity.cpp already
       documents the base, the stride and the +0x4f0e visible flag */
    {368, 0x4ed8, 0x38, 48, 0x36, -1, "Bob-omb Squad slingshot balls"},
    /* func_ov006_020e1c68 renders these 5, gated on both bytes */
    {374, 0x4660, 0x2c, 5, 0x29, 0x2a, "Shuffle Shell shells"},
    /* func_ov006_02102de4 renders these 48 */
    {376, 0x4660, 0x40, 48, 0x3a, -1, "Slots Shot balls"},
    /* func_ov006_020dd594 renders these 40 */
    {378, 0x4660, 0x1c, 40, 0x16, -1, "Coincentration coin stacks"},
};

enum { SPLICE_ROWS = sizeof kSplice / sizeof *kSplice, SPLICE_SLOTS = 48 };
int g_splice_prev[SPLICE_ROWS][SPLICE_SLOTS];
unsigned char g_splice_have[SPLICE_ROWS][SPLICE_SLOTS];
int g_splice_scene_seen = -2;   /* history dies with the scene */
unsigned g_splice_hops;
unsigned g_gapless_ticks;

}  // namespace

void hal_gapless_splice(void)
{
    /* THE GAME'S OWN HEARTBEAT, counted before any early-out: the four
       engaged scenes' behavior thunks call this once per game tick and call
       it from nowhere else, so this counter advances exactly when the game
       simulates and freezes exactly when it does not -- a host-side pause
       (F5's save-state capture) stops the thunks and therefore stops this.
       The seam-ghost pass paces itself off it; see hal_gapless_ticks. */
    ++g_gapless_ticks;
    if (!hal_gapless_world()) return;
    unsigned char *scene = hal_gap_scene();
    if (!scene) return;
    if (g_splice_scene_seen != g_gapless_scene) {
        g_splice_scene_seen = g_gapless_scene;
        std::memset(g_splice_have, 0, sizeof g_splice_have);
        g_splice_hops = 0;
    }
    const int g = g_gapless_head_ds;
    if (g <= 0) return;
    const int zone = g << 12;
    for (unsigned r = 0; r < SPLICE_ROWS; ++r) {
        const SpliceArray &a = kSplice[r];
        if (a.scene != g_gapless_scene) continue;
        for (int i = 0; i < a.count && i < SPLICE_SLOTS; ++i) {
            unsigned char *slot = scene + a.base + i * a.stride;
            const int live = *(slot + a.live1) &&
                             (a.live2 < 0 || *(slot + a.live2));
            if (!live) {
                g_splice_have[r][i] = 0;
                continue;
            }
            int *py = (int *)(slot + 4);
            const int y = *py;
            if (g_splice_have[r][i] && y > -zone && y < 0) {
                const int dy = y - g_splice_prev[r][i];
                if (dy > 0) { *py = y + zone; ++g_splice_hops; }
                else if (dy < 0) { *py = y - zone; ++g_splice_hops; }
                /* the first carry of a run announces itself, so any log can
                   say whether the splice ever fired without a per-scene
                   report having to know about it */
                if (g_splice_hops == 1 && (dy > 0 || dy < 0))
                    std::fprintf(stderr, "[gapless] first splice carry: %s "
                                 "slot %d, world %d -> %d\n", a.what, i,
                                 y >> 12, *py >> 12);
            }
            g_splice_prev[r][i] = *py;
            g_splice_have[r][i] = 1;
        }
    }
}

unsigned hal_gapless_splice_hops(void) { return g_splice_hops; }

unsigned hal_gapless_ticks(void) { return g_gapless_ticks; }

/* THE SCENE IS PART OF THE ANSWER. The flag alone would keep reading 1 after
   the minigame that engaged it has ended, and a run report that says "gapless"
   about a scene this never touched is worse than no line at all. */
int hal_gapless_engaged(void)
{
    return g_gapless_on && hal_gap_scene_id() == g_gapless_scene;
}

/* THE TOP SCREEN'S OAM SOURCE, pulled by hal/message_compositor.cpp's two
   OBJ rasters at read time. Those rasters run BEFORE the frame's OAM::Load
   while the bottom screen scans out after it, so with the mod engaged the
   top half of a straddling sprite was one frame of travel behind the bottom
   half -- the owner filmed a crossing bob-omb's halves ticking on alternate
   frames. While the mod is engaged for the running scene this returns the
   working shadow OAM::Load is about to upload, picked by Load's own bank
   mapping; everywhere else it returns 0 and the rasters read the uploaded
   OAM exactly as they always have. A PULL rather than a push, because the
   first push lived in the stacked-compose path, which a headless capture
   runs once at exit and the window runs after the composite -- the value
   arrived after every read that needed it. */
unsigned hal_gapless_oam_src_a(void)
{
    if (!hal_gapless_engaged()) return 0;
    if (data_0209e660) return 0x0209e674u;
    return (*(volatile unsigned short *)0x04000304 & 0x8000) ? 0x0209e674u
                                                             : 0x0209ea74u;
}

/* GHOST ATTRIBUTES: the sprite identities (attr2 words) whose objects the
   compositor should CONTINUE across the seam when the game despawns them at
   the top screen's bottom edge. Owner's spec for Shuffle Shell: "make the
   top snow not despawn and just keep going all the way down". Per scene,
   because an identity is only meaningful inside one game's tile set; empty
   everywhere else and the whole ghost pass stands down. */
const unsigned short *hal_gapless_ghost_attrs(int *count)
{
    static const unsigned short kSnow374[] = {0x2048, 0x2049, 0x204a,
                                              0x204b, 0x204c};
    *count = 0;
    if (!hal_gapless_engaged()) return 0;
    if (hal_gap_scene_id() != 374) return 0;
    *count = (int)(sizeof kSnow374 / sizeof *kSnow374);
    return kSnow374;
}

/* ---- THE ROUTER HOOK: which OAM entries were placed in WORLD coordinates ----
 *
 * WHY THERE IS A HOOK AT ALL. The per-entry correction has to be applied to the
 * submissions the framework's five G-adding OAM routers make and to no others,
 * and nothing in the finished OAM tells the two apart: same attribute words,
 * same slot allocation, interleaved indices, and a table of attribute words
 * would be a guess that stopped being true the first time a sprite changed
 * tile. So the routed set is RECORDED AT THE CALL. This is that recording.
 *
 * AND IT IS PORT-SIDE, which was the constraint. src/ and include/ are not
 * touched: port/CMakeLists.txt gives the five router TUs -- and only those five
 * -- two compile definitions each, using the same set_source_files_properties
 * mechanism this port already uses in a dozen places to rename a symbol inside
 * one translation unit. The first renames the router's own definition to
 * <name>__rom so that the wrapper below can carry the real name; the second
 * renames the OAM::Render overload that TU declares extern "C" so that its own
 * calls land on the shims below. Every other caller of OAM::Render in the
 * program, and every other definition of anything, is untouched.
 *
 * WHICH CALL IS THE ROUTED ONE, and this is the whole reason the wrapper takes
 * the router's arguments rather than just setting a flag. Each of the five
 * bodies reaches OAM::Render on more than one path:
 *
 *     Render(0, attr, x, a2 + 0xc0 + G, ...)   the WORLD-frame top-engine
 *                                              submission, the one that carries
 *                                              the G term
 *     Render(0 or 1, attr, x, a2, ...)         the same object handed to the
 *                                              other engine, or to this one in
 *                                              screen space, with no G term
 *
 * so "inside a router" is not enough, and the engine argument is not enough
 * either -- func_ov004_020aff38's two-player branch passes 0 on both. What IS
 * enough is a2 itself: the wrapper has it, it is the third argument of all five
 * bodies, and 0xc0 + G is never zero, so `py == a2 + 0xc0 + G` picks the
 * G-carrying call and can pick nothing else. No band test is duplicated here
 * and no branch of the ROM's is second-guessed.
 *
 * WHERE THE CORRECTION GOES, and there are two answers because OAM::Render has
 * TWO culls: `y + h < 0` and `y > 0xc0`.
 *
 *   AT SUBMISSION, normally. The ROM's own row is py + G_rom, so that is what
 *   is submitted, and OAM::Render then culls on the ROM's number rather than on
 *   one G_rom lower. That is what recovers the entries the mod loses outright
 *   -- 215 of them over 300 frames on scene 368, sprites entering from the top
 *   whose whole box was above row 0 once the mod had moved it up. A display
 *   pass cannot recover those: they never reached OAM.
 *
 *   AT THE RASTER, for the band. A world row in -G_rom..-1 has a ROM row past
 *   0xc0, which is off the bottom of a DS's top screen, and OAM::Render's
 *   `y > 0xc0` throws it away rather than let it reach the band the gapless
 *   picture draws it in. So when the corrected submission is refused, the
 *   uncorrected one is made instead -- the submission the mod makes today,
 *   which OAM::Render accepts -- and the entry carries a RESIDUAL of G_rom
 *   that the engine A raster and hinge_paint add for that slot alone.
 *
 * THE REFUSAL IS THE TEST, rather than a prediction of it. OAM::Render's cull
 * runs after the cel offset, the affine doubling and the rotation have moved
 * the box, so a test here would be a copy of that arithmetic that could drift
 * from it. A culled call writes nothing and increments no counter, so trying
 * the ROM's row first costs one call and cannot leave a mark behind; the affine
 * allocator deduplicates on the matrix, so even the affine overloads allocate
 * once. And the fallback cannot invent an entry the ROM would not have: a call
 * refused by the BOTTOM cull is refused again at a LOWER y, so the only
 * submission the second call can win is the one the top cull refused.
 *
 * OFF, ALL OF THIS IS TWO STORES. per_entry_ds is zero on every path in the
 * program except a scene the GaplessMinigames mod is engaged for, so the
 * wrapper pushes an int and pops it and the shims forward. */

extern "C" {

/* THE REAL OAM::Render OVERLOADS, under their own names. This TU carries no
   rename, so these declarations are the ROM bodies and not the shims. The
   signatures are the ones the router TUs themselves declare, argument for
   argument; the return is spelled as a pointer here because the slot is read
   out of it. */
void *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int, void *, int, int, int, int,
                                              int, int);
void *_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void *, int, int, int, int,
                                               void *);
void *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void *, int, int, int,
                                                  int, int, int, int, int);

/* THE MAIN OAM SHADOW, reached AS BYTES for the reason the block over
   data_ov004_020beb6c gives: the mount owns this storage and a second
   declaration of it as a struct array would be an opinion about the layout
   rather than a use of it. Only its address is taken, to turn the pointer
   OAM::Render returns into a slot index. data_0209e660 is the dual-OAM flag,
   read for the one thing it decides here: which hardware OAM this shadow
   reaches. */
extern unsigned char data_0209e674[];
extern unsigned char data_0209e660;

/* THE MAIN SHADOW'S ENTRY COUNTER, and it is the whole of why this file no
   longer reads OAM::Render's return value. See WHAT THE RETURN VALUE IS WORTH
   below: two of the three overloads never produce the pointer this used to
   test. The counter is the thing OAM::Render actually maintains -- it names the
   next free slot, it is incremented once per entry the call accepts, and it is
   the same word whether the call was culled early or late. Declared `int`, the
   type its own definition and every ROM reader spells.

   ALWAYS THIS COUNTER AND NEVER data_0209e670, because OAM::Render picks its
   pair on `!draw || data_0209e660 == 1` and every G-carrying submission the
   five routers make passes draw = 0. Checked in all five bodies rather than
   assumed. */
extern int data_0209e664;

/* The five router bodies, renamed inside their own TUs by the port's build. */
int func_ov004_020aff38__rom(void *, int, int, int, int, int, int);
void func_ov004_020afdd0__rom(void *, int, int, int, int);
void func_ov004_020b023c__rom(void *, int, int, int, void *);
void func_ov004_020b0380__rom(void *, int, int, void *);
void RenderOamBothScreens__rom(void *, int, int, int, int, void *);

}  /* extern "C" */

namespace {

/* THE ROUTER'S OWN a2, for as long as its body is running. A stack rather than
   a single int, because one router calling another is a shape this file must
   not have an opinion about; four deep is more than the ROM has and the guard
   is what keeps a wrong opinion from becoming a memory bug. */
enum { ROUTE_MAX = 4 };
int g_route_depth;
int g_route_a2[ROUTE_MAX];

struct RoutedCall {
    explicit RoutedCall(int a2)
    {
        if (g_route_depth >= 0 && g_route_depth < ROUTE_MAX)
            g_route_a2[g_route_depth] = a2;
        ++g_route_depth;
    }
    ~RoutedCall() { --g_route_depth; }
};

/* THE CORRECTION THIS CALL EARNS, in DS rows, and zero for every call that is
   not the routed one. See WHICH CALL IS THE ROUTED ONE above: the G-carrying
   submission is the only y that can equal a2 + 0xc0 + G, because 0xc0 + G is
   never zero. */
int routed_adjust(int py)
{
    if (g_route_depth <= 0 || g_route_depth > ROUTE_MAX) return 0;
    if (py != g_route_a2[g_route_depth - 1] + 0xc0 + read_raw()) return 0;
    /* The banded measurement arm keeps its constant correction. Nothing else
       corrects ANYTHING any more. A ramp that eased the correction out
       toward the seam was tried for one evening and measured wrong by the
       owner's own play: the routers submit a crossing object to BOTH engines,
       and any correction applied to the top copy and not the bottom one
       opens a gap of exactly that many sprite rows between the two halves at
       the seam -- the object hits a line, vanishes, and reappears below it.
       The two copies must agree everywhere, so the correction is zero
       everywhere. What a world-anchored sprite standing on screen-anchored
       art needs is its WORLD constant moved, per game, at that game's own
       latch -- the audit the gapless table's note has demanded all along --
       not a display-side nudge. */
    return per_entry_ds();
}

/* IS THE MAIN SHADOW THE BLOCK ENGINE A WILL BE SHOWING? OAM::Load's own test,
   mirrored rather than assumed: with the dual-OAM flag set it always sends
   data_0209e674 to 0x07000000, and otherwise POWCNT1's display-swap bit
   decides. A mark against the wrong engine's OAM would be a residual applied to
   somebody else's sprite, so this is checked rather than taken on trust. */
int main_shadow_is_e674(void)
{
    if (data_0209e660) return 1;
    return ((*(volatile unsigned short *)0x04000304 & 0x8000) >> 15) == 1;
}

/* THE SPLIT, COUNTED, because a mechanism with two branches and evidence for
   one of them is a mechanism half proven. SM64DS_OBJSHIFT_TRACE=1 names every
   entry the top cull refused at the ROM's row -- the ones the band carries on a
   residual -- and the totals say how often each branch ran. Off, this is two
   increments of a static. */
int routed_trace(void)
{
    static int on = -1;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_OBJSHIFT_TRACE");
        on = s && *s && *s != '0';
    }
    return on;
}

unsigned g_routed_rom, g_routed_band;

void routed_split(int py, int adj, int fell_back)
{
    if (fell_back) ++g_routed_band; else ++g_routed_rom;
    if (!routed_trace()) return;
    std::fprintf(stderr, "[routed] world %d: %s (submitted %d, ROM row %d); "
                 "%u at the ROM's row, %u on a band residual\n",
                 py - 0xc0 - read_raw(),
                 fell_back ? "the top cull refused the ROM's row, so the band "
                             "carries it on a residual"
                           : "submitted at the ROM's row",
                 fell_back ? py : py + adj, py + adj, g_routed_rom,
                 g_routed_band);
}

/* ---- WHAT THE RETURN VALUE IS WORTH, AND WHY THIS COUNTS SLOTS INSTEAD ------
 *
 * THE SHIMS USED TO TEST OAM::Render's RETURN VALUE, on the reading that a
 * culled call returns a null pointer and an accepted one returns the entry. That
 * is true of exactly ONE of the three overloads this file calls, and the other
 * two never produce it:
 *
 *   8 args, ..._Fix12IiEi     src defines it returning OamAttr *, null on every
 *                             cull. THE READING HOLDS HERE and this overload's
 *                             behaviour is unchanged by what follows.
 *
 *   7 args, ...P9Matrix2x2    src defines it returning VOID. Whatever is in the
 *                             return register at each exit is leftover
 *                             arithmetic -- the counter, x+w, y+h -- and it is
 *                             never zero. So `if (r)` was always taken, the
 *                             fallback below it was dead code, and the pointer
 *                             handed to the old routed_mark pointed outside the
 *                             shadow and failed its range test. Entries the top
 *                             cull refused were therefore DROPPED: never
 *                             re-submitted, never marked, never drawn in the
 *                             band.
 *
 *   10 args, ..._Fix12IiES3_ii  the C-linkage name is a forwarder in
 *                             hal/reverse_bridges.cpp that calls the C++ member
 *                             and returns 0 unconditionally. So `if (r)` was
 *                             NEVER taken, the second call ALWAYS ran, and every
 *                             routed submission through func_ov004_020afdd0 was
 *                             rendered TWICE -- once at the ROM's row and once
 *                             at the mod's row, 32 rows apart, with no mark on
 *                             either. That is the doubled sprite the player
 *                             reported.
 *
 * SO THE TEST IS THE COUNTER, which is the thing OAM::Render actually maintains
 * and the one signal all three overloads produce identically. data_0209e664
 * names the next free slot in the main shadow; the call increments it once per
 * entry it accepts and leaves it alone on every cull. Reading it either side of
 * the call says whether the submission landed and, exactly, WHICH SLOTS it
 * landed in -- which the 10-argument overload needs anyway, because it walks a
 * list and can accept several entries in one call where the old
 * pointer-and-one-slot reading could only ever record the last.
 *
 * AND EACH ENTRY IS THEN DRAWN EXACTLY ONCE. The second call is made only when
 * the first allocated NOTHING, so the two can never both be in the shadow;
 * whatever the second one allocates is marked with the residual, and the band
 * draws those rows while the top screen's raster clips them. */

int oam_slot_count(void) { return data_0209e664; }

/* MARK THE SLOTS A CALL JUST TOOK, from c0 up to but not including c1. The
   bounds test is the shadow's own 128 entries; a counter outside them is a
   state this file should record nothing about rather than guess at. */
void routed_mark_range(int c0, int c1, int resid)
{
    if (!main_shadow_is_e674()) return;
    for (int s = c0; s < c1; ++s)
        if (s >= 0 && s < 128) ntr::ppu_obj_routed_record(s, resid);
}

}  /* namespace */

extern "C" {

void *port_oam_render_routed_8(int draw, void *obj, int px, int py, int pal,
                               int prio, int scale, int rot)
{
    const int adj = routed_adjust(py);
    if (!adj)
        return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(draw, obj, px, py, pal,
                                                       prio, scale, rot);
    const int c0 = oam_slot_count();
    void *r = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(draw, obj, px, py + adj,
                                                      pal, prio, scale, rot);
    const int c1 = oam_slot_count();
    if (c1 != c0) {
        routed_mark_range(c0, c1, 0);
        routed_split(py, adj, 0);
        return r;
    }
    r = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(draw, obj, px, py, pal, prio,
                                                scale, rot);
    const int c2 = oam_slot_count();
    routed_mark_range(c0, c2, adj);
    if (c2 != c0) routed_split(py, adj, 1);
    return r;
}

void *port_oam_render_routed_mtx(int draw, void *obj, int px, int py, int pal,
                                 int prio, void *mtx)
{
    const int adj = routed_adjust(py);
    if (!adj)
        return _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(draw, obj, px, py, pal,
                                                        prio, mtx);
    const int c0 = oam_slot_count();
    void *r = _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(draw, obj, px, py + adj,
                                                       pal, prio, mtx);
    const int c1 = oam_slot_count();
    if (c1 != c0) {
        routed_mark_range(c0, c1, 0);
        routed_split(py, adj, 0);
        return r;
    }
    r = _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(draw, obj, px, py, pal, prio,
                                                 mtx);
    const int c2 = oam_slot_count();
    routed_mark_range(c0, c2, adj);
    if (c2 != c0) routed_split(py, adj, 1);
    return r;
}

void *port_oam_render_routed_10(int draw, void *obj, int px, int py, int pal,
                                int prio, int sx, int sy, int a8, int a9)
{
    const int adj = routed_adjust(py);
    if (!adj)
        return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(draw, obj, px, py,
                                                           pal, prio, sx, sy,
                                                           a8, a9);
    const int c0 = oam_slot_count();
    void *r = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(draw, obj, px,
                                                          py + adj, pal, prio,
                                                          sx, sy, a8, a9);
    const int c1 = oam_slot_count();
    if (c1 != c0) {
        routed_mark_range(c0, c1, 0);
        routed_split(py, adj, 0);
        return r;
    }
    r = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(draw, obj, px, py, pal,
                                                    prio, sx, sy, a8, a9);
    const int c2 = oam_slot_count();
    routed_mark_range(c0, c2, adj);
    if (c2 != c0) routed_split(py, adj, 1);
    return r;
}

/* THE FIVE WRAPPERS. Each carries the real name every caller in the program
   spells, captures the world row its body is about to route, and hands the call
   straight to the ROM body under its renamed symbol. Nothing else: no test, no
   branch of the router's repeated here, and with the mode off the whole of this
   is a push and a pop. */

int func_ov004_020aff38(void *a0, int a1, int a2, int a3, int a4, int a5,
                        int a6)
{
    RoutedCall rc(a2);
    return func_ov004_020aff38__rom(a0, a1, a2, a3, a4, a5, a6);
}

void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4)
{
    RoutedCall rc(a2);
    func_ov004_020afdd0__rom(a0, a1, a2, a3, a4);
}

void func_ov004_020b023c(void *a0, int a1, int a2, int a3, void *a4)
{
    RoutedCall rc(a2);
    func_ov004_020b023c__rom(a0, a1, a2, a3, a4);
}

void func_ov004_020b0380(void *a0, int a1, int a2, void *a3)
{
    RoutedCall rc(a2);
    func_ov004_020b0380__rom(a0, a1, a2, a3);
}

void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5)
{
    RoutedCall rc(a2);
    RenderOamBothScreens__rom(a0, a1, a2, a3, a4, a5);
}

}  /* extern "C" */
