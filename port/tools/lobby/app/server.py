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

import json
import os
import random
import socket
import sys
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

REVISION = "lobby-1"

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
CONTRACT_MIN = 1
# v3, run vs16: the sixteen-player wire. The version itself gains no new FIELD
# -- `match_players` has been a v2 optional since SEAT16 -- but it gains a new
# legal RANGE for that field, and the two are not the same kind of change:
#
#   a new field  a v2 client cannot send it, so it cannot be hurt by it
#   a new range  a v2 client CAN send 8, and would then be handed a
#                sixteen-field SM64DS_VS_NAMES its own validator refuses,
#                spawn nothing, and have no idea why
#
# So the range is gated on the version too (see wide_ok / dial_max_for), and
# that is what makes v3 a real version rather than a number that went up.
CONTRACT_V = 3

# The version each v2 addition arrived in. All three are 2 and all three say so
# by name rather than by the digit, so a later bump cannot accidentally lock v2
# clients out of the fields v2 introduced -- and so the union above is legible
# as three entries rather than as one number repeated.
V_DIAL = 2       # params.match_players
V_COLOR = 2      # create/join color+shoes, and the `color` verb itself

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

# The version a caller must speak before the server will accept a dial above
# NARROW_PLAYERS. Derived, one expression, one reader -- the same discipline
# recompute_dial() has, and for the same reason: a bound restated anywhere else
# is a bound that will drift.
WIDE_MIN_V = 3


def dial_max_for(ver):
    """The largest match_players THIS CALLER may ask for.

    The deployment's own bound (DIAL_MAX) capped by what the caller's contract
    version can survive being told. A v2 launcher validates
    SM64DS_VS_NAMES at exactly four fields and refuses to spawn on anything
    else, so handing it a wide room would be a room it can join and cannot
    play -- worse than a refusal, because the refusal at least says why.
    """
    if ver >= WIDE_MIN_V:
        return DIAL_MAX
    return min(DIAL_MAX, NARROW_PLAYERS)

# The largest "first to N stars" a host may pick. FIVE TODAY BECAUSE AN ARENA
# HOLDS FIVE STARS, not because the number five is special: the owner picks the
# target, and a mode that went past five would need the game to respawn stars.
# It is a knob rather than a literal so that raising it is a compose edit on the
# day the game side lands, and so no reader has to hunt for a scattered 5.
STAR_TARGET_MAX = env_int("STAR_TARGET_MAX", 5, 1, 99)

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

# Stage B knobs. Read here so the compose file and this file cannot drift, and
# so `health` can report what the room semantics would be, but nothing in stage
# A arms a match.
ARM_DEADLINE_S = env_int("ARM_DEADLINE_S", 20, 5, 120)
GO_GRACE_S = env_int("GO_GRACE_S", 5, 1, 60)
MATCH_TIMEOUT_S = env_int("MATCH_TIMEOUT_S", 1200, 60, 86400)
RELAY_ADDR = env_str("RELAY_ADDR", "135.148.26.201:41234")

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
                "member_not_ready")

# What the game reports as the win condition, plus "draw" for an unparseable
# marker (spec 5.3). The server never interprets it; it echoes it in the event.
WIN_VALUES = ("time-up", "star-target", "draw")

STATES = ("lobby", "arming", "go", "in_match", "closed")

_RNG = random.SystemRandom()

# ------------------------------------------------------------------- utils


def log(msg):
    """One line to stdout. Docker picks these up; `docker logs` shows them."""
    ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    sys.stdout.write("%s %s\n" % (ts, msg))
    sys.stdout.flush()


def blunt_host(host):
    """A logging form of a client address with the host part blunted.

    Same intent as relay.py's short_addr: keep enough to tell two clients apart
    in a log and to spot a region, drop the rest. The source PORT is dropped
    entirely rather than kept, because behind Caddy it is the proxy's ephemeral
    port and says nothing about the client.
    """
    if not host:
        return "x"
    if ":" in host:
        groups = host.split(":")
        head = ":".join(g for g in groups[:2] if g)
        return (head + "::x") if head else "::x"
    octets = host.split(".")
    if len(octets) == 4:
        return "%s.%s.x.x" % (octets[0], octets[1])
    return "x"


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


