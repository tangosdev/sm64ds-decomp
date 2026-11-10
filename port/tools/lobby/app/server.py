#!/usr/bin/env python3
"""sm64ds-lobby - room state and chat for the PC port's Multiplayer button.

STAGE A: create, join, poll, chat, params, kick, leave, health.
STAGE B (params/start/race): `start` arms a match, `ready` confirms a launcher
can spawn it, `failed` aborts it back to lobby -- and NOTHING spawns until every
playing seat has readied. The seat count is frozen at `start` and forced into
every plan (SM64DS_VS_PLAYERS), which is the join-race guarantee (section 4).
STAGE C (rematch): `result` reports a finished match, returns the room to lobby
with the params intact, and discards the per-match comms code so the next match
is a fresh relay session.

`kick` is the spec's own open question 4, which the owner answered yes to. It
is the twelfth verb rather than one of the eleven the spec froze, and it needs
no version bump because nothing has shipped against version 1 yet. It reuses
the `left` event with the `why: "kicked"` the event grammar already carries.

WHAT THIS PROCESS IS FOR, AND WHAT IT REFUSES TO BE. It holds a handful of
rooms in memory so two launchers can find each other and talk. That is all. It
has no database, no disk, no credential of any kind, and it never makes an
outbound request. It does not speak to the relay and the relay does not speak
to it, so a lobby outage cannot touch a match that is already running.

It is also the only unauthenticated write surface in the whole port kit, which
is why it runs in its own container rather than inside port_ingest: an open
handler must not share a process with the thing that writes the release zips
every launcher downloads and runs.

THE SECURITY LAW, in the order it is applied to every request:
  1. Content-Length is parsed BEFORE any rejection (see the drain trap below).
  2. The sender's rate budget is charged before any parsing work is done.
  3. The path must be exactly one of the verbs this file serves.
  4. Every field is checked against a fixed grammar. An unknown key is a 400,
     not an ignore: the launcher and this server ship together, so a field that
     turns up without a version bump is a bug or an attack and there is no
     third option.
  5. Nothing a client sends is ever parsed for meaning, logged, used in a file
     name, or interpolated into anything. `nick` and `text` are opaque bytes
     that go in and come back out.

THE KEEP-ALIVE DRAIN TRAP, carried over from port_ingest rather than
rediscovered. With HTTP/1.1 keep-alive, replying to a request WITHOUT reading
the body it announced leaves those bytes in the socket, and the next read
parses the middle of that body as a request line. One rejected request then
appears twice, once as its real status and once as a phantom 400. So: parse
Content-Length first, and reject only through `_reject`, which drains the
announced body (bounded) before it answers.
"""

import hashlib
import hmac
import json
import os
import random
import socket
import sys
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

REVISION = "lobby-6"

# ------------------------------------------------------- the contract version
#
# The contract is versioned, and the version is NEGOTIATED rather than pinned,
# because section 3.0 makes an unknown key a 400 and says "adding a field is a
# `v` bump". A hard bump would refuse every shipped launcher on the next
# restart, which is the opposite of what the rule is for. So:
#
#   * a request may carry any `v` from CONTRACT_MIN to CONTRACT_V,
#   * the answer echoes the version the request used,
#   * a field introduced at v2 is a `bad_field` at v1, exactly as strict as
#     before for a client that claims to speak v1.
#
# THERE IS ONE v2 AND IT IS THE UNION, defined here and nowhere else.
#
# This needs saying because it was very nearly not true. Two lanes grew the
# contract on the same night over fields that did not overlap -- the player
# dial (`match_players` on `params`) and the colour picker (`color`/`shoes` on
# create/join, and the `color` verb) -- and each of them, reasonably, called
# its own addition "v2". Merged, that would have shipped a number that means
# two different things: a launcher built against either lane would announce
# `"v":2` and support half of what v2 is, and the half it did not know about
# would fail as `bad_field` from a server that considers itself compatible.
# A version is a PROMISE ABOUT A WHOLE FIELD SET, so it can only ever have one
# definition, and `VERB_FIELDS` below is that definition.
#
# v1: create/join/poll/chat/params/preflight/start/ready/result/failed/kick/
#     leave, as shipped in launcher 0.3.0.
# v2: EVERYTHING BELOW, together, or it is not v2:
#       * `params` may carry `match_players`  - the host's player-count dial
#       * `create`/`join` may carry `color` + `shoes`
#       * the `color` verb exists at all      - a v2-ONLY VERB
#     A client claiming v2 is claiming all three. There is no half of v2.
#
# (The history below is kept because the field table still records the version
# each field arrived in. Since v5 the floor is v5, so none of it is negotiable
# any more: see v5 at the bottom of this block.)
#
# v3, run vs16: the sixteen-player wire. The version itself gains no new FIELD
# -- `match_players` has been a v2 optional since SEAT16 -- but it gains a new
# legal RANGE for that field, and the two are not the same kind of change:
#
#   a new field  a v2 client cannot send it, so it cannot be hurt by it
#   a new range  a v2 client CAN send 8, and would then be handed a
#                sixteen-field SM64DS_VS_NAMES its own validator refuses,
#                spawn nothing, and have no idea why
#
# So the range was gated on the version too, and that is what made v3 a real
# version rather than a number that went up (retired by the v5 floor).
#
# v4, ADVENTURE co-op (M2): a room gains an optional `session_type` field on
# `create`. Default "vs" -- an unset field is exactly the versus match every
# room was before this version -- and "adventure" for the state-broadcast
# ghost session (status/ADVENTURE.md). A v1..v3 client cannot send it, so it
# cannot be hurt by it, and a room it creates is a versus room as always. The
# type rides the go plan so the launcher can boot an adventure console
# (SM64DS_ADVENTURE=1 + SM64DS_LEVEL) instead of a versus arena
# (SM64DS_VS_MAP); the relay and the seam are already session-type blind, so
# nothing below the lobby changes.
#
# v5, MULTIPLAYER 1.0 (run mp1, NETARCH1 design section 5), AND THE FLOOR MOVES
# WITH IT. Every request below v5 is refused `bad_version`, which launchers
# 0.3.x..0.5.x already turn into "Check for an update". The negotiation above
# was right for additive versions; v5 is not additive, because the game under
# it changed: a 1.0 match needs a relay v2 seat, a server-minted Luigi seed and
# a session digest in every plan, and a pre-1.0 launcher would spawn a game
# that cannot pair with a 1.0 host (or worse, pairs and desyncs). So v5 is the
# only version this build speaks, and everything the older gates guarded (the
# narrow client in a wide room, the v2-only dial) is gone with the clients
# they guarded against. What v5 IS, all of it, or it is not v5:
#   * create/join carry `game`, the game build id; a join from another build
#     is 409 `game_mismatch`
#   * params (vs rooms) may carry `luigi` and `voice`; start mints `seed`
#   * every go plan carries relay_v, want_seat, luigi, seed, voice, digest
#   * adventure rooms: one hub code for the room's life, no start gate
#   * the `world` and `world_ping` verbs
# v6, PARTY AND COOP ROOMS (run mp1 FIELD5, lane ROOMS1). ADDITIVE: the floor
# stays 5, so a launcher that speaks v5 keeps its VS rooms and its world exactly
# as they were. What v6 adds, all of it, or it is not v6:
#   * create's session_type may be "party" or "coop" (a v5 caller sending
#     either is `bad_session_type`, the same refusal an unknown type always
#     got, which the launcher words as "Check for an update");
#   * create may carry `save_name` (coop only, required there): the display
#     name of the host's co-op save. The save's PATH is never sent to this
#     service, never stored, and a field that looks like one is refused;
#   * joining a party or coop room needs v6 (a v5 launcher would boot it as a
#     plain adventure room), same refusal;
#   * party and coop rooms have a host Start (`start`), unlike adventure
#     rooms: it releases the room, and the plan every member polls carries
#     `started` / `launch` so a launcher can launch together, and a later
#     joiner (who is handed a started plan) launches on joining.
#   * ROOM SETTINGS (ROOMSET1, additive within v6): create may carry
#     `settings`, an object of 0/1 toggles for a party or coop room (see
#     ROOM_SETTINGS below); the `settings` verb lets the host change them
#     while the room has not started. Every plan of a party or coop room
#     carries the room's `settings`, so each launcher can turn them into env.
#     A v6 create without `settings` gets the defaults, exactly as before.
CONTRACT_MIN = 5
CONTRACT_V = 6
V_SESSION = 4    # create.session_type
V_ROOMS = 6      # session_type party / coop, create.save_name

# The session types a room may be. "vs" is the versus match; "adventure" is the
# ghost-presence session (design section 1): every console plays its own save
# from the title, and the relay's hub rules carry only ghost frames, identity,
# chat and voice between them. "party" (the shared rollback world) is NOT a 1.0
# room type (Tango, 09-28): it is refused as an unknown value at do_create.
SESSION_TYPES = ("vs", "adventure", "party", "coop")

# Run mp1 FIELD5: "party" is now a 1.0-line room type, and so is "coop". Both are
# HUB rooms like an adventure room (one relay hub code for the room's life, a
# plan at create/join and on every poll, the room outlives nobody it should not)
# with a host Start on top. NOT the old rollback "party" of the 09-02 slice,
# which was a shared synced world; this one shares stars and bodies only, and
# coop is the synced world. Neither type uses the VS dial, ready, go or result.
ROOM_KINDS = ("party", "coop")
HUB_TYPES = ("adventure", "party", "coop")

# ROOM SETTINGS (ROOMSET1). Player-to-player behaviour in a party or coop room
# is a set of toggles the HOST picks when the room is made (Tango: a room
# toggle, not a build switch). Each is 0 or 1 and each has one name here, which
# is also the launch env the game reads (SM64DS_ROOM_<NAME> upper-cased):
#   collision  1 = friends are solid bodies, 0 = ghosts (SM64DS_ROOM_COLLISION)
#   interact   1 = hits / stomps / eats count, 0 = none (SM64DS_ROOM_INTERACT)
#   damage     1 = a friend's hit can hurt a friend, 0 = off (SM64DS_ROOM_DAMAGE)
#   tp         1 = TP to friends, coop only (SM64DS_ROOM_TP)
#   starshare  1 = stars are shared, party only (SM64DS_ROOM_STARSHARE)
#   soloexit   1 = only the player who collects a star leaves the level, 0 =
#              everyone in it leaves (SM64DS_ROOM_SOLOEXIT, party and coop;
#              STAREXIT1). A key still empties its boss room either way.
# Each kind takes only its own names: a party room has no tp, a coop room no
# starshare. An unknown name, or a name of the other kind, is refused.
ROOM_SETTINGS = {
    "party": ("collision", "interact", "damage", "starshare", "soloexit"),
    "coop": ("collision", "interact", "damage", "tp", "soloexit"),
}
# A name a client never sent (an older launcher) is its default here, so an
# older launcher's room still gets soloexit 1.
ROOM_SETTING_DEFAULTS = {"collision": 1, "interact": 1, "damage": 0,
                         "tp": 1, "starshare": 1, "soloexit": 1}

# The longest and the allowed shape of a coop save's DISPLAY name. Opaque text
# the lobby only stores and echoes. The characters that make a path (separators,
# a drive colon, wildcards, quotes, pipes) are refused outright, so a launcher
# bug or a hostile client cannot make this service carry a path, and the name
# is safe to show anywhere. The launcher derives its own file name from it.
SAVE_NAME_MAX = 24
LAUNCH_NO_MAX = 1000000
SAVE_NAME_BAD = "\\" + '/:*?"<>|'

# v5 ADVENTURE ROOMS. The seat the plan asks the relay for: "any free seat".
# The relay assigns the lowest free hub seat and stamps it; the lobby does not
# hand out adventure seats, because people come and go without it.
WANT_ANY_SEAT = 255

# The version each v2 addition arrived in. All three are 2 and all three say so
# by name rather than by the digit, so a later bump cannot accidentally lock v2
# clients out of the fields v2 introduced -- and so the union above is legible
# as three entries rather than as one number repeated.
V_DIAL = 2       # params.match_players
V_COLOR = 2      # create/join color+shoes, and the `color` verb itself
# Voice chat is shelved: every room's voice is 0 and every plan says so. The
# code paths stay; flip this to bring the host's switch back.
VOICE_SHELVED = True
V_ONE = 5        # the 1.0 fields: game, luigi, voice; the world verbs

# ------------------------------------------------------------------ config
#
# Everything tunable comes from the environment so the box can be retuned with
# a compose edit and a restart, with no code change and no release.


def env_int(name, default, lo, hi):
    """An integer knob, clamped. A junk value is the default, never a crash."""
    try:
        return max(lo, min(hi, int(os.environ.get(name, default))))
    except (TypeError, ValueError):
        return default


def env_str(name, default):
    return os.environ.get(name, default) or default


LISTEN_PORT = env_int("LISTEN_PORT", 8091, 1, 65535)
LISTEN_ADDR = env_str("LISTEN_ADDR", "0.0.0.0")

MAX_ROOMS = env_int("MAX_ROOMS", 64, 1, 4096)
# SECREV1: the room table is shared by everybody, and rooms are cheap to make
# (30 an hour per address), so three addresses could hold all 64 for as long as
# they kept them alive. One address may host at most this many rooms at once;
# the answer is the existing 429 too_fast, which every launcher already shows.
ROOMS_PER_ADDR = env_int("ROOMS_PER_ADDR", 3, 1, 64)

# THE HARD CEILING ON THE DIAL. Sixteen is the number the owner asked the dial
# to reach, and it is written here once so that every other bound in this file
# is derived from it rather than typed again. It is a bound on what the LOBBY
# will express, not a claim about what the GAME can run: see the ceiling map in
# README.md. The game's proven ceiling today is four.
DIAL_HARD_MAX = 16

# How many seats a room holds, and how many of them the deployment's game build
# can actually play. Both clamps now reach DIAL_HARD_MAX so that raising the
# ceiling is a compose edit, and both DEFAULTS are unchanged, so a deployment
# that edits nothing behaves exactly as it did.
MAX_SEATS = env_int("MAX_SEATS", 4, 2, DIAL_HARD_MAX)
GAME_MAX_PLAYERS = env_int("GAME_MAX_PLAYERS", 2, 1, DIAL_HARD_MAX)
MAX_WAITERS = env_int("MAX_WAITERS", 96, 1, 4096)

# SEATS IN A PARTY ROOM AND IN A COOP ROOM (run mp1 FIELD5: Tango ruled 4 for
# both). Their own knobs, apart from MAX_SEATS (the VS dial's room size), so a
# VS deployment raised to sixteen does not change them. The game's hub holds
# four bodies today; the wire and this service are sized so eight or sixteen is
# a later bump of these two numbers (and of the game), not a rewrite.
PARTY_SEATS = env_int("PARTY_SEATS", 4, 2, DIAL_HARD_MAX)
COOP_SEATS = env_int("COOP_SEATS", 4, 2, DIAL_HARD_MAX)

# THE DIAL'S SERVER-ENFORCED UPPER BOUND, derived, never configured directly.
# A host cannot pick a number bigger than the room holds, and cannot pick a
# number bigger than the deployment says the game can run. Advertised in every
# room view as `dial_max` so the launcher greys the rest of the control instead
# of finding out by being refused.
#
# NOT floored at 2. A deployment that sets GAME_MAX_PLAYERS=1 gets DIAL_MAX=1
# and every dial value is refused, which is honest: `start` already needs two
# playing seats, so such a deployment could never start a match anyway, and
# flooring the dial at 2 would have quietly made its joiners playing members.
DIAL_MAX = 0

# What a room's dial reads before the host touches it: the deployment's own
# capability, so a room that nobody configures plays the way it always did.
# With the compose file's GAME_MAX_PLAYERS=4 this is 4, which is byte-for-byte
# the seating rule this service had before the dial existed.
DIAL_DEFAULT = 0


def recompute_dial():
    """Derive the dial's bounds from the two knobs. Called at import, and by
    the unit tests after they move a knob -- which is exactly what a compose
    edit plus a restart does. A function rather than two module-level
    expressions so a test cannot restate the derivation and then drift from
    it."""
    global DIAL_MAX, DIAL_DEFAULT
    DIAL_MAX = min(DIAL_HARD_MAX, MAX_SEATS, GAME_MAX_PLAYERS)
    DIAL_DEFAULT = DIAL_MAX


recompute_dial()

# THE WIDTH AT WHICH THE GAME'S WIRE CHANGES, and therefore the width at which
# SM64DS_VS_NAMES and SM64DS_VS_COLORS change shape. Four or fewer is wire
# version 2 and four fields; five or more is wire version 3 and sixteen fields.
# The game derives the same boundary from the same number
# (port/hal/comms_loopback.cpp, THE TWO WIRES), which is what stops the service
# and the game from having two opinions about one match.
NARROW_PLAYERS = 4

# (Until v5 a caller below v3 was held to NARROW_PLAYERS here, because a v2
# launcher validates SM64DS_VS_NAMES at exactly four fields. The v5 floor
# retired every such launcher, so the dial's bound is the deployment's alone.)

# The largest "first to N stars" a host may pick. FIVE TODAY BECAUSE AN ARENA
# HOLDS FIVE STARS, not because the number five is special: the owner picks the
# target, and a mode that went past five would need the game to respawn stars.
# It is a knob rather than a literal so that raising it is a compose edit on the
# day the game side lands, and so no reader has to hunt for a scattered 5.
STAR_TARGET_MAX = env_int("STAR_TARGET_MAX", 5, 1, 99)

# KING OF THE STAR: the largest "first to N points" a host may pick, and the
# default a launcher pre-fills. One star spawns for the whole match; whoever
# holds it earns +1 point per real second, and the first to `king_target`
# points wins. Points are seconds-of-possession, so the target reads as
# "seconds you must hold in total". 30 is a brisk round; the ceiling is a knob
# rather than a literal so raising it is a compose edit. Unlike star_target
# this needs no arena respawn -- the single star is transferable, not consumed.
KING_TARGET_MAX = env_int("KING_TARGET_MAX", 999, 1, 9999)
KING_TARGET_DEFAULT = env_int("KING_TARGET_DEFAULT", 30, 1, 9999)

# How long a kicked address is refused by the room that kicked it.
#
# KEYED ON THE CLIENT ADDRESS, WHICH IS THE ONLY HANDLE THIS DESIGN HAS. There
# are no accounts by the owner's own ruling, and a member token is minted per
# join, so "the same person" is not a thing this service can recognise. Two
# consequences, both stated rather than hidden: a kicked player who changes
# address walks straight back in, and a second player in the same house shares
# the cooldown. That is why it is a COOLDOWN and not a ban for the room's
# lifetime -- a permanent block on a shared address would lock out an innocent
# housemate for as long as the room lived. The real remedy for someone who will
# not leave is still the host closing the room, which costs one button.
KICK_COOLDOWN_S = env_int("KICK_COOLDOWN_S", 120, 0, 86400)

MEMBER_TIMEOUT_S = env_int("MEMBER_TIMEOUT_S", 45, 5, 3600)
ROOM_IDLE_S = env_int("ROOM_IDLE_S", 1800, 30, 86400)
CLOSED_KEEP_S = env_int("CLOSED_KEEP_S", 60, 5, 3600)
# v5: an adventure room outlives its creator and closes only after it has been
# empty this long (design section 5: ten minutes). It is exempt from
# ROOM_IDLE_S, because a room whose members are all out playing pushes no
# events for as long as they play.
ADV_EMPTY_S = env_int("ADV_EMPTY_S", 600, 30, 86400)

# v5 WORLD ROOMS (design section 5): always-on adventure hubs that nobody
# creates. The pool is per game build; a world holds WORLD_SEATS members seen
# in the last WORLD_MEMBER_S seconds (the launcher sends `world_ping` every
# 10 s while its game runs), and a world with no live member for WORLD_EMPTY_S
# is dropped. MAX_WORLDS bounds the whole pool (every build together).
# WORLD_EMPTY_S is 30 s, not the ten minutes it started at (review REVLOBBY5,
# S2): a world nobody plays in costs nothing to reopen, while ten minutes made
# a burst of opens cheap to hold for a long time.
WORLD_SEATS = env_int("WORLD_SEATS", 32, 2, 32)   # WORLD32: a world holds 32
WORLD_MEMBER_S = env_int("WORLD_MEMBER_S", 30, 5, 3600)
WORLD_EMPTY_S = env_int("WORLD_EMPTY_S", 30, 5, 86400)
MAX_WORLDS = env_int("MAX_WORLDS", 64, 1, 4096)
# THE PER-ADDRESS CAP (review REVLOBBY5, S2): how many live world tokens one
# client address may hold across the whole pool. Four covers a household
# behind one connection. A `world` call from an address already at the cap
# retires that address's least recently heard token first, so a relaunch or a
# re-ask never locks a real player out, and one address can never stand for
# more than this many members (padding a world full of phantoms, or holding
# worlds open, then takes WORLD_SEATS / this many addresses per world).
WORLD_ADDR_TOKENS = env_int("WORLD_ADDR_TOKENS", 4, 1, WORLD_SEATS)

