#!/usr/bin/env python3
"""THE ARENA REFEREE CONTRACT (run mp1, lane REFEREE1; VSSYNC1 plan sections
2-5, phase A).

One definition of everything the game and the relay's referee agree on in an
arena-mode VS session: the session handshake, the kind table, the byte layouts
of the referee's kinds, the event and claim types, the reason codes and the
numbers the referee decides with. This file is the source; it writes

    arena_contract.json   the same data for any tool or test (A3, A6)
    arena_wire.h          the C side (A3 copies it into port/hal unchanged)

    python arena_contract.py --write     regenerate both
    python arena_contract.py --check     exit 1 if either is stale

referee.py carries its own copy of the numbers (the relay box runs it alone);
test_referee.py checks that copy against this file, so neither can drift.

SYNS (the console's own-body state, 48 bytes) is ARENACORE1's layout (plan
section 2.1); it is repeated here only for the fields the referee reads.
Python 3 standard library only.
"""

import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
JSON_PATH = os.path.join(HERE, "arena_contract.json")
H_PATH = os.path.join(HERE, "arena_wire.h")

CONTRACT_VERSION = 2   # 2: Luigi Infection (mode 2), lane REFLUIGI1

# ------------------------------------------------------------- the session

SESSION = {
    "hello": "HELLO v2 (relay.py) with role 3 = ARENA member; want_seat 0..15 "
             "or 0xFF (any); flags bit0 = this console sends voice",
    "role_arena": 3,
    "seats": 16,
    "keepalive_ms": 1000,
    "idle_s": 10,
    "ack_status": {
        "0": "ok: byte 6 is my seat (0..15), stamped on everything I send",
        "1": "full: 16 seats taken",
        "2": "bad request: from an ARENA HELLO this means the relay predates "
             "arena mode (an older relay rejects role 3); fall back",
        "3": "version: this code is a version 1 session",
        "4": "seat taken: the pinned seat is held; ask again with 0xFF",
        "5": "wrong kind: this code is a hub or star session",
    },
    "seat_numbering": "every 'seat' field in SYNE / SYNR is 0..15 (0xFF = "
                      "none); SYNS held / eaten_by are seat+1 (0 = none), as "
                      "plan 2.1 writes them",
    "referee_mark": 0xFE,
    "referee_clock": "referee ms, u32, from a random base PER SEAT: every "
                     "time a seat receives (SYNQ t_ref, SYNR header and "
                     "event times) and every stamp it sends is in its own "
                     "base; use them only relative to your own pongs",
    "no_relay": "arena mode needs a relay (a local relay copy for tests); the "
                "loopback mesh has no referee",
}

# The arena session's kind table: tag -> lengths, route, cap per second per
# sender. "all" = every other seat; "ref" = the referee only, never forwarded;
# "all+ref" = forwarded to every other seat AND read by the referee; "seat6" =
# the seat named at byte 6; "none" = dropped (a seat sending SYNR is also
# struck). Byte 5 of every kind is the relay-stamped sender seat.
ARENA_KINDS = [
    # tag     lengths            route       cap  note
    ("SYNS", [48],              "all+ref",  35, "own-body state 30 Hz (A0)"),
    ("SYNO", [32],              "all+ref",  35, "object frame: a star in flight"),
    ("SYNE", [24],              "ref",      20, "claim to the referee"),
    ("SYNR", [272],             "none",      0, "referee events; referee -> "
                                               "seats only"),
    ("SYNP", [12, 20],          "all|ref",   6, "12 = peer ping (as hub); 20 "
                                               "with byte 6 = 0xFE = referee "
                                               "clock ping"),
    ("SYNQ", [12, 20],          "seat6|ref", 6, "12 = peer pong (as hub); 20 = "
                                               "the referee's clock pong (from "
                                               "a seat: dropped)"),
    ("SYNI", list(range(24, 65)), "all",     3, "identity (as hub)"),
    ("SYNT", list(range(12, 133)), "all",    8, "chat (as hub)"),
    ("SYNV", [176, 340],        "all",      30, "voice (as hub)"),
]
NOT_IN_ARENA = ["SYNG", "MP2L", "SYNK", "SYN1"]

# ------------------------------------------------------------- layouts
# [name, offset, size, ctype, note]; little endian; Fix12 = units * 4096.

SYNS_FIELDS = [   # ARENACORE1's layout (plan 2.1), the fields the referee reads
    ["tag", 0, 4, "u32", "'SYNS'"],
    ["version", 4, 1, "u8", "1"],
    ["seat", 5, 1, "u8", "relay-stamped"],
    ["flags", 6, 1, "u8", "SYNS_F_*"],
    ["character", 7, 1, "u8", ""],
    ["frame", 8, 4, "u32", "sender's match frame, 30 Hz, monotonic"],
    ["pos", 12, 12, "s32[3]", "x, y, z Fix12"],
    ["vel", 24, 6, "s16[3]", "per-frame delta in 1/64 unit"],
    ["yaw", 30, 2, "s16", ""],
    ["anim_id", 32, 2, "u16", "< 194"],
    ["anim_cursor", 34, 2, "u16", ""],
    ["state_idx", 36, 1, "u8", "index into the 78 g_player_states (STATE_NAMES)"],
    ["state_step", 37, 1, "u8", ""],
    ["invuln", 38, 1, "u8", "0..36"],
    ["power", 39, 1, "u8", "SYNS_P_*"],
    ["held", 40, 1, "u8", "0 none, 1..16 = seat+1 in my mouth, 0x20+ object"],
    ["eaten_by", 41, 1, "u8", "0 free, 1..16 = seat+1 whose mouth I am in"],
    ["ack", 42, 2, "u16", "newest referee event seq applied"],
    ["sfx", 44, 2, "u8[2]", ""],
    ["reserved", 46, 2, "u16", "0"],
]
SYNS_FLAGS = ["live", "grounded", "teleport", "paused", "talking", "in_hurt",
              "no_control", "star_lock"]          # bit 0..7, plan 2.1 order
SYNS_POWER = {"fire": 0x01, "wings": 0x02, "metal": 0x04, "vanish": 0x08,
              "mega": 0x10, "no_cap": 0x20}       # ARENACORE1's vs_arena.h