class Member(object):
    __slots__ = ("seat", "token", "nick", "display", "playing", "pre_ok",
                 "armed", "seen", "chat_allowance", "chat_stamp", "last_addr",
                 "color", "shoes", "v")

    def __init__(self, seat, token, nick, playing, pre_ok, now, addr="",
                 color="", shoes="", v=1):
        # The contract version this member joined at. 0.3.2: the dial and the
        # start both refuse to take a room past four while anyone seated is
        # below WIDE_MIN_V, because that member's build would be handed a
        # sixteen-field plan it refuses at spawn and take the match down.
        self.v = v
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
    def __init__(self, code, now, host_v=CONTRACT_MIN):
        self.code = code
        self.cond = threading.Condition(LOCK)
        self.members = {}       # seat -> Member
        self.by_token = {}      # token -> seat
        self.host_seat = 1
        self.state = "lobby"
        self.map = 0
        self.win_mode = "time"
        self.star_target = None
        # THE HOST'S PLAYER-COUNT DIAL. How many of this room's seats play;
        # everybody past it watches. A room parameter like map and win_mode,
        # set through `params`, frozen from Start until the room is back in
        # lobby. It starts at the deployment's own capability so a host who
        # never opens the settings dialog gets the old behaviour exactly.
        # RUN vs16: bounded by the HOST'S contract version, not only by the
        # deployment's. On a sixteen-player deployment a v2 host would
        # otherwise get a room that defaults to sixteen, be handed a
        # sixteen-field SM64DS_VS_NAMES its own build validates at four, and
        # spawn nothing -- without ever touching the dial it is not allowed to
        # move. The room's default capability cannot exceed what its host is
        # able to drive.
        #
        # On every deployment that exists today the two answers are the same
        # number, so this is invisible where nothing can use it.
        self.match_players = dial_max_for(host_v)
        self.match = None
        # STAGE B/C match state, all None/empty in lobby and reset by
        # _reset_match. Frozen at the arming instant so a join, a leave or a
        # params edit that races the freeze cannot make two members disagree.
        self.comms_code = None      # 8-char, minted per match, discarded at end
        self.agreed_players = 0     # playing-seat count frozen at arm
        self.match_map = 0          # params frozen into the plan at arm
        self.match_win_mode = "time"
        self.match_star_target = None
        self.match_dial = 0         # the dial the host had set at arm
        self.names = ""             # SM64DS_VS_NAMES, built once, slot order
        self.colors = ""            # SM64DS_VS_COLORS, likewise
        self.slot_of = {}           # seat -> slot (0..3), packed among playing
        self.arm_deadline = None    # monotonic; arming must complete by here
        self.go_at = None           # monotonic; go->in_match after GO_GRACE
        self.match_start = None     # monotonic; in_match->lobby after timeout
        # The last match id a result was accepted for, so a SECOND result for a
        # match already resolved is idempotent (200) rather than stale (409).
        self.last_result_match = None
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
          2. THE HOST IS ALWAYS SEAT 1. The creator takes seat 1, and a host
             leaving closes the room rather than handing the room to anybody
             else, so no live room ever has a host at another number.
          3. Seat numbers are distinct and always inside 1..MAX_SEATS, so the
             slots derived from them are distinct and always inside 0..3.

        Reusing a freed number rather than counting ever upward is deliberate:
        the numbers have to stay inside the game's four slots, and property 1
        means reuse can never renumber a member who is still in the room.

        WHY ANY OF THIS MATTERS OFF THIS FILE. Over the relay the parent's
        ACCEPT is a broadcast with no recipient field, so two children claiming
        the same slot cannot be told apart by the game. The LOBBY is the only
        place a slot can be handed out uniquely, and stage B's start plan does
        exactly that from these numbers: host = seat 1 = slot 0.

        ONE THING STAGE B STILL HAS TO DECIDE, flagged here so it is not
        discovered late. With GAME_MAX_PLAYERS below MAX_SEATS the playing seats
        need not be contiguous: seats 1, 2 and 3 with 2 leaving promotes 3, and
        the playing seats are then 1 and 3. `slot = seat - 1` gives slots 0 and
        2, with a hole at 1; `slot = rank among the playing seats` gives 0 and
        1, packed. Both are computable from this model and neither needs a
        change here. The packed reading is very probably the one the game wants.
        """
        for n in range(1, MAX_SEATS + 1):
            if n not in self.members:
                return n
        return None

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

        THE HOST IS NEVER DEMOTED. Seat 1 is the host and the host is the
        parent of the game session; a spectating host would hand out a go plan
        with no parent in it. Demoting from the highest seat down cannot reach
        seat 1 while the dial is at least 1, and the loop refuses it anyway
        rather than relying on that.
        """
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
        return {
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
            # The host's dial, and the largest value the server will accept for
            # it. `dial_max` is what lets the launcher grey out the rest of the
            # control instead of discovering the bound by being refused.
            "match_players": self.match_players,
            "dial_max": DIAL_MAX,
            "map": self.map,
            "win_mode": self.win_mode,
            "star_target": self.star_target,
            "members": [self.members[s].view() for s in sorted(self.members)],
            "match": self.match,
        }

    def close(self, why):
        if self.state == "closed":
            return
        self.state = "closed"
        self.closed_at = time.monotonic()
        self.why = why
        self.push("closed", why=why)
        log("room %s close (%s)" % (self.code, why))


