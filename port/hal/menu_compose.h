/* menu_compose.h -- the one-screen menu compose layer (run menu1, lane P0CORE).
 *
 * WHAT THIS IS. A general, ROM-free layer that takes pieces of the two DS
 * screens (top 2D layers, the top picture, the bottom screen), places them on
 * ONE picture of any size and aspect, draws them, and turns a host click or a
 * pad press back into the DS touch the ROM already understands. The
 * level-clear save prompt (SaveMenuOnTop, hal/sub_screen.cpp swap_*) is the
 * proof of concept this generalises; nothing here calls into that code and
 * nothing in the frame calls this yet (phase 1 wires four hooks, see
 * runs/menu1/in/DESIGN.md section D).
 *
 * THE UNIT. One canvas unit is one DS pixel scaled by h / 192, kept as the
 * reduced fraction unum / uden (as swap_geom_for does) so the draw and the
 * inverse are handed small numbers. canvas_w = w * 192 / h (floored, only used
 * to pick the narrow or wide variant): 256 at 4:3, 307 at 16:10, 341 at 16:9,
 * 448 at 21:9, 683 at 32:9. The canvas is 192 units tall and spans the whole
 * picture; placement uses the exact w * uden / unum units wide, not canvas_w.
 *
 * SCALE RULE. A piece is s.w * knum / kden units wide (kden/knum from the
 * allowed steps 1, 5/6, 3/4, 2/3, 1/2), exactly, and both its edges are the floor of
 * the exact host position (menu_layout_dest), so its dest size is within one
 * host pixel of the exact size and pieces that meet on a canvas edge share a
 * host edge. A piece that overruns the picture by no more than one canvas unit
 * plus a pixel (rounding of an odd size such as 1022x767) is clamped to it.
 * A recipe builds a NARROW layout below MENU_WIDE_CANVAS (300) and a WIDE one
 * at or above it; wide spreads through anchors and offsets, it never stretches.
 *
 * COORDINATES. A rect in a *source* is DS pixels of that source (DsRect). A
 * piece's destination is an integer HOST rect in dest[i] = {x, y, w, h}; the
 * draw and the inverse both read it (one arithmetic): host X inside the rect
 * maps to DS x = s.x + (X - x0) * s.w / dw, floored.
 *
 * PIXELS are 0xAARRGGBB; alpha 0 is "nothing here".
 */
#ifndef MENU_COMPOSE_H
#define MENU_COMPOSE_H

#include <stdint.h>
#include <math.h>

#define MENU_WIDE_CANVAS 300      /* canvas_w at or above this takes the wide variant */
#define MENU_MAX_PIECES  24
#define MENU_MAX_HOTSPOTS 24

/* layer bits for MenuPiece.layers and the owner byte of a MenuTopCell:
   bit 0..3 = BG0..BG3, bit 4 = OBJ. (The brief wrote OBJ=4 as an index; as a
   MASK it is bit 4 so BG3 and OBJ never collide.) */
enum { MENU_L_BG0 = 1, MENU_L_BG1 = 2, MENU_L_BG2 = 4, MENU_L_BG3 = 8, MENU_L_OBJ = 16,
       MENU_L_ALL = 31 };

enum MenuSrc   { SRC_TOP_2D,      /* engine-A cells lifted out by layer owner, kept in DS space */
                 SRC_TOP_PIC,     /* the tick framebuffer (3D + 2D) at host resolution */
                 SRC_TOP_HOSTROW, /* legacy: lc_ov host-space band, moved by rows only */
                 SRC_SUB,         /* engine-B scan-out with a layer mask */
                 SRC_HOSTIMG };   /* ASSET2: a host picture (hal/menu_hostimg.h) scaled to the dest rect at host resolution */

enum { MENU_AX_L = 0, MENU_AX_C = 1, MENU_AX_R = 2 };
enum { MENU_AY_T = 0, MENU_AY_M = 1, MENU_AY_B = 2 };

struct DsRect  { short x, y, w, h; };               /* DS 256x192 coordinates */

