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
 * NOTHING HERE TOUCHES THE SIMULATION, with ONE named exception at the foot of
 * this header. The game's own G is read and never written by the layout, the
 * fills, the art or the peek; the actors move exactly as they did, and all four
 * of those decide how many rows of IMAGE sit between the halves and what goes
 * in them and nothing else. The exception is hal_gapless_minigames_latch,
 * which is the opt-in GaplessMinigames mod, is off by default, and says so out
 * loud every time a minigame starts -- read its block before assuming the
 * sentence above is still absolute.
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

/* ---- GaplessMinigames, THE ONE THING IN THIS FEATURE THAT IS A MOD ---------
 *
 * Everything else here reads the game's G and never writes it, which is what
 * lets the header above open with "NOTHING HERE TOUCHES THE SIMULATION". This
 * function is the exception and it is deliberately the only one: when the
 * player has set GaplessMinigames and the scene is one this port has PROVEN
 * gapless, it writes ZERO into the game's own word, and from that moment the
 * ROM's own OAM router behaves as if the two screens were adjacent -- an object
 * leaves the bottom screen's top row and is on the top screen's bottom row on
 * the next frame, with no hidden rows in between.
 *
 * CALLED FROM EVERY SEATED MINIGAME'S InitResources THUNK, immediately after
 * the real body returns, and that placement is the whole safety argument. A
 * minigame's InitResources is where its G is set (Bob-omb Squad's setter call
 * is the last statement before the field writes at the tail of
 * src/_ZN15dScMgPachinko_c13InitResourcesEv.cpp), so writing zero any earlier
 * is overwritten and writing it later leaves a window in which something can
 * bake a coordinate off the old value. hal/screen_gap.cpp carries the audit of
 * every G reader that is linked into this program and which of them cache.
 *
 * "WHEN ABLE" IS A TABLE, and it is a table rather than a policy for the
 * honest reason: only a scene whose G consumers have actually been enumerated
 * and whose gapless behaviour has been measured is in it. Every other minigame
 * keeps the DS's behaviour with the setting on, and says so.
 *
 * IT ALWAYS SAYS WHICH OF THE THREE IT DID -- engaged, unsupported, or off --
 * one line per minigame latch, because "I ticked the box and nothing happened"
 * has to be answerable from a log rather than from a guess. */
void hal_gapless_minigames_latch(void);

/* 1 when the mode is on AND it engaged for the scene now running: the
 * program's own answer to "is this run the ROM's timing or not". */
int hal_gapless_engaged(void);
unsigned hal_gapless_oam_src_a(void);

/* The bandless modes. VISUAL: the band leaves the picture, nothing else.
   SPLICE (hal_gapless_world): additionally, an object entering the hidden
   hinge rows is carried straight across them -- call hal_gapless_splice once
   per behavior tick from the scene's own thunk; it is a no-op unless the
   running scene is an engaged FULL row. hal_gapless_splice_hops counts the
   carries, for run reports. */
int hal_gapless_visual(void);
int hal_gapless_world(void);
void hal_gapless_splice(void);
unsigned hal_gapless_splice_hops(void);

/* ---- THE OBJECT SHIFT, the display half of the mod above -------------------
 *
 * THE DS ROWS THE TOP ENGINE'S OBJ LAYER IS DRAWN LOWER THAN THE ENGINE PUTS
 * IT, which is G_rom, the value the running minigame's own InitResources wrote
 * before hal_gapless_minigames_latch stored zero over it.
 *
 * ZERO UNLESS SM64DS_GAPLESS_OBJ_SHIFT=1 ASKS FOR IT, and nothing asks. Zeroing
 * G moves only the sprites the framework's own OAM router placed, and this
 * moves ALL of them, so with it on the score rows and the top screen's artwork
 * go 32 rows down a screen they were never displaced from. hal/screen_gap.cpp's
 * obj_shift_ds carries the measurement and names what a correct version needs;
 * ntr/ppu.h carries the geometry under THE OBJECT SHIFT, and
 * port/tools/objshift.py is how both were measured.
 *
 * TWO READERS, and they must agree within a frame: hal/message_compositor.cpp
 * shifts engine A's OBJ raster by it, and hal_screen_layout sizes the band from
 * it so the rows a shifted sprite is pushed into exist in the image.
 *
 * WHAT THAT SENTENCE MEANS NOW. There are two mechanisms behind it and the
 * three functions below say which is which:
 *
 *   hal_gapless_obj_shift_ds        the BAND's world rows. Non-zero whenever
 *                                   either mechanism is live, because either
 *                                   way the image needs the same G_rom rows
 *                                   between the halves. This is what the
 *                                   layout is told.
 *   hal_gapless_obj_raster_shift_ds the LAYER shift, the falsified one, zero
 *                                   unless SM64DS_GAPLESS_OBJ_SHIFT=1.
 *   hal_gapless_per_entry_ds        the PER ENTRY correction, which is the
 *                                   default, and SM64DS_GAPLESS_PER_ENTRY=0
 *                                   is its kill switch. Applied at the five
 *                                   routers' own calls; see THE ROUTER HOOK at
 *                                   the foot of hal/screen_gap.cpp.
 *
 * The last two are never both non-zero, so an A/B is one mechanism against the
 * other rather than one on top of the other. */
int hal_gapless_obj_shift_ds(void);
int hal_gapless_obj_raster_shift_ds(void);
int hal_gapless_per_entry_ds(void);

#endif /* PORT_SCREEN_GAP_H */