SYNO_FIELDS = [
    ["tag", 0, 4, "u32", "'SYNO'"],
    ["version", 4, 1, "u8", "1"],
    ["seat", 5, 1, "u8", "relay-stamped: the launching console"],
    ["obj_class", 6, 1, "u8", "1 = star"],
    ["obj_id", 7, 1, "u8", "star instance (1..255) from STAR_WAKE / SPILL"],
    ["frame", 8, 4, "u32", "sender's match frame"],
    ["pos", 12, 12, "s32[3]", "Fix12"],
    ["vel", 24, 6, "s16[3]", "per-frame delta in 1/64 unit"],
    ["state", 30, 1, "u8", "1 flying, 2 resting, 3 gone"],
    ["flags", 31, 1, "u8", "bit0 teleport"],
]

SYNE_FIELDS = [
    ["tag", 0, 4, "u32", "'SYNE'"],
    ["version", 4, 1, "u8", "1"],
    ["seat", 5, 1, "u8", "relay-stamped"],
    ["kind", 6, 1, "u8", "CLAIM_*"],
    ["target", 7, 1, "u8", "star instance / victim seat / map (READY)"],
    ["claim_id", 8, 2, "u16", "per seat, 1.., +1 per NEW claim, wraps "
                              "skipping 0; a resend repeats it"],
    ["aux", 10, 2, "u16", "per kind (CLAIMS)"],
    ["stamp", 12, 4, "u32", "referee ms of the touch, from my clock offset"],
    ["my_frame", 16, 4, "u32", "my own match frame of the touch (my SYNS "
                               "frames)"],
    ["ref_frame", 20, 4, "u32", "HIT / EAT: the victim's SYNS frame I was "
                                "drawing (rounded down); else 0"],
]

PING_FIELDS = [   # SYNP 20, console -> referee
    ["tag", 0, 4, "u32", "'SYNP'"],
    ["version", 4, 1, "u8", "2"],
    ["seat", 5, 1, "u8", "relay-stamped"],
    ["to", 6, 1, "u8", "0xFE = the referee"],
    ["zero", 7, 1, "u8", "0"],
    ["t_cons", 8, 4, "u32", "console ms at send (echoed)"],
    ["echo_ref", 12, 4, "u32", "t_ref of the newest referee pong I got, 0 = "
                               "none"],
    ["held_ms", 16, 2, "u16", "my ms between getting that pong and sending "
                              "this ping; 0xFFFF = none"],
    ["zero2", 18, 2, "u16", "0"],
]
PONG_FIELDS = [   # SYNQ 20, referee -> the pinger only
    ["tag", 0, 4, "u32", "'SYNQ'"],
    ["version", 4, 1, "u8", "2"],
    ["seat", 5, 1, "u8", "0xFE (the referee)"],
    ["to", 6, 1, "u8", "the pinger's seat"],
    ["zero", 7, 1, "u8", "0"],
    ["t_cons", 8, 4, "u32", "the ping's t_cons, echoed"],
    ["t_ref", 12, 4, "u32", "referee ms at send"],
    ["rtt_ms", 16, 2, "u16", "the referee's min RTT for this seat, 0xFFFF "
                             "unknown"],
    ["cap_ms", 18, 2, "u16", "this seat's back-dating cap now"],
]

SYNR_HEADER = [
    ["tag", 0, 4, "u32", "'SYNR'"],
    ["version", 4, 1, "u8", "1"],
    ["seat", 5, 1, "u8", "0xFE (the referee); accept SYNR only from the relay "
                         "with 0xFE here"],
    ["n_events", 6, 1, "u8", "0..8, newest first"],
    ["match_state", 7, 1, "u8", "MS_*"],
    ["ref_now", 8, 4, "u32", "referee ms at send"],
    ["newest_seq", 12, 2, "u16", "newest event seq of the session stream, 0 "
                                 "= none yet"],
    ["roster", 14, 2, "u16", "seats in the match (bit = seat)"],
    ["t0", 16, 4, "u32", "referee ms of match start, 0 = not started"],
    ["match_ms", 20, 4, "u32", "match length, 0 = no clock"],
    ["scores", 24, 16, "s8[16]", "THE LEDGER, per seat; mirror into "
                                 "data_0209f310"],
    ["progress", 40, 1, "u8", "stars collected so far (data_0209f208 "
                              "mirror)"],
    ["first_to_n", 41, 1, "u8", "0 = off"],
    ["flags", 42, 1, "u8", "SYNR_F_*"],
    ["part", 43, 1, "u8", "snapshot: high nibble part index, low nibble part "
                          "count; else 0"],
    ["order_row", 44, 1, "u8", "VS_STAR_SPAWN_ORDERS row (0..5)"],
    ["map", 45, 1, "u8", ""],
    ["live_stars", 46, 1, "u8", "star instances alive"],
    ["zero", 47, 1, "u8", "0"],
]
EVENT_FIELDS = [   # 8 of these follow the header, 28 bytes each
    ["seq", 0, 2, "u16", "1.. per session (wraps skipping 0); 0 = snapshot "
                         "record, not part of the stream"],
    ["type", 2, 1, "u8", "EV_*"],
    ["seat", 3, 1, "u8", "subject seat, 0xFF none"],
    ["a", 4, 1, "u8", ""],
    ["b", 5, 1, "u8", ""],
    ["c", 6, 2, "u16", ""],
    ["d", 8, 2, "u16", ""],
    ["e", 10, 2, "u16", ""],
    ["t", 12, 4, "u32", "referee ms"],
    ["x", 16, 4, "s32", ""],
    ["y", 20, 4, "s32", ""],
    ["z", 24, 4, "s32", ""],
]
SYNR_EVENTS = 8
SYNR_LEN = 48 + 28 * SYNR_EVENTS

SYNR_FLAGS = {"snapshot": 0x01, "directed": 0x02, "resend": 0x04}
MATCH_STATES = {"waiting": 0, "countdown": 1, "running": 2, "over": 3}

# ------------------------------------------------------------- claims