struct MenuPiece {
  MenuSrc src; uint8_t layers;                      /* bits as MENU_L_* */
  DsRect s; uint8_t autocrop;                       /* shrink s to the opaque bbox of `layers`, latched at open */
  uint8_t key_backdrop;                             /* drop engine-B backdrop pixels */
  int (*row_ok)(int sy);                            /* optional row filter (menu_plate_row today); nonzero = draw the row */
  uint8_t ax, ay; short dx, dy;                     /* anchor (L/C/R, T/M/B) + offset in canvas units (DS px) */
  uint8_t knum, kden;                               /* scale (1/1 = native DS size); allowed 1, 5/6, 3/4, 2/3, 1/2 */
  uint8_t touch;                                    /* clicks on it map through to a DS touch (SRC_SUB only) */
  uint8_t bg;                                       /* P0CORE: a backdrop piece; exempt from the no-overlap rule */
  /* HOOK1 (appended): LEGACY FIXED PLACEMENT, for the level-clear plates, whose placement and sampling are
     swap_geom_for's own rational (pnum/pden DS pixels per host pixel, not one of the allowed steps). When
     lden > 0 the recipe set the dest rect itself in HOST pixels (fx, fy, fw, fh), the anchors and the scale
     are ignored, and a dest pixel (xx, yy) samples the source at s.x + floor(xx * lden / lnum) (clamped to
     s), draw and inverse alike. Zero in every other piece. */
  short fx, fy, fw, fh; int lnum, lden;
  /* ASSET1 (appended): THE 3D LAYER AS ONE ELEMENT AND A FILL.
     keycover: an SRC_TOP_PIC piece draws only the pixels the 3D engine wrote (MenuSources.pic_cover), so a sprite
     object the ROM submitted as 3D quads (the title's plates, wrench, head, stars) arrives with its own
     transparency and drags no background along. The ROM's draw filter (src/func_ov007_020c421c.c port arm) is what
     keeps the objects that should not be in the picture (the wallpaper) out of it.
     xfill: an SRC_SUB piece whose dest is fill_w canvas units wide (not s.w) and repeats source column s.x across
     it, rows mapped normally: the sky gradient's edge carried out to the edge of a wide picture, nothing stretched
     (a column of a vertical gradient is the same everywhere). */
  uint8_t keycover, xfill; short fill_w;
  /* oam_on: an SRC_SUB piece (layers should be MENU_L_OBJ) draws only engine-B OAM entries oam_lo..oam_hi (inclusive,
     0..127): one sprite, or a set, alone, with its own transparency (alpha 0 where the entry drew nothing, as a
     key_backdrop piece reads it). Select by index; an index that churns between scenes is the recipe's to re-read. */
  uint8_t oam_on, oam_lo, oam_hi;
  /* ASSET2 (appended): HOST-RESOLUTION ELEMENTS. Both are placed by the fixed host rect (fx, fy, fw, fh with lnum = lden = 1),
     the recipe's own arithmetic, so a size is whatever the window is, never a DS-pixel multiple.
     hostgrad: an SRC_SUB piece that draws a SMOOTH vertical gradient over its dest rect instead of copying pixels. The colours
     are the engine-B plane's own (columns s.x .. s.x + s.w - 1 of the piece's layers, averaged per DS row, so a banded or
     dithered DS gradient becomes its mean ramp); runs of equal rows become one stop at their centre and the ramp is linear
     between stops, evaluated per HOST row with a 4x4 ordered dither of one 8-bit level, so no step is visible at any size.
     It also publishes the frame's fade factor (sky luminance now over the brightest seen on this page) for hostimg pieces.
     hostimg: an SRC_HOSTIMG piece; the id is a MENU_IMG_* of menu_hostimg.h. Drawn premultiplied over what is already there,
     scaled by the fade factor (the picture fades with the engine-B screen it replaces). */
  uint8_t hostgrad, hostimg;
  /* hl: a hostimg plank's own look, since the ROM's 3D plate that used to show it is not drawn. bit 0 = selected (brightened,
     pulsing on the ROM's 8-frame cadence), bit 1 = pressed (sunk and darkened). The compose also treats the piece as
     selected while it owns the mouse focus, and pressed while the real stylus is held on it. A touch hostimg piece maps a
     click proportionally into its source rect s (the ROM's own box), not 1:1. */
  uint8_t hl;
  /* ASSET4 (appended): nodraw = a touch-only piece (maps clicks, draws nothing). has_ex / ex = a keycover piece skips the 3D pixels
     inside the DS source rect ex (so a whole-layer piece never contains another piece's element, here the wrench). */
  uint8_t nodraw, has_ex; DsRect ex;
  /* ASSET5 (appended): THE TITLE CHOREOGRAPHY.
     alpha: 0 = opaque; 1..254 = a hostimg or keycover piece is drawn at alpha/255 (the logo's fades, the face's fade-in).
     nofade: a hostimg piece ignores the engine-B sky fade factor (the sky is no longer engine B's).
     wall: an SRC_HOSTIMG piece that is the WALLPAPER (dest = the fixed host rect fx, fy, fw, fh): the DS lower screen's own gradient
     (measured from the ROM's render, a straight ramp top to bottom, dithered) with the "Super Mario 64 PC" pattern tiled over it.
     The pattern tile is wtw x wth host pixels, its origin wox, woy from the dest rect's corner; wink is the ink's opacity (0..255).
     The finished wallpaper is cached per size and copied per frame.
     iris: a piece that blackens everything OUTSIDE a five-pointed star (the title's star iris, drawn clean at the window's resolution
     with an anti-aliased edge): centre icx, icy in host pixels, outer radius ir16 in 1/16 host pixels (0 = all black). */
  uint8_t alpha, nofade, wall, iris, wink;
  short wtw, wth, wox, woy;
  int icx, icy, ir16;
  /* FACE1 (appended): fixdest = the recipe set the dest rect in HOST pixels (fx, fy, fw, fh) and the source rect s is stretched over it exactly
     (draw and inverse as for any scaled piece, no lden rational): an arbitrary, per-frame scale, which the allowed steps cannot give. The
     face follows the ROM head object's own lerp in the press transition with it, so the head stays where the main menu drew it. */
  uint8_t fixdest;
  /* FACE2 (appended): paper = an SRC_HOSTIMG piece that is the drawing page's canvas paper (the DS title's paper texture, a 32 x 32 tile
     measured off the ROM's own render, repeated edge to edge from the 256 x 192 area's corner and smoothed at window resolution), drawn
     over the whole fixed host rect (fx, fy, fw, fh) at the piece's alpha. snap = an SRC_HOSTIMG piece that draws the last saved snapshot of
     the composed picture (MenuLayout.snap_save) over the whole rect at the piece's alpha: the held frame a crossfade starts from or ends on.
     has_ex2 / ex2 = a second skipped rect for a keycover piece (as has_ex / ex). */
  uint8_t paper, snap, has_ex2;
  DsRect ex2;
  /* FACE3 (appended): paper == 2 is the green paper of the drawing page's free-draw state (the same 32 x 32 layout, its own six greens);
     has_ex3 / ex3 = a third skipped rect for a keycover piece (the ink bar). */
  uint8_t has_ex3;
  DsRect ex3;
  /* PAUSE3 (appended): custom = the recipe draws this piece itself. menu_piece_draw calls custom(L, piece index, dst, w, h, stride) in piece order
     and nothing else is drawn for it; the callback reads only the layout (L->facts, L->w / L->h) and the picture, so it stays on the draw side of
     the decide / draw contract. A custom piece has no source, no touch and no hotspot of its own. */
  void (*custom)(const struct MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride);
  /* ARROWSLOW1 (appended): bob = a fixed host rect piece that bobs (the main menu's cursor arrow). Its dest y is drawn as
     bob_y0 + bob_amp * menu_bob_curve(bob_t - 1 + a) / 4 host pixels, a = the picture's place between the tick before and this tick
     (1 on the tick's own picture), so every picture between ticks shows its own place. fy holds the tick's own (a = 1). */
  uint8_t bob;
  short bob_y0;
  float bob_amp;
  int bob_t;
  /* FLIPSMOOTH1 (appended): turn = a plank turning over about its centre (the title's online planks, menu_online_plank_phase): 1 = its face
     before the turn's middle, 2 = its face after. turn_a = the turn's phase on this tick in ticks (0 .. 2 * turn_half), turn_fw = the face's
     full width in host pixels. A picture between ticks draws it at phase p = turn_a - 1 + a: width |1 - p / turn_half| of turn_fw (40 permille
     at least, the tick's own law) about the tick rect's centre, and only while p is on its face's side of the middle. The tick's own picture
     (a = 1) is the tick's rect, untouched; a face-1 piece on a face-2 tick (the old face, added for the pictures before the middle) is hidden. */
  uint8_t turn;
  uint8_t turn_half;
  short turn_fw;
  float turn_a;
};
/* ARROWSLOW1: the cursor arrow's bob in half units (0..4), one cycle every 24 ticks (a third of the ROM's 8-tick pace), eased (a cosine); ph in ticks, fractional
   between ticks */
