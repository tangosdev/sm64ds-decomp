/* hal/pad_keyboard.h: the letter grid plank, typing with a controller (run after093, lane LOBBYKEYS1).

   A field opened with the pad (the lobby page's chat line and server name, the online file select's ROOM CODE plank) pops this plank up: the stick
   or the D-pad moves a highlight over the keys, A types the key, B deletes one letter (B on an empty field closes the grid), START sends, and the
   grid's own BACK key closes it. The mouse can click its keys too. Keyboard typing into the field is untouched: the grid only adds a way in.

   ONE GRID, TWO PAGES. The grid's keys are laid out in DS units, a 256 x 192 frame that each page scales to its picture height and centres
   (the online menu's own boxes are placed the same way), so the lobby page (its own surface) and the file select (a custom piece of the title
   recipe) draw the same plank and map a click the same way. The logic side (pad_keyboard.cpp) has no drawing and no device reads: each page
   hands it the pad's words and gets back what to do with its field; the draw side (pad_keyboard_draw.cpp) only draws a PkbView. */
#ifndef PAD_KEYBOARD_H
#define PAD_KEYBOARD_H
#include <stdint.h>

enum PkbKind { PKB_TEXT = 0, PKB_CODE = 1 };      /* letters both cases, digits, space and punctuation / the room code alphabet only */
enum PkbOk { PKB_OK_SEND = 0, PKB_OK_OK = 1, PKB_OK_JOIN = 2 };   /* the label of the submit key */
enum PkbEvent { PKB_NONE = 0, PKB_TYPE, PKB_DELETE, PKB_SUBMIT, PKB_CLOSE };

/* the pad for one frame, in the grid's own words (each page translates its device bits) */
struct PkbPad {
    unsigned dirs;        /* held: 1 up, 2 down, 4 left, 8 right */
    int a, b, start, y;   /* edges (the frame the button went down) */
};

/* what the draw side needs (the title carries kind, key and caps through its facts blob) */
struct PkbView {
    int kind, ok, key, caps;
    const char *text;     /* the field's text so far */
    int caret;            /* 1 while the caret shows */
    int max_len;
};

/* the plank's box in DS units */
#define PKB_PANEL_X 8
#define PKB_PANEL_W 240
#define PKB_PANEL_BOTTOM 188

int  pkb_open(int owner, int kind, int ok, int max_len);      /* owner: any non-zero id of the page; returns 1 */
void pkb_close(void);
int  pkb_is_open(void);
int  pkb_owner(void);                                         /* 0 when closed */
int  pkb_kind(void);
int  pkb_key(void);                                           /* the highlighted key */
int  pkb_caps(void);
void pkb_view(PkbView *v, const char *text, int caret);       /* the live grid as a view */

/* one frame of the pad. len = the field's length now. Returns a PkbEvent; PKB_TYPE puts the typed character in *ch. */
int  pkb_pad(const PkbPad *in, int len, char *ch);
/* a click (or a stylus tap) at (x, y) in DS units: PKB_NONE when it missed every key (inside the panel or not: see pkb_in_panel) */
int  pkb_click(float x, float y, int len, char *ch);
int  pkb_in_panel(float x, float y);
void pkb_hover(float x, float y);                             /* the mouse over a key highlights it */

/* the layout, shared by the logic and the draw side */
void pkb_panel_box(int kind, int *x, int *y, int *w, int *h); /* DS units */
int  pkb_key_count(int kind);
/* key i: its box in DS units and its label (caps applied); returns 0 for an index out of range */
int  pkb_key_box(int kind, int i, float *x, float *y, float *w, float *h);
const char *pkb_key_label(int kind, int ok, int i, int caps, char *buf, int cap);
void pkb_text_box(int kind, float *x, float *y, float *w, float *h);   /* the text line at the top of the plank */

/* the draw side (pad_keyboard_draw.cpp; walk_window only): the plank onto a surface, (x0, y0) = where DS (0, 0) lands, s = pixels per DS unit */
void pkb_draw(uint32_t *px, int stride, int w, int h, float x0, float y0, float s, const PkbView *v);
#endif