CLAIMS = [
    # id name           target              aux / stamp / frames
    (1, "READY", "map (0..3)",
     "aux = time limit s (0 none); stamp = rules: b0 first_to_n, b1 mode (0 "
     "classic, 2 Luigi Infection; 1 is kept for King of the Star and refused, "
     "like every other value), b2 capacity 2..16, b3 marker count "
     "1..8; my_frame = the go plan digest (u32, same on every console); "
     "ref_frame 0. Every console sends it once booted; config must match the "
     "first READY"),
    (2, "MARKER_BREAK", "star instance (a waking marker)",
     "aux = the breaker's camera yaw (the ROM pop direction); stamp; my_frame"),
    (3, "GRAB", "star instance (free)", "aux 0; stamp; my_frame"),
    (4, "HIT", "victim seat",
     "aux = hitFlags >> 5 (F2 mask 0x27fe0); stamp; my_frame; ref_frame = the "
     "victim frame I drew"),
    (5, "HURT_SELF", "0xFF",
     "aux = cause 1 chomp, 2 iron ball, 3 other; always accepted, spills one "
     "star; sanctions knockback speed for 2 s"),
    (6, "PIT", "0xFF",
     "aux bit0 = this fall costs a star (the ROM St_DeadPit path); always "
     "accepted; sanctions the respawn teleport for 3 s"),
    (7, "QUIT", "0xFF", "leaving now (clean exit): stars spill at once"),
    (8, "EAT", "victim seat",
     "PROVISIONAL (A5): tongue on a puppet; ref_frame = victim frame drawn"),
    (9, "SPIT", "victim seat", "PROVISIONAL (A5): eater spits"),
    (10, "SWALLOW", "victim seat", "PROVISIONAL (A5): eater swallows (egg)"),
    (11, "ESCAPE", "eater seat", "PROVISIONAL (A5): victim got out on its own"),
    (12, "CATCH", "victim seat",
     "GRAB1: Wario catches a stunned player (the ROM's Player::TryGrab); "
     "stamp; my_frame; ref_frame = the victim frame I drew"),
    (13, "THROW", "victim seat (the one I hold)",
     "GRAB1: let go / throw (vertical 0x10000); aux = my yaw (u16); "
     "ref_frame = launch speed (Fix12)"),
    (14, "SWING", "victim seat (the one I hold)",
     "GRAB1: the spin throw (vertical 0x20000); aux = launch yaw (u16); "
     "ref_frame = launch speed (Fix12)"),
    (15, "BREAK", "holder seat",
     "GRAB1: the victim struggled free of a hold"),
    (16, "STOMP", "victim seat",
     "GRAB1: I landed on a player (the ROM's func_ov002_020d869c stomp: "
     "JumpedOnByPlayer + func_ov002_020d9298); stamp; my_frame; ref_frame = "
     "the victim frame I drew"),
    (17, "CAP_TAKE", "cap id (1..31 a cap the arena places, 32..255 one a hit "
                     "knocked off)",
     "VSREF3: my body touched a cap and the ROM's pickup started; aux = the "
     "cap's place hash (placed caps: (slot & 0xFF) | ((x_units ^ z_units) & 0xFF) "
     "<< 8, slot = the actor's death-table id, 0 for a knocked-off one); stamp; my_frame"),
    (18, "CAP_DROP", "the look (0 Mario, 1 Luigi, 2 Wario) of the cap that "
                     "came off",
     "VSREF3: my body was hurt and the ROM popped my cap (St_Hurt_Init -> "
     "func_ov002_020d93ac); aux = my angle Y (u16); ref_frame = my angle X "
     "(low 16) | my angle Z (high 16); my_frame = the frame it popped; the "
     "pop position is the referee's own sample of me"),
    (19, "TAG", "victim seat",
     "LUIGI (mode 2 only): my Luigi body touched a survivor's body (Classic's "
     "team guard, unmatched/LuigiInfection_Hit.cpp, runs on any player "
     "contact, before the ROM's attack and stomp branches); stamp; my_frame; "
     "ref_frame = the victim frame I drew. A Luigi's HIT and STOMP infect "
     "too; TAG is for a touch that is neither"),
]

