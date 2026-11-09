/* THE LAUNCHER HAND-OFF, GAME SIDE (run after093, lane SEAMVS1; the hand-off contract v1).
 *
 * After a VS match end, a coop leave or an online leave the game is back at its title in the same process, so the launcher hands the
 * next match to the running game instead of starting a second one. It writes <bundle>/handoff/request.json; this file polls for it at
 * the title, takes it (renamed to request.taken, so a second pickup is impossible), reads it and hands it to the handler registered for
 * its kind, then answers in <bundle>/handoff/response.json: accepted, refused (and the game then quits with exit code 8 so the launcher
 * starts a fresh game with the same names) or expired.
 *
 * A HANDLER does, in this process, what a fresh process started with the request's names would end up doing: the session record
 * filled, both copies of the environment written (set) and emptied (clear), the mods latched again under the multiplayer rule, the save
 * medium switched, the session installed live and the scene entered. It returns 0 when the running game took the match, else the
 * reason it cannot (said in the response and in the "[handoff] refused" line). It is called on the game thread, at the title, with no
 * session up. An unregistered kind is refused. */
#ifndef PORT_HAL_HANDOFF_POLL_H
#define PORT_HAL_HANDOFF_POLL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PortHandoffRequest {
    const char *kind;               /* "adventure", "vs", "coop" */
    const char *nonce;              /* the launcher's 16 hex digits */
    int nset;                       /* the launcher's setEnv, in file order */
    const char *const *set_name;    /* "SM64DS_<NAME>" */
    const char *const *set_value;
    int nclear;                     /* the launcher's clearEnv */
    const char *const *clear_name;
} PortHandoffRequest;

/* 0 = the running game took the match; else why not (a refusal: the response says it, the game quits with exit code 8) */
typedef const char *(*PortHandoffFn)(const PortHandoffRequest *req);

/* Register the handler of one kind (at static initialisation is fine: the table is constant-initialised). A second registration of a
   kind replaces the first. 1 = registered, 0 = refused (no kind, no handler, table full). */
int port_handoff_register(const char *kind, PortHandoffFn fn);

/* The request's value for one name of its set list, or 0 when the request does not set it. */
const char *port_handoff_value(const PortHandoffRequest *req, const char *name);

/* The environment a fresh process would be started with: every name of the clear list out of BOTH copies of the environment (the CRT's,
   which getenv reads, and the Win32 block a later restart would inherit), then every name of the set list into both. Returns how many
   names were touched. */
int port_handoff_apply_env(const PortHandoffRequest *req);

/* A handler that refuses after port_handoff_apply_env: every name of the set list out of both copies again. */
void port_handoff_unset_env(const PortHandoffRequest *req);

/* LOBBYNET1: the in-game lobby's start (hal/lobby_client.cpp) hands its own request to the handler of its kind, in this process, the way a
   picked-up request.json is handed: only when a request could be picked up now (the title live, no session up). Returns 0 when the
   running game took it; else why not ("busy: ..." when the title cannot take one now, or the handler's refusal; nothing is left
   applied). No response file is written and nothing quits: the caller decides what a refusal means. */
const char *port_handoff_dispatch(const PortHandoffRequest *req);

/* 0 = a request may be handed over now; else why not (the title poll's own test). */
const char *port_handoff_busy(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_HAL_HANDOFF_POLL_H */
