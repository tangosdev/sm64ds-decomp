/* menu_hostimg.h -- a host-resolution picture for the one-screen menus (run menu1, lane ASSET2).
 *
 * The first user is the "Super Mario 64 PC" logo of the title main menu. The PNG is decoded once (stb_image, the
 * reader hdtex and the minimap already use), premultiplied, and scaled with a separable triangle/area filter to the
 * exact destination size, cached per size. Pixels are PREMULTIPLIED 0xAARRGGBB so a draw is dst = src + dst * (255 - a) / 255
 * and the colour of a transparent texel can never bleed into an edge.
 *
 * WHERE THE FILE COMES FROM, first hit wins (run hunt8, lane UIMOD1: the port carries no picture bytes):
 *   SM64DS_MENU_LOGO=<path>        outright; a path that cannot be read means "no logo" (the DS logo is then drawn)
 *   SM64DS_MENU_ART_DIR=<folder>   a dev folder with the same names
 *   hd_ui/menu/<name>.png          the "HD UI Textures" mod (ui_art.h, any enabled mod; later wins)
 * Nothing here can fail loudly: no picture = menu_hostimg_ready() is 0 and the recipe keeps the DS logo layer. */
#ifndef MENU_HOSTIMG_H
#define MENU_HOSTIMG_H
#include <stdint.h>
enum { MENU_IMG_LOGO = 1, MENU_IMG_BTN_MP = 2, MENU_IMG_BTN_ADV = 3, MENU_IMG_BTN_REC = 4, MENU_IMG_ARROW_UP = 5, MENU_IMG_ARROW_DOWN = 6, MENU_IMG_PATTERN = 7, MENU_IMG_STAR = 8,
       MENU_IMG_BTN_SAVE_CONT = 9, MENU_IMG_BTN_SAVE_QUIT = 10, MENU_IMG_BTN_CONT = 11,   /* SAVE2: the level-clear planks */
       MENU_IMG_ARROW_ORANGE = 12,   /* POLISH1A: the file pages' orange cursor arrow (HD, replaces the ROM file_menu_U one) */
       MENU_IMG_GO_CLICK = 13,       /* HDART1: thunderit's "Click to select" (the DS "Touch to select" text) for the game over screen */
       MENU_IMG_GO_GRAD = 14,        /* HDART1: thunderit's smooth HD gradient for the game over screen */
       MENU_IMG_GO_L0 = 15,          /* HDART2: thunderit's eight GAME OVER glyphs (G A M E O V E R), ids 15..22, one picture each */
       MENU_IMG_GO_L7 = 22,
       MENU_IMG_COUNT = 23 };
/* PAUSE2 (appended): extension ids 32..63. Their picture list lives with the lane that owns the art (hal/menu_pause_art.cpp answers
   menu_ext_art_src), so adding art never touches this file's table. menu_hostimg_is_plank: the ids the compose lights when selected
   and sinks when pressed (the original planks and every extension plank). */
#define MENU_IMG_EXT_BASE 32
#define MENU_IMG_EXT_END 64
#ifdef __cplusplus
extern "C" {
#endif
int menu_ext_art_src(int id, const char **file);   /* 1 when id is an extension picture; file = its name under menu/ in the mod ("pause3/pk_resume.png") */
int menu_ext_art_is_plank(int id);
int menu_hostimg_is_plank(int id);
int menu_hostimg_ready(int id);                                   /* 1 once the picture decoded */
int menu_hostimg_size(int id, int *w, int *h);                    /* source pixel size */
const uint32_t *menu_hostimg_scaled(int id, int dw, int dh);      /* premultiplied ARGB, dw*dh, cached; null on failure */
unsigned menu_hostimg_generation(void);                          /* FLIPSMOOTH1: bumped by every art reset (port_menu_hostimg_hd_ui_reset); a copy kept elsewhere is stale once it moves */
int menu_hostimg_prefetch(int id, int dw, int dh);               /* PAUSEFIX1: 1 when cached at that size; else queues a background decode + scale (once) and returns 0, or -1 when 4 jobs already wait */
int menu_hostimg_prefetch_poll(void);                            /* PAUSEFIX1: installs finished background jobs (cheap); 1 while any is still queued or running */
int menu_hostimg_predecode(int id);                              /* MENUPACE1: queues a background decode alone (no scale); 1 when already decoded, 0 queued, -1 queue full */
#ifdef __cplusplus
}
#endif
#endif