EVENTS = [
    # id name             fields
    (1, "START", "seat 0xFF; a order row; b first_to_n; c time limit s; d "
                 "roster; e marker count; t = T0 (match frame 0, in the "
                 "future by COUNTDOWN_MS); x seed (u32); y map; z rules word "
                 "(READY stamp)"),
    (2, "STAR_WAKE", "seat 0xFF; a instance; b star id "
                     "(VS_STAR_SPAWN_ORDERS[row][c]); c progress index p "
                     "(data_0209f208); t now"),
    (3, "MARKER_BROKEN", "seat breaker; a instance; b star id; c claim id; d "
                         "camera yaw; t stamp; xyz breaker position (Fix12). "
                         "The breaker's console launches the star (SYNO)"),
    (4, "STAR_TAKEN", "seat winner; a instance; b winner's new score; c claim "
                      "id; e progress after; t stamp; xyz winner position"),
    (5, "DENY", "DIRECTED to the claimant only (seq 0, flag directed): seat "
                "claimant; a claim kind; b reason (DENY_*); c claim id; d "
                "winner seat when LOST else 0xFF; e target echo; t now"),
    (6, "HIT", "seat victim; a attacker; b 0 hit / 1 trade; c attacker's "
               "claim id; d aux (hitFlags >> 5); e spilled instance (0 none); "
               "t stamp; xyz attacker position (Fix12) for Player::Hurt"),
    (7, "CLASH", "seat victim; a attacker; c claim id; t stamp; xyz attacker "
                 "position: both bounce, no star"),
    (8, "SPILL", "seat who lost it (0xFF none); a new instance; b destination "
                 "marker (referee RNG); c cause 1 hit 2 leave 3 hazard 4 pit; "
                 "d that seat's new score; t now; xyz origin (Fix12)"),
    (9, "MATCH_OVER", "seat 0xFF; a reason (OVER_*); b winner count; c "
                      "winners mask; t end; the header ledger is final"),
    (10, "LEAVE", "seat leaver; a 1 quit, 2 silent PRESENCE_MS, 3 relay "
                  "detach; d stars spilled; t now"),
    (11, "JOIN", "seat joiner; a 0 before start, 1 mid-match; t now"),
    (12, "STAR_STATE", "SNAPSHOT ONLY (seq 0): seat 0xFF; a instance; b "
                       "state 1 waking / 2 free; c marker; d star id; t since; "
                       "xyz last known position"),
    (13, "ACCEPTED", "DIRECTED to the claimant only (seq 0, flag directed): "
                     "seat claimant; a claim kind; c claim id; d spilled "
                     "instance or 0; t now (READY, HURT_SELF, PIT, QUIT)"),
    (14, "EATEN", "PROVISIONAL (A5): seat victim; a eater; c claim id; t "
                  "stamp; xyz eater position"),
    (15, "RELEASED", "PROVISIONAL (A5): seat victim; a eater; b 1 spit 2 "
                     "swallow 3 escape 4 leave; c claim id (SPIT / SWALLOW / "
                     "ESCAPE; 0 for a leave); t now"),
    (16, "GRABBED", "GRAB1: seat victim; a holder; c claim id (CATCH); t "
                    "stamp; xyz holder position (Fix12)"),
    (17, "THROWN", "GRAB1: seat victim; a holder; b 1 throw 2 swing 3 break "
                   "4 leave / forced; c claim id (THROW / SWING / BREAK; 0 "
                   "for 4); d launch yaw (u16); t now; x launch speed "
                   "(Fix12); yz 0"),
    (18, "STOMPED", "GRAB1: seat victim; a stomper; c claim id; t stamp; xyz "
                    "stomper position (Fix12). No star, no hurt: the ROM's "
                    "squash and the stomper's bounce"),
    (19, "CAP_TAKEN", "VSREF3: seat taker; a cap id; b look (0..2); c claim "
                      "id; d ms until a placed cap is back (0 = a knocked-off "
                      "cap never comes back); t stamp; xyz taker position. "
                      "Every other console deletes its copy of the cap; the "
                      "taker keeps the ROM's own pickup"),
    (20, "CAP_DROPPED", "VSREF3: seat victim; a new cap id (32..255); b look "
                        "(0..2); c claim id; d angle Y; e angle X (angle Z "
                        "is 0 on the wire); t stamp; xyz pop position "
                        "(Fix12, already the ROM's +0x82000 above the body). "
                        "Every console but the victim's spawns the cap the "
                        "ROM spawns (actor 0x10d, param (look << 8) | 5, "
                        "speed 0xb000)"),
    (21, "CAP_STATE", "SNAPSHOT ONLY (seq 0): seat 0xFF; a cap id; b 1 a "
                      "placed cap that is taken / 2 a knocked-off cap lying "
                      "there; c ms until the placed cap is back / the "
                      "knocked-off one expires; d look; xyz last position"),
    (22, "INFECTED", "LUIGI (mode 2): seat = the player who is a Luigi now; a "
                     "by: the tagging Luigi's seat, 0xFF when the referee "
                     "made it (pick, hand-off, late join); b cause (INF_*); "
                     "c the tagger's claim id (HIT / STOMP / TAG), 0 when the "
                     "referee made it; d the Luigi mask after (bit = seat); e "
                     "survivors left in the match; t stamp (the first pick: "
                     "T0); xyz the tagger's position (Fix12; 0 when the "
                     "referee made it). No hurt, no star: swap the body to "
                     "Luigi at once"),
    (23, "TEAM_STATE", "SNAPSHOT ONLY (seq 0), mode 2: seat 0xFF; a 1 = the "
                       "first Luigi is picked, 0 = not yet; c the Luigi mask; "
                       "d the roster mask; t the pick time (T0)"),
]

DENY_REASONS = [
    (1, "STALE", "match not running, or the claim is older than history"),
    (2, "GONE", "star missing / already taken / marker already broken"),
    (3, "LOST", "contested: another seat's earlier stamp won (d = winner)"),
    (4, "RANGE", "claimant / attacker too far from the target at that time"),
    (5, "STATE", "claimant hurt / in a mouth / no-control, or not attacking"),
    (6, "INVULN", "victim invincible, or already hit inside HIT_IMMUNE_MS"),
    (7, "MISMATCH", "READY config differs from the match"),
    (8, "UNSUPPORTED", "mode or claim kind not in this phase"),
    (9, "BAD", "a field out of range"),
    (10, "TOO_OLD", "lag compensation past COMP_MAX_MS"),
    (11, "SELF", "target is the claimant, or not a seat in the match"),
    (12, "TEAM", "Luigi Infection team rule: a Luigi may not take a cap, drop "
                 "one, eat or catch; a survivor may not take the Luigi cap or "
                 "TAG"),
]

OVER_REASONS = [(1, "TIME_UP"), (2, "FIRST_TO_N"), (3, "LAST_STANDING"),
                (4, "EVERYONE_LEFT"), (5, "LUIGIS_WIN"), (6, "SURVIVORS_WIN")]

# READY rules byte 1. King of the Star keeps 1 and stays refused (shelved).
MODES = [(0, "CLASSIC"), (1, "KING"), (2, "LUIGI")]
MODES_ACCEPTED = [0, 2]

# INFECTED field b: why this player is a Luigi now.
INFECT_CAUSES = [(1, "PICK", "the first Luigi, picked at T0"),
                 (2, "HIT", "a Luigi's HIT claim"),
                 (3, "STOMP", "a Luigi's STOMP claim"),
                 (4, "TAG", "a Luigi's TAG claim (body contact)"),
                 (5, "HANDOFF", "every Luigi left: the referee picked a new one"),
                 (6, "LATE_JOIN", "joined after the first pick")]