static inline double menu_bob_curve(double ph)
{
    return 2.0 - 2.0 * cos(ph * (6.283185307179586 / 24.0));
}

struct MenuHotspot { uint8_t piece; DsRect box;     /* bottom-screen DS box = the ROM's own touch box */
                     int8_t rom_index;              /* >=0 ROM cursor index, -1 touch only */
                     int8_t nav[4]; };              /* indexed by MENU_DIR_*: -2 automatic spatial navigation,
                                                       -1 blocked, >=0 hotspot index to go to */

enum { MENU_DIR_UP = 0, MENU_DIR_DOWN = 1, MENU_DIR_LEFT = 2, MENU_DIR_RIGHT = 3 };

enum { MENU_BACKDROP_TOP_AS_IS = 0,   /* pause: the ROM already dims */
       MENU_BACKDROP_DIM_AUTO  = 1,   /* DS arithmetic evy 8 to black unless the ROM already dims */
       MENU_BACKDROP_TOP_FULL  = 2 }; /* title: the top screen is the artwork */

enum { MENU_STAGE_PRE_FADE = 0, MENU_STAGE_POST_FADE = 1 };

/* hal_menu_frame_begin's scene_id when the host's frame gate is closed (the stacked layout): no recipe
   may match, the fallback included. */
#define MENU_SCENE_GATED (-2)

struct MenuRecipe;

/* MENUSPLIT1: the ROM values a recipe's build() may depend on, read ONCE per tick by the recipe's own facts
   function (MenuRecipe.facts) on the logic thread inside the decide step, and carried in MenuLayout.facts. build()
   reads these and nothing else of the ROM; so the draw thread can re-solve a layout (the picture changed size under
   the frame) without touching ROM memory. A field is a raw copy of the ROM value, named for the recipe that reads it.
   Zero = "the ROM said 0 / nothing", which is what the selftests (no game state) get. Append only. */
struct MenuFacts {
  int16_t language;         /* pause: GetOwnerLanguage() (the OK boxes' width differs in German / Italian) */
  uint8_t pause_f2f0;       /* pause: data_0209f2f0, 1 = no card pages, the ROM hides the course arrows */
  uint8_t starsel_valid;    /* star select: the live scene object answered (scene 4) */
  uint8_t starsel_n;        /* star select: +0x114, the icon count */
  uint8_t starsel_mask;     /* star select: +0x131, which icons are available */
  uint8_t starsel_x[8];     /* star select: +0x11a.., the icon x bytes */
  uint8_t starsel_head_y;   /* star select: +0x12b, the Mario head's centre row (it bobs) */
  int32_t spare[8];         /* recipes added later */
  uint8_t blob[1024];        /* INT2: a recipe's own plain-data facts (the title and the Rec Room keep a struct here); zero = nothing read */
};

