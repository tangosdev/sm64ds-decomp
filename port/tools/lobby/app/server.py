#!/usr/bin/env python3
"""sm64ds-lobby - room state and chat for the PC port's Multiplayer button.

STAGE A: create, join, poll, chat, params, kick, leave, health. No match
arming, no game spawn; `start`, `ready`, `result` and `failed` are not served
yet and answer 404 unknown_verb like any other unknown verb.

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
MAX_SEATS = env_int("MAX_SEATS", 4, 2, 4)
GAME_MAX_PLAYERS = env_int("GAME_MAX_PLAYERS", 2, 1, 4)
MAX_WAITERS = env_int("MAX_WAITERS", 96, 1, 4096)

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

# Bodies larger than this are refused unread. DRAIN_CAP is how much of an
# over-long body we are still willing to read off the socket to keep the
# connection usable; past that the connection is closed instead.
BODY_MAX = 4096
DRAIN_CAP = 65536

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
                 "armed", "seen", "chat_allowance", "chat_stamp", "last_addr")

    def __init__(self, seat, token, nick, playing, pre_ok, now, addr=""):
        self.seat = seat
        self.token = token
        self.nick = nick            # exactly as sent, never rewritten
        self.display = nick         # recomputed whenever the roster changes
        self.playing = playing
        self.pre_ok = pre_ok
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

    def view(self):
        return {"seat": self.seat, "display": self.display,
                "playing": self.playing, "pre_ok": self.pre_ok,
                "armed": self.armed}


class Room(object):
    def __init__(self, code, now):
        self.code = code
        self.cond = threading.Condition(LOCK)
        self.members = {}       # seat -> Member
        self.by_token = {}      # token -> seat
        self.host_seat = 1
        self.state = "lobby"
        self.map = 0
        self.win_mode = "time"
        self.star_target = None
        self.match = None
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
        for n in range(1, MAX_SEATS + 1):
            if n not in self.members:
                return n
        return None

    def playing_count(self):
        return sum(1 for m in self.members.values() if m.playing)

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
            "max_players": GAME_MAX_PLAYERS,
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
    err = shape(body, ("v", "nick"), ("pre_ok",))
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

    room = Room(code, now)
    token = new_token()
    m = Member(1, token, nick, GAME_MAX_PLAYERS >= 1, pre_ok, now, who)
    room.members[1] = m
    room.by_token[token] = 1
    room.host_seat = 1
    room.refresh_displays()
    ROOMS[code] = room
    log("room %s create by %s (1/%d)" % (code, blunt_host(who), MAX_SEATS))
    return 200, {"room": code, "token": token, "member": 1,
                 "cursor": room.seq, "view": room.view(1)}


def do_join(body, who, now):
    err = shape(body, ("v", "room", "nick"), ("pre_ok",))
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

    seat = room.free_seat()
    if seat is None:
        return 409, {"error": "room_full"}
    token = new_token()
    playing = room.playing_count() < GAME_MAX_PLAYERS
    room.members[seat] = Member(seat, token, nick, playing, pre_ok, now, who)
    room.by_token[token] = seat
    room.refresh_displays()
    room.push("joined", seat=seat, display=room.members[seat].display)
    log("room %s join %s seat %d %s (%d/%d)"
        % (code, blunt_host(who), seat, "playing" if playing else "watching",
           len(room.members), MAX_SEATS))
    return 200, {"room": code, "token": token, "member": seat,
                 "cursor": room.seq, "view": room.view(seat)}


def do_chat(body, who, now):
    err = shape(body, ("v", "room", "token", "text"))
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
    """Host only. In stage A this exists for one reason beyond completeness:
    it is the host-only verb the stage's own negative proof exercises with
    another member's token."""
    err = shape(body, ("v", "room", "token", "map", "win_mode"),
                ("star_target",))
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
    room.push("params", map=mp, win_mode=win_mode, star_target=star_target)
    log("room %s params map=%d win=%s target=%s"
        % (code, mp, win_mode, star_target if star_target else "-"))
    return 200, {"cursor": room.seq}


def do_leave(body, who, now):
    err = shape(body, ("v", "room", "token"))
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
        for s in sorted(room.members):
            if not room.members[s].playing:
                room.members[s].playing = True
                # No new event kind: section 3.5's `kind` list is closed, and
                # the `left` event above already moved the cursor. The promotion
                # reaches both windows in the `view` every poll answer carries.
                log("room %s promote seat %d" % (room.code, s))
                break