NUMBERS = {
    "FRAME_MS": 1000.0 / 30.0,
    "HEARTBEAT_HZ": 4,
    "CLAIM_RESEND_MS": 60,
    "VERDICT_TIMEOUT_MS": 500,
    "HOLD_MAX_MS": 150,
    "CAP_MARGIN_MS": 25,
    "CAP_UNKNOWN_MS": 100,
    "FUTURE_SLACK_MS": 5,
    "COMP_MAX_MS": 200,
    "HIT_IMMUNE_MS": 1000,
    "TRADE_MS": 34,
    "REACH_H": 250,
    "REACH_V": 250,
    "GRAB_R": 200,
    "MARKER_R": 300,
    "ATTACK_LOOKBACK_FRAMES": 10,
    "PRESENCE_MS": 3000,
    "START_WAIT_MS": 20000,
    "COUNTDOWN_MS": 3000,
    "OVER_LINGER_MS": 3000,
    "SNAPSHOT_LAG_MS": 2000,
    "HISTORY_FRAMES": 64,
    "INVULN_CHEAT_MS": 300,
    "STRIKE_WINDOW_MS": 10000,
    "STRIKES_TO_KICK": 3,
    "POS_LIMIT_UNITS": 32768,
    "AMP_FACTOR": 3,
    "AMP_CREDIT_CAP": 16384,
    "GRAB_STUN_MAX_MS": 5000,
    "GRAB_RELEASE_MIN_MS": 500,
    "GRAB_STUCK_MS": 3000,
    "GRAB_SPEED_MAX": 0x40000,
    "STOMP_MIN_MS": 250,
    "CAP_R": 200,
    "CAP_FLIGHT_SLACK": 1500,
    "CAP_FLIGHT_MS": 7000,
    "CAP_RESPAWN_MS": 1500,
    "CAP_LIFE_MS": 7000,
    "CAP_WORN_MS": 3000,
    "CAP_HURT_EVIDENCE_MS": 1500,
    "CAP_DROP_WAIT_MS": 150,
    "CAP_DROP_MIN_MS": 1000,
    "LUIGI_COUNTDOWN_MS": 5000,
    "TAG_REACH_H": 130,
    "TAG_REACH_V": 200,
}

# The 78 Player::State records in g_player_states order
# (port/hal/pmf3_player_states.cpp; scripts state_names.py). SYNS state_idx.
STATE_NAMES = [
    "St_LedgeHang", "St_LedgeGrab", "St_CeilingGrate", "St_YoshiPower",
    "St_Swallow", "St_InYoshiMouth", "St_SmallLaunchUp", "St_Hurt",
    "St_HurtWater", "St_Electrocute", "St_BurnFire", "St_BurnLava",
    "St_DeadHit", "St_DeadPit", "St_Walk", "St_Wait", "St_Grabbed",
    "St_TurnAround", "St_Jump", "St_Fall", "St_Thrown", "St_SideFlip",
    "St_SlideKickRecover", "St_Fly", "St_NoControl", "St_Owl", "St_DebugFly",
    "St_WindCarry", "St_Balloon", "St_Teleport", "St_TornadoSpin", "St_Cannon",
    "St_Squish", "St_Shell", "St_StomachSlide", "St_ButtSlide",
    "St_DizzyStars", "St_HoldLight", "St_Bonk", "St_HoldHeavy", "St_WallSlide",
    "St_BowserEarthquake", "St_WallJump", "St_SlopeJump", "St_StuckInGround",
    "St_Land", "St_OnWall", "St_Spin", "St_Talk", "St_CrazedCrate",
    "St_WaitQuicksand", "St_LevelEnter", "St_JumpQuicksand", "St_Crouch",
    "St_Respawn", "St_Crawl", "St_BackFlip", "St_OpeningWakeUp", "St_LongJump",
    "St_PunchKick", "St_EndingFly", "St_GroundPound", "St_Dive", "St_Throw",
    "St_GrabBowserTail", "St_SwingPlayer", "St_SweepKick", "St_SlideKick",
    "St_CameraZoom", "St_Null", "St_Swim", "St_WaterJump",
    "St_MetalWaterGround", "St_MetalWaterWater", "St_Climb", "St_Headstand",
    "St_WallJump", "St_HeadstandJump",
]

# A HIT needs the attacker in one of these within ATTACK_LOOKBACK_FRAMES of its
# claimed frame (F2 hitFlags: spin/pound, punch, kick, breakdance, slide kick,
# dive, tongue, fire).
ATTACK_STATES = ["St_YoshiPower", "St_Spin", "St_PunchKick", "St_GroundPound",
                 "St_Dive", "St_SweepKick", "St_SlideKick",
                 "St_SlideKickRecover", "St_Land", "St_SwingPlayer"]
# Nobody in one of these can grab, hit or be granted anything (F2 gate).
DISABLED_STATES = ["St_Hurt", "St_HurtWater", "St_Electrocute", "St_BurnFire",
                   "St_BurnLava", "St_DeadHit", "St_DeadPit",
                   "St_InYoshiMouth", "St_NoControl", "St_Squish",
                   "St_DizzyStars", "St_Respawn", "St_Thrown", "St_Grabbed"]
# A victim mid punch / kick facing the attacker within 0x4000 clashes.
# func_ov002_020d853c tests St_PunchKick only (data_ov002_02110574 = index 59),
# a punch-frame window from data_ov002_0210a5dc that phase A does not read (the
# whole punch counts), and AngleDiff(Vec3_HorzAngle(victim, attacker), victim
# yaw) < 0x4000; the referee takes the horizontal angle as atan2(dx, dz) in
# 0x10000 units (A3: confirm against Vec3_HorzAngle).
CLASH_STATES = ["St_PunchKick"]

# Speed sanity (M2 envelope + 25%), horizontal units per frame. Phase A: the
# referee LOGS strikes; kicking is phase B (STRIKES1).
SPEED_CLASSES = {
    "walk": {"limit": 50, "states": ["St_Walk", "St_Wait", "St_TurnAround",
                                     "St_Crouch", "St_Crawl", "St_PunchKick",
                                     "St_YoshiPower", "St_HoldLight",
                                     "St_HoldHeavy", "St_Throw",
                                     "St_Talk", "St_Spin"]},
    "slide": {"limit": 125, "states": ["St_StomachSlide", "St_ButtSlide",
                                       "St_SlideKick", "St_SlideKickRecover",
                                       "St_Dive"]},
    "exempt": {"limit": None, "states": ["St_InYoshiMouth", "St_Respawn",
                                         "St_Thrown", "St_Teleport",
                                         "St_Cannon", "St_DeadPit",
                                         "St_SmallLaunchUp", "St_Swallow",
                                         "St_LevelEnter"]},
    "air": {"limit": 90, "states": "everything else"},
}
SPEED_AVG_LIMIT = 60          # over 10 frames, outside a slide
SPEED_AVG_FRAMES = 10