# ------------------------------------------------------------- rate limits


class Buckets(object):
    """Per-address limiters, swept so a flood of one-shot addresses cannot
    grow this dictionary without bound."""

    def __init__(self):
        self.tokens = {}        # host -> [allowance, stamp]
        self.creates = {}       # host -> [timestamps]
        self.joins = {}         # host -> [timestamps]
        self.bad = {}           # host -> [count, window_start, ignored_until]

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

    def sweep(self, now):
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


def v_color_pair(body, ver):
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
    # A fixed enum, never free text. member_not_ready is server-minted only,
    # so a client that sends it is refused like any other unknown reason.
    if s not in FAIL_REASONS or s == "member_not_ready":
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
    "create":    (("v", "nick"),
                  # A colour pick travels as a PAIR or not at all; do_create
                  # enforces the pairing. Here they are two keys that may
                  # appear from v2.
                  {"pre_ok": 1, "color": V_COLOR, "shoes": V_COLOR}),
    "join":      (("v", "room", "nick"),
                  {"pre_ok": 1, "color": V_COLOR, "shoes": V_COLOR}),
    "poll":      (("v", "room", "token", "cursor"), {"wait": 1}),
    "chat":      (("v", "room", "token", "text"), {}),
    "params":    (("v", "room", "token", "map", "win_mode"),
                  # star_target is required-iff-stars and forbidden otherwise;
                  # do_params enforces that pairing. Here it is only "a key
                  # this verb may carry, since v1".
                  {"star_target": 1, "match_players": V_DIAL}),
    # THE WHOLE VERB IS v2. Its fields are required, so they carry no version
    # of their own -- the verb's own `since` is what refuses a v1 caller.
    "color":     (("v", "room", "token", "color", "shoes"), {}, V_COLOR),
    "preflight": (("v", "room", "token", "pre_ok"), {}),
    "start":     (("v", "room", "token"), {}),
    "ready":     (("v", "room", "token", "match"), {}),
    "result":    (("v", "room", "token", "match", "win", "scores"), {}),
    "failed":    (("v", "room", "token", "match", "reason"), {}),
    "kick":      (("v", "room", "token", "seat"), {}),
    "leave":     (("v", "room", "token"), {}),
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
    color, shoes, err = v_color_pair(body, body.get("v", 1))
    if err:
        return 400, {"error": err}

    if not BUCKETS.allow_create(who, now):
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
    room = Room(code, now, request_version(body))
    token = new_token()
    # The host plays if the dial has room for anybody at all. The host is also
    # the session's parent, so a host who is not playing is not a thing any
    # later code has to handle: `apply_dial` refuses to demote seat 1.
    m = Member(1, token, nick, room.match_players >= 1, pre_ok, now, who,
               color, shoes, request_version(body))
    room.members[1] = m
    room.by_token[token] = 1
    room.host_seat = 1
    room.refresh_displays()
    ROOMS[code] = room
    log("room %s create by %s (1/%d)" % (code, blunt_host(who), MAX_SEATS))
    return 200, {"room": code, "token": token, "member": 1,
                 "cursor": room.seq, "view": room.view(1)}


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
    color, shoes, err = v_color_pair(body, body.get("v", 1))
    if err:
        return 400, {"error": err}

    if not BUCKETS.allow_join(who, now):
        return 429, {"error": "too_fast"}

    room = ROOMS.get(code)
    if room is None or room.state == "closed":
        return 404, {"error": "no_such_room"}
    room.forget_old_kicks(now)
    if who in room.kicked_addr:
        return 403, {"error": "kicked"}
    if len(room.members) >= MAX_SEATS:
        return 409, {"error": "room_full"}
    if room.state != "lobby":
        return 409, {"error": "in_match"}
    # RUN vs16: A NARROW CLIENT CANNOT JOIN A WIDE ROOM, and it is turned away
    # here rather than seated and then handed a plan it cannot use.
    #
    # Above four players the game speaks wire version 3 and both name strings
    # carry sixteen fields. A v2 build validates them at four and refuses to
    # spawn, so without this it would sit in the room, be counted as playing,
    # arm, and then fail at launch -- taking the whole match with it, because
    # `failed` returns the room to lobby. A refusal at the door is the same
    # answer given early enough to be actionable.
    if (room.match_players > NARROW_PLAYERS
            and request_version(body) < WIDE_MIN_V):
        return 409, {"error": "needs_newer_client"}

    seat = room.free_seat()
    if seat is None:
        return 409, {"error": "room_full"}
    token = new_token()
    # The dial decides, not the deployment knob. Same rule as before -- the
    # first N in the room play -- with N now chosen by the host instead of
    # fixed for the whole server.
    playing = room.playing_count() < room.match_players
    room.members[seat] = Member(seat, token, nick, playing, pre_ok, now, who,
                                color, shoes, request_version(body))
    room.by_token[token] = seat
    room.refresh_displays()
    room.push("joined", seat=seat, display=room.members[seat].display)
    log("room %s join %s seat %d %s (%d/%d)"
        % (code, blunt_host(who), seat, "playing" if playing else "watching",
           len(room.members), MAX_SEATS))
    return 200, {"room": code, "token": token, "member": seat,
                 "cursor": room.seq, "view": room.view(seat)}


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
    if win_mode not in ("time", "stars"):
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

    # THE DIAL. Refused, not clamped, and refused with its own code so the
    # launcher can say a true sentence about it. Clamping was the other option
    # and it is the wrong one: a host who asks for eight and silently gets four
    # has been lied to, and would only find out by counting heads in the room.
    # The bound the refusal enforces is advertised as `dial_max` in every view,
    # so a launcher that reads the view never has to be refused at all.
    #
    # AND THE UPPER BOUND IS THE CALLER'S, NOT THE DEPLOYMENT'S ALONE. Run
    # vs16: above four players the game speaks a different wire and the two
    # name/colour strings carry sixteen fields, so a v2 caller asking for eight
    # is refused with the same code and for a better reason than the
    # deployment's capability -- its own build could not play the room it was
    # asking for. `dial_max` in the room view is still the DEPLOYMENT's number,
    # unchanged in meaning for launcher 0.3.0, and a v3 launcher greys against
    # it correctly because for a v3 caller the two bounds are the same.
    match_players = None
    if "match_players" in body:
        match_players, err = v_int(body, "match_players", 2,
                                   dial_max_for(request_version(body)),
                                   "bad_match_players")
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
        return 409, {"error": "not_in_lobby"}

    room.map = mp
    room.win_mode = win_mode
    room.star_target = star_target
    # A v1 client sends no dial, and a room whose host uses one keeps whatever
    # it had rather than being reset by an old launcher's ordinary settings
    # edit. That is the whole of the "0.3.0 is unaffected" story on this verb.
    if match_players is not None:
        # 0.3.2: the door check in do_join is join-time only. A v2 client
        # seated in a four-wide room, followed by a v3 host dialing it to
        # eight, would be the same failure one verb later -- so the dial is
        # refused while anyone seated cannot play the width asked for.
        if match_players > NARROW_PLAYERS and any(
                m.v < WIDE_MIN_V for m in room.members.values()):
            return 409, {"error": "seated_client_too_old"}
        room.match_players = match_players
        room.apply_dial()
    # `match_players` rides the event as well as the view. Older launchers
    # ignore an extra field on an event they already know (section 3.5 is about
    # unknown KINDS; the field tolerance is what JSON readers do by default and
    # is pinned by a test), and the roster change is visible in the view every
    # poll carries regardless.
    room.push("params", map=mp, win_mode=win_mode, star_target=star_target,
              match_players=room.match_players)
    log("room %s params map=%d win=%s target=%s players=%d/%d"
        % (code, mp, win_mode, star_target if star_target else "-",
           room.playing_count(), room.match_players))
    return 200, {"cursor": room.seq}


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