def do_kick(body, who, now):
    """Host only. The owner's answer to the spec's open question 4.

    Ten lines of state and one honest limitation, both in KICK_COOLDOWN_S
    above. A kick frees the seat exactly the way a leave does -- same `left`
    event, same promotion -- and differs only in the `why` and in the two
    records that keep the kicked client out for a while.
    """
    err = shape(body, ("v", "room", "token", "seat"))
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


def do_poll(body, who, now):
    """The only push channel, and the entire reliability story.

    `cursor` is a per-room monotonic sequence, so nothing can be missed: a
    client that misses ten polls gets all ten events on the eleventh. The
    request is held until there is something newer than the cursor or `wait`
    seconds pass, and it stamps the member's heartbeat on the way in.
    """
    err = shape(body, ("v", "room", "token", "cursor"), ("wait",))
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
        out["events"] = list(room.chat)
    else:
        out["events"] = room.since(cursor)
    return 200, out


VERBS = {
    "create": do_create,
    "join": do_join,
    "poll": do_poll,
    "chat": do_chat,
    "params": do_params,
    "kick": do_kick,
    "leave": do_leave,
}

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

    # The default handler logs every request to stderr with the full client
    # address in it. We log what we choose to log and nothing else.
    def log_message(self, fmt, *args):
        pass

    # -- plumbing --------------------------------------------------------

    def client_host(self):
        """The client's address, blunted for logging and keyed for limits.

        Behind Caddy the socket peer is always Caddy, so per-IP limits would
        be per-SERVER limits without this. X-Forwarded-For's FIRST entry is
        the original client; the header is capped and sanity-checked because
        it is client-supplied on any direct deployment.
        """
        peer = self.client_address[0] if self.client_address else ""
        if TRUST_XFF:
            xff = self.headers.get("X-Forwarded-For", "")
            if xff and len(xff) <= 256:
                first = xff.split(",")[0].strip()
                if first and len(first) <= 45 and all(
                        c in "0123456789abcdefABCDEF.:" for c in first):
                    return first
        return peer

    def _drain(self, announced):
        """Read the body the client announced so the next request on this
        keep-alive connection parses a request line and not the middle of this
        one. Past DRAIN_CAP we close instead of reading."""
        if announced <= 0:
            return
        if announced > DRAIN_CAP:
            self.close_connection = True
            return
        left = announced
        try:
            while left > 0:
                chunk = self.rfile.read(min(left, 8192))
                if not chunk:
                    break
                left -= len(chunk)
        except OSError:
            self.close_connection = True

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
        """The ONE way this server refuses anything. Drains first, always."""
        global REJ_RATE, REJ_SHAPE, REJ_AUTH
        self._drain(announced)
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
            % (host, self.command, self.path, status, code))

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
            # Drained if we can afford to, then closed either way: a client
            # that sends a body this big is not one we want to keep a
            # connection open for.
            self._drain(announced)
            self.close_connection = True
            self._send(413, {"v": 1, "error": "too_large"})
            with LOCK:
                BUCKETS.note_reject(self.client_host(), time.monotonic())
            log("reject %s POST %s 413 too_large"
                % (blunt_host(self.client_host()), self.path))
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
        except (ValueError, UnicodeDecodeError):
            self._reject(400, "bad_shape", 0)
            return
        if not isinstance(body, dict):
            self._reject(400, "bad_shape", 0)
            return

        # 6. THE VERSION. A field that appears without a version bump is a bug
        #    or an attack, so the version is checked before the fields are.
        ver = body.get("v")
        if isinstance(ver, bool) or not isinstance(ver, int) or ver != 1:
            self._reject(400, "bad_version", 0)
            return

        with LOCK:
            status, payload = fn(body, host_key, time.monotonic())
        payload = dict(payload)
        payload["v"] = 1
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
                % (blunt_host(host_key), self.path, status,
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
    log("sm64ds-lobby %s starting on %s:%d" % (REVISION, LISTEN_ADDR, LISTEN_PORT))
    log("limits rooms=%d seats=%d players=%d waiters=%d body=%d "
        "req=%d/s burst=%d create=%d/h join=%d/min star_max=%d kick_cool=%ds"
        % (MAX_ROOMS, MAX_SEATS, GAME_MAX_PLAYERS, MAX_WAITERS, BODY_MAX,
           RATE_REQ_PER_S, RATE_BURST, RATE_CREATE_PER_HOUR, RATE_JOIN_PER_MIN,
           STAR_TARGET_MAX, KICK_COOLDOWN_S))
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