# VS_STAR_SPAWN_ORDERS (ROM arm9 data 0x02075720, 6 rows x 12), as
# port/hal/bob_enemy_bridges.cpp carries it. Progress index p past 11 reads
# star id 0 here (documented divergence: the ROM would read past the row).
STAR_ORDERS = [
    [0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 3, 2, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 2, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 2, 3, 1, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 3, 2, 1, 4, 0, 0, 0, 0, 0, 0, 0],
    [0, 3, 1, 2, 4, 0, 0, 0, 0, 0, 0, 0],
]

RULES = [
    "The referee sends a seat nothing, and counts no claim from it, until "
    "the seat's clock ping (SYNP 20) echoes the exact t_ref of a referee "
    "pong it received (echo_ref) with held_ms = how long it held that pong. "
    "Ping at 2 Hz from the moment the seat is paired; the second ping "
    "verifies it. After that the referee sends a seat at most 3x the bytes "
    "the seat sends it (SYNS / SYNO / SYNE / SYNP, banked up to 16 KB): keep "
    "SYNS flowing (10 Hz idle is plenty) from pairing on, or heartbeats pause "
    "until you send again (every SYNR re-carries the 8 newest events).",
    "Claims are decided in STAMP order once referee time passes stamp + H, H = "
    "the largest back-dating cap among the other seats in the match (<= "
    "HOLD_MAX_MS). Exact stamp ties: fewer stars first, then the match seed's "
    "seat order.",
    "A stamp is clamped into [arrival - cap, arrival] (cap = min(HOLD_MAX_MS, "
    "rtt_min/2 + CAP_MARGIN_MS); CAP_UNKNOWN_MS before any clock ping); "
    "clamps are counted per seat.",
    "Every claim (seat, claim_id) gets exactly one terminal event naming it: "
    "MARKER_BROKEN, STAR_TAKEN, HIT, CLASH, EATEN, RELEASED, GRABBED, "
    "THROWN, STOMPED, ACCEPTED or DENY. A resend "
    "after the verdict gets that same event again (directed if it left the "
    "8-event window). DENY and ACCEPTED are directed records (seq 0) to the "
    "claimant alone, never in the stream; the others are stream events "
    "everyone hears. Resend a claim every CLAIM_RESEND_MS until named; no "
    "verdict in VERDICT_TIMEOUT_MS = treat as denied.",
    "The claimant of an event: its seat for DENY, ACCEPTED, STAR_TAKEN and "
    "MARKER_BROKEN; its field a for HIT, CLASH and EATEN (seat = the "
    "victim); seat or a for RELEASED (escape: the victim; spit / swallow: "
    "the eater).",
    "Every SYNR carries the 8 newest events (newest first). It goes out at "
    "once when events are made and as a HEARTBEAT_HZ heartbeat. Apply events "
    "in seq order exactly once and report the newest applied in SYNS.ack; a "
    "seat whose ack trails past the window gets the missed events again "
    "(directed, flag resend) while the referee still holds them, else a "
    "SNAPSHOT (directed SYNR parts, flag snapshot: START replay + STAR_STATE "
    "records + MATCH_OVER if over, seq 0); after the last part set applied = "
    "header newest_seq.",
    "Every collect (order star or spilled star) advances progress p; the "
    "referee then wakes VS_STAR_SPAWN_ORDERS[row][p] as a new instance "
    "(STAR_WAKE), the ROM's own rule with the referee's row.",
    "The ledger (header scores) is the only score. HIT = victim -1 and a SPILL "
    "(if it had any); leave / QUIT = all of that seat's stars spill; HURT_SELF "
    "and PIT (aux bit0) spill one.",
    "MATCH_OVER only from the referee: first_to_n reached, referee time >= T0 "
    "+ match_ms, one seat left of a match that had two or more, or everyone "
    "left. Winners = every seat on the top score.",
    "A seat the referee dropped from the match (LEAVE naming itself) must send "
    "READY again to come back (JOIN mid-match, score 0, SNAPSHOT).",
    "GRAB1 (the Wario hold): CATCH is a HIT-shaped claim from a Wario "
    "(SYNS character 2) in a punch or dive against a victim in the stun a "
    "Wario hit leaves (St_Hurt within GRAB_STUN_MAX_MS of that hit, or "
    "St_DizzyStars); verdict GRABBED (seat = victim, a = holder) or DENY. One "
    "hold per holder and per victim; a held seat cannot be hit or eaten. THROW "
    "and SWING (holder) and BREAK (victim) end a hold no sooner than "
    "GRAB_RELEASE_MIN_MS after the GRABBED, as THROWN (b 1 throw, 2 swing, 3 "
    "break, 4 forced: a leave, a quit, or the victim not in St_Grabbed for "
    "GRAB_STUCK_MS); the launch speed is capped at GRAB_SPEED_MAX. STOMP "
    "(GRAB1, F9-d): an airborne claimant falling onto a player in reach, "
    "STOMPED to everyone (the victim squashes, the stomper bounces); never "
    "a star, never a hurt; one per pair per STOMP_MIN_MS.",
    "VSREF3 (caps): a cap the arena places has an id 1..31 (its death-table "
    "slot + 1: the arena's cap table) and a cap a hit knocks off has one the referee "
    "issues (32..255). CAP_TAKE is a claim like GRAB: the earliest stamp wins "
    "(DENY LOST d = winner for the others, GONE once the cap is not there), "
    "the claimant must be able to act (STATE) and within CAP_R of the cap "
    "(RANGE; a knocked-off cap flies and slides for CAP_FLIGHT_MS and gets "
    "CAP_FLIGHT_SLACK more, measured from its pop point). A taken placed cap is back CAP_RESPAWN_MS after "
    "the stamp, on every console from the event time; a knocked-off cap lies "
    "CAP_LIFE_MS. CAP_DROP is the hurt wearer's report that the ROM popped its "
    "cap: granted only to a seat whose own SYNS showed that cap look within "
    "CAP_WORN_MS and showed a hurt within CAP_HURT_EVIDENCE_MS, once per "
    "wearing; the pop position is the referee's own sample of the seat, never "
    "the claim's.",
    "Modes: 0 classic and 2 Luigi Infection; any other READY mode (1 is kept "
    "for King of the Star) is DENY UNSUPPORTED. TAG outside a mode 2 match is "
    "an unknown kind, as before mode 2 existed: no verdict, a strike. EAT / "
    "SPIT / SWALLOW / ESCAPE are provisional for A5; speed "
    "strikes are logged, not kicked.",
    "LUIGI INFECTION (mode 2; mode 0 never sees any of this). The start "
    "countdown is LUIGI_COUNTDOWN_MS instead of COUNTDOWN_MS; the referee "
    "freezes nobody during it (movement is the game's) and, as in every mode, "
    "decides no claim before T0 but QUIT (DENY STALE). At T0 the referee "
    "picks the first Luigi: sorted(roster)[seed % n], seed = START x, n = "
    "players in the match (Classic's port_luigi_seed rule), INFECTED b PICK. "
    "The team is the referee's alone (the Luigi mask). A Luigi is immune: a "
    "HIT, STOMP, EAT, CATCH or TAG on a Luigi is DENY INVULN, from anyone. A "
    "Luigi's HIT (attack state, reach, lag checks as a hit), STOMP (as a "
    "stomp) or TAG (body contact: TAG_REACH_H / TAG_REACH_V) on a survivor "
    "is INFECTED: no hurt, no star, and the victim's invincibility, power and "
    "hit-immunity do not stop it (Classic's guard runs before the ROM's "
    "resolution); a victim in a mouth or a hold is DENY INVULN. A Luigi's "
    "EAT, CATCH, CAP_TAKE and CAP_DROP are DENY TEAM; a survivor's CAP_TAKE of "
    "a Luigi cap (look 1) and a survivor's TAG are DENY TEAM. Survivor vs "
    "survivor is the classic rules. Infecting a player ends any hold or "
    "mouthful it is part of (THROWN b 4 / RELEASED b 4 first). Stars spill and "
    "score as in classic but decide nothing: first_to_n is ignored. The end: "
    "no survivor left in the match = LUIGIS_WIN (winners = the Luigis); the "
    "clock runs out with a survivor = SURVIVORS_WIN (winners = the "
    "survivors); one player left of a match that had two or more = that "
    "player's team wins (5 or 6); everyone left = EVERYONE_LEFT. Leavers: "
    "the last Luigi leaves with two or more players left = the referee picks "
    "a new Luigi by the same rule over the players left (INFECTED b "
    "HANDOFF); the last survivor leaves = LUIGIS_WIN. A player who joins "
    "after the pick (READY mid-match, a rejoin too) is a Luigi at once "
    "(INFECTED b LATE_JOIN, before its SNAPSHOT); one who joins before the "
    "pick is in the pick. A mode 2 SNAPSHOT carries a TEAM_STATE record "
    "after the START replay.",
]


