/* settings_live: the in-game settings engine (run menu1, lane SETLIVE1).

   One small API under the settings page: get, set, save, by id. Every set
   changes the running game at once (the frame loops re-copy their working
   values when host_settings_ctl_gen moves) and queues the settings.json key,
   saved at most every half second while a slider is dragged and always before
   exit (sl_flush). The launcher's changes land live too: host_settings_poll
   re-reads these keys when the file changes.

   Values are plain ints so a page can drive every row the same way:
     keys / pads   the Win32 virtual-key code / XInput mask (0 = unbound)
     enums         the index into names[]
     bools         0 / 1
     percents      the number the slider shows
   SL_MINIMAP is the minimap size in percent (100 = 1.0x).

   Defaults reproduce today's feel exactly: StickDeadzone 24 is the shipped
   7849 of 32767, RunThreshold 89 is the ROM's own point (no remap), the two
   sensitivities at 100 and InvertCameraY off change nothing.

   RunThreshold's curve: the stick's travel past the dead zone is cut at the
   player's point T. Below it the magnitude runs 0..0xe46 linearly (walk speed
   is linear in the magnitude, so walking is stretched or squeezed to fill the
   ROM's walk range); from T up it runs 0xe47..0x1000 linearly, so the ROM's
   own run test (>= 0xe47, src/func_ov002_020d4748.cpp) trips exactly at T.
   (Single player. The multiplayer fold uses T itself as the run point.) */
#ifndef PORT_SETTINGS_LIVE_H
#define PORT_SETTINGS_LIVE_H

#include "host_settings.h"   /* HOST_KEY_COUNT, HOST_PAD_COUNT: the id bases below follow them */

enum SlId {
    /* 0..HOST_KEY_COUNT-1: HOST_KEY_* (keyboard), then HOST_PAD_COUNT pads: HOST_PAD_*. The bases follow the host enums, so a new
       host key or pad shifts every id after it together with the INFO[] table, whose row count settings_live.cpp checks. */
    SL_KEY_BASE = 0,
    SL_PAD_BASE = HOST_KEY_COUNT,
    SL_MOUSE_BASE = HOST_KEY_COUNT + HOST_PAD_COUNT,   /* CAMPACK1: HOST_MOUSE_* (mouse buttons) */
    SL_RUN_MODE = HOST_KEY_COUNT + HOST_PAD_COUNT + HOST_MOUSE_COUNT, /* enum: button, analog, auto */
    SL_STICK_DEADZONE,          /* percent 0..50, default 24 */
    SL_RUN_THRESHOLD,           /* percent 30..100, default 89 */
    SL_CAMERA_MODE,             /* enum: analog, freecam, ds */
    SL_SWAP_CAMERA_TURN,        /* bool */
    SL_INVERT_CAMERA_Y,         /* bool */
    SL_CAMERA_SENSITIVITY,      /* percent 10..400, default 100 */
    SL_MOUSE_SENSITIVITY,       /* percent 10..400, default 100 */
    SL_MOUSE_CAPTURE,           /* bool */
    SL_CAMERA_STICK_ZOOM,       /* bool: right stick up / down zooms (CAMPACK1) */
    SL_VOLUME,                  /* percent 0..100 */
    SL_MINIMAP_SCALE,           /* percent of 1.0x */
    SL_IMPROVED_MINIMAP,        /* bool, read only: latched with the geometry */
    SL_MINIGAME_GAP,            /* bool: the DS hinge shown between the screens in a minigame (PAUSE3) */
    SL_GAP_LOOK,                /* enum: solid, blur, peek */
    SL_GAP_COLOR,               /* enum: a few preset colours for the solid look */
    SL_DRAW_DISTANCE,           /* percent of the ROM's draw distance: 100 (default), 150, 200, 400, 10000 (max); the page shows stops */
    SL_FRAME_RATE,              /* int: 0 native, 60..240; live (FPSLIVE1): the in-game page shows it as a drop-down of stops */
    SL_DEBUG_MENU,              /* bool: the F5 debug menu may open (CHARSLOT1); off by default */
    SL_CHAT_SCALE,              /* percent: 75 / 100 / 125 / 150 / 200; live (CHATSCALE1); the in-game page shows it as a drop-down */
    SL_CHARS_FROM_START,        /* bool: custom characters open from the start, without the white door unlock (CHARSET1); off by default */
    SL_CHARACTER_STRIP,         /* bool: the star select's character strip (CAPSTOGGLE1); on by default, off = the DS's caps row */
    SL_COINS_MIDDLE,            /* bool: HUDMENU2 H10, the coin counter at the top middle like SM64; off by default (the DS puts it beside the stars) */
    SL_KEEP_LIVES,        /* bool: KeepLives; off by default (SHELVED1) */
    SL_ONLINE_EVENTS,           /* bool: take the host's events in Adventure Online (EVENTS2); on by default */
    SL_FIX_ORIGINAL_BUGS,       /* bool: FixOriginalBugs; on by default (RABBIT1) */
    SL_WIDE_DRAWING,            /* bool: WideDrawingPage; on by default (DOODLEWIDE1) */
    SL_PEACH_JINGLE,            /* bool: PeachJingle; on by default (PEACHJINGLE1) */
    SL_COUNT
};

