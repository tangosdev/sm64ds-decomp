/* THE DS SCREEN GAP: one layout, computed once, read by everybody.
 *
 * WHAT THE GAP IS. A DS's two panels have a hinge between them, and Super Mario
 * 64 DS's minigame framework knows it. ov004 carries one word,
 * data_ov004_020beb6c, that each minigame sets at InitResources to the number of
 * DS rows of hinge to pretend are there -- 32 for most of them, 48 for two, 16
 * for the trampoline family, 80 for the snowball -- and RenderOamBothScreens
 * submits the top engine's sprites at `a2 + 0xc0 + that word`. So the world a
 * minigame simulates is 192 + G + 192 rows tall, and the middle G of them are
 * behind plastic on the hardware and were behind NOTHING in this port: the
 * stacked presentation put the two screens edge to edge, which squeezed those
 * rows out and made anything crossing between the screens jump.
 *
 * This is the seam put back. A band of G DS rows is drawn between the halves,
 * filled with a colour, with the light of the two pictures around it, or with a
 * picture drawn by hand for that one minigame (hal/gap_art.h), and optionally
 * with the sprites that are genuinely inside it -- including the ones the ROM's
 * own OAM cull drops for the middle of a crossing (hal/gap_continuity.cpp).
 *
 * NOTHING HERE TOUCHES THE SIMULATION. The game's own G is read and never
 * written; the actors move exactly as they did. This decides how many rows of
 * IMAGE sit between the halves and what goes in them, and that is all.
 *
 * WHY ONE STRUCT. Before the gap, "the bottom screen starts one screen height
 * down" was true by construction, so the blit and the stylus inverse could each
 * spell it and never disagree. With a band between them that sentence is wrong
 * by the band's height, and a display and a touch transform that disagree by 96
 * client pixels is a bug a player reports as "the buttons moved". So there is
 * ONE computation, ntr::stack_layout, fed by this file, and the compose, the
 * DIB header, the window size, the BMP capture and both mappers all read its
 * fields rather than doing the arithmetic again.
 */
#ifndef PORT_SCREEN_GAP_H
#define PORT_SCREEN_GAP_H

#include "ntr/ppu.h"

/* THE LAYOUT, live. Recomputed when the game's G changes or when the settings
 * are first read, and cached otherwise; the pointer is stable for the life of
 * the program and its contents are what the last refresh decided.
 *
 * Safe before anything has booted: with no ov004 loaded the word reads 0, which
 * is the gapless layout -- exactly the geometry this program had before the gap
 * existed. */
const ntr::StackLayout *hal_screen_layout(void);

/* A counter that steps every time the layout's SHAPE changes -- its height, in
 * practice, which is the only field a consumer outside the compose has to react
 * to. walk_window watches it to know when to re-size the window and re-shape the
 * DIB header; anything that only reads the layout per frame can ignore it. */
unsigned hal_screen_layout_generation(void);

/* The game's own G, before the MinigameGap setting is applied: the raw
 * data_ov004_020beb6c. For diagnostics -- the layout is the thing to use. */
int hal_screen_gap_raw(void);

#endif /* PORT_SCREEN_GAP_H */