def contract():
    return {
        "contract": "sm64ds arena referee", "version": CONTRACT_VERSION,
        "session": SESSION,
        "kinds": [{"tag": t, "lengths": l, "route": r, "cap": c, "note": n}
                  for t, l, r, c, n in ARENA_KINDS],
        "not_in_arena": NOT_IN_ARENA,
        "layouts": {
            "SYNS": {"length": 48, "owner": "ARENACORE1 (plan 2.1)",
                     "fields": SYNS_FIELDS, "flags": SYNS_FLAGS,
                     "power": SYNS_POWER},
            "SYNO": {"length": 32, "fields": SYNO_FIELDS},
            "SYNE": {"length": 24, "fields": SYNE_FIELDS},
            "SYNP_REF": {"length": 20, "fields": PING_FIELDS},
            "SYNQ_REF": {"length": 20, "fields": PONG_FIELDS},
            "SYNR": {"length": SYNR_LEN, "header": SYNR_HEADER,
                     "event": EVENT_FIELDS, "events": SYNR_EVENTS,
                     "flags": SYNR_FLAGS, "match_states": MATCH_STATES},
        },
        "claims": [{"id": i, "name": n, "target": t, "fields": f}
                   for i, n, t, f in CLAIMS],
        "events": [{"id": i, "name": n, "fields": f} for i, n, f in EVENTS],
        "deny_reasons": [{"id": i, "name": n, "note": d}
                         for i, n, d in DENY_REASONS],
        "over_reasons": [{"id": i, "name": n} for i, n in OVER_REASONS],
        "modes": [{"id": i, "name": n, "accepted": i in MODES_ACCEPTED}
                  for i, n in MODES],
        "infect_causes": [{"id": i, "name": n, "note": d}
                          for i, n, d in INFECT_CAUSES],
        "numbers": NUMBERS,
        "state_names": STATE_NAMES,
        "attack_states": ATTACK_STATES,
        "disabled_states": DISABLED_STATES,
        "clash_states": CLASH_STATES,
        "speed_classes": SPEED_CLASSES,
        "speed_avg": {"limit": SPEED_AVG_LIMIT, "frames": SPEED_AVG_FRAMES},
        "star_orders": STAR_ORDERS,
        "rules": RULES,
    }


def render_json():
    return json.dumps(contract(), indent=1, sort_keys=False) + "\n"


# ------------------------------------------------------------- the C header

_CTYPE = {"u8": "unsigned char", "s8": "signed char", "u16": "unsigned short",
          "s16": "short", "u32": "unsigned", "s32": "int"}


def _c_struct(name, fields, total):
    out = ["struct %s {" % name]
    for fname, off, size, ctype, note in fields:
        base, _, count = ctype.partition("[")
        decl = "%s %s%s;" % (_CTYPE[base], fname,
                              ("[" + count) if count else "")
        out.append("    %-34s /* +%2d %s */" % (decl, off, note)
                   if note else "    %-34s /* +%2d */" % (decl, off))
    out.append("};")
    out.append("static_assert(sizeof(%s) == %d, \"%s size\");"
               % (name, total, name))
    for fname, off, size, ctype, note in fields:
        out.append("static_assert(offsetof(%s, %s) == %d, \"%s.%s\");"
                   % (name, fname, off, name, fname))
    return out


