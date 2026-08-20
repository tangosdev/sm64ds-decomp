/* BAND CONTINUITY: the per-scene table of objects the gap band keeps whole.
 *
 * WHAT THIS SOLVES. With GapPeek on, the band shows what the two engines really
 * submitted for those rows and nothing else, which is the honest view and also
 * a view with a hole in it: the ROM's own OAM cull drops an object whose sprite
 * sits wholly between the two screens, so a thing crossing the seam winks out
 * for the middle of its crossing. See the note over BandTrack in ntr/ppu.h for
 * the arithmetic. This file is the other half of that: per scene, WHICH objects
 * to keep drawn and WHERE THEY ARE, read out of the scene object the game is
 * ticking.
 *
 * WHAT IT DOES NOT DO. It does not draw anything and it does not know what an
 * object looks like. It reports a position and a size; ntr/ppu_sub.cpp matches
 * that against the OAM entries the engines really submitted, caches the entry
 * while one of them has it, and re-renders THAT ENTRY -- the game's own tile,
 * palette and size out of the game's own VRAM -- while neither does. An object
 * this file names but the game has never submitted is never drawn.
 *
 * ADDING A GAME IS ONE ROW AND ONE READER. The table at the bottom is scene id
 * -> reader, and a reader's whole job is to walk the scene's own array and fill
 * a BandTrack per object that reaches the band. Coincentration (378) is the
 * next one: its coins are 8x8 at G = 32, which is a 23-row dead zone by the
 * same arithmetic, and it needs a reader over its own coin array and nothing
 * else in this file changed.
 */

#include "hal/gap_continuity.h"

#include "hal/gap_art.h"
#include "ntr/ppu.h"

#include <cstdio>