struct MenuLayout { int w, h, unum, uden, canvas_w; /* unit = h/192 reduced; canvas_w = w*192/h */
                    MenuPiece p[MENU_MAX_PIECES]; int np; MenuHotspot hs[MENU_MAX_HOTSPOTS]; int nh;
                    int dest[MENU_MAX_PIECES][4]; int focus; uint8_t lift_owners; uint8_t stage; uint8_t backdrop;
                    /* lift_owners is a MASK of MENU_L_* layer bits: phase 1 lifts an engine-A cell into
                       the top2d plane when (lift_owners >> owner) & 1, where MenuTopCell.owner is the layer
                       INDEX 0..4 (BG0..BG3, OBJ). MenuPiece.layers is a mask in the same bit order. */
                    /* P0CORE additions (never removed, only appended) */
                    int8_t back;                  /* hotspot the B / Backspace key presses, -1 none */
                    int variant;                  /* what build() was asked for */
                    const MenuRecipe *recipe;
                    /* HOOK1 (appended): engine-A layers (MENU_L_* bits) the compositor drops from the picture
                       and draws nowhere (level-clear: BG2, "TOUCH TO SELECT") */
                    uint8_t hide_owners;
                    /* MENUSPLIT1 (appended): this tick's ROM facts for build(); preserved by menu_layout_begin */
                    MenuFacts facts;

                    /* ASSET3 (appended): the recipe shows its own selection (arrows, a lit plank), so the generic focus outline
                       of a hovered or pad-focused hotspot is not painted over it */
                    uint8_t no_focus_paint;

                    /* FACE2 (appended): save the composed picture after this frame's pieces are drawn (the snapshot a snap piece draws) */
                    uint8_t snap_save;
                    uint8_t key_sentinel;
                    /* LOGOFADE1 (appended): glide_on = piece glide_piece (placed in host pixels, lden) glides between ticks: a picture between ticks
                       draws it a of the way (hal_menu_set_relift_alpha) from glide_from, its rect on the tick before, to its own fx, fy, fw, fh */
                    uint8_t glide_on; uint8_t glide_piece; short glide_from[4];
                    int wz; };                     /* MENUS093E: permille the page behind the touch piece is zoomed by about the canvas centre (a wide window's file pages), 0 = none: a touch that is not on a hotspot is the point of the ZOOMED page, so the water's ripple (the ROM places it at the touch) lands under the pointer */       /* MENUS093E: key_backdrop pieces key a sentinel backdrop, not palette entry 0 (see hal_menu_key_sentinel) */

enum { MENU_DRV_NATIVE = 0, MENU_DRV_ROM_CURSOR = 1, MENU_DRV_TOUCH = 2, MENU_DRV_MIXED = 3 };
enum { MENU_KEY_NONE = 0, MENU_KEY_SAVE_MENU_ON_TOP = 1, MENU_KEY_ONE_SCREEN_MENUS = 2 };
enum { MENU_RF_FALLBACK = 1, MENU_RF_START_CONFIRM = 2, MENU_RF_PAD_B_BACK = 4 };   /* START_CONFIRM: the Start key (Enter) confirms like the jump binding does (POLISH1A: the Rec Room); PAD_B_BACK: the pad's B (the DS A key bit 0x0001, free on this page) is the host back (PADMENUS1) */

struct MenuRecipe {
  const char *name;
  int  (*up)(int scene_id);                 /* 0 = not up; else a stable variant id (hysteresis inside) */
  void (*build)(int variant, MenuLayout *); /* pieces + hotspots for this variant and L->canvas_w */
  int  (*rom_cursor)(int dir, int *sel);    /* optional ROM-cursor driver; returns 1 if it moved */
  uint8_t enable_key;                       /* MENU_KEY_* : SaveMenuOnTop or OneScreenMenus */
  uint8_t driver;                           /* MENU_DRV_* */
  /* P0CORE additions */
  uint8_t flags;                            /* MENU_RF_* */
  uint8_t nvariants;                        /* variant ids 1..nvariants build something; 0 = stub (selftest skips) */
  /* HOOK1 (appended): the pad gate. Non-null and returning 0 means "the ROM is not reading the menu
     right now": hal_menu_pad then touches nothing (no strip, no A -> Start). Called once a frame while the
     recipe is live, so it is also where a ROM_CURSOR driver forgets its cursor when the menu goes away. */
  int  (*pad_ok)(void);
  /* MENUSPLIT1 (appended): fills the ROM facts build() needs. Called once per tick by the decide step (logic thread),
     only for the recipe that was picked. Null = the recipe's build() reads no ROM (lc, the fallback, the test row). */
  void (*facts)(MenuFacts *f);
  /* TITLE1 (appended): a native recipe's pad translation. Non-null: hal_menu_pad hands it the pressed word
     and the host back flag and returns (no focus, no synthesised back touch, no strip). It exists for a
     ROM that reads the pad itself but with a different key meaning than the port's (the title's confirm
     is the DS A key, 0x0001; the port's confirm is 0x0002). */
  void (*pad_map)(uint16_t *pressed, int host_back);
};

