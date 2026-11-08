/* hal/lobby_page.h: the in-game lobby page (run after093, lane LOBBYUI1; Tango's 11-02 design, in/LOBBY_COMMON.md).

   WHAT IT IS. The launcher's room screen moved into the game: a full-screen page over the online file select with the cork background,
   the word LOBBY at the top, the room code top left (an eye hides it, a copy button puts it on the clipboard), the back arrow top right
   (leave), the chat box on the left (keyboard typing only), on the right the server name, the room settings, the coop save file (New /
   Continue), the mods box (greyed, "Soon": phase 2), the privacy box (code only for now) and one wide box that is READY for every
   player and becomes START for the host once everyone is ready; along the bottom one seat per player (the name plate in the player's
   colours; a hover or the pad focus on a seat shows a small mute button, and for the host a kick button). The VS lobby is the same page
   with the VS settings and no save file or mods box.

   WHO DRIVES IT. hal/lobby_client.h (lane LOBBYNET1) is the room: this page only reads lobby_state() and calls the client's verbs. A
   tree without that header (this lane's branch alone) builds the page on a local stand-in client that answers like a room on a server
   (the proof rows of the page's own look and input run on it).

   THE SEAT. Like the white door's picker (hal/char_door.cpp) the page borrows the debug menu's pause in tests/walk_window.cpp (lp_frame,
   menu_draw): the title stands still under it, the devices are read behind the same gates every other reader uses, and the page draws
   onto the finished picture. The online file select asks for it (HOST, or a room code typed into the ROOM CODE plank); the request is
   honoured on the next frame. The page closes on Leave / Back, after a failed create or join's Back, and a short while after the start
   (LOBBY_STARTING), so the start path of the client runs with the title free.

   INPUT. Mouse: hover lights a control, a click presses it. Pad / keyboard: the D-pad / arrows move the focus between controls, A /
   Enter / Space presses, B / Escape / Backspace is Back (or leaves a text field). Typing (chat, server name) is the keyboard only: a
   click or A on the field gives it the focus, Enter sends / stores, Escape leaves the field. */
#ifndef PORT_HAL_LOBBY_PAGE_H
#define PORT_HAL_LOBBY_PAGE_H

#include <stdint.h>

struct SpInput;

#ifdef __cplusplus
extern "C" {
#endif

/* the online file select asks (logic thread): HOST opens the Coop / VS choice first; a room code opens the page joining that room */
void lobby_page_request_host(void);
void lobby_page_request_join(const char *code);
int  lobby_page_pending(void);
void lobby_page_clear_request(void);
void lobby_page_open_now(void);
int  lobby_page_open(void);                      /* 1 while the page is up */
void lobby_page_cancel(void);                    /* something else took the pause away: leaves the room */
/* proof rows only (compiled out of the kit): SM64DS_LOBBY_SHOW=<what>@<frame> asks for the page at that frame. Called once a frame while
   the page is closed; 1 = a request was made this frame */
int  lobby_page_env_poll(void);
/* one logic frame with this frame's devices (pad_buttons: the XInput word, 0 without a pad). 1 while still up, 0 on the frame it closed */
int  lobby_page_frame(const struct SpInput *in, unsigned pad_buttons);
/* draw onto an opaque picture (0x00RRGGBB / 0xFFRRGGBB), w x h pixels, stride pixels a row */
void lobby_page_draw(uint32_t *px, int stride, int w, int h);

#ifdef __cplusplus
}
#endif
#endif