namespace {

/* ---- scene 368, dScMgPachinko_c (Bob-omb Squad) ----------------------------
 *
 * THE SLINGSHOT BALL, and it is NOT the falling bob-ombs. The scene carries two
 * 0x38-stride arrays and only one of them can vanish:
 *
 *   scene+0x4660, 30 slots   the bob-ombs. Drawn through func_ov004_020b023c,
 *                            which submits to BOTH OAM buffers unconditionally
 *                            and lets each engine's own clip decide, and their
 *                            sprite is 32x32 affine at double size -- a 64x64
 *                            box, wider than the whole band. They have no dead
 *                            zone and need nothing from this file.
 *   scene+0x4ed8, 48 slots   the balls the plunger fires. Drawn through
 *                            func_ov004_020aff38, which picks ONE engine by a
 *                            band test on world y and RETURNS, and their sprite
 *                            is 16x16. This is the one with the hole.
 *
 * THE HOLE, from func_ov004_020aff38 (src/func_ov004_020aff38.cpp) and
 * OAM::Render's cull, with G = 32 and py = the ball's world y in DS pixels:
 *
 *     py in [-288, -33]   the top engine, at y = py + 0xc0 + G
 *     py in  [-64, 191]   the bottom engine, at y = py     (only if the top
 *                         test failed, because the first match returns)
 *
 * so a ball at py in [-32, -9] reaches only the bottom engine, at OAM y = py-8,
 * and OAM::Render's `if (y + h < 0) return;` throws it away for every py <= -9.
 * TWENTY-FOUR WORLD ROWS of the crossing in which the ROM draws no ball at all.
 *
 * THE SPRITE, from src/func_ov006_020fe1d0.c and the OamAttr it passes. The
 * render loop walks all 48 slots, skips any whose +0x4f0e (visible) is zero,
 * and calls func_ov004_020aff38(data_ov006_02137614, X>>12, Y>>12, -1, -1,
 * 0x1000, 0). The attr block behind that pointer is data_ov006_02137618 =
 * {248,0,248,65,20,17,255,255}: attr0 0x00F8, attr1 0x41F8, attr2 0x1114, which
 * is a 16x16 square, 4bpp, tile 276, palette 1, priority 0, and NOT affine --
 * the call passes scale 0x1000 and rotation 0, which is exactly the case
 * OAM::Render treats as a plain sprite. The attr's own y and x fields are the
 * CEL OFFSET, -8 and -8, which OAM::Render adds to the position, so the box's
 * top-left is the ball's centre minus eight in each axis.
 *
 * WHICH MAKES THE BAND ROW ARITHMETIC, and it is the same for both engines,
 * which is what lets one number be reported rather than one per engine:
 *
 *     top engine     OAM y = py + 0xc0 + G - 8, band index = that - 192
 *     bottom engine  OAM y = py - 8,            band index = that + G
 *     both           band index = py - 8 + G
 *
 * THE FIELDS ARE READ, NEVER WRITTEN. This runs inside the compose, after the
 * game has ticked, and the two words it reads are the same ones the game's own
 * render loop read a moment earlier. */
const int kPachinkoBallBase = 0x4ed8;      /* slot 0: X, +4 Y (both Fix12) */
const int kPachinkoBallStride = 0x38;
const int kPachinkoBallCount = 0x30;       /* 48, the count its render loop uses */
const int kPachinkoVisible = 0x4f0e;       /* the flag that render loop tests */

int pachinko_balls(unsigned char *scene, ntr::BandTrack *out, int max,
                   int gap_ds)
{
    int n = 0;
    for (int i = 0; i < kPachinkoBallCount && n < max; ++i) {
        const int at = i * kPachinkoBallStride;
        if (!*(unsigned char *)(scene + kPachinkoVisible + at)) continue;
        const int px = *(int *)(scene + kPachinkoBallBase + at) >> 12;
        const int py = *(int *)(scene + kPachinkoBallBase + 4 + at) >> 12;
        ntr::BandTrack &t = out[n];
        /* EVERY BALL SHARES ONE CACHE SLOT PER EIGHT, and that is exact rather
           than a compromise: all 48 slots are drawn from the SAME OamAttr
           template, so the attribute triple cached for one of them IS the
           attribute triple for any other. The fold only has to be stable, and
           the array index is. */
        t.slot = i % ntr::BAND_TRACK_MAX;
        t.x = px - 8;
        t.y = py - 8 + gap_ds;
        t.w = 16;
        t.h = 16;
        ++n;
    }
    return n;
}

/* ---- the table ------------------------------------------------------------ */
struct ContinuityRow {
    int scene_id;
    int (*read)(unsigned char *scene, ntr::BandTrack *out, int max,
                int gap_ds);
    const char *what;
};

const ContinuityRow kRows[] = {
    {368, pachinko_balls, "dScMgPachinko_c, the 48 slingshot balls at +0x4ed8"},
};

int (*g_read)(unsigned char *, ntr::BandTrack *, int, int);

/* The hook ntr calls once per peek frame. The scene pointer is re-read every
   frame rather than latched, because a scene that has ended leaves it null and
   a stale `this` would be read through. */
int continuity_hook(ntr::BandTrack *out, int max, int gap_ds)
{
    unsigned char *scene = hal_gap_scene();
    if (!scene || !g_read) return 0;
    return g_read(scene, out, max, gap_ds);
}

}  // namespace

void hal_band_continuity_latch(int scene_id)
{
    g_read = 0;
    for (unsigned i = 0; i < sizeof kRows / sizeof *kRows; ++i)
        if (kRows[i].scene_id == scene_id) {
            g_read = kRows[i].read;
            std::fprintf(stderr, "[gapcont] scene %d: band continuity on for "
                         "%s\n", scene_id, kRows[i].what);
            break;
        }
    /* INSTALLING CLEARS ntr's ATTRIBUTE CACHE, which is why this is called on
       every latch and not only when a reader is found: leaving the previous
       scene's cached OAM entry in place would let a tile from the last minigame
       be re-rendered into this one's band. Null is a clean uninstall. */
    ntr::ppu_band_continuity(g_read ? continuity_hook : 0);
}