def drop_member(room, seat, why):
    """Free a seat. The host leaving closes the room; anyone else leaving
    promotes the lowest-numbered spectator if a playing seat opened up."""
    m = room.members.get(seat)
    if m is None:
        return
    if seat == room.host_seat:
        room.close("host_left")
        return
    was_playing = m.playing
    del room.members[seat]
    room.by_token.pop(m.token, None)
    room.refresh_displays()
    room.push("left", seat=seat, display=m.display, why=why)
    log("room %s leave seat %d (%s) (%d/%d)"
        % (room.code, seat, why, len(room.members), MAX_SEATS))
    if was_playing:
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
    """
    return {seat: slot for slot, seat in enumerate(playing_seats(room))}


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
    role = "parent" if seat == room.host_seat else "child"
    plan = {
        "role": role,
        "code": room.comms_code,
        "relay": RELAY_ADDR,
        "map": room.match_map,
        "players": room.agreed_players,
        "slot": room.slot_of.get(seat, 0),
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
    }
    if room.match_win_mode == "stars":
        plan["star_target"] = room.match_star_target
    return plan


def _reset_match(room):
    """Back to a clean lobby: clear the frozen match state and every armed
    flag. Does NOT push an event; the caller pushes result/failed."""
    room.state = "lobby"
    room.match = None
    room.comms_code = None
    room.agreed_players = 0
    room.match_star_target = None
    room.match_dial = 0
    room.names = ""
    room.colors = ""
    room.slot_of = {}
    room.arm_deadline = None
    room.go_at = None
    room.match_start = None
    for m in room.members.values():
        m.armed = False


def match_to_lobby(room, reason):
    """A match that could not run returns to lobby with a `failed` event.
    Shared by an explicit `failed`, the arming deadline and the match timeout.
    """
    mid = room.match
    _reset_match(room)
    if mid:
        room.push("failed", match=mid, reason=reason)
    log("room %s failed match=%s reason=%s" % (room.code, mid, reason))


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
    if room.state != "lobby":
        return 409, {"error": "not_in_lobby"}
    ps = playing_seats(room)
    if len(ps) < 2:
        return 409, {"error": "not_enough_players"}
    # 0.3.2: belt and braces over do_params -- no wide start with an old
    # client seated, whatever path got the room here.
    if room.match_players > NARROW_PLAYERS and any(
            m.v < WIDE_MIN_V for m in room.members.values()):
        return 409, {"error": "seated_client_too_old"}
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
    room.match_dial = room.match_players
    room.slot_of = assign_slots(room)
    room.names = build_names(room)
    room.colors = build_colors(room)
    for m in room.members.values():
        m.armed = False
    room.state = "arming"
    room.arm_deadline = now + ARM_DEADLINE_S
    room.push("arming", match=room.match, deadline_ms=ARM_DEADLINE_S * 1000)
    log("room %s arm match=%s players=%d deadline=%ds"
        % (code, room.match, room.agreed_players, ARM_DEADLINE_S))
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
    if not m.playing:
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
    if not m.playing:
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
        if seat == room.host_seat:
            match_to_lobby(room, reason)
        else:
            m.playing = False
            m.armed = False
            room.push("dropped", seat=seat, reason=reason)
            log("room %s dropped seat %d from match=%s reason=%s (%d still playing)"
                % (room.code, seat, room.match, reason, room.playing_count()))
            if room.playing_count() < 2:
                match_to_lobby(room, reason)
    return 200, {"cursor": room.seq}


def do_result(body, who, now):
    """A playing member reporting a finished match. The FIRST valid result for
    the current match returns the room to lobby; later ones are idempotent
    (spec 3.9, 5.4)."""
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
        # The first result for the current match. Append the event, discard the
        # comms code, clear the match, drop back to lobby. Params, seats, nicks,
        # tokens and chat all survive (spec 5.6): "same again" is one button.
        mid = room.match
        room.last_result_match = mid
        _reset_match(room)
        room.push("result", match=mid, win=win, scores=scores)
        log("room %s result match=%s win=%s" % (code, mid, win))
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
    if room.state == "go":
        out["plan"] = member_plan(room, seat)
    return 200, out


VERBS = {
    "create": do_create,
    "join": do_join,
    "poll": do_poll,
    "chat": do_chat,
    "params": do_params,
    "preflight": do_preflight,
    "color": do_color,
    "start": do_start,
    "ready": do_ready,
    "result": do_result,
    "failed": do_failed,
    "kick": do_kick,
    "leave": do_leave,
}

# EVERY VERB HAS A ROW, AND EVERY ROW HAS A VERB. Checked at import rather than
# on the first request that needs it: a verb added to VERBS with no row in
# VERB_FIELDS would otherwise be a KeyError inside a handler -- a 500 on a live
# box instead of a service that refuses to start.
assert set(VERBS) == set(VERB_FIELDS), (
    "VERBS and VERB_FIELDS disagree: %s"
    % sorted(set(VERBS) ^ set(VERB_FIELDS)))

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

        # HEARTBEAT AND IDLE TIMEOUTS APPLY ONLY IN LOBBY. During a match a
        # member disconnecting is nothing (spec 6): the ROM copes, and the room
        # holds until a result or the 20-minute match timeout above. Dropping a
        # member -- worse, the host, which closes the room -- because a busy game
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
            if room.state != "closed" and now - room.touched > ROOM_IDLE_S:
                room.close("idle")
            if room.state != "closed" and not room.members:
                room.close("idle")
        room.forget_old_kicks(now)

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
                log("stats rooms=%d members=%d waiters=%d req=%d rej_rate=%d "
                    "rej_shape=%d rej_auth=%d"
                    % (len(ROOMS), sum(len(r.members) for r in ROOMS.values()),
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
                "rooms": len(ROOMS),
                "members": sum(len(r.members) for r in ROOMS.values()),
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
    log("contract v%d..v%d, player dial 2..%d (seats=%d, game=%d)"
        % (CONTRACT_MIN, CONTRACT_V, DIAL_MAX, MAX_SEATS, GAME_MAX_PLAYERS))
    log("limits rooms=%d seats=%d players=%d waiters=%d body=%d read_timeout=%ds "
        "req=%d/s burst=%d create=%d/h join=%d/min star_max=%d kick_cool=%ds"
        % (MAX_ROOMS, MAX_SEATS, GAME_MAX_PLAYERS, MAX_WAITERS, BODY_MAX,
           HANDLER_TIMEOUT_S, RATE_REQ_PER_S, RATE_BURST, RATE_CREATE_PER_HOUR,
           RATE_JOIN_PER_MIN, STAR_TARGET_MAX, KICK_COOLDOWN_S))
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