# Stage B knobs. Read here so the compose file and this file cannot drift, and
# so `health` can report what the room semantics would be, but nothing in stage
# A arms a match.
ARM_DEADLINE_S = env_int("ARM_DEADLINE_S", 20, 5, 120)
GO_GRACE_S = env_int("GO_GRACE_S", 5, 1, 60)
MATCH_TIMEOUT_S = env_int("MATCH_TIMEOUT_S", 1200, 60, 86400)
# run mp1 L-J1B: every live non-host seat has posted the SAME result and the
# host has sent nothing since the first of those votes for this long: the
# match is over (see host_silent_vote). Not in the compose file on purpose:
# the default is the reviewed value. The floor is the member heartbeat (45 s):
# under the launcher's 20 s poll hold plus its error backoff a LIVE host can
# look silent (REVLOBBYJOIN1-B note D: at 10 a live host's match ended on the
# guest's draw after 16 s).
HOST_SILENT_S = env_int("HOST_SILENT_S", 60, 45, 3600)
RELAY_ADDR = env_str("RELAY_ADDR", "135.148.26.201:41234")

# v5: the relay protocol the plan tells the game to speak (design section 3).
# 2 once relay v2 is deployed (S1). A 1.0 game that meets a v1 relay falls back
# on its own ("bad request" to the v2 HELLO), so 2 is safe to ship before the
# deploy; 1 is the operator's switch if the deploy is rolled back, and it also
# turns the world pool off (a world room needs the relay's hub rules).
RELAY_V = env_int("RELAY_V", 2, 1, 2)

RATE_REQ_PER_S = env_int("RATE_REQ_PER_S", 20, 1, 10000)
RATE_BURST = env_int("RATE_BURST", 40, 1, 20000)
RATE_CREATE_PER_HOUR = env_int("RATE_CREATE_PER_HOUR", 30, 1, 100000)
RATE_JOIN_PER_MIN = env_int("RATE_JOIN_PER_MIN", 60, 1, 100000)

CHAT_EVERY_S = 2.0          # one message per two seconds per member...
CHAT_BURST = 3              # ...with three saved up

# The bad-sender rule, and it is a rate limit like the rest, so like the rest
# it comes from the environment: a sender refused BAD_LIMIT times inside
# BAD_WINDOW_S seconds is refused outright for BAD_IGNORE_S. Same shape
# relay.py uses. Setting BAD_LIMIT very high is how a test suite made of
# nothing but deliberate refusals stays out of its own penalty box.
BAD_LIMIT = env_int("BAD_LIMIT", 10, 1, 1000000)
BAD_WINDOW_S = float(env_int("BAD_WINDOW_S", 10, 1, 3600))
BAD_IGNORE_S = float(env_int("BAD_IGNORE_S", 10, 0, 3600))

MAX_BUCKETS = 8192          # hard ceiling on tracked source addresses
SWEEP_INTERVAL_S = 1.0
STATS_INTERVAL_S = 60.0

# Bodies larger than this are refused unread.
BODY_MAX = 4096

# The deadline on every blocking socket read for one connection: reading the
# request line, reading the headers, reading the body. Without it a client that
# announces a body and then sends it one byte a decade holds its worker thread
# forever, and enough such clients from one host take the whole container down
# (a thread is a pid, and the compose caps pids). socketserver applies this to
# the connection in setup(), and handle_error already swallows the TimeoutError
# it raises, so a stalled connection is dropped in silence. A long poll does
# NOT ride this deadline: it blocks on a Condition, not on a socket read, and it
# is capped by WAIT_MAX and MAX_WAITERS instead. Env-overridable only so the
# security test can drive it low; 20 s is the production value.
HANDLER_TIMEOUT_S = env_int("HANDLER_TIMEOUT_S", 20, 1, 3600)

EVENT_RING = 100            # events a room remembers
CHAT_RING = 40              # chat lines a room remembers, for a resync

WAIT_MAX = 25               # seconds a long poll may hold

# Whether X-Forwarded-For is trusted for the client address. ON by default
# because the only deployment is behind our own Caddy on a private network,
# where the socket peer is always Caddy and per-IP limits would otherwise be
# per-SERVER limits. Turn it off for a deployment that is reachable directly.
TRUST_XFF = env_int("TRUST_XFF", 1, 0, 1) == 1

# ------------------------------------------------------------------ grammar

ROOM_ALPHABET = "ABCDEFGHJKMNPQRSTUVWXYZ23456789"   # no I, L, O, 0, 1
HEX_ALPHABET = "0123456789abcdef"
ROOM_LEN = env_int("ROOM_LEN", 6, 4, 8)
TOKEN_LEN = 32              # 128 bits
COMMS_LEN = 8               # stage B; kept here so the two lengths sit together

NICK_MAX = 16
TEXT_MAX = 200
DISPLAY_MAX = 20            # NICK_MAX + " (n)"
GAME_NAME_MAX = 16

MATCH_LEN = 16             # a match id, 16 lowercase hex, minted per arm

COLOR_LEN = 6              # a picked colour, "rrggbb", stored lower case

# v5: the game build id a launcher sends on create/join/world. The game's own
# version string, 1..24 of [0-9A-Za-z._-]; compared byte for byte, never parsed.
GAME_ID_MAX = 24
GAME_ID_ALPHABET = ("0123456789abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ._-")


def game_id_ok(s):
    return (isinstance(s, str) and 1 <= len(s) <= GAME_ID_MAX
            and all(ch in GAME_ID_ALPHABET for ch in s))


def parse_game_ids(raw):
    """GAME_IDS: the real game build ids this server serves, comma-separated
    (the launcher's `game`, e.g. "1.0.0-0123abcd"). The release step writes it
    into docker-compose.yml. Returns (ids, dropped): an entry outside the
    build-id grammar is dropped (and said so at start), never half-used."""
    ids, dropped = [], []
    for part in (raw or "").split(","):
        part = part.strip()
        if not part:
            continue
        (ids if game_id_ok(part) else dropped).append(part)
    return tuple(dict.fromkeys(ids)), dropped


# THE BUILD ALLOW LIST (review REVLOBBY5, S2). A world is keyed by build id,
# so a made-up id used to open a fresh world per call and one home connection
# filled the pool in a minute. With a list, every create/join/world naming a
# build not on it is refused 409 `unknown_game` (the launcher says "check for
# an update"). EMPTY (unset) is the safe default, not the open one: code rooms
# then take any well-formed build (a dev box, a test harness) and the WORLD
# POOL IS OFF (503 world_off), so a deploy that forgot the compose line cannot
# reopen the hole.
GAME_IDS, GAME_IDS_DROPPED = parse_game_ids(os.environ.get("GAME_IDS", ""))


def game_allowed(game):
    return not GAME_IDS or game in GAME_IDS


def world_open():
    """The pool needs the relay's hub rules (RELAY_V 2) and a build list."""
    return RELAY_V >= 2 and bool(GAME_IDS)


# ARENA MODE (run mp1, lane A4; VSSYNC1 plan sections 6 and 8). A VS match is
# played one of two ways, and the go plan's `netmode` says which:
#   "lockstep"  every console waits on every other's inputs (1.0's VS until
#               now, and the fallback for one release)
#   "arena"     every console runs its own Yoshi with no input delay, the
#               others are drawn slightly behind, and a referee on the relay
#               settles stars, hits, the clock and the winner
# The game picks its relay session from the netmode; the lobby still never
# speaks to the relay. Arena needs a game build that has it and a relay that
# runs the referee, so it is OFF until the deploy names the build here.
NETMODES = ("arena", "lockstep")

# ARENA_GAME_IDS: the builds that play arena, comma-separated, same grammar as
# GAME_IDS. EMPTY (the default) = arena off for every build, which is exactly
# this server before arena existed. "*" = every build: dev and test boxes
# only, never the live compose file (a build without arena would be handed a
# netmode it does not know; it would still refuse at its own door, because
# the digest counts the netmode, but nobody should rely on that).
_ARENA_RAW = (os.environ.get("ARENA_GAME_IDS", "") or "").strip()
ARENA_ANY = _ARENA_RAW == "*"
ARENA_GAME_IDS, ARENA_GAME_IDS_DROPPED = (
    ((), []) if ARENA_ANY else parse_game_ids(_ARENA_RAW))

# The largest arena match. Phase A is 2 to 4 players; phase B raises it with a
# compose edit. A bigger match plays lockstep.
ARENA_MAX_PLAYERS = env_int("ARENA_MAX_PLAYERS", 4, 2, DIAL_HARD_MAX)

# A new room's pick. Unknown values read as the default, like every knob.
NETMODE_DEFAULT = env_str("NETMODE_DEFAULT", "arena")
if NETMODE_DEFAULT not in NETMODES:
    NETMODE_DEFAULT = "arena"

# The win modes arena can referee today. King of the Star stays on lockstep
# until the referee has its rules (plan phase C). Luigi Infection is not a win
# mode (it rides on `luigi` with win_mode "time"); ARENA_LUIGI_GAME_IDS below
# decides whether a Luigi match may use the referee.
ARENA_WIN_MODES = ("time", "stars")

# ARENA_LUIGI_GAME_IDS: the builds whose Luigi Infection matches play arena
# (relay referee contract 2: READY mode 2, EV_INFECTED, end reasons 5 and 6).
# Same grammar as ARENA_GAME_IDS, and a build must be on BOTH lists. EMPTY (the
# default) = every Luigi match plays lockstep, exactly this server before the
# referee knew Luigi. "*" = every arena build: dev and test boxes only.
# Keyed on the exact build id, never a minimum version: the server compares
# ids and never parses them, and an id names one build (version + git tip), so
# a build only gets here when the deploy names it. A build that predates the
# referee's Luigi rules would sit on the countdown forever if it were handed an
# arena Luigi plan, and nothing in its request can say so.
#
# Who can be in such a room: only that build. A join must carry the room's own
# `game` (ONE GAME BUILD PER ROOM, room_join), so an older game that tries to
# join is refused at the door with 409 `game_mismatch`, words every 1.0 game
# and launcher already show ("This room runs a different game version.
# Everyone needs the same update."). It never reaches the countdown. A Luigi
# room made by an older build is not on this list and plays lockstep as today.
_ARENA_LUIGI_RAW = (os.environ.get("ARENA_LUIGI_GAME_IDS", "") or "").strip()
ARENA_LUIGI_ANY = _ARENA_LUIGI_RAW == "*"
ARENA_LUIGI_GAME_IDS, ARENA_LUIGI_GAME_IDS_DROPPED = (
    ((), []) if ARENA_LUIGI_ANY else parse_game_ids(_ARENA_LUIGI_RAW))


def arena_ok(room):
    """True when this room's build can play arena on this server at all: the
    build is on ARENA_GAME_IDS (or the list is "*") and the plans name relay
    v2. Says nothing about the match's settings; arena_for does that."""
    if RELAY_V < 2:
        return False
    return ARENA_ANY or room.game in ARENA_GAME_IDS


def arena_luigi_ok(room):
    """True when this room's build can play Luigi Infection on the arena
    referee: it can play arena at all (arena_ok) and it is on
    ARENA_LUIGI_GAME_IDS (or that list is "*")."""
    if not arena_ok(room):
        return False
    return ARENA_LUIGI_ANY or room.game in ARENA_LUIGI_GAME_IDS


def arena_for(room, players, win_mode, luigi):
    """The netmode a match with these settings plays in this room. Arena only
    when the host picked it, the build and relay can play it, the match is
    2..ARENA_MAX_PLAYERS players, the win mode is one the referee knows and,
    with Luigi on, the build is on ARENA_LUIGI_GAME_IDS; lockstep otherwise.
    Never an error: a host who picked arena for a six-player King match just
    plays lockstep, and the view says so."""
    if (room.session_type == "vs" and room.netmode == "arena"
            and arena_ok(room)
            and 2 <= players <= ARENA_MAX_PLAYERS
            and win_mode in ARENA_WIN_MODES
            and (not luigi or arena_luigi_ok(room))):
        return "arena"
    return "lockstep"

# WHAT A COLOUR-CARRYING REQUEST NEEDS TO CLAIM: see V_COLOR at the top of this
# file, and VERB_FIELDS for where it is enforced. This block used to declare a
# PROTO_VERSIONS/PROTO_COLOR_V pair of its own, which was the cross-lane defect:
# two independent definitions of "v2" over disjoint field sets. They are gone
# and the union at the top is the only definition.
#
# The compatibility story, which is unchanged and worth keeping here. A v1
# launcher never sends a colour and never reads one, so it plays in a coloured
# room with a built-in Yoshi and nothing else about it changes. A v1 REQUEST
# that carries a colour field is refused as `bad_field` -- the version is what
# says whether a field may appear, so honouring a v2 field under a v1 header
# would make the bump decorative. Answers echo back the version the caller sent.
#
# The new keys DO appear in every `view` and every plan regardless of version,
# and that is safe rather than sloppy: an answer is JSON and the launcher's
# reader ignores a property it has no field for -- the same leniency the event
# `kind` set already relies on (LobbyClient.cs, "the launcher IGNORES A KIND IT
# DOES NOT KNOW"), and pinned by the launcher's own unknown-field test.
# Versioning what a client may SEND is what protects the server; versioning
# what it may RECEIVE would only mean two code paths.

# The reason a match could not run, a CLOSED enum and never free text
# (spec 3.9). "timeout" also covers the 20-minute match watchdog (spec 5.8).
FAIL_REASONS = ("spawn_failed", "no_pairing", "wrong_player_count",
                "startup_error", "user_cancelled", "timeout",
                # server-minted, never sent by a client: the arming deadline
                # passed with some playing seat not ready.
                "member_not_ready",
                # server-minted, never sent by a client (run mp1, L-J1): the
                # host left a VS room while its match was arming, going or
                # running, so the match ended for everybody and the host role
                # passed on (the `host` event that follows names the new one).
                "host_left")
SERVER_REASONS = ("member_not_ready", "host_left")

# What the game reports as the win condition, plus "draw" for an unparseable
# marker (spec 5.3). The server never interprets it; it echoes it in the event.
# run mp1: the game's marker also says king-target (King of the Star),
# luigi-all-infected and survivor-timeout (Luigi Infection, a team win), and a
# launcher that reads a go plan carrying "netmode" (this lobby) posts them as
# printed; an older launcher keeps to the first three.
WIN_VALUES = ("time-up", "star-target", "draw",
              "king-target", "luigi-all-infected", "survivor-timeout")

STATES = ("lobby", "arming", "go", "in_match", "closed")

_RNG = random.SystemRandom()

# ------------------------------------------------------------------- utils


def log(msg):
    """One line to stdout. Docker picks these up; `docker logs` shows them."""
    ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    sys.stdout.write("%s %s\n" % (ts, msg))
    sys.stdout.flush()


def blunt_host(host):
    """What a log line shows for a client address: nothing of it.

    Since 10-21 no part of a client address reaches a log (before, the first
    two octets did). Callers keep calling this so every log site stays one
    place to audit; the rate limiter keys on the real address in memory only.
    """
    return "client"


PATH_LOG_MAX = 80


def safe_path(path):
    """A request path made safe to write into a log line.

    The path is client-supplied and reaches the log on every reject. Two
    things it must not carry into a terminal an operator reads: length -- an
    8 KB path wrote an 8 KB log line, a 2000x amplification against a 4 KB body
    cap -- and control bytes, which is how a raw ANSI escape reached the log
    intact. So it is truncated and every byte outside 0x20..0x7E is shown as
    '?'. This is a log-hygiene measure, not the routing decision: do_POST still
    matches the real, untruncated path.
    """
    if not path:
        return "-"
    out = "".join(ch if 0x20 <= ord(ch) <= 0x7E else "?"
                  for ch in path[:PATH_LOG_MAX])
    if len(path) > PATH_LOG_MAX:
        out += "...(%d)" % len(path)
    return out


def byte_len(s):
    """UTF-8 length, or -1 for a string that cannot be encoded at all.

    json.loads happily produces lone surrogates out of a \\udXXX escape, and
    those raise on encode. A field that cannot even be measured is a bad field.
    """
    try:
        return len(s.encode("utf-8"))
    except (UnicodeEncodeError, AttributeError):
        return -1


def printable_ascii(s, allow_comma=True):
    """Every byte in 0x20..0x7E, and optionally not the comma.

    Comma is barred from a nickname and nowhere else, because the four
    nicknames reach the game in one comma-separated environment variable
    (SM64DS_VS_NAMES) and barring one character is cheaper than any escaping
    scheme. Chat text never leaves the lobby, so it keeps its commas.
    """
    for ch in s:
        o = ord(ch)
        if o < 0x20 or o > 0x7E:
            return False
        if not allow_comma and o == 0x2C:
            return False
    return True


def new_token():
    return "".join(_RNG.choice(HEX_ALPHABET) for _ in range(TOKEN_LEN))


def new_room_code():
    return "".join(_RNG.choice(ROOM_ALPHABET) for _ in range(ROOM_LEN))


def new_match_id():
    return "".join(_RNG.choice(HEX_ALPHABET) for _ in range(MATCH_LEN))


def new_comms_code():
    # 8 chars from the SAME alphabet as the room code, but MINTED FRESH PER
    # MATCH and never shown in any UI. Separating it from the room code is the
    # whole of section 2.1: reading your invite code out loud must not hand a
    # stranger a relay child seat. A fresh code per match also sidesteps the
    # relay's 90-second held-seat trap on a rematch (spec 5).
    return "".join(_RNG.choice(ROOM_ALPHABET) for _ in range(COMMS_LEN))


def new_seed():
    """The Luigi Infection seed for one match (v5): a u32 from the system RNG.

    Minted HERE, at `start`, for every VS match whether Luigi is on or off, so
    no launcher and no player chooses it and every console of the match reads
    the same number from the same plan.
    """
    return _RNG.getrandbits(32)


# THE SESSION DIGEST (design 2.3 + section 5). The game's ACCEPT check computes
# the same number from its own environment, so a console launched with an env
# that disagrees with the host's (a hand edit, a stale settings key, a
# different build's defaults) refuses at the door with exit code 3 instead of
# desyncing at frame 240. The canonical string, field by field:
#   map|players|win|target|king|luigi|seed|netmode|wire
#   map      SM64DS_VS_MAP 0..3            players  SM64DS_VS_PLAYERS 2..16
#   win      time | stars | king           target   star target, else 0
#   king     king target, else 0           luigi    0 | 1
#   seed     the u32 seed when luigi is 1, else 0 (the plan carries the
#            minted seed either way; an off room does not digest it)
#   netmode  the plan's "netmode": "lockstep", or "arena" (run mp1 A4)
#   wire     2 when players <= 4, else 3
# decimal integers, no padding, no spaces. FNV-1a 32 over the ASCII bytes.
# This is the game's own rule (hal/comms_loopback.cpp session_digest, the
# NETCORE1 session check), and test_units.py (test_session_digest) pins it
# with that lane's published examples.
NETMODE_ONLINE = "lockstep"
FNV32_OFFSET = 2166136261
FNV32_PRIME = 16777619


def fnv1a32(data):
    h = FNV32_OFFSET
    for b in data:
        h ^= b
        h = (h * FNV32_PRIME) & 0xFFFFFFFF
    return h


def session_canonical(map_id, players, win_mode, star_target, king_target,
                      luigi, seed, netmode=NETMODE_ONLINE):
    wire = 2 if players <= NARROW_PLAYERS else 3
    return "%d|%d|%s|%d|%d|%d|%d|%s|%d" % (
        map_id, players, win_mode,
        star_target if win_mode == "stars" and star_target else 0,
        king_target if win_mode == "king" and king_target else 0,
        1 if luigi else 0, (seed & 0xFFFFFFFF) if luigi else 0, netmode, wire)


def session_digest(*args, **kw):
    return fnv1a32(session_canonical(*args, **kw).encode("ascii"))


# --------------------------------------------------------------- room state
#
# ONE module-level lock guards every room, every member and every counter
# below it. Each room's Condition is built on that same lock, so a long poll
# releases the whole thing while it waits and re-takes it when it wakes.

LOCK = threading.Lock()
ROOMS = {}                  # code -> Room
WAITERS = 0                 # long polls currently held, server-wide

REQ_COUNT = 0
REJ_RATE = 0
REJ_SHAPE = 0
REJ_AUTH = 0

WORLDS = {}                 # hub code -> World (v5 world rooms)


