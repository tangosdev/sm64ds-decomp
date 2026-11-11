/* LOBBY CLIENT (run after093, lane LOBBYNET1): the game talks to the lobby server itself (phase 1 of the in-game lobby).

   This header is the contract between the lobby page (hal/lobby_page.cpp, lane LOBBYUI1) and the network side
   (hal/lobby_client.cpp, lane LOBBYNET1). Its shape is fixed by in/LOBBY_COMMON.md; a change is announced in both
   lanes' status files, never made silently.

   Threading: every call here is made from the frame thread and returns at once. Network work runs on a worker
   thread; lobby_tick() (once a frame while the page is up) folds finished replies into the state and queues the
   next poll. lobby_state() returns a snapshot owned by the client that stays valid until the next lobby_tick().

   Errors: state.error holds one plain sentence for the page to show; it is empty when there is nothing to say.
   phase LOBBY_FAILED means the last create / join did not get the player into a room. */
#ifndef PORT_HAL_LOBBY_CLIENT_H
#define PORT_HAL_LOBBY_CLIENT_H

enum LobbyKind  { LOBBY_COOP = 0, LOBBY_VS = 1 };
enum LobbyPhase { LOBBY_NONE, LOBBY_BUSY, LOBBY_IN_ROOM, LOBBY_STARTING, LOBBY_FAILED };

struct LobbySeat { char name[32]; unsigned body_rgb, shoe_rgb; int ready, is_host, is_me, muted; int member; };   /* member: the server's seat number, stable while the player stays */
struct LobbySetting { const char *key; const char *label; int value, min, max; const char *const *choices; int n_choices; };

struct LobbyState {
    LobbyPhase phase; LobbyKind kind; int am_host;
    char code[8]; char server_name[48]; char error[96];
    int n_seats, max_seats; LobbySeat seats[16];
    int n_settings; LobbySetting settings[16];   /* the kind's rules, host edits, frozen after the first start */
    int coop_save_continue;                      /* coop: 0 New, 1 Continue */
    int n_chat; char chat[32][96];               /* newest last, "name: text" */
    char error_head[40];                         /* LOBBY_FAILED's heading when it is not "could not get in" ("" = that) */
    int can_rejoin;                              /* REJOIN1: a started coop room whose game this console has left (quit, or the
                                                    connection was lost): lobby_rejoin_game() takes it back in (JOIN GAME) */
};

const LobbyState *lobby_state(void);
void lobby_tick(void);                            /* once a frame from the page; does the polling off the frame thread */
void lobby_create(LobbyKind kind);
void lobby_join(const char *code);
void lobby_leave(void);
void lobby_set_ready(int ready);
void lobby_start(void);                           /* host, all ready */
void lobby_set_setting(int index, int value);     /* host */
void lobby_set_coop_save(int continue_save);      /* host, coop */
void lobby_set_server_name(const char *name);     /* stored only */
void lobby_send_chat(const char *text);
void lobby_kick(int seat);                        /* host */
void lobby_ban(int seat);                         /* host: a kick that keeps that player out until the room closes (lobby v7) */
void lobby_mute(int seat, int muted);             /* local only */
void lobby_rejoin_game(void);                     /* REJOIN1: back into the room's running coop game (when can_rejoin) */

#endif