enum SlKind { SL_KIND_KEY, SL_KIND_PAD, SL_KIND_ENUM, SL_KIND_BOOL, SL_KIND_PERCENT, SL_KIND_MOUSE };

struct SlInfo {
    const char *key;            /* the settings.json key */
    SlKind kind;
    int min, max, def;          /* range and default (keys: 0..255 / pad mask) */
    const char *const *names;   /* enum spellings, else 0 */
    int live;                   /* 1 = applies at once, 0 = needs a restart */
};

/* NULL outside 0..SL_COUNT-1 */
const SlInfo *sl_info(int id);
/* the id of the row with this settings.json key, -1 if this build has none (a page asks by name for a row another lane may add) */
int sl_find(const char *key);

int sl_get(int id);
/* 1 when accepted and applied, 0 when refused (range, read-only, reserved) */
int sl_set(int id, int value);

/* 0 when the setting has no effect in the current state, so the page can grey
   it: camera sensitivity, invert-Y and mouse sensitivity do nothing in the DS
   camera mode (the ROM steps a fixed 0x400 per tick); the dead zone is the
   stick's whatever the run mode is; RunThreshold only matters in analog run
   mode. */
int sl_applicable(int id);

/* Write anything still queued (mouse-up on a slider, closing the page). */
int sl_flush(void);

/* "" or the reason the last sl_set was refused (Start cannot lose both its key
   and its button, or the pause could not open) */
const char *sl_set_refused(void);

/* ---- rebinding: the run-rebind capture, for any binding --------------------
   sl_capture_start arms a capture for a KEY id (waits for a keyboard press) or
   a PAD id (waits for a pad button or trigger pull). Escape, or sl_capture_cancel,
   backs out. Keys the window owns (escape, F1..F12, tab, the arrows for any
   action but the four arrow slots, enter for any action but Start) and any
   key/button already bound to ANOTHER action are refused: the capture stays
   armed and sl_capture_refused says why. The pad's d-pad, BACK and the right
   stick's click are reserved (the menu and the camera mode toggle); A and B
   bind to the gameplay actions but not to Start / Select. */
enum { SL_CAP_IDLE = 0, SL_CAP_WAITING, SL_CAP_BOUND, SL_CAP_CANCELLED };
int sl_capture_start(int id);
void sl_capture_cancel(void);
int sl_capture_state(void);       /* SL_CAP_*; BOUND / CANCELLED read once, then IDLE */
int sl_capture_id(void);          /* the id being captured, -1 if none */
const char *sl_capture_refused(void);   /* "" or the last refusal's reason */

/* the old F5 run row: the first key OR pad button pressed becomes the run key
   or pad */
int sl_capture_start_run(void);

/* for the window code (tests/walk_window.cpp) -- not for the page */
int sl_capture_take_arm(void);    /* 1 once after a start: arm g_rebind_capture */
int sl_capture_take_cancel(void); /* 1 once after a cancel: disarm it */
int sl_capture_offer_key(int vk); /* a key was pressed; 1 = capture finished */
int sl_capture_offer_pad(unsigned bit);
int sl_capture_offer_mouse(int vk);   /* CAMPACK1: a mouse button (VK 2 / 4 / 5 / 6) was pressed during a mouse-row capture; 1 = capture finished */
int sl_capture_wants_mouse(void);     /* 1 while a MOUSE row waits (the page must not treat a click as 'cancel') */
int sl_capture_active(void);

#endif