class Member(object):
    __slots__ = ("seat", "token", "nick", "display", "playing", "pre_ok",
                 "armed", "seen", "chat_allowance", "chat_stamp", "last_addr",
                 "color", "shoes")

    def __init__(self, seat, token, nick, playing, pre_ok, now, addr="",
                 color="", shoes=""):
        self.seat = seat
        self.token = token
        self.nick = nick            # exactly as sent, never rewritten
        self.display = nick         # recomputed whenever the roster changes
        self.playing = playing
        self.pre_ok = pre_ok
        # The two colours this player picked, six lower-case hex each, or ""
        # for "I did not pick, give me the built-in". Always BOTH or NEITHER:
        # a Yoshi with a custom body and ROM boots is not a thing the owner
        # asked for and not a state worth having two of.
        self.color = color          # the body colour, and the lobby swatch
        self.shoes = shoes
        self.armed = False
        self.seen = now
        self.chat_allowance = float(CHAT_BURST)
        self.chat_stamp = now
        # The address this seat was last seen at, kept ONLY so a kick has
        # something to put its cooldown against. Never logged whole, never
        # sent to any client, and dropped with the member.
        self.last_addr = addr

    @property
    def game_name(self):
        """The string that reaches the game for the winner banner.

        Derived HERE, by the server, and copied into every member's plan at
        start, so that no launcher ever builds it and two launchers cannot
        disambiguate a duplicate nickname differently. Stage B spends it;
        section 3.4 of the spec says the server derives it, so it is derived.
        """
        return self.display[:GAME_NAME_MAX]

    @property
    def game_colors(self):
        """This seat's field of SM64DS_VS_COLORS: "bbbbbb:ssssss" or "".

        Derived HERE, like `game_name`, so no launcher ever assembles it and
        two launchers cannot disagree about a colour the way they could about a
        duplicate nickname.
        """
        if not self.color or not self.shoes:
            return ""
        return "%s:%s" % (self.color, self.shoes)

    def view(self):
        return {"seat": self.seat, "display": self.display,
                "playing": self.playing, "pre_ok": self.pre_ok,
                "armed": self.armed, "color": self.color, "shoes": self.shoes}


class Room(object):
    def __init__(self, code, now, game=""):
        self.code = code
        self.cond = threading.Condition(LOCK)
        self.members = {}       # seat -> Member
        self.by_token = {}      # token -> seat
        self.host_seat = 1
        self.state = "lobby"
        # ADVENTURE co-op (v4): "vs" (the versus match, the default and what
        # every room was before v4) or "adventure" (the ghost-presence session
        # where each console runs its own solo level). Set once at create and
        # constant for the room's life, so member_plan reads it live rather
        # than freezing a copy at arm. A v1..v3 create never carries the field
        # and so is always "vs".
        self.session_type = "vs"
        # PARTY / COOP rooms (v6): the host's Start releases the room. `started`
        # is False until then; `launch_no` counts the host's Starts (a second
        # Start while nobody's game is up relaunches everyone), and rides every
        # plan so a launcher launches ONCE per number. `save_name` is a coop
        # room's display name for its save: the path is never here.
        self.started = False
        self.launch_no = 0
        self.save_name = ""
        # ROOM SETTINGS (ROOMSET1): the host's toggles for a party or coop room,
        # name -> 0/1, only the names its kind takes. Empty for every other kind.
        self.settings = {}
        self.map = 0
        self.win_mode = "time"
        self.star_target = None
        # KING OF THE STAR target, meaningful only when win_mode == "king".
        # None in every other mode, exactly like star_target.
        self.king_target = None
        # THE HOST'S PLAYER-COUNT DIAL. How many of this room's seats play;
        # everybody past it watches. A room parameter like map and win_mode,
        # set through `params`, frozen from Start until the room is back in
        # lobby. It starts at the deployment's own capability so a host who
        # never opens the settings dialog gets the old behaviour exactly.
        self.match_players = DIAL_MAX
        # v5 VS settings, host-set through `params` like map and win_mode.
        # luigi: Luigi Infection (0 off). voice: whether the match allows
        # proximity voice. Voice chat is SHELVED (Tango, 10-08): the field and
        # its wire stay, the value is always 0 (VOICE_SHELVED), so no go plan
        # ever tells a game to open a microphone.
        self.luigi = 0
        self.voice = 0 if VOICE_SHELVED else 1
        # ARENA MODE (run mp1 A4): the host's netmode pick, "arena" or
        # "lockstep". What a match actually plays is resolved at `start`
        # (arena_for) into match_netmode below.
        self.netmode = NETMODE_DEFAULT
        # v5: THE GAME BUILD this room plays, the creator's `game` string, set
        # once at create. Every join must carry the same string: two builds in
        # one match is how a room desyncs at frame 240 instead of refusing at
        # the door, and the lobby is the first door.
        self.game = game
        self.match = None
        # STAGE B/C match state, all None/empty in lobby and reset by
        # _reset_match. Frozen at the arming instant so a join, a leave or a
        # params edit that races the freeze cannot make two members disagree.
        self.comms_code = None      # 8-char, minted per match, discarded at end
        self.agreed_players = 0     # playing-seat count frozen at arm
        self.match_map = 0          # params frozen into the plan at arm
        self.match_win_mode = "time"
        self.match_star_target = None
        self.match_king_target = None
        self.match_dial = 0         # the dial the host had set at arm
        self.match_luigi = 0        # v5: luigi/voice frozen at arm, and the
        self.match_voice = 1        # seed minted at arm, one per match
        self.match_seed = 0
        self.match_netmode = None   # A4: "arena"/"lockstep" frozen at arm
        self.names = ""             # SM64DS_VS_NAMES, built once, slot order
        self.colors = ""            # SM64DS_VS_COLORS, likewise
        self.slot_of = {}           # seat -> slot (0..3), packed among playing
        self.arm_deadline = None    # monotonic; arming must complete by here
        self.go_at = None           # monotonic; go->in_match after GO_GRACE
        self.match_start = None     # monotonic; in_match->lobby after timeout
        # The last match id a result was accepted for, so a SECOND result for a
        # match already resolved is idempotent (200) rather than stale (409).
        self.last_result_match = None
        # run mp1 L-J1: the results posted for the current match so far, one
        # per seat, seat -> (win, scores tuple). A non-host's result is a
        # VOTE; see do_result. Cleared by _reset_match.
        self.result_votes = {}
        self.vote_at = {}           # seat -> when its vote came in (L-J1B)
        # A fresh room starts at cursor 1 with nothing behind it, so the very
        # first real event is seq 2 and a client that polls from the cursor it
        # was handed at create sees everything that happened after it arrived.
        self.seq = 1
        self.events = []        # trimmed to EVENT_RING
        self.chat = []          # trimmed to CHAT_RING, for a resync answer
        self.created = now
        self.touched = now      # last event, for the idle timer
        self.closed_at = None
        self.why = None
        # v5 adventure rooms: when the last member left (monotonic), or None
        # while anybody is in. The reaper closes the room ADV_EMPTY_S later.
        self.empty_since = None
        # Kicked addresses and the tokens they held, each with an expiry. The
        # token record exists only so a kicked launcher gets told WHY on its
        # next poll instead of a bare "you are not a member".
        self.kicked_addr = {}
        self.kicked_token = {}

    # -- roster ----------------------------------------------------------

    def forget_old_kicks(self, now):
        for table in (self.kicked_addr, self.kicked_token):
            for key, until in list(table.items()):
                if now >= until:
                    del table[key]

    def free_seat(self):
        """The lowest seat number nobody holds, or None if the room is full.

        THE SEAT NUMBER IS THE BASIS FOR THE GAME'S SLOT, so three properties of
        it are load-bearing and are asserted in test_units.py rather than left
        to be true by accident:

          1. A member's seat number NEVER CHANGES while they hold it. Nothing
             here renumbers anybody: a member keeps the number they were given
             until they leave, through other people joining, leaving, being
             removed and being promoted.
          2. THE HOST STARTS AS SEAT 1. The creator takes seat 1. Since run
             mp1 (L-J1) a VS host leaving passes the role to the lowest
             playing seat instead of closing the room (adventure rooms always
             did), so a live room can have its host at another number; the
             host's slot 0 is then assign_slots' job, which puts the host
             first by name rather than trusting it to be the lowest seat.
          3. Seat numbers are distinct and always inside 1..MAX_SEATS, so the
             slots derived from them are distinct and always inside 0..3.

        Reusing a freed number rather than counting ever upward is deliberate:
        the numbers have to stay inside the game's four slots, and property 1
        means reuse can never renumber a member who is still in the room.

        WHY ANY OF THIS MATTERS OFF THIS FILE. Over the relay the parent's
        ACCEPT is a broadcast with no recipient field, so two children claiming
        the same slot cannot be told apart by the game. The LOBBY is the only
        place a slot can be handed out uniquely, and stage B's start plan does
        exactly that from these numbers: the host is slot 0.

        ONE THING STAGE B STILL HAS TO DECIDE, flagged here so it is not
        discovered late. With GAME_MAX_PLAYERS below MAX_SEATS the playing seats
        need not be contiguous: seats 1, 2 and 3 with 2 leaving promotes 3, and
        the playing seats are then 1 and 3. `slot = seat - 1` gives slots 0 and
        2, with a hole at 1; `slot = rank among the playing seats` gives 0 and
        1, packed. Both are computable from this model and neither needs a
        change here. The packed reading is very probably the one the game wants.
        """
        for n in range(1, self.seat_cap() + 1):
            if n not in self.members:
                return n
        return None

    def seat_cap(self):
        """How many seats THIS room holds: MAX_SEATS for a VS or adventure
        room, PARTY_SEATS / COOP_SEATS for those kinds (never above the
        sixteen the hub holds)."""
        if self.session_type == "party":
            return PARTY_SEATS
        if self.session_type == "coop":
            return COOP_SEATS
        return MAX_SEATS

    def playing_count(self):
        return sum(1 for m in self.members.values() if m.playing)

    def apply_dial(self):
        """Make the roster agree with `match_players` after the host moves the
        dial, moving as few people as possible.

        MINIMAL CHURN IS THE WHOLE DESIGN. Raising the dial PROMOTES the
        lowest-numbered watchers until the playing count reaches the new
        number; lowering it DEMOTES the highest-numbered players until it does.
        Nobody who can stay in their role is moved, so a host nudging the dial
        from 3 to 4 and back does not shuffle the three people who were already
        playing.

        WHY NOT "the N lowest seats play", which would be one line: because
        seat numbers are REUSED (see free_seat). Seats 1,3,4 with the dial at 2
        play {1,3}; a fifth person joining takes the freed seat 2, and the
        rank rule would then throw seat 3 out of a match they were already in
        to make room for somebody who just walked in. Promotion-on-vacancy and
        this function are the same rule stated twice, and both move a member
        only when a seat actually opened or closed.

        THE HOST IS NEVER DEMOTED. The host (seat 1 unless it passed) is the
        parent of the game session; a spectating host would hand out a go plan
        with no parent in it. Demoting from the highest seat down cannot reach
        seat 1 while the dial is at least 1, and the loop refuses it anyway
        rather than relying on that.
        """
        if self.session_type != "vs":
            # An adventure room has no match and so no dial: every member
            # plays their own game, all of them "playing".
            return
        want = self.match_players
        seats = sorted(self.members)
        while self.playing_count() < want:
            for s in seats:
                if not self.members[s].playing:
                    self.members[s].playing = True
                    break
            else:
                break
        while self.playing_count() > want:
            for s in reversed(seats):
                if self.members[s].playing and s != self.host_seat:
                    self.members[s].playing = False
                    break
            else:
                break

    def refresh_displays(self):
        """Disambiguate duplicate nicknames, once, here.

        The first seat to claim a nickname keeps it plain; every later seat
        with the same nickname is shown with its seat number after it. The
        launcher never does this itself, so both windows show the same words.
        """
        seen = set()
        for seat in sorted(self.members):
            m = self.members[seat]
            if m.nick in seen:
                m.display = ("%s (%d)" % (m.nick, m.seat))[:DISPLAY_MAX]
            else:
                m.display = m.nick
                seen.add(m.nick)

    # -- events ----------------------------------------------------------

    def push(self, kind, **fields):
        self.seq += 1
        ev = {"seq": self.seq, "kind": kind}
        ev.update(fields)
        self.events.append(ev)
        if len(self.events) > EVENT_RING:
            del self.events[:len(self.events) - EVENT_RING]
        if kind == "chat":
            self.chat.append(ev)
            if len(self.chat) > CHAT_RING:
                del self.chat[:len(self.chat) - CHAT_RING]
        self.touched = time.monotonic()
        self.cond.notify_all()
        return self.seq

    def since(self, cursor):
        return [e for e in self.events if e["seq"] > cursor]

    def needs_resync(self, cursor):
        """True when the client's cursor is older than anything we still hold.

        A client that misses ten polls gets all ten events on the eleventh;
        a client that misses more than the ring is told to replace its state
        rather than append to it.
        """
        if cursor > self.seq:
            return True
        if self.events and cursor < self.events[0]["seq"] - 1:
            return True
        return False

    def view(self, you):
        v = {
            "state": self.state,
            "host": self.host_seat,
            "you": you,
            "slots": len(self.members),
            # UNCHANGED MEANING, deliberately. Launcher 0.3.0 reads
            # `max_players` and the dial does not get to redefine a field a
            # shipped binary already understands. It is still the deployment's
            # capability. The two new fields below are additive: an older
            # launcher's JSON reader drops what it has no property for.
            "max_players": GAME_MAX_PLAYERS,
            # ADVENTURE co-op (v4): "vs" or "adventure". Additive like the two
            # fields above -- a pre-v4 launcher's JSON reader has no property
            # for it and drops it, and every room it can create is "vs" anyway.
            "session_type": self.session_type,
            # The host's dial, and the largest value the server will accept for
            # it. `dial_max` is what lets the launcher grey out the rest of the
            # control instead of discovering the bound by being refused.
            "match_players": self.match_players,
            "dial_max": DIAL_MAX,
            "map": self.map,
            "win_mode": self.win_mode,
            "star_target": self.star_target,
            "king_target": self.king_target,
            # v5: the two VS settings, so every launcher shows the host's pick.
            "luigi": self.luigi,
            "voice": self.voice,
            "members": [self.members[s].view() for s in sorted(self.members)],
            "match": self.match,
        }
        if self.session_type in ROOM_KINDS:
            # v6, additive like every field above: whether the host has
            # released the room, how many times, how many seats it holds, and
            # (coop) the save's display name.
            v["started"] = self.started
            v["launch"] = self.launch_no
            v["room_seats"] = self.seat_cap()
            v["settings"] = dict(self.settings)
            if self.session_type == "coop":
                v["save_name"] = self.save_name
        if self.session_type == "vs":
            # ARENA MODE (run mp1 A4), additive like every field above. The
            # host's pick; whether this room's build can play arena here at
            # all (false = the launcher greys the switch); the arena size
            # limit; what a Start pressed now would resolve to; and the
            # running match's resolved netmode (null in lobby).
            v["netmode"] = self.netmode
            v["arena_ok"] = arena_ok(self)
            # Whether a Luigi Infection match in this room can play arena
            # (false = Luigi plays lockstep here; the in-game lobby, which
            # opens arena rooms only, then does not offer Luigi).
            v["arena_luigi"] = arena_luigi_ok(self)
            v["arena_max"] = ARENA_MAX_PLAYERS
            v["arena_now"] = arena_for(self, self.playing_count(),
                                       self.win_mode, self.luigi)
            v["match_netmode"] = self.match_netmode
        return v

    def close(self, why):
        if self.state == "closed":
            return
        self.state = "closed"
        self.closed_at = time.monotonic()
        self.why = why
        self.push("closed", why=why)
        log("room %s close (%s)" % (self.code, why))


class World(object):
    """One always-on adventure hub (v5). Nothing in it but a number, the hub
    code, and per member token when it last proved it was alive and the client
    address it was issued to. No nick and no colour is kept: the world's
    members meet each other through the game's identity frames, never through
    the lobby. The address is held ONLY for WORLD_ADDR_TOKENS, like a room
    member's last_addr for a kick: never logged whole, never sent to anybody,
    dropped with the token."""

    def __init__(self, game, no, code, now):
        self.game = game
        self.no = no
        self.code = code
        self.members = {}       # token -> [last seen (monotonic), address]
        self.empty_since = now

    def count(self, now):
        return sum(1 for m in self.members.values()
                   if now - m[0] <= WORLD_MEMBER_S)

    def forget_stale(self, now):
        for tok, m in list(self.members.items()):
            if now - m[0] > WORLD_MEMBER_S:
                del self.members[tok]


def retire_over_cap(who, now):
    """Before issuing `who` a new world token, retire its least recently heard
    live tokens until it holds fewer than WORLD_ADDR_TOKENS. Returns how many
    were retired. A retired token's next world_ping is 403 not_a_member, which
    the launcher answers by asking `world` again."""
    mine = []
    for w in WORLDS.values():
        for tok, m in w.members.items():
            if m[1] == who and now - m[0] <= WORLD_MEMBER_S:
                mine.append((m[0], w.code, tok))
    mine.sort()
    retired = 0
    while len(mine) - retired >= WORLD_ADDR_TOKENS:
        _, code, tok = mine[retired]
        WORLDS[code].members.pop(tok, None)
        retired += 1
    return retired


# ------------------------------------------------------------- rate limits


class Buckets(object):
    """Per-address limiters, swept so a flood of one-shot addresses cannot
    grow this dictionary without bound."""

    def __init__(self):
        self.tokens = {}        # host -> [allowance, stamp]
        self.creates = {}       # host -> [timestamps]
        self.joins = {}         # host -> [timestamps]
        self.bad = {}           # host -> [count, window_start, ignored_until]
        # host -> when it was last refused, and when a refusal last could not
        # be recorded (the table was full): host_silent_vote (L-J1C).
        self.refused = {}
        self.refused_lost = float("-inf")

    def _room(self, table, host):
        if host not in table and len(table) >= MAX_BUCKETS:
            return False
        return True

    def allow_request(self, host, now):
        b = self.tokens.get(host)
        if b is None:
            if not self._room(self.tokens, host):
                return False
            self.tokens[host] = [float(RATE_BURST) - 1.0, now]
            return True
        b[0] = min(float(RATE_BURST), b[0] + (now - b[1]) * RATE_REQ_PER_S)
        b[1] = now
        if b[0] < 1.0:
            return False
        b[0] -= 1.0
        return True

    def _windowed(self, table, host, now, window, limit):
        stamps = table.get(host)
        if stamps is None:
            if not self._room(table, host):
                return False
            stamps = table[host] = []
        cut = now - window
        while stamps and stamps[0] < cut:
            stamps.pop(0)
        if len(stamps) >= limit:
            return False
        stamps.append(now)
        return True

    def allow_create(self, host, now):
        return self._windowed(self.creates, host, now, 3600.0,
                              RATE_CREATE_PER_HOUR)

    def allow_join(self, host, now):
        return self._windowed(self.joins, host, now, 60.0, RATE_JOIN_PER_MIN)

    def ignored(self, host, now):
        b = self.bad.get(host)
        return b is not None and now < b[2]

    def note_reject(self, host, now):
        """A sender that keeps getting refused is refused outright for a while.

        Same shape relay.py uses, and for the same reason: answering a broken
        or hostile client at full speed is work we are doing on its behalf.
        """
        if self._room(self.refused, host):
            self.refused[host] = now
        else:
            self.refused_lost = now
        b = self.bad.get(host)
        if b is None:
            if not self._room(self.bad, host):
                return
            self.bad[host] = [1, now, 0.0]
            return
        if now - b[1] > BAD_WINDOW_S:
            b[0], b[1] = 1, now
            return
        b[0] += 1
        if BAD_IGNORE_S > 0 and b[0] >= BAD_LIMIT:
            b[2] = now + BAD_IGNORE_S
            b[0], b[1] = 0, now

    def last_refused(self, host):
        """When a request from `host` was last refused (-inf if never, or not
        within HOST_SILENT_S). A refusal the full table could not record
        counts for every address: the guard that reads this fails closed."""
        return max(self.refused.get(host, float("-inf")), self.refused_lost)

    def sweep(self, now):
        # An entry older than HOST_SILENT_S cannot change host_silent_vote's
        # answer any more, so it goes.
        for host, t in list(self.refused.items()):
            if now - t > HOST_SILENT_S:
                del self.refused[host]
        for host, b in list(self.tokens.items()):
            if now - b[1] > 60.0:
                del self.tokens[host]
        for table, window in ((self.creates, 3600.0), (self.joins, 60.0)):
            for host, stamps in list(table.items()):
                while stamps and stamps[0] < now - window:
                    stamps.pop(0)
                if not stamps:
                    del table[host]
        for host, b in list(self.bad.items()):
            if now > b[2] and now - b[1] > BAD_WINDOW_S * 2:
                del self.bad[host]


BUCKETS = Buckets()

# -------------------------------------------------------------- validation
#
# Every field the server will ever store from a client is validated here and
# nowhere else. Each checker answers (value, None) or (None, "error_code").
#
# The order inside a checker matters and is the spec's: a string's LENGTH IN
# BYTES is checked before its content, and a string longer than its cap is
# "too_long" rather than the field's own code, so an oversized field cannot
# reach any grammar loop at all.


def want_str(body, key, code):
    v = body.get(key)
    if not isinstance(v, str):
        return None, code
    return v, None