def render_h():
    L = []
    w = L.append
    w("// GENERATED by port/tools/relay/arena_contract.py -- do not edit; "
      "change the")
    w("// generator and run `python arena_contract.py --write`. Run mp1, lane "
      "REFEREE1.")
    w("//")
    w("// THE ARENA REFEREE WIRE, game side (VSSYNC1 plan sections 2-5, phase "
      "A). The")
    w("// same data as arena_contract.json; the rules the referee decides by "
      "are listed")
    w("// there and in referee.py. SYNS (own-body state) is ARENACORE1's "
      "layout and")
    w("// lives in its own header; the referee's kinds are here. Little "
      "endian, packed.")
    w("// Accept SYNR / SYNQ(20) only from the relay address with byte 5 == "
      "0xFE.")
    w("")
    w("#ifndef PORT_ARENA_WIRE_H")
    w("#define PORT_ARENA_WIRE_H")
    w("")
    w("#include <cstddef>")
    w("")
    w("namespace arena_wire {")
    w("")
    w("enum : int {")
    w("    kContractVersion = %d," % CONTRACT_VERSION)
    w("    kRoleArena       = %d,      // HELLO v2 role byte"
      % SESSION["role_arena"])
    w("    kRefereeMark     = 0x%02X,   // byte 5 of SYNR / SYNQ(20), byte 6 "
      "of SYNP(20)" % SESSION["referee_mark"])
    w("    kSynsBytes       = 48,")
    w("    kSynoBytes       = 32,")
    w("    kSyneBytes       = 24,")
    w("    kRefPingBytes    = 20,")
    w("    kSynrBytes       = %d," % SYNR_LEN)
    w("    kSynrHeaderBytes = 48,")
    w("    kSynrEventBytes  = 28,")
    w("    kSynrEvents      = %d," % SYNR_EVENTS)
    w("};")
    w("")
    w("enum : unsigned {   // first four bytes as a little-endian u32")
    for tag in ("SYNS", "SYNO", "SYNE", "SYNR"):
        v = int.from_bytes(tag.encode("ascii"), "little")
        w("    kTag%s = 0x%08xu,   // '%s'" % (tag[3], v, tag))
    w("};")
    w("")
    w("enum Claim : unsigned char {")
    for i, n, t, f in CLAIMS:
        w("    CLAIM_%s = %d," % (n, i))
    w("};")
    w("")
    w("enum Event : unsigned char {")
    for i, n, f in EVENTS:
        w("    EV_%s = %d," % (n, i))
    w("};")
    w("")
    w("enum Deny : unsigned char {")
    for i, n, d in DENY_REASONS:
        w("    DENY_%s = %d,   // %s" % (n, i, d))
    w("};")
    w("")
    w("enum Over : unsigned char {")
    for i, n in OVER_REASONS:
        w("    OVER_%s = %d," % (n, i))
    w("};")
    w("")
    w("enum Mode : unsigned char {   // READY rules byte 1")
    for i, n in MODES:
        w("    MODE_%s = %d,%s" % (n, i, "" if i in MODES_ACCEPTED
                                    else "   // refused (DENY_UNSUPPORTED)"))
    w("};")
    w("")
    w("enum Infect : unsigned char {   // EV_INFECTED field b")
    for i, n, d in INFECT_CAUSES:
        w("    INF_%s = %d,   // %s" % (n, i, d))
    w("};")
    w("")
    w("enum MatchState : unsigned char {")
    for n, v in MATCH_STATES.items():
        w("    MS_%s = %d," % (n.upper(), v))
    w("};")
    w("")
    w("enum : unsigned char {")
    for n, v in SYNR_FLAGS.items():
        w("    SYNR_F_%s = 0x%02x," % (n.upper(), v))
    for i, n in enumerate(SYNS_FLAGS):
        w("    SYNS_F_%s = 0x%02x," % (n.upper(), 1 << i))
    for n, v in SYNS_POWER.items():
        w("    SYNS_P_%s = 0x%02x," % (n.upper(), v))
    w("};")
    w("")
    w("// The numbers the referee decides with (ms unless named).")
    for k, v in NUMBERS.items():
        if isinstance(v, float):
            w("constexpr double k%s = %r;" % (_camel(k), v))
        else:
            w("constexpr int k%s = %d;" % (_camel(k), v))
    w("")
    w("#pragma pack(push, 1)")
    for line in _c_struct("RefClaim", SYNE_FIELDS, 24):
        w(line)
    w("")
    for line in _c_struct("RefPing", PING_FIELDS, 20):
        w(line)
    w("")
    for line in _c_struct("RefPong", PONG_FIELDS, 20):
        w(line)
    w("")
    for line in _c_struct("ObjFrame", SYNO_FIELDS, 32):
        w(line)
    w("")
    for line in _c_struct("RefEvent", EVENT_FIELDS, 28):
        w(line)
    w("")
    hdr = SYNR_HEADER + [["ev", 48, 28 * SYNR_EVENTS, "RefEvent[%d]"
                          % SYNR_EVENTS, "newest first"]]
    _CTYPE["RefEvent"] = "RefEvent"
    for line in _c_struct("RefFrame", hdr, SYNR_LEN):
        w(line)
    del _CTYPE["RefEvent"]
    w("#pragma pack(pop)")
    w("")
    w("// VS_STAR_SPAWN_ORDERS as the referee uses it (ROM 0x02075720).")
    w("constexpr unsigned char kStarOrders[6][12] = {")
    for row in STAR_ORDERS:
        w("    { %s }," % ", ".join(str(v) for v in row))
    w("};")
    w("")
    w("}  // namespace arena_wire")
    w("")
    w("#endif  // PORT_ARENA_WIRE_H")
    return "\n".join(L) + "\n"


def _camel(k):
    return "".join(p.capitalize() for p in k.lower().split("_"))


def main(argv):
    want = {JSON_PATH: render_json(), H_PATH: render_h()}
    if "--write" in argv:
        for path, text in want.items():
            with open(path, "w", newline="\n") as fh:
                fh.write(text)
            print("wrote %s" % os.path.basename(path))
        return 0
    stale = []
    for path, text in want.items():
        try:
            with open(path, "r", newline="") as fh:
                have = fh.read()
        except OSError:
            have = None
        if have is None or have.replace("\r\n", "\n") != text:
            stale.append(os.path.basename(path))
    if stale:
        print("STALE: %s (run arena_contract.py --write)" % ", ".join(stale))
        return 1
    print("arena contract files up to date")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
