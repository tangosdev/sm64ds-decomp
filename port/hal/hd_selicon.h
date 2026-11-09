/* hd_selicon.h -- the star select's icons as a mod's art (run after093, lane HDSELICON1).

   Three pieces of the star select's upper screen (scene 4, dScStarSel_c) are drawn from a mod's redrawn art when an enabled mod carries
   it, and from the cartridge's own tiles otherwise (no mod = the 1.0 picture exactly):
     - the star next to the star total (engine A OBJ, 16 x 16, cmn_icon tile 158 palette 6, bottom right);
     - the coin next to the coin record (engine A OBJ, 16 x 16 cell with 14 x 16 of coin, cmn_icon tile 154 palette 0);
     - the course medal (engine A BG2, scenario_player_select/course_number_bg*.ncg, one per language; courses 1 to 15 only, the
       Bowser and secret stages show the koopa background instead and keep it).
   Where, how big, the fade and the order against the other layers all stay the ROM's: only the pixels the icon's own DS cells show
   are replaced. hal/message_compositor.cpp finds the icons in the frame and erases their DS cells; it paints the art itself when the
   layer goes to the picture, and hal/menu_compose.cpp paints it when the one-screen recipe lifts the layer into its pieces.

   WHERE THE ART LIVES. In the "HD UI Textures" mod (any enabled mod; looked up with port_mods_ui_file, a later mod wins per file):
       hd_ui/starselect/star_counter.png          the star, drawn over the 16 x 16 sprite
       hd_ui/starselect/coin_score.png            the coin, drawn over the coin's 14 x 16 pixels
       hd_ui/starselect/course_medal_<lang>.png   lang = en, fr, de, it, es (the game language); drawn over the medal's own box
   Any size at least the DS size is accepted (a whole multiple keeps the pixels square: 8x is 128 x 128 for the star); the picture is
   box filtered to the screen's size. A picture whose shape is more than 12 percent off the DS shape is refused (the DS art stays) and
   the game says so once. A missing language keeps the DS medal. In a lobby or a match only an Online Compatible mod is on. */
#ifndef HD_SELICON_H
#define HD_SELICON_H

struct HdSelArt {
    int w, h;                      /* the image's own size */
    const unsigned char *rgba;     /* w * h * 4, straight alpha */
    int gen;                       /* a new number for every picture decoded, for the scaled-picture cache */
};

enum { HDSEL_STAR = 0, HDSEL_COIN = 1, HDSEL_MEDAL = 2, HDSEL_N = 3 };

/* one icon found in this frame: its DS rectangle, the layer it is on (2 = BG2, 4 = OBJ) and the art */
struct HdSelJob {
    int id, x, y, w, h, owner;
    int lift;                      /* 1 = the one-screen recipe lifts this layer: hal/menu_compose.cpp paints it */
    const HdSelArt *art;
};

/* the art for icon `id` when the star select is the live scene and an enabled mod carries it (the medal for the game's language, and
   only on courses 1 to 15); null otherwise. id -1 asks only whether the star select is live (non-null = yes) and lets the cache forget
   what it found when it is not, so a mod change made at the title is seen at the next star select. Game thread only. */
extern "C++" const HdSelArt *hd_selicon_art(int id);

/* hal/message_compositor.cpp: the art box filtered to W x H, as ARGB words (alpha FF = drawn, 0 = not); cached */
extern "C++" const unsigned *hd_selicon_scaled(const HdSelArt *a, int W, int H);
/* hal/message_compositor.cpp: this frame's icons whose layer the recipe lifts (for hal/menu_compose.cpp); returns how many */
extern "C++" int hd_selicon_lifted_jobs(const HdSelJob **out);

#endif