/* the level-clear recipe's functions (menu_recipe_lc.cpp), named by the table */
int  menu_lc_up(int scene_id);
void menu_lc_build(int variant, MenuLayout *L);
int  menu_lc_rom_cursor(int dir, int *sel);
int  menu_lc_pad_ok(void);

/* the title family's recipe (menu_recipe_title.cpp), run menu1 lane TITLE1 */
int  menu_title_up(int scene_id);
void menu_title_build(int variant, MenuLayout *L);
void menu_title_pad_map(uint16_t *pressed, int host_back);
void menu_title_facts(MenuFacts *f);
#define MENU_TITLE_NVARIANTS 7
#define MENU_TITLE_V_DOODLE 5            /* the drawing page's variant id (menu_recipe_title.cpp V_DOODLE) */

/* the ONE static table (menu_recipes.cpp) */
extern const MenuRecipe g_menu_recipes[];
extern const int        g_menu_nrecipes;
#define MENU_SCENE_TEST (-77)               /* the TEST recipe is up for this scene id only */
#define MENU_SCENE_TEST_CROP (-78)          /* the TEST recipe, variant 2 (its top band autocrops) */

/* ---- the planes the real buffers arrive in (phase 1 fills these) ---------
   All pointers are borrowed for the frame. A null plane means "that source is
   not available this frame": its pieces draw nothing and say so in the trace. */
struct MenuTopCell { uint32_t argb; uint8_t bm; uint8_t owner; uint8_t pad[2]; };

struct MenuSources {
  const uint32_t *sub;            /* SRC_SUB: 256x192 ARGB engine-B scan-out (already engine-B bright) */
  uint32_t sub_backdrop;          /* the engine-B backdrop colour, RGB compared, for key_backdrop */
  uint32_t sub_backdrop_masked;   /* MENUS093E: the colour a masked plane starts undrawn pixels from while bd_masked_on (the live plane keeps sub_backdrop) */
  int bd_masked_on;
  const MenuTopCell *top2d;       /* SRC_TOP_2D: 256x192 cells, row-major; owner is a layer INDEX 0..4 */
  const uint32_t *pic;            /* SRC_TOP_PIC: host framebuffer, pic_stride pixels per row */
  int pic_stride, pic_w, pic_h;
  int pic_rx, pic_ry, pic_rw, pic_rh;  /* where the 256x192 top screen sits inside pic (host px) */
  const uint8_t *pic_cover;       /* ASSET1: the 3D coverage plane of the same frame, same stride as pic; null = no key */
  const uint32_t *hostrow;        /* SRC_TOP_HOSTROW: host plane, same stride as the destination; s is in HOST pixels */
  int hostrow_stride, hostrow_w, hostrow_h;
  uint32_t (*bm_apply)(uint32_t argb, uint8_t bm);  /* engine-A fade/brightness verdict, null = identity */
  int rom_dimming;                /* the ROM is already dimming the game (BACKDROP_DIM_AUTO then draws nothing) */
  /* HOOK1 (appended): the engine-B scan-out for a piece's layer mask (MENU_L_* bits, BG0..BG3, OBJ=bit 4),
     already engine-B bright, 256x192 ARGB; null = use `sub`. The host makes at most two extra scan-outs a
     frame and reuses the live one when the mask equals the live enables. */
  const uint32_t *(*sub_for)(uint8_t layers);
  /* ASSET1: the same with an engine-B OAM index range (inclusive); counts against the same two-scan-out cap */
  const uint32_t *(*sub_for_oam)(uint8_t layers, int lo, int hi);
};

/* HOOK1: what the engine-A compositor must lift this frame (H1b). Null when nothing is lifted. */
struct MenuLiftInfo {
  uint8_t lift2d;                 /* owner bits sent to the DS-space top2d plane instead of the picture */
  uint8_t hide;                   /* owner bits drawn nowhere */
  int nbands;                     /* host-row bands, SRC_TOP_HOSTROW pieces: owner bits in layers, rows [r0, r1) */
  struct { uint8_t layers; int r0, r1; } band[8];   /* SAVE2: eight (the new save layout lifts and hides six) */
};

/* MENUSPLIT1: ONE TICK'S MENU DECISION. A plain-old-data record (no pointers into ROM memory, only into the static
   recipe table and static row filters), made by hal_menu_decide on the logic thread and consumed by the draw side. It is
   copied by value into the frame packet (hal_menu_decision_copy) and bound on the draw thread
   (hal_menu_draw_frame_begin). About 3 KB. */
struct MenuDecision {
  unsigned serial;            /* decide count, 1 up; 0 = never decided */
  int live;                   /* a recipe (or the fallback) owns this tick's picture; the rest is valid only when set */
  int recipe, variant;        /* g_menu_recipes index, the recipe's variant id */
  int fallback;               /* the fallback recipe is up */
  int w, h;                   /* the picture size the layout was solved for */
  int focus_shown;            /* the pad / hover cursor is showing (input state as of the decide) */
  uint16_t latched;           /* bit i: piece i's autocrop rect is latched and already applied to L.p[i].s */
  int lift_on;                /* the compositor must lift per `lift` this tick */
  MenuLiftInfo lift;
  int real_held;              /* INT2: a real stylus is down (the draw sinks a pressed plank from this, not from the input state) */
  MenuLayout L;               /* solved for (w, h), latches applied, L.facts = the ROM facts of this tick */
};

