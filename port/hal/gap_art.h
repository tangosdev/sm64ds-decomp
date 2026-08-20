/* THE SCENE'S HAND-DRAWN GAP BAND: one BMP per minigame, loaded at the latch.
 *
 * WHAT THIS IS FOR. The gap band (port/hal/screen_gap.h) is the G DS rows the
 * hardware hides behind the hinge, put back as picture. Two of the three fills
 * are arithmetic -- a flat colour, or a wash lit by the two screens around it --
 * and neither can be a drawing of the machine the minigame is played on. This
 * is the third: a picture drawn by hand for one scene, sized to that scene's own
 * band, loaded off disk beside the game.
 *
 * THE FILE CONTRACT, and it is deliberately narrow.
 *
 *   <bundle>/gap/scene_<id>.bmp        e.g. gap/scene_368.bmp for Bob-omb Squad
 *
 * <bundle> is the folder the game is launched from -- the same three candidates
 * settings.json is looked for in, in the same order, so the art sits beside the
 * settings that ask for it. <id> is the scene's own id, decimal, as the ROM
 * carries it: dScMgBase_c's `this` is data_ov004_020beb68 and its ActorBase
 * actorID is at +0xc.
 *
 *   * 24-bit or 32-bit BMP, BI_RGB (uncompressed). No palettes, no bitfields,
 *     no RLE. A 32-bit file's alpha byte is IGNORED; see the key below.
 *   * 256 x G, or 512 x 2G. The first is DS pixels, one for one. The second is
 *     the 2x sheet, and it is downsampled by NEAREST -- the top-left of each
 *     2x2 block -- because the band is drawn at an integer scale and a filter
 *     would invent pixels nobody drew, and because a nearest sample of the key
 *     colour is still exactly the key colour.
 *   * BAND ROWS ONLY. The template Tango draws in has 64 rows of top-screen
 *     context above the band and 64 below it so the art can be lined up against
 *     the real picture; those context rows are CROPPED before the file gets
 *     here. A file that still has them is the wrong height and is refused.
 *   * MAGENTA #FF00FF IS THE TRANSPARENCY KEY, exactly that value and no
 *     tolerance. A keyed pixel shows whatever is behind the art: BLACK under
 *     GapPeek, and the ambient wash otherwise. That is what makes a partly-drawn
 *     band possible -- a painted machine down the middle, the default backdrop
 *     either side of it.
 *
 * EVERY REFUSAL IS ONE LINE AND A FALLBACK, never a crash and never a guess. A
 * missing file, a wrong size, a wrong depth, a truncated file: each says so on
 * stderr once per latch and leaves the band exactly as it would have been with
 * no art at all. This is a file a player can put anything in.
 *
 * WHEN IT LOADS. At the LATCH, which is when the scene's G is first read or
 * changes -- once per minigame, not once per frame. See hal/screen_gap.cpp.
 */
#ifndef PORT_GAP_ART_H
#define PORT_GAP_ART_H

#include <cstdint>

/* The live minigame scene object, or null when no minigame is running. This is
   the ROM's own dScMgBase_c `this`: the base constructor func_ov004_020b2adc
   ends with `data_ov004_020beb68 = self`, and every ov004 body that needs the
   current scene reads it there. */
unsigned char *hal_gap_scene(void);

/* Its ActorBase actorID (+0xc), or -1 with no scene. 368 is Bob-omb Squad, 374
   Curling, 376 Slots Shot; the ids are the ROM's own and the same ones
   SM64DS_SCENE takes. */
int hal_gap_scene_id(void);

/* The band art for this scene, or null when there is none to have.
 *
 * 256 * gap_ds pixels, row major, the band's TOP row first, 0xAARRGGBB with
 * alpha 0xFF for a painted pixel and alpha 0 for the magenta key. The buffer
 * belongs to this file and stays valid until the next call with a different
 * (scene_id, gap_ds) pair, which is the next latch.
 *
 * `for_custom` says which of the two askers is asking, and it changes only what
 * a refusal SAYS: the "custom" fill mode falls back to the ambient wash, and
 * peek's backdrop falls back to black. Both get null. */
const uint32_t *hal_gap_art(int scene_id, int gap_ds, int for_custom);

#endif /* PORT_GAP_ART_H */
