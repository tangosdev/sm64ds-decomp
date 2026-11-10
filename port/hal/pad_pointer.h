/* pad_pointer.h: the PAD POINTER (run hunt8, lane PADMENUS1), a virtual stylus driven by the controller.

   The DS's bottom screen is touch only: every minigame, the drawing pages and the ROM's own two-screen menus read the stylus and nothing else.
   This gives a controller player the same input as an INPUT MAPPING on top of the game, never new game logic: the right stick (and, in a
   minigame, the D-pad) moves a small arrow over the touch screen, the confirm button (A by default, the jump binding) is the pen going down
   and up. What the ROM sees is exactly what a mouse click on that spot would have made: TouchInfo through poll_touch (hal/sub_screen.cpp).

   Two spaces, because the picture has two shapes:
     STACKED  both DS screens one above the other (minigames, the ROM's menus with OneScreenMenus off). The pointer lives in DS bottom-screen
              pixels, so the touch it makes is exact and independent of window size.
     MENU     a one-screen menu recipe is live (OneScreenMenus on): the touch pieces are cut from the sub screen and re-placed, so the pointer
              lives in picture pixels and the touch goes through the same inverse the mouse uses (hal_menu_touch_inverse).
   INSET    a scene that shows the bottom screen as a corner panel (the Rec Room with OneScreenMenus off): DS pixels, the arrow drawn inside the panel.
   Anywhere else (a course, no picture to point at) it does nothing.

   When it is on: in a minigame as soon as a controller is answering; everywhere else only once the right stick has been pushed, and it hands
   the buttons back (A goes to the game again) as soon as the D-pad or the left stick is used. While it is on, A and (in a minigame) the D-pad
   do not reach the game: they are the pointer's. Inert with no controller. */
#ifndef HAL_PAD_POINTER_H
#define HAL_PAD_POINTER_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct PadPtrIn {
    int pad_live;            /* a controller answered this frame */
    int rx, ry;              /* right stick, +-32767, Y positive up */
    int lx, ly;              /* left stick, same */
    int dpad;                /* bit0 up, bit1 down, bit2 left, bit3 right */
    int confirm;             /* the confirm button is down (the jump binding) */
    int frame;               /* the host frame number (proof traces) */
} PadPtrIn;

typedef struct PadPtrOut {
    int eat_confirm;         /* clear the confirm button before the game reads it */
    int eat_dpad;            /* clear the D-pad before the game reads it */
} PadPtrOut;

/* once per scene-loop frame, logic thread, after the pad poll and the scripted pads */
void hal_padptr_frame(const PadPtrIn *in, PadPtrOut *out);

/* poll_touch's question: is the pen down this poll, and where. STACKED: DS bottom-screen pixels. MENU: picture pixels (the caller runs
   hal_menu_touch_inverse). INSET: DS pixels. Returns 0 when it is not down, and then x / y are untouched. *mode = 1 stacked, 2 menu, 3 inset. */
int  hal_padptr_poll(int *mode, int *x, int *y);

/* the arrow (and the first-use hint), drawn into the finished picture. Stacked: the stacked image (stride = layout width). Menu: the
   one-screen picture (w x h, stride in pixels). No-ops while the pointer is off. */
void hal_padptr_draw_stacked(unsigned int *img, int w, int h);
void hal_padptr_draw_menu(unsigned int *dst, int w, int h, int stride);
void hal_padptr_draw_inset(unsigned int *dst, int w, int h, int stride);   /* the corner panel of a non-stacked scene (the ROM's own Rec Room with OneScreenMenus off) */

/* poll_touch saw the real pen (a mouse press) on the touch screen: the pointer steps aside until the pad is used again */
void hal_padptr_mouse_down(void);

/* 1 while the pointer is on (for the proof traces and the pad seam that must not double a confirm) */
int  hal_padptr_on(void);

/* LOBBYKEYS1: 1 while the controller's own confirm (the pad's jump binding) is down this frame; a key or mouse binding never sets it */
int  hal_padptr_pad_confirm(void);

#ifdef __cplusplus
}
#endif
#endif