/* ---- the core (C++), also what the selftest drives ---------------------- */
void menu_layout_begin(MenuLayout *L, int w, int h);                 /* zero it, unit + canvas_w */
void menu_layout_dest(MenuLayout *L);                                /* piece dest rects from anchors */
void menu_layout_solve(MenuLayout *L, const MenuRecipe *r, int variant, int w, int h);
int  menu_scale_allowed(int knum, int kden);
int  menu_add_piece(MenuLayout *L, const MenuPiece &p);              /* returns the index or -1 */
int  menu_add_hotspot(MenuLayout *L, int piece, int bx, int by, int bw, int bh, int rom_index);
MenuPiece menu_piece_make(MenuSrc src, int sx, int sy, int sw, int sh, int ax, int ay, int dx, int dy,
                          int knum, int kden);
int  menu_hit(const MenuLayout *L, int bx, int by, int *piece, int *dsx, int *dsy);   /* topmost touch piece, 1 = inside */
void menu_inverse(const MenuLayout *L, int bx, int by, int *drag_piece, int *dsx, int *dsy, int *in);
int  menu_hotspot_rect(const MenuLayout *L, int hs, int r[4]);      /* host rect, 0 if empty */
void menu_hotspot_centre(const MenuLayout *L, int hs, int *hx, int *hy);
int  menu_focus_move(MenuLayout *L, int dir);                        /* returns the new focus */
int  menu_focus_hover(MenuLayout *L, int bx, int by);                /* 1 if a hotspot is under the point */
void menu_piece_draw(const MenuLayout *L, int i, const MenuSources *src, uint32_t *dst, int w, int h);
void menu_highlight_paint(const MenuLayout *L, uint32_t *dst, int w, int h, unsigned frame);
int  menu_seam_census(const MenuLayout *L, int piece, const MenuSources *src, int counts[4]);
int  menu_autocrop_rect(const MenuPiece *p, const MenuSources *src, DsRect *out);
void menu_dim(uint32_t *dst, int w, int h, int evy);
int  menu_wide(const MenuLayout *L);                                 /* canvas_w >= MENU_WIDE_CANVAS */
int  menu_selftest(void);                                            /* 0 = all PASS */
/* ASSET5: 1 when the star of the iris piece (outer radius R host pixels, centre cx, cy) contains the whole w x h picture
   (the star is star-shaped about its centre, so it contains the picture exactly when it contains the picture's border) */
int  menu_iris_covers(int w, int h, float cx, float cy, float R);