def v_room(body):
    s, err = want_str(body, "room", "bad_room")
    if err:
        return None, err
    n = byte_len(s)
    if n > ROOM_LEN:
        return None, "too_long"
    if n != ROOM_LEN:
        return None, "bad_room"
    for ch in s:
        if ch not in ROOM_ALPHABET:
            return None, "bad_room"
    return s, None


def v_token(body):
    s, err = want_str(body, "token", "bad_token")
    if err:
        return None, err
    n = byte_len(s)
    if n > TOKEN_LEN:
        return None, "too_long"
    if n != TOKEN_LEN:
        return None, "bad_token"
    for ch in s:
        if ch not in HEX_ALPHABET:
            return None, "bad_token"
    return s, None


def v_nick(body):
    s, err = want_str(body, "nick", "bad_nick")
    if err:
        return None, err
    n = byte_len(s)
    if n > NICK_MAX:
        return None, "too_long"
    if n < 1:
        return None, "bad_nick"
    if not printable_ascii(s, allow_comma=False):
        return None, "bad_nick"
    if s != s.strip(" "):
        return None, "bad_nick"
    if not s.strip(" "):
        return None, "bad_nick"
    return s, None


def v_save_name(body):
    """The display name of a coop room's save (contract v6).

    Opaque, size-capped text the service stores and echoes and never reads
    for meaning. 1..SAVE_NAME_MAX bytes of printable ASCII (commas allowed,
    leading or trailing space not), and none of the characters that make a
    path: a launcher that sent a file path here by mistake, or a client that
    tried to make this service carry one, is refused rather than stored.
    """
    s, err = want_str(body, "save_name", "bad_save_name")
    if err:
        return None, err
    n = byte_len(s)
    if n > SAVE_NAME_MAX:
        return None, "too_long"
    if n < 1:
        return None, "bad_save_name"
    if not printable_ascii(s):
        return None, "bad_save_name"
    if s != s.strip(" "):
        return None, "bad_save_name"
    for ch in s:
        if ch in SAVE_NAME_BAD:
            return None, "bad_save_name"
    return s, None


def settings_for(kind, given, base=None):
    """A party or coop room's settings, from what a client sent (ROOMSET1).

    `given` is the JSON object the client sent ({} when it sent none). `base`
    is what the room has now (an update) or None (a create: the defaults).
    Only the names `kind` takes are accepted, each 0 or 1 (a real int: a
    bool, a float or a string is refused). Answers (dict, None) or
    (None, error_code); the refusal names the kind of mistake, not the value.
    """
    names = ROOM_SETTINGS[kind]
    if base is None:
        out = {k: ROOM_SETTING_DEFAULTS[k] for k in names}
    else:
        out = dict(base)
        for k in names:                  # a room made before a name existed
            out.setdefault(k, ROOM_SETTING_DEFAULTS[k])
    if not isinstance(given, dict):
        return None, "bad_settings"
    for k, v in given.items():
        if k not in names:
            return None, "bad_setting_key"
        if isinstance(v, bool) or not isinstance(v, int) or v not in (0, 1):
            return None, "bad_setting_value"
        out[k] = v
    return out, None


def v_text(body):
    s, err = want_str(body, "text", "bad_text")
    if err:
        return None, err
    n = byte_len(s)
    if n > TEXT_MAX:
        return None, "too_long"
    if n < 1:
        return None, "bad_text"
    if not printable_ascii(s):
        return None, "bad_text"
    return s, None


def v_int(body, key, lo, hi, code):
    v = body.get(key)
    # bool is an int in Python and must not be one here.
    if isinstance(v, bool) or not isinstance(v, int):
        return None, code
    if v < lo or v > hi:
        return None, code
    return v, None


def v_bool(body, key):
    v = body.get(key)
    if not isinstance(v, bool):
        return None, "bad_field"
    return v, None


def v_color(body, key):
    """Six hex digits, either case in, LOWER CASE out.

    Normalised here rather than echoed verbatim so that "FF0000" and "ff0000"
    are the same colour everywhere downstream: the same roster row, the same
    no-op test in the `color` verb, and the same byte in SM64DS_VS_COLORS. The
    game parses either case, so this buys nothing there -- it buys the property
    that two launchers cannot produce two different strings for one colour.
    """
    s, err = want_str(body, key, "bad_color")
    if err:
        return None, err
    n = byte_len(s)
    if n > COLOR_LEN:
        return None, "too_long"
    if n != COLOR_LEN:
        return None, "bad_color"
    for ch in s:
        if ch not in "0123456789abcdefABCDEF":
            return None, "bad_color"
    return s.lower(), None


def v_color_pair(body):
    """The two colour keys, which travel together or not at all.

    Returns (color, shoes, error). ("", "") means the caller picked nothing,
    which is the default and is not an error. One of the two alone IS an error:
    the alternative is a half-recoloured Yoshi and a rule about which half wins.

    A COLOUR IS A v2 FIELD, AND THIS FUNCTION NO LONGER SAYS SO. It used to
    carry its own `ver < PROTO_COLOR_V` check, downstream of a `shape()` call
    that listed `color`/`shoes` as plain optional keys -- so the gate lived in
    the validator instead of in the contract, and nothing would have failed if
    a later verb accepted a colour and forgot to repeat the check. VERB_FIELDS
    carries it now: a v1 body reaching here cannot contain either key, because
    `shape_for` refused it as `bad_field` before this ran. Same answer, one
    authority, and the table walk in test_units.py proves it for every version.
    """
    has = ("color" in body, "shoes" in body)
    if not any(has):
        return "", "", None
    if not all(has):
        return None, None, "bad_color"
    color, err = v_color(body, "color")
    if err:
        return None, None, err
    shoes, err = v_color(body, "shoes")
    if err:
        return None, None, err
    return color, shoes, None


def v_match(body):
    s, err = want_str(body, "match", "bad_match")
    if err:
        return None, err
    n = byte_len(s)
    if n > MATCH_LEN:
        return None, "too_long"
    if n != MATCH_LEN:
        return None, "bad_match"
    for ch in s:
        if ch not in HEX_ALPHABET:
            return None, "bad_match"
    return s, None


def v_game(body):
    """The game build id (v5): ^[0-9A-Za-z._-]{1,24}$, compared, never parsed.

    It is a claim the launcher makes about the game it will spawn, and the only
    thing the lobby does with it is refuse to seat two different claims in one
    room. A lying launcher gains nothing by it: the game's own session check
    (the build id and digest in the ACCEPT) is the second door.
    """
    s, err = want_str(body, "game", "bad_game")
    if err:
        return None, err
    n = byte_len(s)
    if n > GAME_ID_MAX:
        return None, "too_long"
    if n < 1:
        return None, "bad_game"
    for ch in s:
        if ch not in GAME_ID_ALPHABET:
            return None, "bad_game"
    return s, None


def v_code(body, key):
    """A relay/hub code the lobby minted: 8 characters of the room alphabet."""
    s, err = want_str(body, key, "bad_code")
    if err:
        return None, err
    n = byte_len(s)
    if n > COMMS_LEN:
        return None, "too_long"
    if n != COMMS_LEN:
        return None, "bad_code"
    for ch in s:
        if ch not in ROOM_ALPHABET:
            return None, "bad_code"
    return s, None


def v_win(body):
    s, err = want_str(body, "win", "bad_win")
    if err:
        return None, err
    if s not in WIN_VALUES:
        return None, "bad_win"
    return s, None


def v_scores(body):
    v = body.get("scores")
    # 0.3.2: four scores from a four-player-or-fewer match (the marker prints
    # four as its floor), up to DIAL_HARD_MAX from a wider one. The first
    # sixteen-player night ended with every launcher's result refused here
    # because this line still said exactly four.
    if (not isinstance(v, list)
            or len(v) < NARROW_PLAYERS or len(v) > DIAL_HARD_MAX):
        return None, "bad_scores"
    out = []
    for x in v:
        if isinstance(x, bool) or not isinstance(x, int) or x < 0 or x > 99:
            return None, "bad_scores"
        out.append(x)
    return out, None


def v_reason(body):
    s, err = want_str(body, "reason", "bad_reason")
    if err:
        return None, err
    # A fixed enum, never free text. member_not_ready and host_left are
    # server-minted only, so a client that sends one is refused like any
    # other unknown reason.
    if s not in FAIL_REASONS or s in SERVER_REASONS:
        return None, "bad_reason"
    return s, None


def shape(body, required, optional=()):
    """Unknown key, missing key: both are a 400. Deliberately strict."""
    allowed = set(required) | set(optional)
    for k in body:
        if k not in allowed:
            return "bad_field"
    for k in required:
        if k not in body:
            return "bad_field"
    return None


# --------------------------------------------------- the field table, by verb
#
# EVERY FIELD EVERY VERB TAKES, AND THE CONTRACT VERSION IT ARRIVED IN. This
# table is the only place a verb's field set is written down, and `shape_for`
# below is the only thing that reads it, so a handler cannot accept a field the
# table does not list and cannot list a field it does not gate.
#
# WHY IT IS A TABLE AND NOT A LINE IN EACH HANDLER. The dial's gate started as
# one hand-written expression inside do_params -- "if the request says v2, add
# match_players to the optional set" -- with one test aimed at it. That shape
# has a specific and nasty failure mode: the NEXT lane adds a v2-only field,
# forgets to write the same expression, and the field becomes silently
# acceptable at v1. No test fails, because the test that exists names the one
# field somebody remembered to gate. A forgotten gate has to be IMPOSSIBLE
# rather than merely untested, so the version a field arrived in is data, and
# test_units.py walks this table and proves every versioned field is refused
# below its own version -- including fields nobody has written yet.
#
# THE SHAPE. `(required, optional)`; `optional` maps a field name to the
# FIRST contract version that accepts it. Required fields are v1 by
# construction: a later version cannot make a field mandatory without breaking
# every earlier client, so adding one is a new verb, not a new field.
# A row is `(required, optional)` or `(required, optional, since)`, where
# `since` is the first contract version in which THE VERB ITSELF exists. It
# defaults to the contract floor, so only a verb added after v1 spells it.
#
# The verb-level version was added for `color`. A field-level gate could not
# express it: `color` is not an old verb that grew a new key, it is a verb that
# did not exist at v1 at all, and gating only its fields would have left a v1
# client able to call it with its required set and be answered.
VERB_FIELDS = {
    "create":    (("v", "nick", "game"),
                  # A colour pick travels as a PAIR or not at all; do_create
                  # enforces the pairing. Here they are two keys that may
                  # appear from v2. session_type is a v4 optional (ADVENTURE);
                  # do_create checks its value against SESSION_TYPES.
                  {"pre_ok": 1, "color": V_COLOR, "shoes": V_COLOR,
                   "session_type": V_SESSION,
                   # v6: a coop room's save display name (do_create requires
                   # it for coop and refuses it anywhere else).
                   "save_name": V_ROOMS,
                   # v6 (ROOMSET1): the room's toggles, party and coop only.
                   "settings": V_ROOMS}),
    "join":      (("v", "room", "nick", "game"),
                  {"pre_ok": 1, "color": V_COLOR, "shoes": V_COLOR}),
    "poll":      (("v", "room", "token", "cursor"), {"wait": 1}),
    "chat":      (("v", "room", "token", "text"), {}),
    "params":    (("v", "room", "token", "map", "win_mode"),
                  # star_target is required-iff-stars and forbidden otherwise;
                  # do_params enforces that pairing. Here it is only "a key
                  # this verb may carry, since v1".
                  {"star_target": 1, "king_target": 1, "match_players": V_DIAL,
                   # v5: Luigi Infection and the voice switch, VS rooms only;
                   # do_params refuses either on an adventure room.
                   "luigi": V_ONE, "voice": V_ONE,
                   # run mp1 A4: the host's netmode pick, VS rooms only.
                   "netmode": V_ONE}),
    # THE WHOLE VERB IS v2. Its fields are required, so they carry no version
    # of their own -- the verb's own `since` is what refuses a v1 caller.
    "color":     (("v", "room", "token", "color", "shoes"), {}, V_COLOR),
    "preflight": (("v", "room", "token", "pre_ok"), {}),
    "start":     (("v", "room", "token"), {}),
    # v6 (ROOMSET1): the host changes a party or coop room's toggles before Start.
    "settings":  (("v", "room", "token", "settings"), {}, V_ROOMS),
    "ready":     (("v", "room", "token", "match"), {}),
    "result":    (("v", "room", "token", "match", "win", "scores"), {}),
    "failed":    (("v", "room", "token", "match", "reason"), {}),
    "kick":      (("v", "room", "token", "seat"), {}),
    "leave":     (("v", "room", "token"), {}),
    # v5 WORLD ROOMS. Both verbs are v5 as a whole. `skip` is the world the
    # relay just answered "full" for (the game exited with code 4).
    "world":     (("v", "nick", "game"),
                  {"color": V_COLOR, "shoes": V_COLOR, "skip": V_ONE}, V_ONE),
    "world_ping": (("v", "code", "token"), {}, V_ONE),
}


def verb_since(verb):
    """The first contract version in which this verb exists."""
    row = VERB_FIELDS[verb]
    return row[2] if len(row) > 2 else CONTRACT_MIN


def request_version(body):
    """The contract version a request CLAIMS, floored.

    The transport refuses anything outside CONTRACT_MIN..CONTRACT_V before a
    handler runs, so by the time this is called `v` is a good integer. Falling
    back to the floor rather than trusting that keeps it safe to call directly
    from a test, which is how shape_for has always read it -- this is the same
    three lines, named once, so a handler that needs the version reads the
    SAME number the shape gate read rather than its own copy of the rule.
    """
    ver = body.get("v")
    if isinstance(ver, bool) or not isinstance(ver, int):
        return CONTRACT_MIN
    return ver


def shape_for(verb, body):
    """Validate a body against the verb's row of the table, at the version the
    body claims. A key the table does not list at this version is `bad_field`,
    exactly as an undefined key is -- which is what keeps section 3.0's
    strictness true for a v1 client after v2 exists.
    """
    row = VERB_FIELDS[verb]
    required, optional = row[0], row[1]
    # The transport refuses a version outside CONTRACT_MIN..CONTRACT_V before
    # any handler runs, so by here `v` is a good integer. Falling back to the
    # floor rather than trusting it keeps this function safe to call directly
    # from a test.
    ver = request_version(body)
    # v5: THE RANGE, here as well as in the transport, so a handler called
    # directly (the unit tests do) refuses a pre-floor body the same way the
    # wire does instead of quietly serving it.
    if ver < CONTRACT_MIN or ver > CONTRACT_V:
        return "bad_version"
    # THE VERB ITSELF FIRST. A verb that does not exist at the caller's version
    # is refused before its fields are looked at, so a v1 caller cannot learn
    # anything about a v2 verb from the shape of its refusal.
    if ver < verb_since(verb):
        return "bad_field"
    allowed = [name for name, since in optional.items() if ver >= since]
    return shape(body, required, allowed)


# ------------------------------------------------------------------- verbs
#
# Each handler runs with LOCK already held and answers (status, payload).
# None of them does any I/O and none of them can block, except `poll`, which
# blocks only on its own room's Condition and so releases LOCK while it waits.


def seat_of(room, token):
    return room.by_token.get(token)


def refuse_member(room, token, now):
    """The 403 a token that is not seated gets, and why.

    A token the host kicked is answered `kicked` rather than `not_a_member`,
    so the launcher can say what happened in a plain sentence instead of
    guessing between "you were removed" and "this room forgot you".
    """
    room.forget_old_kicks(now)
    if token in room.kicked_token:
        return 403, {"error": "kicked"}
    return 403, {"error": "not_a_member"}


def rooms_hosted_by(who):
    """How many open rooms this address is the host of (SECREV1)."""
    n = 0
    for room in ROOMS.values():
        host = room.members.get(room.host_seat)
        if host is not None and host.last_addr == who:
            n += 1
    return n


def do_create(body, who, now):
    err = shape_for("create", body)
    if err:
        return 400, {"error": err}
    nick, err = v_nick(body)
    if err:
        return 400, {"error": err}
    pre_ok = False
    if "pre_ok" in body:
        pre_ok, err = v_bool(body, "pre_ok")
        if err:
            return 400, {"error": err}
    color, shoes, err = v_color_pair(body)
    if err:
        return 400, {"error": err}
    game, err = v_game(body)
    if err:
        return 400, {"error": err}
    if not game_allowed(game):
        return 409, {"error": "unknown_game"}
    # ADVENTURE co-op (v4): the session type. shape_for has already refused the
    # key for a v1..v3 body, so reaching here with it present means a v4 caller;
    # the value is the only thing left to check. Absent is "vs", the default and
    # what every pre-v4 room is.
    session_type = "vs"
    if "session_type" in body:
        session_type = body["session_type"]
        if not isinstance(session_type, str) or session_type not in SESSION_TYPES:
            return 400, {"error": "bad_session_type"}
        # v6: party and coop are v6 types. A v5 caller naming one gets the
        # refusal an unknown type has always got (bad_session_type, which the
        # launcher words as "Check for an update"), so a v5 client is exactly
        # as it was.
        if session_type in ROOM_KINDS and request_version(body) < V_ROOMS:
            return 400, {"error": "bad_session_type"}
    # v6: a coop room needs the display name of the host's save, and no other
    # kind takes one. (shape_for has already refused the key below v6.)
    save_name = ""
    if session_type == "coop":
        if "save_name" not in body:
            return 400, {"error": "bad_save_name"}
        save_name, err = v_save_name(body)
        if err:
            return 400, {"error": err}
    elif "save_name" in body:
        return 400, {"error": "bad_field"}
    # ROOMSET1: the room's toggles. Party and coop only; absent means the
    # defaults. Checked before the rate bucket and the room cap, like the rest.
    settings = {}
    if session_type in ROOM_KINDS:
        settings, err = settings_for(session_type, body.get("settings", {}))
        if err:
            return 400, {"error": err}
    elif "settings" in body:
        return 400, {"error": "bad_field"}

    if not BUCKETS.allow_create(who, now):
        return 429, {"error": "too_fast"}
    if rooms_hosted_by(who) >= ROOMS_PER_ADDR:
        return 429, {"error": "too_fast"}
    if len(ROOMS) >= MAX_ROOMS:
        return 503, {"error": "full"}

    code = None
    for _ in range(8):
        candidate = new_room_code()
        if candidate not in ROOMS:
            code = candidate
            break
    if code is None:
        return 503, {"error": "busy"}

    # The host's own contract version bounds the room's default width; see
    # Room.__init__. Defaulted at the floor so a direct Room() in a test is
    # narrow unless it says otherwise.
    room = Room(code, now, game)
    room.session_type = session_type
    room.save_name = save_name
    room.settings = settings
    if session_type in HUB_TYPES:
        # THE ROOM'S HUB CODE, minted once and kept for the room's life: an
        # adventure room is one relay hub session that people drop in and out
        # of, not a sequence of matches. Like a VS match's code it is never
        # the room code, so reading the invite aloud does not hand anybody a
        # relay seat without the lobby.
        room.comms_code = new_comms_code()
    token = new_token()
    # The host plays if the dial has room for anybody at all. The host is also
    # the session's parent, so a host who is not playing is not a thing any
    # later code has to handle: `apply_dial` refuses to demote seat 1.
    m = Member(1, token, nick, room.match_players >= 1, pre_ok, now, who,
               color, shoes)
    room.members[1] = m
    room.by_token[token] = 1
    room.host_seat = 1
    room.refresh_displays()
    ROOMS[code] = room
    log("room %s create %s by %s (1/%d)"
        % (code, session_type, blunt_host(who), room.seat_cap()))
    out = {"room": code, "token": token, "member": 1,
           "cursor": room.seq, "view": room.view(1)}
    if room.session_type in HUB_TYPES:
        out["plan"] = member_plan(room, 1)
    return 200, out


