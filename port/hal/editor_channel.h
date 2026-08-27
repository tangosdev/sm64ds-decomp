/* ---- THE EDITOR CONTROL CHANNEL (port mod) ----------------------------------
 *
 * A loopback line-protocol server the Studio's editor link talks to, so an
 * external editor can inspect and move the objects of a RUNNING level.
 *
 * OPT-IN, AND OFF BY DEFAULT. Nothing here runs, no thread starts and no
 * socket is created unless SM64DS_EDITOR_CHANNEL=1 is in the environment. The
 * shipped game must not grow a listener because a debug feature exists in the
 * binary, so the env check happens before WSAStartup, not after.
 *
 * TWO ENTRY POINTS, AND THE SPLIT IS THE WHOLE DESIGN:
 *
 *   editor_channel_init()   once at startup. Reads the env var and, if armed,
 *                           starts the accept/read thread.
 *   editor_channel_drain()  once per frame, at the FRAME BOUNDARY. Everything
 *                           that touches game state -- walking the actor list,
 *                           writing a position, asking for a level change --
 *                           happens here and nowhere else.
 *
 * The socket thread never reads or writes game state. It parses lines and puts
 * commands on a queue; the game thread drains that queue between frames. That
 * is what makes an editor client safe to attach to a running game: a client
 * that hangs mid-line, or dies with a request in flight, costs the game
 * nothing, because the game only ever looks at a completed queue entry at a
 * point where the world is not half-updated.
 *
 * Bounded per frame (EDITOR_MAX_CMDS_PER_FRAME) so a client that floods the
 * socket cannot turn one frame into an unbounded amount of work.
 */
#ifndef PORT_HAL_EDITOR_CHANNEL_H
#define PORT_HAL_EDITOR_CHANNEL_H

/* Start the channel if SM64DS_EDITOR_CHANNEL=1. Safe to call when it is not
   set (does nothing at all) and safe to call twice (the second is a no-op). */
void editor_channel_init(void);

/* Apply whatever the client asked for. Call exactly once per frame from the
   frame boundary. A no-op -- and cheap -- when the channel is not armed. */
void editor_channel_drain(void);

#endif /* PORT_HAL_EDITOR_CHANNEL_H */