#ifdef __cplusplus
extern "C" {
#endif
/* ---- what phase 1 calls ---------------------------------------------------*/

/* THE DECIDE / DRAW CONTRACT (MENUSPLIT1, run menu1; the render-thread work RENDERTHREAD1 depends on it)
 *
 *   LOGIC THREAD (owns every read of ROM memory, the live scene object, the pad, the mouse):
 *     hal_menu_decide(w, h, scene_id)   once per tick, AFTER the ROM tick, BEFORE the frame packet is handed off.
 *       Picks the recipe and variant (MenuRecipe.up, hysteresis), reads the recipe's ROM facts (MenuRecipe.facts),
 *       solves the layout for the picture size (w, h) (MenuRecipe.build sees only L->facts), carries the cursor, applies
 *       the autocrop latches, and builds the lift plan. Publishes the result as a MenuDecision and as the input-side
 *       record of "the layout the player saw".
 *     hal_menu_decision_copy(&d)        the decision to put in the frame packet (a value copy).
 *     hal_menu_live()                   the latest decision is live (the input side, and hunt8's "compose this tick on
 *                                       the CPU" switch). Never reads draw state.
 *     hal_menu_pad, hal_menu_hover, hal_menu_touch_inverse, the synthesised-touch queue: read the latest decision's
 *       layout (hal_menu_touch_inverse: the layout last committed, plus the draw's autocrop / resize amendments), never
 *       the draw's working copy.
 *
 *   DRAW THREAD (today the same thread, right after the decide; the render thread later):
 *     hal_menu_draw_frame_begin(&d)     bind this frame's decision (null = take the in-process one). Once, at the head of
 *                                       the 2D compose (H1).
 *     hal_menu_draw_live(), hal_menu_draws_in(stage), hal_menu_uses_src(src), hal_menu_lift_info(),
 *     hal_menu_set_sources(), hal_menu_present(dst, w, h, stage): read ONLY the bound decision and the frame's planes.
 *       THE FRAME'S PLANES are the frame packet's job, not the decision's: the engine-B scan-outs (VRAM / OAM / palette /
 *       engine registers), the lifted top2d / hostrow planes, the picture copy and present rect, the engine-A fade state,
 *       the engine-B backdrop colour. They are hardware state, not ROM program memory, and the render thread snapshots them
 *       with the rest of the frame. NO ROM address, ROM function or scene object is read on this side.
 *
 *   THE FEEDBACK from the draw to the logic thread is small, locked and layout-only: an autocrop rect latched the first
 *   frame a piece has pixels (applied by the next decide); an amendment of the seen layout when the draw had to re-solve
 *   for a different picture size or latched a rect; and an ack that a NEW menu's first picture is up (until then the
 *   input does not invert through the new layout: a click never reaches a button the player has not seen, nor goes through
 *   the previous menu's rects). Nothing else flows back: no draw state is ever read by the input.
 *
 *   hal_menu_frame_begin(w, h, scene_id) = hal_menu_decide + hal_menu_draw_frame_begin(null): what a one-thread build
 *   calls at H1, unchanged for callers that predate the split.
 *
 *   SM64DS_MENU_DRAW_LATE=1 (debug): the draw binds the PREVIOUS tick's decision (the render thread's lateness, simulated
 *   on one thread). A tick whose decision differs from the previous one (a menu opening, closing or changing variant or
 *   size) draws no menu and lifts nothing, so the picture is the plain one, never a lift without its draw. */
void hal_menu_decide(int w, int h, int scene_id);          /* logic thread */
unsigned hal_menu_decision_copy(MenuDecision *out);        /* logic thread; returns the serial (0 = nothing decided yet) */
void hal_menu_draw_frame_begin(const MenuDecision *d);     /* draw thread; null = this process's latest (or previous, DRAW_LATE) */
int  hal_menu_draw_live(void);                  /* the bound decision is live */
/* Once per DS frame at H1, before anything is lifted: decide + bind. Picks the recipe, builds the layout for (w, h). */
void hal_menu_frame_begin(int w, int h, int scene_id);
int  hal_menu_live(void);                       /* the latest decision: a recipe (or the fallback) owns the picture */
int  hal_menu_draws_in(int stage);                /* bound decision live AND the recipe draws at this stage */
void hal_menu_set_sources(const MenuSources *s);
/* H2. Draws the pieces whose layout stage matches `stage` from the bound decision and the planes. */
void hal_menu_present(uint32_t *dst, int w, int h, int stage);
/* H3. Last frame's DRAWN layout. in = 1 only inside a touch piece. */
void hal_menu_touch_inverse(int bx, int by, int *dsx, int *dsy, int *in);
int  hal_menu_hover(int bx, int by);            /* mouse hover sets focus and shows the cursor; 1 if over a hotspot */
void hal_menu_touch_end(void);                  /* the stylus came up: forget the drag piece */
/* H4. `pressed` is one PadData pressed word; the recipe's driver decides what is consumed.
   `host_back` is a HOST flag, not a pad bit: phase 1 passes 1 on the frame Backspace or the bound
   B-equivalent key goes down. The ROM has no pad "back" at all (every ROM back control is a touch
   box) and the DS A key (0x0001) is a ROM confirm on the title (IsButtonInputValid mask 0xf; the
   title Behavior tests pressed & 9), so no pad bit can carry it. When the live recipe names a back
   hotspot (MenuLayout.back) the flag queues a synthesised touch at that hotspot's DS centre; with
   no back hotspot it does nothing. Bit 0x0001 is neither read nor stripped here.

   PHASE 1 MUST (both reviewer findings, REVP0CORE 2 and 6):
   - The ROM reads the pressed bits in TWO words: data_020a0e58 + slot*4 + 2 and the split
     spelling data_020a0e5a + slot*4 (IsButtonInputValid reads only the split one, see
     src/IsButtonInputValid.c). hal_lc_menu_pad (hal/sub_screen.cpp) writes BOTH and uses the
     local slot data_020a0e40. The hook must do the same: k = *pressed; hal_menu_pad(&k, back);
     *pressed = k; *split = k. Calling with only one word updated lets the ROM see the old bits.
   - Skip the call in a netplay transport session, exactly as hal_lc_menu_pad is skipped
     (!b5_transport, walk_window.cpp near :13744): a pad rewrite in a session desyncs. A session's
     menus are mouse/touch only; hal_menu_set_netplay stays as a second guard (ROM_CURSOR and MIXED
     then act as TOUCH).
   - The held word (data_020a0e58 + 0) is not touched by this call, so a TOUCH menu still sees
     held direction bits there; only the pressed bits are rewritten. */
void hal_menu_pad(uint16_t *pressed, int host_back);
/* settings seam: key -> enabled. Default (no function set) = everything off. */
void hal_menu_set_enable_fn(int (*fn)(int key));
int  hal_menu_hover_box(int *x, int *y, unsigned *stamp);   /* ASSET3: the hovered hotspot's DS box origin; stamp changes per hover event */
int  hal_menu_focus_set(int hs);      /* PAUSE3: show the pad cursor on hotspot hs of the live layout (a touch-only page starts on its safe choice); 1 when done */
int  hal_menu_focus_shown(int *hs);   /* PAUSE3: 1 and the hotspot index when the pad / hover cursor is showing on a hotspot of the live layout */
int  hal_menu_last_size(int *w, int *h);   /* PAUSE2: the picture size of this tick's decide (a recipe's idle warm-up asks for it); 0 before the first decide */
void hal_menu_set_netplay(int on);              /* in a transport session ROM_CURSOR is refused; TOUCH is used */
/* the synthesised-touch queue: single producer (hal_menu_pad), single consumer (poll_touch) */
int  hal_menu_touch_queue_press(int dsx, int dsy);  /* 1 queued; 0 refused (in flight or a real stylus is held) */
int  hal_menu_touch_queue_poll(int *dsx, int *dsy, int *down); /* once per poll; 1 = drive the stylus: down 1 held, 0 the release edge */
void hal_menu_touch_real_held(int held);        /* poll_touch tells the queue the real stylus state each poll */
int  hal_menu_fallback_frames(void);
/* HOOK1 */
const MenuLiftInfo *hal_menu_lift_info(void);   /* this frame's lift plan, null = lift nothing */
int  hal_menu_uses_src(int src);                /* the live layout has a piece of this MenuSrc */
int  hal_menu_interp_mode(void);                /* MENUFOLD1: SM64DS_MENU_INTERP 0 snap / 1 default (MENUINTERP2: re-lift per picture) / 2 proof instrument (every menu tick blends, as the tick's pieces) / 3 MENUFOLD1's gate (snap when a piece takes 3D pixels) */
int  hal_menu_relift_why(void);                 /* MENUINTERP2: 0 = ok, else 1 mode/fallback, 2 stage or the drawing page's held frame (snap_save) */
void hal_menu_relift_dump(void);                /* MENUINTERP2 debug: one trace line of the bound layout's pieces */
void hal_menu_present_relift(uint32_t *dst, int w, int h, int stage);   /* MENUINTERP2: hal_menu_present for a picture between ticks */
void hal_menu_set_relift_alpha(double a);      /* ARROWSLOW1: the next re-draws between ticks are a of the way from the tick before to this tick (bob pieces) */
float hal_menu_piece_bob_frac(int piece);       /* ARROWSLOW1: the picture being drawn shifts this bob piece down by this fraction of a pixel (0..1) */
int  hal_menu_key_sentinel(void);               /* MENUS093E: the bound layout wants its masked engine-B planes drawn over a sentinel backdrop (exact key_backdrop) */
int  hal_menu_needs_picsnap(void);              /* MENUINTERP2: the tick must keep the picture as the menu found it for a re-draw between ticks */
int  hal_menu_relift_ok(void);                  /* MENUINTERP2: a tick the gate snaps may still blend if every in-between re-draws the menu on its own 3D picture (hal_menu_relift) */
int  hal_menu_interp_ok(void);                  /* MENUFOLD1: the pictures between ticks may run under this tick's menu (no 3D-sourced piece); 1 when no menu is bound */
int  hal_menu_uses_cover(void);                 /* ASSET1: the live layout has a keycover piece (the host then supplies pic_cover) */
void hal_menu_note_cost(double prep_us);       /* ASSET1: the host adds its source-preparation time for the frame */
int  hal_menu_enabled(int key);                 /* ASSET1: a MENU_KEY_* setting as the recipes see it */
int  hal_menu_trace_on(void);                   /* SM64DS_MENU_TRACE or its alias SM64DS_SWAP_TRACE */
void hal_menu_set_dst_stride(int stride);       /* pixels per row of the buffer hal_menu_present draws into; 0 = w */
void hal_menu_set_layer_mode(int on);           /* SAVEOVL1: the buffer is a menu layer (premultiplied alpha kept) */
void hal_menu_set_layer2(unsigned *p);            /* SAVEOVL1: the layer's second plane */
long long hal_menu_layer_conflicts(void);       /* SAVEOVL1: pixels the layer could not hold since layer mode was set on */
int  hal_menu_layer_ok(void);                   /* SAVEOVL1: the bound recipe may be drawn into a menu layer */
void hal_menu_note_lift_drawn(void);            /* the draw honoured the lift promise (LIFT WITHOUT DRAW net) */
int  hal_menu_lc_selftest(void);                /* menu_recipe_lc.cpp: COMPOSED LAYOUT SELFTEST, 0 = PASS */
/* EXTRAS093 (menu cursor sound): every menu surface asks for the ROM's own cursor sound here (the one the star select and the pause
   already played on a pad move, NCS_SE_SYS id 0x12e through func_02012790). Many asks inside one frame make ONE sound, played by
   the flush the two frame loops call once a frame on the logic thread, so a hover and a pad move, or a recipe's own call and the
   generic one, can never double. A surface whose ROM code already plays its own cursor sound (the title, which the ROM drives
   itself) does not call this. */
void host_menu_cursor_sound(void);
void host_menu_cursor_sound_flush(void);
/* FIELDUI1 (U2): a surface with its own ROM cursor sound (the title family, whose ROM plays its own D-pad move sound) installs a hook
   here: the flush asks it first and a nonzero answer means it played the sound (the generic 0x12e is then not played). */
extern int (*g_menu_cursor_snd_hook)(void);
#ifdef __cplusplus
}
#endif

/* pad bits as hal_lc_menu_pad uses them (hal/sub_screen.cpp): DS key word, PadData pressed */
#define MENU_PAD_CONFIRM 0x0002  /* the port's jump binding (pad A, Space, Enter), cited at hal_lc_menu_pad */
/* there is deliberately no MENU_PAD_BACK: 0x0001 is the DS A key (the port's punch), a ROM confirm
   on the title. Back is the host_back argument of hal_menu_pad. */
#define MENU_PAD_START   0x0008
#define MENU_PAD_RIGHT   0x0010
#define MENU_PAD_LEFT    0x0020
#define MENU_PAD_UP      0x0040
#define MENU_PAD_DOWN    0x0080

#endif