def do_join(body, who, now):
    err = shape_for("join", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    nick, err = v_nick(body)
    if err:
        return 400, {"error": err}
    pre_ok = False
    if "pre_ok" in body:
        pre_ok, err = v_bool(body, "pre_ok")
        if err:
            return 400, {"error": err}
    color, shoes, err = v_color_pair(body)
    if err:
        return 400, {"error": err}
    game, err = v_game(body)
    if err:
        return 400, {"error": err}
    if not game_allowed(game):
        return 409, {"error": "unknown_game"}

    if not BUCKETS.allow_join(who, now):
        return 429, {"error": "too_fast"}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    room.forget_old_kicks(now)
    if who in room.kicked_addr:
        return 403, {"error": "kicked"}
    # v6: a party or coop room is joined by a v6 launcher only. A v5 launcher
    # would boot it as a plain adventure room, so it is turned away at the door
    # with the words "Check for an update" (bad_session_type, as a create is).
    if room.session_type in ROOM_KINDS and request_version(body) < V_ROOMS:
        return 400, {"error": "bad_session_type"}
    # v5: ONE GAME BUILD PER ROOM. Refused before the seat checks so a player
    # on the wrong build hears the reason that is actually theirs (somebody in
    # this room runs a different game version) rather than "room full".
    if game != room.game:
        return 409, {"error": "game_mismatch"}
    if len(room.members) >= room.seat_cap():
        return 409, {"error": "room_full"}
    # JOIN AT THE NEXT MATCH (Tango, 09-29; run mp1 L-J1). A VS room whose
    # match is arming, going or running used to refuse the door with 409
    # `in_match`. Now the joiner gets a WAITING seat: playing = False for the
    # match that is on, so it is in no plan, no slot map and no names string,
    # and ready / failed / result refuse it (not_playing). It sees the room,
    # the roster and the chat like any member, and when the match ends
    # _reset_match runs the dial and seats it for the next one. Every cap a
    # lobby join has still holds: the join budget per address, MAX_SEATS,
    # the kick cooldown and the one-build rule above.

    seat = room.free_seat()
    if seat is None:
        return 409, {"error": "room_full"}
    token = new_token()
    # The dial decides, not the deployment knob. Same rule as before -- the
    # first N in the room play -- with N now chosen by the host instead of
    # fixed for the whole server. An adventure room has no dial: everybody
    # in it plays their own game. A VS room with a match on seats nobody
    # into it: the joiner waits (see above).
    playing = (room.session_type != "vs"
               or (room.state == "lobby"
                   and room.playing_count() < room.match_players))
    if not room.members:
        # Only an adventure room can be empty and open (a VS room closes when
        # its last member leaves). Whoever walks into the empty room holds it now.
        room.host_seat = seat
        room.empty_since = None
    room.members[seat] = Member(seat, token, nick, playing, pre_ok, now, who,
                                color, shoes)
    room.by_token[token] = seat
    room.refresh_displays()
    room.push("joined", seat=seat, display=room.members[seat].display)
    log("room %s join %s seat %d %s (%d/%d)"
        % (code, blunt_host(who), seat,
           "playing" if playing else
           ("waiting" if room.state != "lobby" else "watching"),
           len(room.members), room.seat_cap()))
    out = {"room": code, "token": token, "member": seat,
           "cursor": room.seq, "view": room.view(seat)}
    if room.session_type in HUB_TYPES:
        out["plan"] = member_plan(room, seat)
    return 200, out


def do_chat(body, who, now):
    err = shape_for("chat", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    text, err = v_text(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    m.last_addr = who

    # One line per two seconds, three saved up. Refused rather than queued;
    # the launcher greys its Send button instead of showing an error.
    m.chat_allowance = min(float(CHAT_BURST),
                           m.chat_allowance + (now - m.chat_stamp) / CHAT_EVERY_S)
    m.chat_stamp = now
    if m.chat_allowance < 1.0:
        return 429, {"error": "too_fast"}
    m.chat_allowance -= 1.0

    # The text is appended and never read. It is not logged, not parsed, and
    # not stored anywhere but the room's own 40-line ring.
    room.push("chat", seat=seat, display=m.display, text=text)
    return 200, {"cursor": room.seq}


def do_params(body, who, now):
    """Host only. The room's settings: arena, win condition, and -- from
    contract v2 -- how many of the room's seats play.

    `match_players` IS OPTIONAL AND IS v2-ONLY. Optional so that launcher
    0.3.0, which does not know the field exists, keeps setting the arena and
    the win condition with no change of behaviour at all. v2-only so that
    section 3.0's strictness still means something: a client that says it
    speaks v1 and then sends a v2 field is refused `bad_field`, exactly as it
    would be for a field nobody has ever defined.
    """
    err = shape_for("params", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    mp, err = v_int(body, "map", 0, 3, "bad_map")
    if err:
        return 400, {"error": err}
    win_mode, err = want_str(body, "win_mode", "bad_win_mode")
    if err:
        return 400, {"error": err}
    if win_mode not in ("time", "stars", "king"):
        return 400, {"error": "bad_win_mode"}
    star_target = None
    if win_mode == "stars":
        if "star_target" not in body:
            return 400, {"error": "bad_star_target"}
        star_target, err = v_int(body, "star_target", 1, STAR_TARGET_MAX,
                                 "bad_star_target")
        if err:
            return 400, {"error": err}
    elif "star_target" in body:
        # Required iff stars, forbidden otherwise. One shape, not two.
        return 400, {"error": "bad_star_target"}
    # KING OF THE STAR target, the same required-iff/forbidden-otherwise shape
    # as star_target, refused with its own code so the launcher can say a true
    # sentence about it. Only carried, and only accepted, when win_mode=="king".
    king_target = None
    if win_mode == "king":
        if "king_target" not in body:
            return 400, {"error": "bad_king_target"}
        king_target, err = v_int(body, "king_target", 1, KING_TARGET_MAX,
                                 "bad_king_target")
        if err:
            return 400, {"error": err}
    elif "king_target" in body:
        return 400, {"error": "bad_king_target"}

    # THE DIAL. Refused, not clamped, and refused with its own code so the
    # launcher can say a true sentence about it. Clamping was the other option
    # and it is the wrong one: a host who asks for eight and silently gets four
    # has been lied to, and would only find out by counting heads in the room.
    # The bound the refusal enforces is advertised as `dial_max` in every view,
    # so a launcher that reads the view never has to be refused at all.
    #
    match_players = None
    if "match_players" in body:
        match_players, err = v_int(body, "match_players", 2, DIAL_MAX,
                                   "bad_match_players")
        if err:
            return 400, {"error": err}
    # v5: two switches, 0 or 1 and nothing else (not a bool, not a string).
    # Absent means "keep what the room has", like the dial.
    luigi = voice = None
    if "luigi" in body:
        luigi, err = v_int(body, "luigi", 0, 1, "bad_field")
        if err:
            return 400, {"error": err}
    if "voice" in body:
        voice, err = v_int(body, "voice", 0, 1, "bad_field")
        if err:
            return 400, {"error": err}
    # run mp1 A4: the netmode pick, one of two exact strings. Absent keeps
    # the room's, like luigi and voice. Its own refusal code, like bad_map,
    # so a launcher can say a true sentence about it.
    netmode = None
    if "netmode" in body:
        netmode = body["netmode"]
        if not isinstance(netmode, str) or netmode not in NETMODES:
            return 400, {"error": "bad_netmode"}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    room.members[seat].seen = now
    if seat != room.host_seat:
        return 403, {"error": "not_host"}
    if room.state != "lobby":
        return 409, {"error": "not_in_lobby"}
    # v6: a party or coop room has no match settings at all.
    if room.session_type in ROOM_KINDS:
        return 409, {"error": "not_vs"}
    # v5: Luigi Infection, the voice switch and the dial are VS match
    # settings. An adventure room has no match to put them in, so any of them
    # there is a field this room does not have: bad_field, like any undefined
    # key.
    if room.session_type != "vs" and (luigi is not None or voice is not None
                                      or match_players is not None
                                      or netmode is not None):
        return 400, {"error": "bad_field"}

    room.map = mp
    room.win_mode = win_mode
    room.star_target = star_target
    room.king_target = king_target
    # A v1 client sends no dial, and a room whose host uses one keeps whatever
    # it had rather than being reset by an old launcher's ordinary settings
    # edit. That is the whole of the "0.3.0 is unaffected" story on this verb.
    if match_players is not None:
        room.match_players = match_players
        room.apply_dial()
    if luigi is not None:
        room.luigi = luigi
    if voice is not None:
        # Parsed and bounds-checked above like before; the value is dropped
        # while voice is shelved.
        room.voice = 0 if VOICE_SHELVED else voice
    if netmode is not None:
        room.netmode = netmode
    # `match_players` rides the event as well as the view. Older launchers
    # ignore an extra field on an event they already know (section 3.5 is about
    # unknown KINDS; the field tolerance is what JSON readers do by default and
    # is pinned by a test), and the roster change is visible in the view every
    # poll carries regardless.
    # run mp1 A4: a VS room's event also carries the netmode pick.
    extra = {"netmode": room.netmode} if room.session_type == "vs" else {}
    room.push("params", map=mp, win_mode=win_mode, star_target=star_target,
              king_target=king_target, match_players=room.match_players,
              luigi=room.luigi, voice=room.voice, **extra)
    log("room %s params map=%d win=%s target=%s king=%s players=%d/%d "
        "luigi=%d voice=%d netmode=%s"
        % (code, mp, win_mode, star_target if star_target else "-",
           king_target if king_target else "-",
           room.playing_count(), room.match_players, room.luigi, room.voice,
           room.netmode if room.session_type == "vs" else "-"))
    return 200, {"cursor": room.seq}


def do_settings(body, who, now):
    """Host only, before Start: change a party or coop room's toggles (ROOMSET1).

    `settings` is a partial update: the names sent change, the rest keep what
    the room has. Refused once the room is started (the games are up, and a
    change would not reach them) and for any room that is not party or coop.
    """
    err = shape_for("settings", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    room.members[seat].seen = now
    if seat != room.host_seat:
        return 403, {"error": "not_host"}
    if room.session_type not in ROOM_KINDS:
        return 409, {"error": "no_room_settings"}
    if room.state != "lobby":
        return 409, {"error": "not_in_lobby"}
    if room.started:
        return 409, {"error": "already_started"}
    out, err = settings_for(room.session_type, body["settings"], room.settings)
    if err:
        return 400, {"error": err}
    room.settings = out
    room.push("settings", settings=dict(out))
    log("room %s settings %s" % (code, " ".join(
        "%s=%d" % (k, out[k]) for k in ROOM_SETTINGS[room.session_type])))
    return 200, {"cursor": room.seq, "settings": dict(out)}


def do_leave(body, who, now):
    err = shape_for("leave", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    drop_member(room, seat, "quit")
    return 200, {"ok": True}


def pass_host(room, played=()):
    """Hand a VS room's host role on after its host left (run mp1, L-J1).

    THE NEW HOST IS THE LOWEST-NUMBERED SEAT THAT PLAYED THE MATCH THE HOST'S
    LEAVING ENDED (`played`, taken by drop_member before the match was
    reset), so somebody who joined mid-match and waited on a freed low seat
    number is not made host ahead of the people who played it. With no match
    on, or none of its players left, it is the lowest-numbered PLAYING seat,
    or the lowest seat when nobody is playing. A player rather than simply
    the lowest seat, because
    the host is the parent of the next match and apply_dial never demotes
    it: a watcher made host would be forced into play past the dial. The
    seat number does not change (free_seat's property 1); what changes is
    that the host is no longer always seat 1, which is why assign_slots puts
    the host in slot 0 explicitly. Called in lobby, after a running match was
    ended (match_to_lobby), so the new host starts the next one; or (run mp1
    A4) during an ARENA match, with `played` = its live seats (never empty),
    so the new host is a seat already playing it and nobody is promoted."""
    ps = [s for s in played if s in room.members] or playing_seats(room)
    new = ps[0] if ps else min(room.members)
    room.host_seat = new
    m = room.members[new]
    if not m.playing and room.match_players >= 1:
        m.playing = True
        room.apply_dial()
    room.push("host", seat=new, display=m.display)
    log("room %s host -> seat %d" % (room.code, new))


def drop_member(room, seat, why):
    """Free a seat. Anyone but the host leaving promotes the lowest-numbered
    spectator if a playing seat opened up (in lobby only). A VS host leaving
    ends a match that is on (reason host_left) and passes the host role on;
    only the LAST member leaving closes the room."""
    m = room.members.get(seat)
    if m is None:
        return
    if (seat == room.host_seat and room.session_type == "vs"
            and len(room.members) == 1):
        room.close("host_left")
        return
    was_playing = m.playing
    del room.members[seat]
    room.by_token.pop(m.token, None)
    room.refresh_displays()
    room.push("left", seat=seat, display=m.display, why=why)
    log("room %s leave seat %d (%s) (%d/%d)"
        % (room.code, seat, why, len(room.members), MAX_SEATS))
    if room.session_type == "coop" and seat == room.host_seat:
        # A COOP ROOM ENDS WITH ITS HOST (the FIELD5 default): the host's PC
        # holds the save, and the world the guests stand in is the host's.
        # Everyone left in the room reads the close reason on their next poll.
        # (A PARTY room, which shares only stars and bodies, does outlive its
        # creator: the host role passes on below.)
        room.close("host_left")
        return
    if room.session_type != "vs":
        # AN ADVENTURE ROOM OUTLIVES ITS CREATOR (design section 5). Nobody
        # hosts a hub session, so the host role is only the lobby's (kick):
        # it passes to the lowest seat still in, and an empty room waits
        # ADV_EMPTY_S for somebody to come back before the reaper closes it.
        if seat == room.host_seat:
            if room.members:
                room.host_seat = min(room.members)
                room.push("host", seat=room.host_seat,
                          display=room.members[room.host_seat].display)
                log("room %s host -> seat %d" % (room.code, room.host_seat))
        # (An empty room's clock is started by the reaper, on its own `now`.)
        return
    if seat == room.host_seat:
        # THE VS HOST LEFT A ROOM THAT STILL HAS PEOPLE IN IT (Tango, 09-29:
        # Q3). The host is the parent of the game session -- the round clock
        # every console waits on -- so a match that is arming, going or
        # running cannot go on without it: it ends for everybody with the
        # reason host_left, which the launchers word, and everybody lands
        # back in the room. _reset_match runs the dial, so waiting and
        # dropped seats are in the next match. Then the host role passes on
        # and the new host presses Start. Seamless mid-match host hand-over
        # is after 1.0. (Before L-J1 this closed the room for everybody.)
        # The seats still playing the match are read BEFORE the reset: after
        # it the dial has seated the waiting joiners too, and the host should
        # pass to somebody who played (REVLOBBYJOIN1 note B).
        played = sorted(s for s in room.slot_of if in_frozen_match(room, s))
        if (room.match_netmode == "arena" and room.state in ("go", "in_match")
                and played):
            # AN ARENA MATCH HAS NO HOST IN IT (run mp1 A4). Its referee is
            # on the relay and every console runs its own Yoshi, so the
            # host's game leaving changes nothing for the others: the match
            # runs on, the leaver is simply gone from it (its stars are the
            # referee's to spill), and the host role passes now to the
            # lowest seat still playing it. No failed host_left. The match
            # still ends by results (the new host's, or a majority, or the
            # host-silent rule) or MATCH_TIMEOUT_S.
            pass_host(room, played)
            settle_after_host_change(room)
            return
        if room.state != "lobby":
            match_to_lobby(room, "host_left")
        else:
            room.apply_dial()
        pass_host(room, played)
        return
    # NOBODY IS PROMOTED INTO A FROZEN MATCH (run mp1, L-J1). From `start`
    # until the room is back in lobby the roster, the slot map and the names
    # are frozen; a watcher promoted then held no slot, and during `go` every
    # poll hands a playing seat a plan, so the promoted watcher was told to
    # join the running match as a child in slot 0 -- the host's slot. The
    # seat that opened is filled when the match ends: _reset_match runs the
    # dial.
    if was_playing and room.state != "lobby":
        # A player leaving a running match may leave the votes already in as
        # a majority of who is left (see do_result).
        maybe_settle(room)
        return
    if was_playing and room.state == "lobby":
        before = {s for s in room.members if room.members[s].playing}
        # ONE RULE, STATED ONCE. Promotion-on-vacancy and the host's dial are
        # the same thing -- "bring the playing count up to what the room is set
        # to" -- so this calls the dial rather than restating it. It is exactly
        # equivalent to the loop it replaces: a playing seat just freed, so the
        # count is one under the dial and one watcher moves up.
        room.apply_dial()
        now_playing = {s for s in room.members if room.members[s].playing}
        for s in sorted(now_playing - before):
            # No new event kind: section 3.5's `kind` list is closed, and the
            # `left` event above already moved the cursor. The promotion
            # reaches both windows in the `view` every poll answer carries.
            log("room %s promote seat %d" % (room.code, s))


def do_kick(body, who, now):
    """Host only. The owner's answer to the spec's open question 4.

    Ten lines of state and one honest limitation, both in KICK_COOLDOWN_S
    above. A kick frees the seat exactly the way a leave does -- same `left`
    event, same promotion -- and differs only in the `why` and in the two
    records that keep the kicked client out for a while.
    """
    err = shape_for("kick", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    target, err = v_int(body, "seat", 1, MAX_SEATS, "bad_seat")
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    room.members[seat].seen = now
    if seat != room.host_seat:
        return 403, {"error": "not_host"}
    if room.state != "lobby":
        # Removing somebody out of a room whose match is arming or running
        # would change the seat count the match was frozen against.
        return 409, {"error": "not_in_lobby"}
    if target == room.host_seat:
        return 400, {"error": "bad_seat"}
    victim = room.members.get(target)
    if victim is None:
        return 400, {"error": "bad_seat"}

    if KICK_COOLDOWN_S > 0:
        room.kicked_token[victim.token] = now + KICK_COOLDOWN_S
        # The address is remembered only if we know one. It is the address the
        # KICK-ee last spoke from, which the room does not store, so this
        # records the address of whoever is being removed as of their last
        # request -- see the note on KICK_COOLDOWN_S.
        if victim.last_addr:
            room.kicked_addr[victim.last_addr] = now + KICK_COOLDOWN_S
    drop_member(room, target, "kicked")
    log("room %s kick seat %d by host (%d/%d)"
        % (code, target, len(room.members), MAX_SEATS))
    return 200, {"cursor": room.seq, "ok": True}


# ---------------------------------------------------------- the match, B + C
#
# THE ARMING HANDSHAKE, and why nothing spawns until everybody says it can.
# start -> arming -> (all ready) -> go -> in_match -> (result) -> lobby, or at
# any arming/go/in_match step a `failed`/deadline/timeout returns the room to
# lobby WITHOUT any game having spawned earlier than `go`. This is the front
# half of the join-race guarantee (spec section 4): the seat count is frozen at
# `start` and forced into every plan as SM64DS_VS_PLAYERS, and no plan reaches
# any launcher until every playing seat has POSTed `ready`.


def playing_seats(room):
    return sorted(s for s in room.members if room.members[s].playing)


def in_frozen_match(room, seat):
    """True for a seat that is PLAYING the room's current match: playing, and
    one of the seats frozen into slot_of at `start`. ready / failed / result
    for the current match ask this, not `playing` alone (run mp1, L-J1), so a
    seat the match never froze -- a waiting joiner on a reused seat number,
    or anything a future path promotes -- cannot arm, abort or end it."""
    m = room.members.get(seat)
    return m is not None and m.playing and seat in room.slot_of


def assign_slots(room):
    """seat -> slot, PACKED among the playing seats in seat order.

    Host is seat 1 and is always slot 0 (spec 4.2, and the game makes the parent
    slot 0 unconditionally). The rest take 1,2,3 in seat order. Packed rather
    than slot=seat-1 so a promoted spectator (seats 1 and 3 playing, say) still
    maps to slots 0 and 1 with no hole -- the reading stage A recommended and
    the game's own contiguous player_count expects.

    DISTINCT PER SEAT BY CONSTRUCTION, which is the whole point off this machine:
    the relay ACCEPT is a recipientless broadcast, so two children on the same
    slot cannot be told apart, and the lobby is the only place a slot is handed
    out uniquely.

    THE HOST IS PUT FIRST BY NAME, not by being the lowest seat (run mp1,
    L-J1). Since a VS host that leaves passes the role on, the host can be
    seat 3 while a later joiner holds seat 1; seat order alone would then
    give the joiner slot 0 and the parent slot 1. Everybody else keeps seat
    order, so a room whose host is seat 1 gets exactly the map it always did.
    """
    ps = playing_seats(room)
    order = ([room.host_seat] if room.host_seat in ps else []) + \
        [s for s in ps if s != room.host_seat]
    return {seat: slot for slot, seat in enumerate(order)}


def name_field_count(room):
    """FOUR FIELDS OR SIXTEEN, decided by the same number that decides the
    game's wire, and by nothing else.

    run vs16, and the coordinator's ruling carried in README.md section 3:
    NAMES and COLORS move together, in one coordinated version change, when the
    wire moves. The wire moves at NARROW_PLAYERS, so this does too, and both
    strings read this one function so they cannot come out different lengths.

    A room nobody dialled above four gets FOUR, which is what every existing
    deployment, every shipped launcher and every existing proof gets.
    """
    return DIAL_HARD_MAX if room.match_dial > NARROW_PLAYERS else NARROW_PLAYERS


def build_names(room):
    """SM64DS_VS_NAMES: comma-separated fields in SLOT order, with exactly
    three commas (four fields) or exactly fifteen (sixteen). Built ONCE here,
    by the server, so every member's plan carries the byte-identical string and
    no launcher assembles it -- which is what makes "identical env on every
    launcher" structural.
    """
    n = name_field_count(room)
    fields = [""] * n
    for seat, slot in room.slot_of.items():
        if 0 <= slot < n:
            fields[slot] = room.members[seat].game_name
    return ",".join(fields)


def build_colors(room):
    """SM64DS_VS_COLORS: four comma-separated fields in SLOT order, always with
    exactly three commas, each field either empty or "bbbbbb:ssssss".

    Built ONCE here, by the server, for the same reason `names` is: every
    member's plan then carries the byte-identical string, so every copy of the
    game generates every player's palette row from the same input and arrives at
    the same bytes. That is the whole design -- no palette on the wire, nothing
    to trust past six hex digits per colour, and no way for two clients to
    disagree about what somebody looks like.

    An empty field is a slot that picked nothing, and the game leaves that
    player on the ROM's built-in colour. All four empty is a legal string and
    the game says so and moves on; the launcher drops the variable in that case
    anyway, which is cheaper.
    """
    n = name_field_count(room)
    fields = [""] * n
    for seat, slot in room.slot_of.items():
        if 0 <= slot < n:
            fields[slot] = room.members[seat].game_colors
    return ",".join(fields)


def member_plan(room, seat):
    """The per-member go plan (spec 4.3). Spectators get an empty plan and spawn
    nothing; playing members get the whole launch instruction. Every field a
    playing member gets is the frozen match state, so all playing plans are
    identical except role/slot/spawn_delay.
    """
    m = room.members.get(seat)
    if m is None or not m.playing:
        return {"playing": False}
    if room.session_type in ROOM_KINDS:
        # THE PARTY / COOP PLAN (v6). The same hub plan an adventure room has,
        # plus what the launcher needs to launch everyone together: `started`
        # and `launch` (the host's Start count: launch once per number, and a
        # plan that arrives already started is a late joiner's cue to launch
        # on joining), `host` (whose launcher holds the coop save), and the
        # room's seat count. The coop save's display name rides it; its PATH
        # does not exist on this side. The launcher turns this into the
        # SM64DS_ROOM_KIND / SM64DS_PARTY / SM64DS_COOP* environment.
        plan = {"playing": True, "session_type": room.session_type,
                "code": room.comms_code, "relay": RELAY_ADDR,
                "relay_v": RELAY_V, "want_seat": WANT_ANY_SEAT,
                "host": seat == room.host_seat, "started": room.started,
                "launch": room.launch_no, "room_seats": room.seat_cap(),
                # ROOMSET1: the room's toggles, the same for every seat. The
                # launcher turns each into SM64DS_ROOM_<NAME>=<0|1>.
                "settings": dict(room.settings)}
        if room.session_type == "coop":
            plan["save_name"] = room.save_name
        return plan
    if room.session_type == "adventure":
        # THE ADVENTURE PLAN (v5, design section 5): the room's hub code, the
        # relay, and "any seat". The launcher boots the TITLE with
        # SM64DS_ADVENTURE=1 and the comms code/relay, and sets no
        # SM64DS_LEVEL and no SM64DS_VS_*: the player plays their own save.
        return {"playing": True, "session_type": "adventure",
                "code": room.comms_code, "relay": RELAY_ADDR,
                "relay_v": RELAY_V, "want_seat": WANT_ANY_SEAT}
    # THE PLAN GUARD (run mp1, L-J1). A VS plan is only ever the frozen
    # match's, so a seat the match did not freeze gets the spectator's empty
    # plan -- never a default slot. `slot_of.get(seat, 0)` used to hand such a
    # seat slot 0, which is the host's: a watcher promoted mid-match (C4), a
    # seat number reused by a joiner, or an old `go` event replayed from the
    # ring after the match ended would all have been told to join as the
    # parent's slot. slot_of is empty in lobby, so this also covers the last.
    if seat not in room.slot_of:
        return {"playing": False}
    slot = room.slot_of[seat]
    # "parent" is slot 0, which assign_slots gives the host at `start`. Read
    # from the frozen slot rather than the live host seat (run mp1 A4): an
    # arena match keeps running when its host leaves and the host role passes
    # mid-match, and a plan re-sent during `go` must still name the same
    # roles. In a lockstep match the host cannot change mid-match, so this is
    # the same answer it always was.
    role = "parent" if slot == 0 else "child"
    plan = {
        "role": role,
        "code": room.comms_code,
        "relay": RELAY_ADDR,
        # The session type, so the launcher knows which game to boot: "vs"
        # here (the adventure plan returned above). It exports
        # SM64DS_VS_MAP=<map> and the ROM's own VS start sets the mode byte.
        "session_type": room.session_type,
        "map": room.match_map,
        "players": room.agreed_players,
        "slot": slot,
        # The parent goes first; children wait 1.5 s (spec 4.3). It is an
        # optimisation, not the guarantee -- the child's JOIN retries anyway.
        "spawn_delay_ms": 0 if role == "parent" else 1500,
        "playing": True,
        "names": room.names,
        # What the host ASKED for, beside `players`, which is what actually
        # turned up. The launcher exports `players`; `match_players` exists so
        # a launcher can say "3 of the 4 you picked" without guessing.
        "match_players": room.match_dial,
        # Every playing plan carries EVERY slot's colours, not just this
        # seat's: each client draws all four Yoshis, so each client needs
        # all four rows.
        "colors": room.colors,
        # v5 (design section 5). relay_v: the relay protocol to speak.
        # want_seat: the seat to ask the relay for; in a VS star session it
        # is the slot (parent 0, children 1..15), so the relay's stamp and
        # the game's slot are the same number.
        "relay_v": RELAY_V,
        "want_seat": slot,
        # The match settings frozen at arm. The launcher exports luigi and
        # seed as SM64DS_VS_LUIGI_INFECTION / SM64DS_VS_LUIGI_SEED, voice as
        # SM64DS_VOICE_ALLOWED and digest as SM64DS_SESSION_DIGEST. The seed
        # is minted for every match; the digest counts it only with Luigi on.
        "luigi": room.match_luigi,
        "seed": room.match_seed,
        "voice": room.match_voice,
        # run mp1 A4: "arena" or "lockstep", frozen at arm. The launcher
        # exports SM64DS_VS_NETMODE=arena for an arena plan, nothing for a
        # lockstep one; the game opens the matching relay session, and the
        # digest below counts it, so the two can never pair by accident.
        "netmode": room.match_netmode or "lockstep",
        "digest": session_digest(room.match_map, room.agreed_players,
                                 room.match_win_mode, room.match_star_target,
                                 room.match_king_target, room.match_luigi,
                                 room.match_seed,
                                 netmode=room.match_netmode or "lockstep"),
    }
    if room.match_win_mode == "stars":
        plan["star_target"] = room.match_star_target
    # KING OF THE STAR: the launcher exports SM64DS_VS_KING_TARGET=<n> so the
    # host enforces one star all match and the first-to-N-points win.
    if room.match_win_mode == "king":
        plan["king_target"] = room.match_king_target
    return plan


def _reset_match(room):
    """Back to a clean lobby: clear the frozen match state and every armed
    flag, then seat the next match. Does NOT push an event; the caller pushes
    result/failed.

    THE DIAL RUNS HERE (run mp1, L-J1). A seat that sat a match out -- a
    player dropped by its own `failed`, or anybody who joined while the match
    ran -- is `playing = False` for that match's life, because nothing may be
    promoted into a frozen roster. The match ending is the moment that stops
    being true, so the roster is brought back up to the host's dial exactly as
    a vacancy in the lobby would bring it up: lowest seats first, nobody who
    is already playing moved. A room whose dial is below its head count keeps
    its extra members watching, as it always did. The view every poll carries
    shows the new roster; no event kind is added for it."""
    room.state = "lobby"
    room.match = None
    room.comms_code = None
    room.agreed_players = 0
    room.match_star_target = None
    room.match_king_target = None
    room.match_dial = 0
    room.match_luigi = 0
    room.match_voice = 0 if VOICE_SHELVED else 1
    room.match_seed = 0
    room.match_netmode = None
    room.names = ""
    room.colors = ""
    room.slot_of = {}
    room.result_votes = {}
    room.vote_at = {}
    room.arm_deadline = None
    room.go_at = None
    room.match_start = None
    for m in room.members.values():
        m.armed = False
    room.apply_dial()


def match_to_lobby(room, reason):
    """A match that could not run returns to lobby with a `failed` event.
    Shared by an explicit `failed`, the arming deadline and the match timeout.
    """
    mid = room.match
    _reset_match(room)
    if mid:
        room.push("failed", match=mid, reason=reason)
    log("room %s failed match=%s reason=%s" % (room.code, mid, reason))


def finish_match(room, win, scores):
    """The match is over: record it, reset, push the `result` event."""
    mid = room.match
    room.last_result_match = mid
    _reset_match(room)
    room.push("result", match=mid, win=win, scores=list(scores))
    log("room %s result match=%s win=%s" % (room.code, mid, win))


def settled_vote(room):
    """The result a MAJORITY of the match's live seats agree on, or None.

    Live = frozen into slot_of and still playing (not dropped, not gone).
    Only live seats' votes count, and a vote needs strictly more than half of
    the live seats, so no single non-host seat can settle a match that two or
    more others are still playing."""
    live = [s for s in room.slot_of if in_frozen_match(room, s)]
    tally = {}
    for s in live:
        v = room.result_votes.get(s)
        if v is not None:
            tally[v] = tally.get(v, 0) + 1
    for v, n in tally.items():
        if 2 * n > len(live):
            return v
    return None


def host_silent_vote(room, now):
    """The result to end a match with because the host went silent, or None
    (run mp1, L-J1B; REVLOBBYJOIN1 section 3).

    EVERY live non-host seat has posted the SAME result, and the host has
    sent no request since the first of those votes came in, for
    HOST_SILENT_S. Every guest's console printed the same MATCH OVER, so the
    match is over; what is missing is only the host's launcher (it died at
    MATCH OVER without a `leave`). With three or more live seats the guests
    agreeing is already a majority (settled_vote), so in practice this is the
    live pair's rule, which otherwise waited out MATCH_TIMEOUT_S.

    "Host silent after ANY vote" would NOT be safe: an old launcher's quitter
    posts a draw mid-match, and a busy game can starve the host's poll (the
    sweep's 45 s note). Requiring every guest to agree is what rules that out:
    a quitter's draw next to a real result is two results, and a lone guest
    in a pair that quit took the match down with it anyway."""
    if room.state not in ("go", "in_match"):
        return None
    host = room.members.get(room.host_seat)
    if host is None or not in_frozen_match(room, room.host_seat):
        return None
    guests = [s for s in room.slot_of
              if s != room.host_seat and in_frozen_match(room, s)]
    if not guests or any(s not in room.result_votes for s in guests):
        return None
    votes = set(room.result_votes[s] for s in guests)
    if len(votes) != 1:
        return None
    first = min(room.vote_at.get(s, now) for s in guests)
    if host.seen > first:
        return None
    # SILENCE THE RATE LIMITER CAUSED IS NOT SILENCE (REVLOBBYJOIN1-B note
    # C). The budget is per address and charged before the verb runs, so a
    # guest on the host's own public address (a household NAT) could flood it
    # into refusals and the live host's polls would never stamp `seen`. The
    # quiet time counts from the later of the first vote and the host
    # address's last refusal; a host that is really gone still settles once
    # its address has been left alone for HOST_SILENT_S.
    quiet_from = max(first, BUCKETS.last_refused(host.last_addr))
    if now - quiet_from < HOST_SILENT_S:
        return None
    return votes.pop()


def settle_after_host_change(room):
    """An arena match's host role just passed mid-match (run mp1 A4). If the
    new host already posted a result for it, the host's result ends a match,
    so it does now; otherwise the shrunk live roster may already be a
    majority (maybe_settle)."""
    if room.state in ("go", "in_match"):
        v = room.result_votes.get(room.host_seat)
        if v is not None and in_frozen_match(room, room.host_seat):
            finish_match(room, v[0], v[1])
            return
    maybe_settle(room)


def maybe_settle(room):
    """After the live roster of a running match shrank (a drop or a leave),
    the votes already in may now be a majority: end the match if so."""
    if room.state in ("go", "in_match"):
        v = settled_vote(room)
        if v is not None:
            finish_match(room, v[0], v[1])


def try_go(room, now):
    """arming -> go once every playing seat has POSTed ready. Pushes the `go`
    event, whose plan do_poll injects per member.

    THE ROSTER THIS TESTS IS THE FROZEN ONE, NOT THE LIVE ONE, and that is the
    whole of the fix for the arming-roster hole. `agreed_players`, `names` and
    `slot_of` are all frozen at `start`; testing "every playing seat is armed"
    against the LIVE roster meant that if the only other playing seat left inside
    the arming window, `playing_seats` shrank to one, the host's own `ready`
    satisfied all(...), and the room went to `go` carrying a plan that still said
    players=2 and still credited a departed player in `names`. The host then
    spawned, forced SM64DS_VS_PLAYERS=2, waited for a child that could not come,
    and was rescued 20 s later by its own watchdog -- self-healing, but the room
    knowingly issued a plan for a match it could not run. Closing a launcher
    during a twenty-second countdown is ordinary behaviour, not an exotic input.

    COMPARED AS A SET OF SEATS, not as a count, and that is deliberate: a count
    alone still lets the promotion case through. Seats 1(host), 2, 3(spectator)
    freeze at agreed_players=2 with slot_of={1:0, 2:1}; seat 2 leaving during
    arming PROMOTES seat 3, so the live playing seats are {1,3} and the count is
    still 2 -- but seat 3 holds no frozen slot, so member_plan would hand it the
    default slot 0 and collide with the parent, on a `names` string that credits
    the player who left. `slot_of`'s keys ARE the seats frozen at `start`, so
    comparing against them costs nothing and closes both shapes at once. A room
    whose roster moved simply stalls here and the arming deadline returns it to
    lobby with `member_not_ready`, which is the honest outcome.

    (Since run mp1 L-J1 drop_member promotes nobody while a match is frozen,
    so the promotion shape above cannot arise any more; the set comparison
    stays as the second lock on the same door.)
    """
    ps = playing_seats(room)
    frozen = sorted(room.slot_of)
    if ps and ps == frozen and all(room.members[s].armed for s in ps):
        room.state = "go"
        room.go_at = now
        room.push("go", match=room.match)
        log("room %s go match=%s" % (room.code, room.match))


def do_start(body, who, now):
    """Host only. Freeze the match and open the arming window (spec 3.8, 4.1).

    NOTHING SPAWNS HERE. This mints the match id and a fresh comms code, freezes
    the params/roster/slot map/names, and moves the room to `arming`. Every
    playing launcher must then POST `ready` before anybody gets a `go` plan.
    """
    err = shape_for("start", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    room.members[seat].seen = now
    if seat != room.host_seat:
        return 403, {"error": "not_host"}
    if room.session_type in ROOM_KINDS:
        # THE HOST'S PLAY IN A PARTY OR COOP ROOM (v6). No arming, no ready,
        # no match id: the room is released, every member's plan now says
        # `started` with a new `launch` number, and each launcher launches its
        # own game into the room's hub. Anyone who joins later is handed a
        # started plan and launches on joining. Pressed again (nobody's game
        # up any more) it relaunches everyone. Needs only the host: a coop
        # host may start alone and have friends drop in.
        if room.state != "lobby":
            return 409, {"error": "not_in_lobby"}
        room.started = True
        room.launch_no = min(room.launch_no + 1, LAUNCH_NO_MAX)
        room.push("started", launch=room.launch_no)
        log("room %s start %s launch=%d (%d in)"
            % (code, room.session_type, room.launch_no, len(room.members)))
        return 200, {"cursor": room.seq, "launch": room.launch_no}
    if room.session_type != "vs":
        # No start gate in an adventure room (design section 5): every member
        # got its plan at create/join and presses Play whenever it likes.
        return 409, {"error": "not_vs"}
    if room.state != "lobby":
        return 409, {"error": "not_in_lobby"}
    ps = playing_seats(room)
    if len(ps) < 2:
        return 409, {"error": "not_enough_players"}
    if any(not room.members[s].pre_ok for s in ps):
        return 409, {"error": "member_not_ready"}

    # Freeze everything, at one instant, so a join/leave/params during arming
    # cannot make two members disagree (spec 4.7 "where in the start sequence").
    room.match = new_match_id()
    room.comms_code = new_comms_code()
    room.agreed_players = len(ps)
    room.match_map = room.map
    room.match_win_mode = room.win_mode
    room.match_star_target = room.star_target
    room.match_king_target = room.king_target
    room.match_dial = room.match_players
    room.match_luigi = room.luigi
    room.match_voice = room.voice
    room.match_seed = new_seed()
    # run mp1 A4: arena or lockstep, decided once here on the frozen roster
    # and settings, and the same for every plan of this match.
    room.match_netmode = arena_for(room, room.agreed_players,
                                   room.match_win_mode, room.match_luigi)
    room.slot_of = assign_slots(room)
    room.names = build_names(room)
    room.colors = build_colors(room)
    for m in room.members.values():
        m.armed = False
    room.state = "arming"
    room.arm_deadline = now + ARM_DEADLINE_S
    room.push("arming", match=room.match, deadline_ms=ARM_DEADLINE_S * 1000,
              netmode=room.match_netmode)
    log("room %s arm match=%s players=%d luigi=%d voice=%d netmode=%s "
        "deadline=%ds"
        % (code, room.match, room.agreed_players, room.match_luigi,
           room.match_voice, room.match_netmode, ARM_DEADLINE_S))
    return 200, {"cursor": room.seq, "match": room.match}


def do_ready(body, who, now):
    """A playing member: "my launcher can spawn this match." When the last
    playing seat says so, the room moves to `go` (spec 3.9, 4.1)."""
    err = shape_for("ready", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    match, err = v_match(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    # Every ready/result/failed carries the match id, and a stale one is 409:
    # that single check makes them idempotent and un-replayable across matches.
    if room.match is None or match != room.match:
        return 409, {"error": "stale_match"}
    # SPECTATORS DO NOT ARM A MATCH, and they are refused rather than silently
    # ignored, so that `ready`, `failed` and `result` -- the three verbs spec 3.2
    # restricts to "a seated PLAYING member" -- all answer the same way. This one
    # used to be a silent no-op, which was harmless but made the trio disagree.
    if not in_frozen_match(room, seat):
        return 403, {"error": "not_playing"}
    if room.state == "arming":
        m.armed = True
        try_go(room, now)
    # A ready that arrives after the room already went to `go`/`in_match` for the
    # same match is a harmless late duplicate: accepted, idempotent.
    return 200, {"cursor": room.seq}


def do_failed(body, who, now):
    """A playing member reporting a match that could not run. ANY playing
    member's `failed` returns the whole room to lobby (spec 4.4/4.6)."""
    err = shape_for("failed", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    match, err = v_match(body)
    if err:
        return 400, {"error": err}
    reason, err = v_reason(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    if room.match is None or match != room.match:
        return 409, {"error": "stale_match"}
    # ONLY A PLAYING MEMBER MAY ABORT A MATCH (spec 3.2: "a seated PLAYING
    # member"). Without this a spectator -- which is anyone who walked in with a
    # room code, and the design says a room code is not a secret -- could return
    # the room to lobby mid-match, and every launcher that had not yet seen
    # `session up` would kill its own game. The host's only remedy, `kick`, is
    # refused while the room is not in lobby, so the same client could do it to
    # match after match.
    if not in_frozen_match(room, seat):
        return 403, {"error": "not_playing"}
    # Only a real pre-result phase can be failed. If a result already landed the
    # match id was cleared, so this is stale_match above; here the room is still
    # arming/go/in_match and the reporter turns it back to lobby for everybody.
    if room.state == "arming":
        # Nobody is playing yet and the plan has to be remade: the whole
        # room goes back to lobby, as it always did.
        match_to_lobby(room, reason)
    elif room.state in ("go", "in_match"):
        # 0.3.2 field lesson (the first sixteen-player night): once the match
        # has been sent off, ONE member's failure must not end it for the
        # six who are playing. A seven-player match ran for over a minute
        # while one launcher, which never saw its own game pair, reported
        # no_pairing and the lobby pulled every launcher's rug. The game
        # itself already tolerates a peer leaving mid-match (the carrier
        # clears the leaver's live bit), so the honest answer is to drop the
        # one who failed and let the rest play on. The HOST is the parent of
        # the session, so its failure still ends the match; and a match that
        # drops below two players ends too.
        if seat == room.host_seat and room.match_netmode != "arena":
            match_to_lobby(room, reason)
        else:
            # (An ARENA match's host is dropped like anybody else, run mp1
            # A4: nobody's game waits on the host's. The host keeps the
            # room's host role; the match ends by the others' results.)
            m.playing = False
            m.armed = False
            room.push("dropped", seat=seat, reason=reason)
            log("room %s dropped seat %d from match=%s reason=%s (%d still playing)"
                % (room.code, seat, room.match, reason, room.playing_count()))
            if room.playing_count() < 2:
                match_to_lobby(room, reason)
            else:
                maybe_settle(room)
    return 200, {"cursor": room.seq}


def do_result(body, who, now):
    """A playing member reporting a finished match (spec 3.9, 5.4).

    WHO ENDS THE MATCH (run mp1, L-J1; plan.txt G5). It used to be the FIRST
    valid result from anybody, and that was the bug: a launcher whose player
    closed the game mid-match reported a draw, the room flipped to lobby
    with a fake 0-0-0-0 while everybody else was still playing, and their
    real result then got 409 stale_match. Now:
      * the HOST's result ends the match. The host is the parent of the
        session; its game cannot be running on without it, so a result from
        the host is true about the whole room either way;
      * a non-host's result is a VOTE. It ends the match only when strictly
        more than half of the match's live seats (frozen, still playing)
        have posted the SAME win and scores (settled_vote), and a drop or a
        leave that shrinks the live roster re-counts (maybe_settle). One
        quitter's result therefore never ends a match two or more others are
        still in. A vote that does not settle is answered 200 like any
        accepted result; the room's state and the `result` event are what
        say the match ended. A seat's first vote stands.
    Later results for a finished match are idempotent (200) as before."""
    err = shape_for("result", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    match, err = v_match(body)
    if err:
        return 400, {"error": err}
    win, err = v_win(body)
    if err:
        return 400, {"error": err}
    scores, err = v_scores(body)
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    # ONLY A PLAYING MEMBER MAY REPORT A RESULT (spec 3.2), the same gap as
    # `failed` above and closed the same way. A spectator could otherwise end any
    # live match AND write the scoreline every member sees in the room.
    if not m.playing:
        return 403, {"error": "not_playing"}

    if room.match is not None and match == room.match:
        if not in_frozen_match(room, seat):
            return 403, {"error": "not_playing"}
        vote = (win, tuple(scores))
        if seat not in room.result_votes:
            room.result_votes[seat] = vote
            room.vote_at[seat] = now
        decided = vote if seat == room.host_seat else settled_vote(room)
        if decided is None:
            log("room %s result vote seat %d match=%s (%d of %d live)"
                % (code, seat, room.match, len(room.result_votes),
                   sum(1 for s in room.slot_of if in_frozen_match(room, s))))
            return 200, {"cursor": room.seq}
        # The match is over. Append the event, discard the comms code, clear
        # the match, drop back to lobby. Params, seats, nicks, tokens and chat
        # all survive (spec 5.6): "same again" is one button.
        finish_match(room, decided[0], decided[1])
        return 200, {"cursor": room.seq}
    if match == room.last_result_match:
        # Both players reporting is normal and harmless: the second is ignored.
        return 200, {"cursor": room.seq}
    return 409, {"error": "stale_match"}


def do_preflight(body, who, now):
    """A seated member updating its own pre-flight answer.

    WHY THIS VERB EXISTS, from a live match. `pre_ok` was measured once when the
    Multiplayer window opened and sent once at create/join, so a player who
    unpacked their ROM while that window sat open kept a stale "not ready" on
    their seat with no way to shift it. The recovery a real pair actually had to
    find was: close the window, reopen it, check it went green, make a NEW room
    and rejoin. Nobody discovers that alone. A launcher can now re-run its own
    pre-flight and say so.

    IT ONLY EVER SETS THE CALLER'S OWN SEAT. There is no seat argument: a member
    can speak for itself and for nobody else, so this adds no authority anywhere.

    ALLOWED IN EVERY LIVE STATE, not just lobby, and that is deliberate. During a
    match the flag feeds nothing -- the roster froze at `start` -- so letting it
    through is harmless, and it means a player who finishes unpacking while a
    match runs is already ready for the rematch instead of blocking it.
    """
    err = shape_for("preflight", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    pre_ok, err = v_bool(body, "pre_ok")
    if err:
        return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    m.last_addr = who

    # UNCHANGED IS A NO-OP, and that is what bounds this verb. A launcher re-runs
    # its pre-flight on a timer and on window focus, so the common case by far is
    # "still the same answer": no event, no cursor movement, no work for anybody
    # else. Only a real change costs a push.
    if m.pre_ok == pre_ok:
        return 200, {"cursor": room.seq}
    m.pre_ok = pre_ok

    # A NEW EVENT KIND, and it is safe by the contract's own design: section 3.5
    # says a launcher that meets an unknown `kind` ignores it and keeps its
    # cursor moving, which is exactly the leniency that lets a server gain a kind
    # without bricking an older launcher. It is pushed rather than left to the
    # `view` alone so that a member holding a 25-second long poll learns now
    # rather than when its wait expires -- the whole complaint was a roster that
    # would not refresh.
    room.push("preflight", seat=seat, display=m.display, pre_ok=pre_ok)
    log("room %s preflight seat %d %s"
        % (code, seat, "ready" if pre_ok else "not-ready"))
    return 200, {"cursor": room.seq}


def do_color(body, who, now):
    """A seated member changing its own two colours.

    The owner's order: "the ability to choose a hex color to make your yoshi
    and its shoes so two [colors]. and the main color shows next to your name in
    lobby so people can identify you."

    Modelled on `preflight` and for the same reasons. IT ONLY EVER SETS THE
    CALLER'S OWN SEAT -- there is no seat argument, so a member speaks for
    itself and for nobody else and this adds no authority anywhere. UNCHANGED IS
    A NO-OP, so a launcher that re-sends on every keystroke or on window focus
    costs the room nothing; only a real change pushes an event, which is what
    makes the other players' swatches move NOW rather than when their long poll
    expires.

    BOTH COLOURS OR NEITHER, and both are required here rather than optional:
    this verb's whole job is to set a pair, and a caller who wants to go back to
    the built-in Yoshi says so with two empty strings.

    LEGAL ONLY IN `lobby`, and this is the one place it differs from
    `preflight`. A colour is spent at the arming freeze, exactly like a
    nickname: once the room has frozen SM64DS_VS_COLORS into everybody's plan,
    letting a seat change its colour would put a swatch on screen that no
    running game is wearing. Changing it during a match is refused with
    `not_in_lobby` and the player simply changes it before the rematch.
    """
    # THE VERB'S OWN VERSION IS IN THE TABLE, not here. This used to be a
    # `bad_version` check written out below the field check; VERB_FIELDS marks
    # the whole verb as arriving in v2 and `shape_for` refuses a v1 caller as
    # `bad_field` -- the same answer every other out-of-version key gets, so a
    # v1 caller cannot tell a v2 verb apart from a misspelled one.
    err = shape_for("color", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}

    # "" and "" is how a player goes back to the built-in colour, so the empty
    # pair is spelled out here rather than reached through v_color_pair.
    if body["color"] == "" and body["shoes"] == "":
        color, shoes = "", ""
    else:
        color, err = v_color(body, "color")
        if err:
            return 400, {"error": err}
        shoes, err = v_color(body, "shoes")
        if err:
            return 400, {"error": err}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    m = room.members[seat]
    m.seen = now
    m.last_addr = who
    if room.state != "lobby":
        return 409, {"error": "not_in_lobby"}

    if m.color == color and m.shoes == shoes:
        return 200, {"cursor": room.seq}
    m.color = color
    m.shoes = shoes
    room.push("color", seat=seat, display=m.display, color=color, shoes=shoes)
    log("room %s color seat %d %s"
        % (code, seat, ("%s/%s" % (color, shoes)) if color else "built-in"))
    return 200, {"cursor": room.seq}


def do_poll(body, who, now):
    """The only push channel, and the entire reliability story.

    `cursor` is a per-room monotonic sequence, so nothing can be missed: a
    client that misses ten polls gets all ten events on the eleventh. The
    request is held until there is something newer than the cursor or `wait`
    seconds pass, and it stamps the member's heartbeat on the way in.
    """
    err = shape_for("poll", body)
    if err:
        return 400, {"error": err}
    code, err = v_room(body)
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    cursor, err = v_int(body, "cursor", 0, 2147483647, "bad_cursor")
    if err:
        return 400, {"error": err}
    wait = 0
    if "wait" in body:
        # Clamped, not rejected: the spec makes a wait of 0 a plain short poll
        # with an identical contract, so a middlebox that dislikes long
        # requests is answered with a client-side constant, not a redesign.
        raw = body.get("wait")
        if isinstance(raw, bool) or not isinstance(raw, int):
            return 400, {"error": "bad_field"}
        wait = max(0, min(WAIT_MAX, raw))

    room = ROOMS.get(code)
    if room is None:
        return 404, {"error": "no_such_room"}
    seat = seat_of(room, token)
    if seat is None:
        return refuse_member(room, token, now)
    room.members[seat].seen = now
    room.members[seat].last_addr = who

    global WAITERS
    if wait > 0 and not room.since(cursor) and room.state != "closed":
        if WAITERS < MAX_WAITERS:
            WAITERS += 1
            try:
                deadline = time.monotonic() + wait
                while not room.since(cursor) and room.state != "closed":
                    left = deadline - time.monotonic()
                    if left <= 0:
                        break
                    room.cond.wait(left)
                    # The room can be reaped while we sleep.
                    if ROOMS.get(code) is not room:
                        return 404, {"error": "no_such_room"}
                    # Losing the seat while holding a long poll is the NORMAL
                    # way a kicked launcher finds out, so it gets the reason
                    # rather than a bare "you are not a member".
                    if seat not in room.members and room.state != "closed":
                        return refuse_member(room, token, now)
            finally:
                WAITERS -= 1
        # Over MAX_WAITERS the answer is immediate, exactly as if wait were 0.

    # THE HEARTBEAT IS NOT RE-STAMPED HERE, and that is deliberate.
    #
    # It was, once. The stamp went in again after the wait, which credits a
    # member for the whole time it spent asleep in a hold it may not have been
    # alive for: a launcher killed one second into a twenty second hold still
    # looked healthy for twenty seconds after it died, and the seat then took
    # MEMBER_TIMEOUT_S plus the whole hold to come free. Measured at 63 seconds
    # against a 45 second timeout, in the two-window end-to-end.
    #
    # A member proves it is there by SENDING a request. The stamp at the top of
    # this function is that proof and it is the only one. A client polling with
    # a 20 second wait still stamps every 20 seconds, comfortably inside 45.

    out = {"cursor": room.seq, "view": room.view(seat)}
    if room.needs_resync(cursor):
        out["resync"] = True
        events = list(room.chat)
    else:
        events = room.since(cursor)
    # THE GO PLAN IS PER-MEMBER, injected here rather than stored per member in
    # the shared event ring. Each member's `go` event carries its own plan
    # (parent vs child, its slot, its spawn delay); everything else is the
    # frozen match state and is identical across members.
    if any(e.get("kind") == "go" for e in events):
        events = [dict(e, plan=member_plan(room, seat))
                  if e.get("kind") == "go" else e for e in events]
    out["events"] = events
    # AND a top-level plan whenever the room is in `go`, so a client that fell
    # behind the ring and resynced (getting chat only, no `go` event) still has
    # its plan and can spawn. Belt and suspenders; the launcher prefers the
    # event's plan and falls back to this.
    if room.state == "go" or (room.session_type in HUB_TYPES
                              and room.state != "closed"):
        out["plan"] = member_plan(room, seat)
    return 200, out


def world_plan(w):
    return {"code": w.code, "relay": RELAY_ADDR, "relay_v": RELAY_V,
            "want_seat": WANT_ANY_SEAT, "world_no": w.no}


def do_world(body, who, now):
    """v5: "put me in a world". The lowest-numbered world of the caller's game
    build with a free seat (members seen in the last WORLD_MEMBER_S), else a
    new one. The answer is a hub plan like an adventure room's, plus the world
    number for the launcher to show and a token for `world_ping`.

    The nick and colours are checked against the same grammar as everywhere
    else and then dropped: nothing about a player is kept in a world.
    """
    err = shape_for("world", body)
    if err:
        return 400, {"error": err}
    nick, err = v_nick(body)
    if err:
        return 400, {"error": err}
    color, shoes, err = v_color_pair(body)
    if err:
        return 400, {"error": err}
    game, err = v_game(body)
    if err:
        return 400, {"error": err}
    skip = None
    if "skip" in body:
        skip, err = v_code(body, "skip")
        if err:
            return 400, {"error": err}
    if not world_open():
        # A world needs the relay's hub rules (RELAY_V 2) and the build allow
        # list (GAME_IDS); without either the pool is off.
        return 503, {"error": "world_off"}
    if not game_allowed(game):
        return 409, {"error": "unknown_game"}
    if not BUCKETS.allow_join(who, now):
        return 429, {"error": "too_fast"}

    retired = retire_over_cap(who, now)
    if retired:
        log("world retire %d older token(s) of %s (cap %d)"
            % (retired, blunt_host(who), WORLD_ADDR_TOKENS))
    mine = sorted((w for w in WORLDS.values() if w.game == game),
                  key=lambda w: w.no)
    pick = None
    for w in mine:
        if w.code != skip and w.count(now) < WORLD_SEATS:
            pick = w
            break
    if pick is None:
        if len(WORLDS) >= MAX_WORLDS:
            return 503, {"error": "full"}
        taken = {w.no for w in mine}
        no = 1
        while no in taken:
            no += 1
        code = None
        for _ in range(8):
            cand = new_comms_code()
            if cand not in WORLDS:
                code = cand
                break
        if code is None:
            return 503, {"error": "busy"}
        pick = World(game, no, code, now)
        WORLDS[code] = pick
        log("world %s open no=%d (%d worlds)" % (code, no, len(WORLDS)))
    token = new_token()
    pick.members[token] = [now, who]
    pick.empty_since = None
    out = world_plan(pick)
    out["token"] = token
    out["members"] = pick.count(now)
    log("world %s join %s no=%d (%d/%d)" % (pick.code, blunt_host(who),
                                            pick.no, out["members"],
                                            WORLD_SEATS))
    return 200, out


def do_world_ping(body, who, now):
    """v5: "I am still in this world", every 10 s while the game runs. A token
    unheard for WORLD_MEMBER_S has already been forgotten and is refused, and
    the launcher asks `world` again."""
    err = shape_for("world_ping", body)
    if err:
        return 400, {"error": err}
    code, err = v_code(body, "code")
    if err:
        return 400, {"error": err}
    token, err = v_token(body)
    if err:
        return 400, {"error": err}
    w = WORLDS.get(code)
    if w is None:
        return 404, {"error": "no_such_world"}
    seen = w.members.get(token)
    if seen is None or now - seen[0] > WORLD_MEMBER_S:
        w.members.pop(token, None)
        return 403, {"error": "not_a_member"}
    seen[0] = now
    return 200, {"ok": True, "world_no": w.no, "members": w.count(now)}


VERBS = {
    "create": do_create,
    "join": do_join,
    "poll": do_poll,
    "chat": do_chat,
    "params": do_params,
    "preflight": do_preflight,
    "color": do_color,
    "start": do_start,
    "settings": do_settings,
    "ready": do_ready,
    "result": do_result,
    "failed": do_failed,
    "kick": do_kick,
    "leave": do_leave,
    "world": do_world,
    "world_ping": do_world_ping,
}

# EVERY VERB HAS A ROW, AND EVERY ROW HAS A VERB. Checked at import rather than
# on the first request that needs it: a verb added to VERBS with no row in
# VERB_FIELDS would otherwise be a KeyError inside a handler -- a 500 on a live
# box instead of a service that refuses to start.
assert set(VERBS) == set(VERB_FIELDS), (
    "VERBS and VERB_FIELDS disagree: %s"
    % sorted(set(VERBS) ^ set(VERB_FIELDS)))


# ------------------------------------------------------------ host events
#
# PROTOTYPE (lane EVENTS1). One admin-only endpoint, POST /port/lobby/admin/event,
# lets the host put a message, a song or a timed modifier on every player's
# screen in the always-online world. It is OFF (404, exactly like any unknown
# verb) unless BOTH of these are in the lobby's environment:
#
#   EVENT_ADMIN_TOKEN   the host's secret, 24+ characters, sent as
#                       "Authorization: Bearer <token>". Never in the game zip
#                       and never in a log line.
#   EVENT_KEY           16+ characters, shared with the relay
#                       (SM64DS_EVENT_KEY there). The lobby signs each command
#                       with it; the relay refuses anything unsigned or old.
#
# and EVENT_CMD_PORT names the relay's private command port (UDP, loopback).
# The lobby never forwards anything a player sent: the body is checked field by
# field below and re-encoded, so the relay sees only what this file built.
EVENT_ADMIN_TOKEN = os.environ.get("EVENT_ADMIN_TOKEN", "")
EVENT_KEY = os.environ.get("EVENT_KEY", "")
EVENT_CMD_PORT = env_int("EVENT_CMD_PORT", 0, 0, 65535)
EVENT_CMD_HOST = os.environ.get("EVENT_CMD_HOST", "127.0.0.1") or "127.0.0.1"
EVENT_ON = (len(EVENT_ADMIN_TOKEN) >= 24 and len(EVENT_KEY) >= 16
            and EVENT_CMD_PORT > 0)
EVENT_NAMES = {"message": (1, 0), "music": (2, 0), "clear": (4, 0),
               "speed": (3, 1), "onehit": (3, 2), "chaos": (3, 3),
               "race": (3, 4), "castle": (3, 5)}
_EVENT_LAST_TS = [0]


def event_bearer(headers):
    """The token the caller sent, or ''."""
    auth = headers.get("Authorization") or ""
    if auth.lower().startswith("bearer "):
        return auth[7:].strip()
    return (headers.get("X-Admin-Token") or "").strip()


def event_token_ok(given):
    return hmac.compare_digest(given.encode("utf-8", "replace"),
                               EVENT_ADMIN_TOKEN.encode("utf-8", "replace"))


def event_world(value, worlds):
    """The optional "world" field -> (hub code | "", error). `worlds` maps hub
    code -> world number. A world number (1, 2, ...) is looked up; a hub code
    must be one the lobby knows. Empty/absent = every world."""
    if value is None or value == "":
        return "", None
    if isinstance(value, bool):
        return None, "bad_world"
    if isinstance(value, int) or (isinstance(value, str) and value.isdigit()):
        hits = [c for c, no in worlds.items() if no == int(value)]
        if len(hits) == 1:
            return hits[0], None
        return None, "ambiguous_world" if hits else "unknown_world"
    if isinstance(value, str) and value in worlds:
        return value, None
    return None, "unknown_world"


def event_build(body, worlds=None):
    """A request body -> (command text | None, error code). Pure.
    `worlds`: hub code -> world number, for the optional "world" field."""
    if not isinstance(body, dict):
        return None, "bad_shape"
    name = body.get("event")
    if not isinstance(name, str) or name not in EVENT_NAMES:
        return None, "bad_event"
    etype, arg = EVENT_NAMES[name]
    if name == "music":
        arg = body.get("sublevel")
        if isinstance(arg, bool) or not isinstance(arg, int) or not 0 <= arg <= 255:
            return None, "bad_sublevel"
    secs = body.get("secs", 0)
    if isinstance(secs, bool) or not isinstance(secs, int) or not 0 <= secs <= 3600:
        return None, "bad_secs"
    text = body.get("text", "")
    if not isinstance(text, str) or len(text) > 70 or any(
            not 32 <= ord(c) < 127 for c in text):
        return None, "bad_text"
    if name == "message" and not text:
        return None, "bad_text"
    if name == "clear":
        secs = 0
    stars = body.get("stars", 0)
    if isinstance(stars, bool) or not isinstance(stars, int) or not 0 <= stars <= 150:
        return None, "bad_stars"
    if stars and name != "race":
        return None, "bad_stars"
    world, err = event_world(body.get("world"), worlds if worlds is not None else {})
    if err:
        return None, err
    ts = max(int(time.time() * 1000), _EVENT_LAST_TS[0] + 1)
    _EVENT_LAST_TS[0] = ts
    msg = text.encode("ascii").hex() if text else "-"
    line = "EVT1 %d %d %d %d %s %s %d" % (ts, etype, arg, secs, msg,
                                         world or "-", stars)
    sig = hmac.new(EVENT_KEY.encode("ascii", "replace"), line.encode("ascii"),
                   hashlib.sha256).hexdigest()[:32]
    return "%s|%s" % (line, sig), None


def event_send(cmd):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.sendto(cmd.encode("ascii"), (EVENT_CMD_HOST, EVENT_CMD_PORT))
    finally:
        sock.close()

# ------------------------------------------------------------------ reaper


def sweep(now):
    """One pass of the housekeeping. LOCK must be held.

    A closed room is held for CLOSED_KEEP_S so its members get the reason on
    their next poll rather than a bare 404, and only then deleted. Split out
    of the loop below so the unit tests can drive it with a clock of their own
    instead of sleeping through real minutes.
    """
    for code, room in list(ROOMS.items()):
        if room.state == "closed":
            if now - room.closed_at >= CLOSED_KEEP_S:
                del ROOMS[code]
                room.cond.notify_all()
            continue

        # THE MATCH STATE MACHINE'S TIMERS.
        #   arming, deadline passed  -> lobby + failed member_not_ready (spec 4.1)
        #   go, GO_GRACE elapsed     -> in_match (spec 4.1); a quiet transition,
        #                               no event: every launcher already spawned
        #                               on the `go` event.
        #   in_match, MATCH_TIMEOUT  -> lobby + failed timeout (spec 5.8), so a
        #                               crashed pair cannot strand the room.
        if room.state == "arming" and room.arm_deadline is not None \
                and now >= room.arm_deadline:
            match_to_lobby(room, "member_not_ready")
        elif room.state == "go" and room.go_at is not None \
                and now - room.go_at >= GO_GRACE_S:
            room.state = "in_match"
            room.match_start = now
            room.cond.notify_all()
        elif room.state == "in_match" and room.match_start is not None \
                and now - room.match_start >= MATCH_TIMEOUT_S:
            match_to_lobby(room, "timeout")
        # Every guest agrees and the host has been silent HOST_SILENT_S since
        # the first of their votes: the match is over (host_silent_vote).
        vote = host_silent_vote(room, now)
        if vote is not None:
            log("room %s host silent %ds after every guest's same result"
                % (room.code, HOST_SILENT_S))
            finish_match(room, vote[0], vote[1])

        # HEARTBEAT AND IDLE TIMEOUTS APPLY ONLY IN LOBBY. During a match a
        # member disconnecting is nothing (spec 6): the ROM copes, and the room
        # holds until a result or the 20-minute match timeout above. Dropping a
        # member -- worse, the host, which ends the match -- because a busy game
        # starved the poll loop for 45 s would kill a live match. Arming (<=20 s)
        # and go (<=5 s) are too short to trip the 45 s timeout, and their own
        # deadlines already catch a launcher that went away, so this guard is
        # about in_match and is expressed for the whole non-lobby span.
        if room.state == "lobby":
            for seat in sorted(room.members):
                if now - room.members[seat].seen > MEMBER_TIMEOUT_S:
                    drop_member(room, seat, "timeout")
                    if room.state == "closed":
                        break
            if room.session_type in HUB_TYPES:
                # Exempt from ROOM_IDLE_S; closed only after ADV_EMPTY_S empty.
                if room.members:
                    room.empty_since = None
                elif room.empty_since is None:
                    room.empty_since = now
                elif now - room.empty_since >= ADV_EMPTY_S:
                    room.close("idle")
            else:
                if room.state != "closed" and now - room.touched > ROOM_IDLE_S:
                    room.close("idle")
                if room.state != "closed" and not room.members:
                    room.close("idle")
        room.forget_old_kicks(now)

    # v5 world pool: forget stale members, drop a world empty for
    # WORLD_EMPTY_S. Its number is free for the next world of that build.
    for code, w in list(WORLDS.items()):
        w.forget_stale(now)
        if w.members:
            w.empty_since = None
        elif w.empty_since is None:
            w.empty_since = now
        elif now - w.empty_since >= WORLD_EMPTY_S:
            del WORLDS[code]
            log("world %s drop no=%d (empty)" % (code, w.no))

    BUCKETS.sweep(now)


def reaper():
    """Heartbeats, idle rooms, and the closed-room grace, once a second."""
    last_stats = time.monotonic()
    while True:
        time.sleep(SWEEP_INTERVAL_S)
        now = time.monotonic()
        with LOCK:
            sweep(now)
            if now - last_stats >= STATS_INTERVAL_S:
                last_stats = now
                log("stats rooms=%d members=%d worlds=%d world_members=%d "
                    "waiters=%d req=%d rej_rate=%d rej_shape=%d rej_auth=%d"
                    % (len(ROOMS), sum(len(r.members) for r in ROOMS.values()),
                       len(WORLDS),
                       sum(w.count(now) for w in WORLDS.values()),
                       WAITERS, REQ_COUNT, REJ_RATE, REJ_SHAPE, REJ_AUTH))


# -------------------------------------------------------------------- HTTP

PREFIX = "/port/lobby/"


class Handler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"
    server_version = "sm64ds-lobby"
    sys_version = ""

    # The per-connection read deadline. StreamRequestHandler.setup() reads this
    # and calls settimeout() with it, so every rfile read on this connection --
    # the request line, the headers, the body -- is bounded. main() overrides it
    # from HANDLER_TIMEOUT_S; this default is the floor if main is bypassed (the
    # security test imports the module and constructs handlers directly).
    timeout = HANDLER_TIMEOUT_S

    # The default handler logs every request to stderr with the full client
    # address in it. We log what we choose to log and nothing else.
    def log_message(self, fmt, *args):
        pass

    # -- plumbing --------------------------------------------------------

    def client_host(self):
        """The client's address, blunted for logging and keyed for limits.

        Behind Caddy the socket peer is always Caddy, so per-IP limits would
        be per-SERVER limits without this.

        THE LAST ENTRY OF X-FORWARDED-FOR, NEVER THE FIRST. The header is a
        comma-separated trail, oldest first, and the FIRST entry is the one
        furthest from the server -- the one a client can write itself. Reading
        it let a client forge a fresh address per request and walk straight
        through every per-address limit: 120 joins from 120 forged addresses
        with the join cap never engaging, on the review's own measurement.
        The LAST entry is the one the nearest proxy wrote, and it is correct
        under both of Caddy's conventions: if Caddy REPLACES the header the
        trail is one entry and first and last are the same, and if Caddy
        APPENDS the real peer the last entry is the one Caddy itself added.
        Either way the client cannot control it -- as long as Caddy is in
        front, which is what TRUST_XFF asserts.
        """
        peer = self.client_address[0] if self.client_address else ""
        if TRUST_XFF:
            xff = self.headers.get("X-Forwarded-For", "")
            if xff and len(xff) <= 256:
                near = xff.split(",")[-1].strip()
                if near and len(near) <= 45 and all(
                        c in "0123456789abcdefABCDEF.:" for c in near):
                    return near
        return peer

    def _send(self, status, payload):
        body = json.dumps(payload, separators=(",", ":")).encode("utf-8")
        try:
            self.send_response(status)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            if self.close_connection:
                self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(body)
        except OSError:
            self.close_connection = True

    def _reject(self, status, code, announced, verb="?", kind="shape"):
        """The ONE way this server refuses anything.

        A REFUSAL NEVER READS THE BODY IT IS REFUSING. The earlier version
        drained the announced body first, to keep the connection alive, and that
        was a hole: an over-budget sender opened one slow-body connection and
        the 429 that was meant to shed it blocked on the very bytes it was
        refusing, so the limiter charged the request and could not deliver its
        verdict. A refusal has to be instant or it is not a defence.

        So a refusal closes the connection instead. The drain trap it used to
        work around is a KEEP-ALIVE problem -- an unread body being misparsed as
        the next request -- and a closed connection has no next request, so the
        undrained bytes die with the socket. `announced` is kept in the
        signature (callers still pass it) but is deliberately unused now.
        """
        global REJ_RATE, REJ_SHAPE, REJ_AUTH
        self.close_connection = True
        host = blunt_host(self.client_host())
        with LOCK:
            if kind == "rate":
                REJ_RATE += 1
            elif kind == "auth":
                REJ_AUTH += 1
            else:
                REJ_SHAPE += 1
            BUCKETS.note_reject(self.client_host(), time.monotonic())
        self._send(status, {"v": 1, "error": code})
        log("reject %s %s %s %d %s"
            % (host, self.command, safe_path(self.path), status, code))

    # -- GET -------------------------------------------------------------

    def do_GET(self):
        if self.path == PREFIX + "admin/worlds":
            # the host's list of live worlds (number, hub code, members). Same
            # door as the event endpoint: off = 404, wrong token = 403.
            if not EVENT_ON:
                self._reject(404, "unknown_verb", 0)
                return
            if not event_token_ok(event_bearer(self.headers)):
                self._reject(403, "forbidden", 0, kind="auth")
                return
            now = time.monotonic()
            with LOCK:
                rows = [{"no": w.no, "code": c, "members": w.count(now)}
                        for c, w in sorted(WORLDS.items(),
                                           key=lambda kv: kv[1].no)]
            self._send(200, {"v": 1, "worlds": rows})
            return
        if self.path != PREFIX + "health":
            self._reject(404, "unknown_verb", 0)
            return
        with LOCK:
            payload = {
                "ok": True,
                "ts": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                "revision": REVISION,
                # What this build speaks and what its dial will accept, so an
                # operator can answer "why did my launcher get refused" from
                # outside the container.
                "contract_min": CONTRACT_MIN,
                "contract_v": CONTRACT_V,
                "dial_max": DIAL_MAX,
                "relay_v": RELAY_V,
                "world_open": world_open(),
                "game_ids": len(GAME_IDS),
                # run mp1 A4: how many builds play arena (-1 = "*", every
                # build), the arena size limit, and a new room's pick.
                "arena_game_ids": -1 if ARENA_ANY else len(ARENA_GAME_IDS),
                # how many of those builds play Luigi Infection on arena
                # (-1 = "*"); 0 = every Luigi match is lockstep.
                "arena_luigi_game_ids": (-1 if ARENA_LUIGI_ANY
                                         else len(ARENA_LUIGI_GAME_IDS)),
                "arena_max": ARENA_MAX_PLAYERS,
                "netmode_default": NETMODE_DEFAULT,
                "rooms": len(ROOMS),
                "members": sum(len(r.members) for r in ROOMS.values()),
                "worlds": len(WORLDS),
                "world_members": sum(w.count(time.monotonic())
                                     for w in WORLDS.values()),
                "world_seats": WORLD_SEATS,
                "world_member_s": WORLD_MEMBER_S,
                "world_addr_tokens": WORLD_ADDR_TOKENS,
                "waiters": WAITERS,
            }
        self._send(200, payload)

    def do_PUT(self):
        self._method_not_allowed()

    def do_DELETE(self):
        self._method_not_allowed()

    def do_HEAD(self):
        # A HEAD response carries headers and no body, so it cannot go through
        # _send.
        self.close_connection = True
        try:
            self.send_response(405)
            self.send_header("Content-Type", "application/json")
            self.send_header("Content-Length", "0")
            self.send_header("Connection", "close")
            self.end_headers()
        except OSError:
            pass

    def _method_not_allowed(self):
        self.close_connection = True
        self._send(405, {"v": 1, "error": "bad_method"})

    # -- POST ------------------------------------------------------------

    def do_POST(self):
        global REQ_COUNT, REJ_RATE, REJ_SHAPE, REJ_AUTH

        # 1. CONTENT-LENGTH FIRST, before any rejection can be written. See the
        #    drain trap in this file's header: every path below has to know how
        #    many bytes are still owed on this socket.
        raw_len = self.headers.get("Content-Length")
        if raw_len is None:
            self.close_connection = True
            self._reject(411, "length_required", 0)
            return
        try:
            announced = int(raw_len)
        except ValueError:
            self.close_connection = True
            self._reject(411, "length_required", 0)
            return
        if announced < 0:
            self.close_connection = True
            self._reject(411, "length_required", 0)
            return
        if announced > BODY_MAX:
            # Answered and closed WITHOUT reading the body, for the same reason
            # _reject no longer drains: a client that announces a giant body is
            # not one to wait on, and refusing must not block on the bytes being
            # refused. Closing discards the unsent body with the socket.
            self.close_connection = True
            self._send(413, {"v": 1, "error": "too_large"})
            with LOCK:
                BUCKETS.note_reject(self.client_host(), time.monotonic())
            log("reject %s POST %s 413 too_large"
                % (blunt_host(self.client_host()), safe_path(self.path)))
            return

        host_key = self.client_host()
        now = time.monotonic()

        # 2. RATE BUDGET, charged before any parsing work.
        with LOCK:
            if BUCKETS.ignored(host_key, now):
                ignored = True
                allowed = False
            else:
                ignored = False
                allowed = BUCKETS.allow_request(host_key, now)
            REQ_COUNT += 1
        if not allowed:
            self._reject(429, "ignored" if ignored else "too_fast",
                         announced, kind="rate")
            return

        # 2b. THE HOST'S ADMIN ENDPOINT (lane EVENTS1). Off, it is an unknown
        #     verb like any other. On, it needs the host's token before it
        #     reads one byte of the body.
        if self.path == PREFIX + "admin/event":
            self._admin_event(announced)
            return

        # 3. THE PATH. Exactly one of the verbs this file serves, and never
        #    with a query string.
        path = self.path
        if "?" in path or "#" in path:
            self._reject(404, "unknown_verb", announced)
            return
        if not path.startswith(PREFIX):
            self._reject(404, "unknown_verb", announced)
            return
        verb = path[len(PREFIX):]
        if verb == "health":
            self._reject(405, "bad_method", announced)
            return
        fn = VERBS.get(verb)
        if fn is None:
            self._reject(404, "unknown_verb", announced)
            return

        # 4. CONTENT TYPE.
        ctype = (self.headers.get("Content-Type") or "").split(";")[0].strip()
        if ctype.lower() != "application/json":
            self._reject(415, "bad_content_type", announced)
            return

        # 5. THE BODY, read in full because we announced we would.
        try:
            raw = self.rfile.read(announced) if announced else b""
        except OSError:
            self.close_connection = True
            return
        if len(raw) != announced:
            self.close_connection = True
            self._send(400, {"v": 1, "error": "bad_shape"})
            return

        try:
            body = json.loads(raw.decode("utf-8"))
        except (ValueError, UnicodeDecodeError, RecursionError):
            # RecursionError, not just ValueError: a body of deeply nested
            # brackets makes json.loads recurse, and on a low recursion limit
            # (python:3.12-alpine sits lower than the CPython the tests run on)
            # it raises RecursionError, which is a RuntimeError and not a
            # ValueError. Without it here the request would drop with no answer
            # and a stray handler-error line. It is a bad body either way.
            self._reject(400, "bad_shape", 0)
            return
        if not isinstance(body, dict):
            self._reject(400, "bad_shape", 0)
            return

        # 6. THE VERSION. A field that appears without a version bump is a bug
        #    or an attack, so the version is checked before the fields are.
        #    A RANGE, not a single number, and the answer echoes what the
        #    request claimed. A hard pin would have made "adding a field is a
        #    `v` bump" mean "every restart refuses every shipped launcher",
        #    which is not what the rule is for. The strictness the rule is
        #    protecting is kept where it belongs: each verb offers a v1 client
        #    exactly the v1 field set, so a v1 request carrying a v2 field is
        #    still `bad_field`.
        ver = body.get("v")
        if (isinstance(ver, bool) or not isinstance(ver, int)
                or ver < CONTRACT_MIN or ver > CONTRACT_V):
            self._reject(400, "bad_version", 0)
            return

        with LOCK:
            status, payload = fn(body, host_key, time.monotonic())
        payload = dict(payload)
        # Echo the version the caller SENT. A v1 launcher must not start
        # seeing v2 in its answers just because the server grew.
        payload["v"] = ver
        if status != 200:
            kind = "auth" if status in (403, 409) else (
                "rate" if status == 429 else "shape")
            with LOCK:
                if kind == "rate":
                    REJ_RATE += 1
                elif kind == "auth":
                    REJ_AUTH += 1
                else:
                    REJ_SHAPE += 1
                BUCKETS.note_reject(host_key, time.monotonic())
            log("reject %s POST %s %d %s"
                % (blunt_host(host_key), safe_path(self.path), status,
                   payload.get("error", "?")))
        self._send(status, payload)


def _admin_event(self, announced):
    if not EVENT_ON:
        self._reject(404, "unknown_verb", announced)
        return
    if not event_token_ok(event_bearer(self.headers)):
        self._reject(403, "forbidden", announced, kind="auth")
        return
    try:
        raw = self.rfile.read(announced) if announced else b""
        body = json.loads(raw.decode("utf-8"))
    except (OSError, ValueError, UnicodeDecodeError, RecursionError):
        self._reject(400, "bad_shape", 0)
        return
    with LOCK:
        worlds = dict((c, w.no) for c, w in WORLDS.items())
    cmd, err = event_build(body, worlds)
    if err:
        self._send(400, {"v": 1, "error": err})
        return
    try:
        event_send(cmd)
    except OSError:
        self._send(502, {"v": 1, "error": "relay_unreachable"})
        return
    log("admin event %s secs %s world %s" % (body.get("event"), body.get("secs", 0),
                                              body.get("world") or "all"))
    self._send(200, {"v": 1, "ok": True})


Handler._admin_event = _admin_event


class Server(ThreadingHTTPServer):
    daemon_threads = True
    # Correct on Linux, where it only lets a restarted process rebind a port
    # still in TIME_WAIT. On WINDOWS the same flag lets a SECOND process bind a
    # port that is already being listened on, and connections are then split
    # between the two at random -- which is a genuinely baffling half hour if
    # you meet it during local testing. server_bind below closes that.
    allow_reuse_address = True

    def server_bind(self):
        if sys.platform == "win32":
            try:
                self.socket.setsockopt(socket.SOL_SOCKET,
                                       socket.SO_EXCLUSIVEADDRUSE, 1)
                self.allow_reuse_address = False
            except OSError:
                pass
        ThreadingHTTPServer.server_bind(self)
    # One thread per open connection, and long polls hold theirs. The cap is
    # MAX_WAITERS on the lobby side and pids_limit on the container side.

    def handle_error(self, request, client_address):
        """A client that goes away is normal, not an incident.

        socketserver's default prints a full traceback for every dropped
        connection, and a launcher being closed mid-poll drops one every time.
        A log full of tracebacks for the ordinary case is a log nobody reads
        when something is actually wrong, so the ordinary case gets one line
        and anything else keeps its traceback.
        """
        exc = sys.exc_info()[1]
        if isinstance(exc, (ConnectionResetError, ConnectionAbortedError,
                            BrokenPipeError, TimeoutError)):
            return
        log("handler error from %s: %r"
            % (blunt_host(client_address[0] if client_address else ""), exc))


def main():
    Handler.timeout = HANDLER_TIMEOUT_S
    log("sm64ds-lobby %s starting on %s:%d" % (REVISION, LISTEN_ADDR, LISTEN_PORT))
    log("contract v%d..v%d, player dial 2..%d (seats=%d, game=%d), relay v%d"
        % (CONTRACT_MIN, CONTRACT_V, DIAL_MAX, MAX_SEATS, GAME_MAX_PLAYERS,
           RELAY_V))
    log("worlds max=%d seats=%d member=%ds empty=%ds per_address=%d; "
        "adventure rooms empty=%ds"
        % (MAX_WORLDS, WORLD_SEATS, WORLD_MEMBER_S, WORLD_EMPTY_S,
           WORLD_ADDR_TOKENS, ADV_EMPTY_S))
    if GAME_IDS:
        log("game builds served: %d (GAME_IDS); world pool %s"
            % (len(GAME_IDS), "open" if world_open() else "off (RELAY_V 1)"))
    else:
        log("GAME_IDS is empty: rooms take any build and the world pool is "
            "OFF")
    for bad in GAME_IDS_DROPPED:
        log("GAME_IDS entry dropped, not a build id (%d chars)" % len(bad))
    if ARENA_ANY:
        log("arena: EVERY build (ARENA_GAME_IDS=*, a dev/test box setting), "
            "max %d players, new rooms pick %s"
            % (ARENA_MAX_PLAYERS, NETMODE_DEFAULT))
    elif ARENA_GAME_IDS:
        log("arena: %d build(s) (ARENA_GAME_IDS), max %d players, new rooms "
            "pick %s%s" % (len(ARENA_GAME_IDS), ARENA_MAX_PLAYERS,
                            NETMODE_DEFAULT,
                            "" if RELAY_V >= 2 else "; OFF while RELAY_V is 1"))
    else:
        log("arena: OFF (ARENA_GAME_IDS is empty): every VS match is lockstep")
    for bad in ARENA_GAME_IDS_DROPPED:
        log("ARENA_GAME_IDS entry dropped, not a build id (%d chars)"
            % len(bad))
    if ARENA_LUIGI_ANY:
        log("arena Luigi Infection: EVERY arena build (ARENA_LUIGI_GAME_IDS=*,"
            " a dev/test box setting)")
    elif ARENA_LUIGI_GAME_IDS:
        log("arena Luigi Infection: %d build(s) (ARENA_LUIGI_GAME_IDS)"
            % len(ARENA_LUIGI_GAME_IDS))
    else:
        log("arena Luigi Infection: OFF (ARENA_LUIGI_GAME_IDS is empty): "
            "every Luigi match is lockstep")
    for bad in ARENA_LUIGI_GAME_IDS_DROPPED:
        log("ARENA_LUIGI_GAME_IDS entry dropped, not a build id (%d chars)"
            % len(bad))
    log("limits rooms=%d seats=%d players=%d waiters=%d body=%d read_timeout=%ds "
        "req=%d/s burst=%d create=%d/h join=%d/min star_max=%d king_max=%d "
        "kick_cool=%ds"
        % (MAX_ROOMS, MAX_SEATS, GAME_MAX_PLAYERS, MAX_WAITERS, BODY_MAX,
           HANDLER_TIMEOUT_S, RATE_REQ_PER_S, RATE_BURST, RATE_CREATE_PER_HOUR,
           RATE_JOIN_PER_MIN, STAR_TARGET_MAX, KING_TARGET_MAX, KICK_COOLDOWN_S))
    threading.Thread(target=reaper, daemon=True).start()
    httpd = Server((LISTEN_ADDR, LISTEN_PORT), Handler)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        log("sm64ds-lobby stopping")
        httpd.server_close()


if __name__ == "__main__":
    main()
