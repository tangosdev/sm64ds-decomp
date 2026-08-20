/* The screen gap's layout owner. See port/hal/screen_gap.h for what the gap is
   and why the layout is one struct rather than five copies of a subtraction. */

#include "hal/screen_gap.h"

#include "hal/gap_art.h"
#include "hal/gap_continuity.h"
#include "hal/host_settings.h"

#include <cstdio>

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
unsigned g_gen;              /* steps whenever the layout's shape changes */
ntr::StackLayout g_lay;

int read_raw(void)
{
    return (int)((unsigned)data_ov004_020beb6c[0] |
                 ((unsigned)data_ov004_020beb6c[1] << 8) |
                 ((unsigned)data_ov004_020beb6c[2] << 16) |
                 ((unsigned)data_ov004_020beb6c[3] << 24));
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
    /* THE SCENE IS PART OF THE LATCH, and it has to be: two minigames with the
       same G give the same LAYOUT and not the same BAND. 368 and 374 are both
       G = 32, and the art and the continuity reader are per scene, so latching
       on G alone would have left Curling wearing Bob-omb Squad's picture. */
    const int scene = hal_gap_scene_id();
    if (g_have && want == g_raw && scene == g_scene) return &g_lay;

    const int was_h = g_have ? g_lay.h : 0;
    g_raw = want;
    g_scene = scene;

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

    g_lay = ntr::stack_layout(want, mode, host_setting_gap_color(), peek, art);
    /* and the band's per-scene continuity reader, installed at the same moment
       for the same reason: it is per scene, and installing clears the cached
       OAM attributes so nothing crosses from the last minigame into this one */
    hal_band_continuity_latch(scene);
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
