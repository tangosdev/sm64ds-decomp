#!/usr/bin/env python3
"""Arena referee acceptance suite (run mp1, lane REFEREE1).

  python test_referee.py              unit + contention + fuzz (40k)
  python test_referee.py unit         the in-process checks
  python test_referee.py fuzz [N]     N mutated datagrams (default 150000)
  python test_referee.py vectors      rewrite arena_vectors.json and check it

relay.py (with referee.py beside it) is imported and driven through
RelayProtocol.on_datagram / tick with a recording transport and an explicit
clock, so every verdict, route and refusal is checked exactly, with no timing
luck. Consoles are simulated by the Console class below, which speaks the wire
of arena_contract.py.

Python 3 standard library only.
"""

import importlib.util
import json
import math
import os
import random
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RELAY_PY = os.path.join(HERE, "relay.py")
VECTORS = os.path.join(HERE, "arena_vectors.json")
FIX = 4096

S_SYNS = struct.Struct("<4sBBBBI3i3hhHHBBBBBBH2BH")
S_SYNO = struct.Struct("<4sBBBBI3i3hBB")
S_SYNE = struct.Struct("<4sBBBBHHIII")
S_PING = struct.Struct("<4sBBBBIIHH")
S_HDR = struct.Struct("<4sBBBBIHHII16bBBBBBBBB")
S_EVT = struct.Struct("<HBBBBHHHIiii")

EV = dict(START=1, STAR_WAKE=2, MARKER_BROKEN=3, STAR_TAKEN=4, DENY=5, HIT=6,
          CLASH=7, SPILL=8, MATCH_OVER=9, LEAVE=10, JOIN=11, STAR_STATE=12,
          ACCEPTED=13, EATEN=14, RELEASED=15, GRABBED=16, THROWN=17, STOMPED=18,
          CAP_TAKEN=19, CAP_DROPPED=20, CAP_STATE=21, INFECTED=22,
          TEAM_STATE=23)
EVN = dict((v, k) for k, v in EV.items())
CL = dict(READY=1, MARKER_BREAK=2, GRAB=3, HIT=4, HURT_SELF=5, PIT=6, QUIT=7,
          EAT=8, SPIT=9, SWALLOW=10, ESCAPE=11, CATCH=12, THROW=13, SWING=14,
          BREAK=15, STOMP=16, CAP_TAKE=17, CAP_DROP=18, TAG=19)
DENY = dict(STALE=1, GONE=2, LOST=3, RANGE=4, STATE=5, INVULN=6, MISMATCH=7,
            UNSUPPORTED=8, BAD=9, TOO_OLD=10, SELF=11, TEAM=12)

# state indices (g_player_states order)
ST_WALK, ST_WAIT, ST_PUNCH, ST_HURT, ST_DIVE = 14, 15, 59, 7, 62
ST_JUMP, ST_SLIDE = 18, 35
ST_GRABBED, ST_THROWN, ST_HOLDLIGHT, ST_SWING = 16, 20, 37, 65
ST_NOCONTROL = 24


def load_relay(env=None):
    keys = ("SM64DS_RELAY_MAX_CHILDREN", "SM64DS_REFEREE_SEED",
            "SM64DS_REFEREE_KICK")
    saved = dict((k, os.environ.get(k)) for k in keys)
    try:
        for k in keys:
            os.environ.pop(k, None)
        os.environ["SM64DS_RELAY_MAX_CHILDREN"] = "15"
        os.environ["SM64DS_REFEREE_SEED"] = "12345"
        for k, v in (env or {}).items():
            os.environ[k] = v
        spec = importlib.util.spec_from_file_location(
            "relay_ut_%d" % random.randrange(1 << 30), RELAY_PY)
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
    finally:
        for k, v in saved.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
    mod.LOGGED = []
    mod.log = mod.LOGGED.append
    mod.referee.os = os      # (env read at Referee() time stays live)
    return mod


class Result(object):
    def __init__(self, name):
        self.name, self.ok, self.lines, self.n = name, True, [], 0

    def check(self, cond, msg):
        self.n += 1
        if not cond:
            self.ok = False
            self.lines.append("    [FAIL] %s" % msg)
        return bool(cond)

    def note(self, msg):
        self.lines.append("    ---- %s" % msg)

    def report(self):
        print("  %s: %s (%d checks)" % (self.name, "PASS" if self.ok
                                        else "FAIL", self.n))
        for line in self.lines:
            print(line)
        sys.stdout.flush()


class Wire(object):
    def __init__(self):
        self.sent = []

    def sendto(self, data, addr):
        self.sent.append((bytes(data), addr))


def parse_synr(data):
    if len(data) != 272 or data[:4] != b"SYNR":
        return None
    h = S_HDR.unpack_from(data, 0)
    evs = []
    for i in range(h[3]):
        e = S_EVT.unpack_from(data, 48 + 28 * i)
        evs.append(dict(seq=e[0], type=e[1], name=EVN.get(e[1], "?"),
                        seat=e[2], a=e[3], b=e[4], c=e[5], d=e[6], e=e[7],
                        t=e[8], x=e[9], y=e[10], z=e[11]))
    return dict(seat=h[2], n=h[3], state=h[4], ref_now=h[5], newest=h[6],
                roster=h[7], t0=h[8], match_ms=h[9], scores=list(h[10:26]),
                progress=h[26], first_to_n=h[27], flags=h[28], part=h[29],
                row=h[30], map=h[31], live=h[32], events=evs)


class Rig(object):
    def __init__(self, mod=None, env=None):
        self.mod = mod or load_relay(env)
        self.relay = self.mod.RelayProtocol(90.0, 10.0)
        self.wire = Wire()
        self.relay.connection_made(self.wire)
        self.now = 1000.0
        self._port = 40000
        self.consoles = []

    def addr(self):
        self._port += 1
        return ("203.0.113.9", self._port)

    def send(self, addr, data):
        self.relay.on_datagram(data, addr, self.now)
        self.deliver()

    def deliver(self):
        out, self.wire.sent = self.wire.sent, []
        for data, addr in out:
            for c in self.consoles:
                if c.addr == addr:
                    c.receive(data)

    def advance(self, secs, step=0.01, each=None):
        end = self.now + secs
        while self.now < end - 1e-9:
            self.now = min(end, self.now + step)
            if each is not None:
                each()
            self.relay.tick(self.now)
            self.deliver()

    def session(self, code):
        return self.relay.sessions.get(code)

    def ref(self, code):
        return self.relay.sessions[code].referee


def hello(code, role, want=0xFF, flags=0):
    return (b"SMRC" + bytes((2, role, want, flags))
            + code.encode("ascii").ljust(8, b"\x00")[:8])


class Console(object):
    """A simulated arena console: joins, pings the referee, streams SYNS,
    sends claims, applies SYNR events in seq order and acks them."""

    def __init__(self, rig, code, want=0xFF, ping=True):
        self.rig = rig
        self.code = code
        self.addr = rig.addr()
        self.seat = None
        self.got = []           # every datagram received
        self.events = []        # applied stream events, in order
        self.snap = []          # snapshot records
        self.direct = []        # directed records (DENY / ACCEPTED)
        self.applied = 0
        self.last_pong_ref = 0
        self.last_pong_at = None
        self.frame = 0
        self.pos = [0, 0, 0]
        self.yaw = 0
        self.state = ST_WAIT
        self.flags = 0x01 | 0x02
        self.invuln = 0
        self.power = 0
        self.eaten_by = 0
        self.look = 3
        self.vy = 0
        self.cid = 0
        self.headers = []
        self.offset = None      # referee clock estimate: ref = now_ms + offset
        rig.consoles.append(self)
        rig.send(self.addr, hello(code, 3, want))
        if ping:
            self.ping()
            self.ping()

    def now_ms(self):
        return int(self.rig.now * 1000) & 0xFFFFFFFF

    def receive(self, data):
        self.got.append(data)
        if len(data) == 16 and data[:4] == b"SMRA" and data[5] == 0:
            self.seat = data[6]
        elif len(data) == 20 and data[:4] == b"SYNQ":
            f = S_PING.unpack(data)
            self.last_pong_ref = f[6]
            self.last_pong_at = self.now_ms()
            rtt = self.now_ms() - f[5]
            self.offset = f[6] + rtt // 2 - self.now_ms()
        elif len(data) == 272:
            h = parse_synr(data)
            self.headers.append(h)
            if h["flags"] & 0x01:
                self.snap.extend(h["events"])
                idx, cnt = h["part"] >> 4, h["part"] & 15
                if idx == cnt - 1:
                    self.applied = h["newest"]
                return
            for e in h["events"]:
                if e["seq"] == 0 and h["flags"] & 0x02:
                    self.direct.append(e)
                    if e not in self.events:
                        self.events.append(e)
            for e in sorted(h["events"], key=lambda e: e["seq"]):
                if e["seq"] == ((self.applied % 0xFFFF) + 1):
                    self.events.append(e)
                    self.applied = e["seq"]

    def ref_now(self):
        return (self.now_ms() + (self.offset or 0)) & 0xFFFFFFFF

    def ping(self):
        held = 0xFFFF
        if self.last_pong_at is not None:
            held = (self.now_ms() - self.last_pong_at) & 0xFFFF
        self.rig.send(self.addr, S_PING.pack(b"SYNP", 2, 0xFF, 0xFE, 0,
                                             self.now_ms(), self.last_pong_ref,
                                             held, 0))

    def syns(self, advance_frame=True):
        if advance_frame:
            self.frame += 1
        d = S_SYNS.pack(b"SYNS", 1, 0xFF, self.flags, self.look, self.frame,
                        int(self.pos[0] * FIX), int(self.pos[1] * FIX),
                        int(self.pos[2] * FIX), 0, self.vy, 0, self.yaw, 1, 0,
                        self.state, 0, self.invuln, self.power, 0,
                        self.eaten_by, self.applied, 0, 0, 0)
        self.rig.send(self.addr, d)

    def claim(self, kind, target, aux=0, stamp=None, frame=None, ref_frame=0,
              cid=None):
        if cid is None:
            self.cid += 1
            cid = self.cid
        if stamp is None:
            stamp = self.ref_now()
        if frame is None:
            frame = self.frame
        self.rig.send(self.addr, S_SYNE.pack(b"SYNE", 1, 0xFF, kind, target,
                                             cid, aux, stamp & 0xFFFFFFFF,
                                             frame, ref_frame))
        return cid

    def ready(self, cap=2, time_s=120, first_to_n=0, markers=5, mode=0,
              digest=0xC0FFEE, map_=0):
        rules = first_to_n | (mode << 8) | (cap << 16) | (markers << 24)
        return self.claim(CL["READY"], map_, time_s, stamp=rules,
                          frame=digest)

    def find(self, name, **kw):
        out = []
        for e in self.events:
            if e["name"] != name:
                continue
            if all(e.get(k) == v for k, v in kw.items()):
                out.append(e)
        return out

    def verdict(self, cid):
        """The terminal event naming my claim `cid`: the claimant is the
        event's seat for DENY / STAR_TAKEN / MARKER_BROKEN / ACCEPTED, its
        field a for HIT / CLASH / EATEN (seat = the victim), either for
        RELEASED (spit: a = eater; escape: seat = victim)."""
        for e in self.events:
            if e["c"] != cid:
                continue
            n = e["name"]
            if n in ("DENY", "STAR_TAKEN", "MARKER_BROKEN", "ACCEPTED",
                     "CAP_TAKEN", "CAP_DROPPED"):
                who = (e["seat"],)
            elif n in ("HIT", "CLASH", "EATEN", "GRABBED", "STOMPED",
                       "INFECTED"):
                who = (e["a"],)
            elif n in ("RELEASED", "THROWN"):
                who = (e["seat"], e["a"])
            else:
                continue
            if self.seat in who:
                return e
        return None


def streaming(rig, consoles, secs, ping_every=0.5):
    """Advance time while every console streams SYNS at 30 Hz and pings at
    2 Hz (staggered)."""
    state = {"t": 0.0, "next_frame": rig.now, "next_ping": rig.now}
    if getattr(rig, "no_pings", False):
        state["next_ping"] = float("inf")

    def each():
        if rig.now >= state["next_frame"]:
            state["next_frame"] += 1.0 / 30.0
            for c in consoles:
                if c.seat is not None and c in rig.consoles:
                    c.syns()
        if rig.now >= state["next_ping"]:
            state["next_ping"] += ping_every
            for c in consoles:
                if c in rig.consoles:
                    c.ping()
    rig.advance(secs, each=each)


def match(n=2, first_to_n=0, time_s=120, markers=5, env=None, code="ARENA01",
          cap=None, mod=None):
    """A rig with n consoles in a RUNNING match (countdown done)."""
    rig = Rig(mod, env=env)
    cs = [Console(rig, code) for _ in range(n)]
    streaming(rig, cs, 0.6)
    for c in cs:
        c.ready(cap=cap or n, time_s=time_s, first_to_n=first_to_n,
                markers=markers)
    streaming(rig, cs, 3.2)
    return rig, cs


# ================================================================ UNIT


def u_contract(mod):
    res = Result("contract: referee + relay copies match arena_contract.py")
    spec = importlib.util.spec_from_file_location(
        "arena_contract", os.path.join(HERE, "arena_contract.py"))
    ac = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(ac)
    ref = mod.referee
    for k, v in ac.NUMBERS.items():
        res.check(getattr(ref, k, None) == v, "number %s %r vs %r"
                  % (k, getattr(ref, k, None), v))
    for name in ("STATE_NAMES", "ATTACK_STATES", "DISABLED_STATES",
                 "CLASH_STATES", "STAR_ORDERS"):
        res.check(getattr(ref, name) == getattr(ac, name), name)
    res.check(ref.SPEED_WALK == ac.SPEED_CLASSES["walk"]["states"], "walk")
    res.check(ref.SPEED_SLIDE == ac.SPEED_CLASSES["slide"]["states"], "slide")
    res.check(ref.SPEED_EXEMPT == ac.SPEED_CLASSES["exempt"]["states"],
              "exempt")
    for k in ("walk", "slide", "air"):
        res.check(ref.SPEED_LIMITS[k] == ac.SPEED_CLASSES[k]["limit"], k)
    res.check(ref.SPEED_AVG_LIMIT == ac.SPEED_AVG_LIMIT, "avg")
    res.check(ref.SYNR_LEN == ac.SYNR_LEN == 272, "SYNR length")
    route_map = {"all": mod.A_ALL, "all+ref": mod.A_ALLREF, "ref": mod.A_REF,
                 "none": mod.A_NONE, "all|ref": mod.A_PING,
                 "seat6|ref": mod.A_PONG}
    want = dict((t.encode(), (frozenset(l), route_map[r], c))
                for t, l, r, c, n in ac.ARENA_KINDS)
    have = dict((t, (k.exact, route, k.cap))
                for t, (route, k) in mod.ARENA_KINDS.items())
    res.check(want == have, "relay ARENA_KINDS == contract")
    for t in ac.NOT_IN_ARENA:
        res.check(t.encode() not in mod.ARENA_KINDS, "%s not in arena" % t)
    for i, n, t, f in ac.CLAIMS:
        res.check(ref.CLAIM_NAMES.get(i) == n, "claim %d %s" % (i, n))
    for i, n, f in ac.EVENTS:
        res.check(getattr(ref, "EV_" + n) == i, "event %s" % n)
    for i, n, d in ac.DENY_REASONS:
        res.check(getattr(ref, "D_" + n) == i, "deny %s" % n)
    for i, n in ac.OVER_REASONS:
        res.check(getattr(ref, "O_" + n) == i, "over %s" % n)
    for i, n in ac.MODES:
        res.check(getattr(ref, "MODE_" + n) == i, "mode %s" % n)
    res.check(tuple(ac.MODES_ACCEPTED) == tuple(ref.MODES_ACCEPTED),
              "accepted modes")
    for i, n, d in ac.INFECT_CAUSES:
        res.check(getattr(ref, "INF_" + n) == i, "infect cause %s" % n)
    for name, fields, size in (("SYNS", ac.SYNS_FIELDS, ref.S_SYNS.size),
                               ("SYNO", ac.SYNO_FIELDS, ref.S_SYNO.size),
                               ("SYNE", ac.SYNE_FIELDS, ref.S_SYNE.size),
                               ("PING", ac.PING_FIELDS, ref.S_PING.size),
                               ("EVENT", ac.EVENT_FIELDS, ref.S_EVT.size)):
        last = fields[-1]
        res.check(last[1] + last[2] == size, "%s size %d" % (name, size))
        off = 0
        for f in fields:
            res.check(f[1] == off, "%s.%s offset" % (name, f[0]))
            off += f[2]
    res.check(ac.main(["--check"]) == 0, "json + header up to date")
    return res


def u_handshake(mod):
    res = Result("arena handshake, kinds, no amplifier before the echo")
    rig = Rig(mod)
    a = Console(rig, "ARENA02", ping=False)
    res.check(a.seat == 0, "role 3 -> seat 0")
    s = rig.session("ARENA02")
    res.check(s.kind == "arena" and s.referee is not None, "arena session")
    # before the echo: no referee datagram, only same-size pongs
    rig.advance(1.0)
    res.check(all(len(d) == 16 for d in a.got), "nothing but the ACK before "
              "any ping (%s)" % sorted(set(len(d) for d in a.got)))
    a.ping()
    res.check(len(a.got[-1]) == 20 and a.got[-1][5] == 0xFE,
              "first ping -> a 20-byte pong, byte 5 0xFE")
    rig.advance(1.0)
    res.check(not any(len(d) == 272 for d in a.got),
              "no SYNR before the echo")
    a.ping()   # echoes the pong's t_ref
    res.check(any(len(d) == 272 for d in a.got), "echo -> SYNR at once")
    syn = [parse_synr(d) for d in a.got if len(d) == 272]
    res.check(all(h["seat"] == 0xFE for h in syn), "SYNR byte 5 = 0xFE")
    # the send credit: a verified seat that then only keeps its seat with
    # HELLOs (which the relay answers, not the referee) gets <= 3x what it
    # sent the referee, however long it stays seated (REVREFEREE1 H1)
    q = Console(rig, "ARENA02")
    sent = [0]
    q_in = sum(len(d) for d in [b"x" * 20] * 2)   # its two pings
    q.got = []
    for k in range(60):
        rig.send(q.addr, hello("ARENA02", 3, q.seat))
        rig.advance(1.0)
    got = sum(len(d) for d in q.got if d[:4] == b"SYNR")
    res.check(got <= 3 * q_in + 272, "quiet verified seat: %d B of SYNR in "
              "60 s for %d B sent (cap 3x + the verify frame)" % (got, q_in))
    res.check(rig.ref("ARENA02").n_starved > 0, "heartbeats held for credit")
    n0 = len(q.got)
    for k in range(6):          # 0.2 s of 30 Hz SYNS = 864 B of credit
        q.syns()
        rig.advance(1 / 30.0)
    rig.advance(0.4)
    res.check(any(d[:4] == b"SYNR" for d in q.got[n0:]),
              "it hears the referee again once it sends")
    rig.consoles.remove(q)
    # another seat's clock does not verify a forged seat (per-seat base;
    # REVREFEREE1 H1: the attacker reads its own pong, echoes it from forged
    # addresses)
    f_addr = rig.addr()
    rig.relay.on_datagram(hello("ARENA02", 3), f_addr, rig.now)
    rig.wire.sent = []
    fseat = rig.relay.endpoints[f_addr].seat
    a.ping()
    t_a = S_PING.unpack([d for d in a.got if len(d) == 20][-1])[6]
    rig.relay.on_datagram(S_PING.pack(b"SYNP", 2, 0, 0xFE, 0, 1, 0, 0xFFFF, 0),
                          f_addr, rig.now)
    for g in range(t_a - 3, t_a + 4):
        rig.now += 0.2          # inside the SYNP cap: every guess is read
        rig.relay.on_datagram(S_PING.pack(b"SYNP", 2, 0, 0xFE, 0, 1,
                                          g & 0xFFFFFFFF, 0, 0),
                              f_addr, rig.now)
    rig.wire.sent = []
    res.check(not rig.ref("ARENA02").seats[fseat].verified,
              "a forged seat echoing another seat's clock is not verified")
    # spoofed echo (a guessed t_ref) does not verify
    b = Console(rig, "ARENA02", ping=False)
    for guess in (0, 1, 1000, 2000, 123456):
        rig.send(b.addr, S_PING.pack(b"SYNP", 2, 0xFF, 0xFE, 0, 5, guess, 0,
                                     0))
    rig.advance(1.0)
    res.check(not any(len(d) == 272 for d in b.got),
              "guessed echoes never verify")
    res.check(all(len(d) in (16, 20) for d in b.got), "unverified: only ACK "
              "and pongs")
    ref = rig.ref("ARENA02")
    n0 = ref.n_claims
    b.ready(cap=2)
    res.check(ref.n_claims == n0 and not ref.seats[b.seat].ready,
              "an unverified seat's claims do not count")
    # wrong kinds
    rig.send(rig.addr(), hello("HUBCODE1", 2))
    x = Console(rig, "HUBCODE1", ping=False)
    res.check(x.got and x.got[-1][5] == 5, "arena HELLO into a hub -> 5")
    y_addr = rig.addr()
    rig.consoles.append(type("C", (), {"addr": y_addr,
                                       "receive": lambda self, d: None})())
    rig.relay.on_datagram(hello("ARENA02", 2), y_addr, rig.now)
    out = [d for d, ad in rig.wire.sent if ad == y_addr]
    rig.wire.sent = []
    res.check(out and out[-1][5] == 5, "hub HELLO into an arena -> 5")
    rig2 = Rig(mod)
    cs = [Console(rig2, "FULL0001", ping=False) for _ in range(16)]
    res.check(sorted(c.seat for c in cs) == list(range(16)), "16 seats")
    z = Console(rig2, "FULL0001", ping=False)
    res.check(z.seat is None and z.got[-1][5] == 1, "17th -> FULL")
    res.check(rig.relay.stats_line().count("arenas=") == 1, "stats tail")
    return res


def u_routes(mod):
    res = Result("arena routes: forward, referee-only, struck, field drops")
    rig = Rig(mod)
    a, b, c = (Console(rig, "ROUTES01") for _ in range(3))
    for k in (a, b, c):
        k.got = []
    a.syns()
    got_b = [d for d in b.got if d[:4] == b"SYNS"]
    res.check(len(got_b) == 1 and got_b[0][5] == a.seat, "SYNS -> others, "
              "stamped")
    res.check(not [d for d in a.got if d[:4] == b"SYNS"], "not back to me")
    n0 = len(b.got)
    a.claim(CL["GRAB"], 1)
    res.check(not [d for d in b.got[n0:] if d[:4] == b"SYNE"],
              "SYNE never forwarded")
    ref = rig.ref("ROUTES01")
    v0 = ref.n_violations
    rig.send(a.addr, b"SYNR" + bytes(268))
    res.check(ref.n_violations == v0 + 1, "SYNR from a seat: struck")
    res.check(not [d for d in b.got if d[:4] == b"SYNR" and d[5] != 0xFE],
              "and never forwarded")
    rig.send(a.addr, b"SYNQ" + bytes([2, 0, b.seat, 0]) + bytes(12))
    res.check(rig.relay.a_drop["route"] >= 2, "SYNQ 20 from a seat dropped")
    # peer ping / pong 12 as a hub
    rig.send(a.addr, b"SYNP" + bytes([0, 0, b.seat, 0]) + bytes(4))
    res.check(any(d[:4] == b"SYNP" and len(d) == 12 for d in c.got),
              "peer SYNP 12 -> everyone")
    rig.send(b.addr, b"SYNQ" + bytes([1, 0, a.seat, 0]) + bytes(4))
    res.check(any(d[:4] == b"SYNQ" and len(d) == 12 for d in a.got),
              "peer SYNQ 12 -> seat 6")
    for tag, n in ((b"SYNG", 44), (b"MP2L", 0x90), (b"SYN1", 56),
                   (b"SYNK", 24)):
        k0 = rig.relay.a_drop["unknown"]
        rig.send(a.addr, tag + bytes(n - 4))
        res.check(rig.relay.a_drop["unknown"] == k0 + 1, "%s not routed"
                  % tag.decode())
    # field checks: not forwarded
    for label, patch in (("pos past 32768 units", (12, struct.pack(
            "<i", 32769 * FIX))), ("state 78", (36, bytes([78]))),
            ("anim 194", (32, struct.pack("<H", 194))),
            ("invuln 37", (38, bytes([37]))),
            ("eaten_by 17", (41, bytes([17]))),
            ("version 2", (4, bytes([2])))):
        d = bytearray(S_SYNS.pack(b"SYNS", 1, 0, 1, 3, 999, 0, 0, 0, 0, 0, 0,
                                  0, 1, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0))
        off, by = patch
        d[off:off + len(by)] = by
        n0 = len(b.got)
        f0 = rig.relay.a_drop["field"]
        rig.send(a.addr, bytes(d))
        res.check(rig.relay.a_drop["field"] == f0 + 1 and len(b.got) == n0,
                  "SYNS %s: dropped, not forwarded" % label)
    for n in (47, 49, 23, 25, 271, 273, 19, 21):
        l0 = rig.relay.a_drop["length"]
        tag = {47: b"SYNS", 49: b"SYNS", 23: b"SYNE", 25: b"SYNE",
               271: b"SYNR", 273: b"SYNR", 19: b"SYNP", 21: b"SYNP"}[n]
        rig.send(a.addr, tag + bytes(n - 4))
        res.check(rig.relay.a_drop["length"] == l0 + 1, "%s len %d dropped"
                  % (tag.decode(), n))
    # a star session and a hub beside it see none of this
    res.check(all(t not in mod.KINDS for t in (b"SYNS", b"SYNE", b"SYNR",
                                               b"SYNO")),
              "the v2 kind table has no arena kinds")
    return res


def u_match(mod):
    res = Result("match: READY, START, wake, break, grab, ledger, first-to-N")
    rig, (a, b) = match(2, first_to_n=2)
    ref = rig.ref("ARENA01")
    st = a.find("START")
    res.check(len(st) == 1 and st[0]["b"] == 2 and st[0]["c"] == 120,
              "START with first_to_n 2, 120 s")
    res.check(ref.state == 2, "RUNNING after the countdown (%d)" % ref.state)
    row = st[0]["a"]
    wake = a.find("STAR_WAKE")
    res.check(len(wake) == 1 and wake[0]["b"] == mod.referee.STAR_ORDERS[row][0],
              "first STAR_WAKE is the row's star 0")
    inst = wake[0]["a"]
    cid = a.claim(CL["MARKER_BREAK"], inst, aux=0x1234)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(cid)
    res.check(v and v["name"] == "MARKER_BROKEN" and v["d"] == 0x1234,
              "MARKER_BROKEN with the camera yaw (%s)" % v)
    cid2 = b.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.3)
    v2 = b.verdict(cid2)
    res.check(v2 and v2["name"] == "DENY" and v2["b"] == DENY["LOST"]
              and v2["d"] == a.seat, "second breaker: LOST to the first")
    cid3 = b.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    v3 = b.verdict(cid3)
    res.check(v3 and v3["name"] == "STAR_TAKEN" and v3["b"] == 1,
              "grab -> STAR_TAKEN, score 1")
    h = b.headers[-1]
    res.check(h["scores"][b.seat] == 1 and h["scores"][a.seat] == 0,
              "ledger in the header")
    res.check(h["progress"] == 1, "progress 1")
    wake = b.find("STAR_WAKE")
    res.check(len(wake) == 2 and wake[1]["c"] == 1
              and wake[1]["b"] == mod.referee.STAR_ORDERS[row][1],
              "next STAR_WAKE at progress 1")
    cid4 = a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    v4 = a.verdict(cid4)
    res.check(v4 and v4["b"] == DENY["LOST"] and v4["d"] == b.seat,
              "late grab of a taken star: LOST, d = winner")
    inst2 = wake[1]["a"]
    a.claim(CL["MARKER_BREAK"], inst2)
    streaming(rig, [a, b], 0.3)
    b.claim(CL["GRAB"], inst2)
    streaming(rig, [a, b], 0.4)
    over = a.find("MATCH_OVER")
    res.check(len(over) == 1 and over[0]["a"] == 2
              and over[0]["c"] == 1 << b.seat, "first to 2 -> MATCH_OVER, "
              "winner b (%s)" % over)
    res.check(b.find("MATCH_OVER") == over or
              [e["seq"] for e in b.find("MATCH_OVER")] ==
              [e["seq"] for e in over], "same MATCH_OVER on both")
    res.check(ref.state == 3, "OVER")
    res.check(all(e["seat"] == a.seat for e in a.direct)
              and all(e["seat"] == b.seat for e in b.direct),
              "DENY / ACCEPTED reach their claimant only")
    res.check(not [e for e in a.events + b.events
                   if e["name"] in ("DENY", "ACCEPTED") and e["seq"]],
              "and never ride the shared stream")
    late = a.claim(CL["GRAB"], 99)
    streaming(rig, [a, b], 0.2)
    res.check(a.verdict(late)["b"] == DENY["STALE"], "claims after: STALE")
    res.check(ref.invariants() == [], "invariants %s" % ref.invariants())
    return res


def u_timeup(mod):
    res = Result("time-up: referee clock ends it, ties share the win")
    rig, (a, b, c) = match(3, time_s=5)
    ref = rig.ref("ARENA01")
    inst = a.find("STAR_WAKE")[0]["a"]
    a.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b, c], 0.2)
    a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b, c], 0.2)
    inst2 = a.find("STAR_WAKE")[-1]["a"]
    b.claim(CL["MARKER_BREAK"], inst2)
    streaming(rig, [a, b, c], 0.2)
    b.claim(CL["GRAB"], inst2)
    streaming(rig, [a, b, c], 3.0)
    res.check(not a.find("MATCH_OVER"), "not over before 5 s")
    streaming(rig, [a, b, c], 2.5)
    over = c.find("MATCH_OVER")
    res.check(len(over) == 1 and over[0]["a"] == 1, "TIME_UP")
    res.check(over and over[0]["c"] == (1 << a.seat) | (1 << b.seat)
              and over[0]["b"] == 2, "a and b tie on 1: both win")
    t0 = ref.t0
    res.check(over and ref.seat_unwire(c.seat, over[0]["t"]) == t0 + 5000,
              "end time = T0 + 5 s exactly")
    # a grab touched 5 ms after time-up, arriving inside the hold: STALE
    rig, (a, b) = match(2, time_s=4, code="TIMEUP02")
    ref = rig.ref("TIMEUP02")
    end = ref.t0 + ref.match_ms
    inst = a.find("STAR_WAKE")[-1]["a"]
    a.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.2)
    while ref.rt(rig.now) < end + 20:
        streaming(rig, [a, b], 0.01)
    cid = a.claim(CL["GRAB"], inst, stamp=ref.seat_wire(a.seat, end + 5))
    streaming(rig, [a, b], 1.0)
    v = a.verdict(cid)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STALE"]
              and ref.scores[a.seat] == 0,
              "grab stamped after time-up: DENY STALE, no point (%s)" % v)
    return res


def u_contention(mod):
    res = Result("contention: earliest stamp wins, caps clamp, ties")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    sa, sb = ref.seats[a.seat], ref.seats[b.seat]
    res.check(sa.rtt_min is not None and sa.verified, "rtt measured")

    def fresh_star():
        inst = a.find("STAR_WAKE")[-1]["a"]
        a.claim(CL["MARKER_BREAK"], inst)
        streaming(rig, [a, b], 0.3)
        return inst
    # set RTTs: a 40, b 125 (the referee's view)
    rig.no_pings = True     # the rig has no wire delay: keep these RTTs
    sa.rtts.clear(); sa.rtts.append(40); sa.rtt_min = 40
    sb.rtts.clear(); sb.rtts.append(125); sb.rtt_min = 125
    res.check(sa.cap() == 45 and sb.cap() == 87, "caps 45 / 87 (%d %d)"
              % (sa.cap(), sb.cap()))
    # b touched first (stamp 60 ms ago) but its claim arrives after a's
    inst = fresh_star()
    tnow = rig.now
    ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) - 10)
    rig.advance(0.02)
    cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 60)
    streaming(rig, [a, b], 0.4)
    res.check(b.verdict(cb)["name"] == "STAR_TAKEN", "earlier stamp (b) "
              "wins though it arrived later")
    res.check(a.verdict(ca)["b"] == DENY["LOST"], "a: LOST")
    # back-dating past the cap is clamped: a claims 500 ms ago, b 30 ms ago
    sa.rtt_min = 40
    sb.rtt_min = 125
    inst = fresh_star()
    tnow = rig.now
    c0 = sa.clamps
    # a's 500 ms back-date clamps to now - 45; b's 60 is inside its 87 cap
    ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) - 500)
    cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 60)
    streaming(rig, [a, b], 0.4)
    res.check(sa.clamps == c0 + 1, "a's stamp clamped")
    res.check(b.verdict(cb)["name"] == "STAR_TAKEN",
              "clamped back-dater loses to a real earlier touch")
    # from the future: clamped to arrival
    inst = fresh_star()
    tnow = rig.now
    c0 = sa.clamps
    ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) + 5000)
    rig.advance(0.01)
    cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 5)
    streaming(rig, [a, b], 0.4)
    res.check(sa.clamps == c0 + 1, "future stamp clamped")
    res.check(b.verdict(cb)["name"] == "STAR_TAKEN", "future stamp gains "
              "nothing")
    # exact tie: fewer stars first (b has 3, a has 0)
    inst = fresh_star()
    tnow = rig.now
    ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) - 20)
    cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 20)
    streaming(rig, [a, b], 0.4)
    res.check(a.verdict(ca)["name"] == "STAR_TAKEN", "tie -> fewer stars "
              "(a 0 vs b %d)" % ref.scores[b.seat])
    # tie at equal scores -> seed rank (deterministic)
    ref.scores[a.seat] = ref.scores[b.seat] = 2
    inst = fresh_star()
    tnow = rig.now
    ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) - 20)
    cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 20)
    streaming(rig, [a, b], 0.4)
    first = a.seat if ref.seat_rank[a.seat] < ref.seat_rank[b.seat] else b.seat
    win = a if first == a.seat else b
    res.check(win.verdict(ca if win is a else cb)["name"] == "STAR_TAKEN",
              "equal scores -> the seed's seat order")
    # grab while hurt -> STATE; grab a WAKING star -> GONE; range
    inst = a.find("STAR_WAKE")[-1]["a"]
    cg = a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cg)["b"] == DENY["GONE"], "grab a waking star: GONE")
    a.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.3)
    a.state = ST_HURT
    streaming(rig, [a, b], 0.1)
    cg = a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cg)["b"] == DENY["STATE"], "grab while hurt: STATE")
    a.state = ST_WAIT
    ref.stars[inst].known = True
    ref.stars[inst].x = 5000 * FIX
    streaming(rig, [a, b], 0.1)
    cg = a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cg)["b"] == DENY["RANGE"], "grab far from a known "
              "star: RANGE")
    # SYNO from the launcher places the star; from anyone else ignored
    syno = S_SYNO.pack(b"SYNO", 1, 0, 1, inst, a.frame, 7 * FIX, 0, 0, 0, 0,
                       0, 2, 0)
    rig.send(b.addr, syno)
    res.check(ref.stars[inst].x == 5000 * FIX, "SYNO from a non-launcher "
              "ignored")
    rig.send(a.addr, syno)
    res.check(ref.stars[inst].x == 7 * FIX, "SYNO from the launcher places "
              "it")
    cg = a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cg)["name"] == "STAR_TAKEN", "then the grab lands")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_hits(mod):
    res = Result("hits: lag-compensated, invuln, range, clash, trade, spill")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    # give b a star so a hit spills one
    inst = a.find("STAR_WAKE")[-1]["a"]
    b.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.2)
    b.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(ref.scores[b.seat] == 1, "b has 1 star")
    a.pos = [0, 0, 0]
    b.pos = [100, 0, 0]
    a.state = ST_PUNCH
    streaming(rig, [a, b], 0.2)
    # FIX9C: inside the star-get pose (1.5 s after the grab) nothing hurts b
    cid = a.claim(CL["HIT"], b.seat, aux=0x40 >> 5, ref_frame=b.frame - 3)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(cid)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["INVULN"],
              "a hit 0.5 s after b's star (b is in the pose): INVULN (%s)" % v)
    res.check(ref.scores[b.seat] == 1, "... b keeps the star")
    streaming(rig, [a, b], 1.4)                  # the pose is over
    a.pos = [0, 0, 0]
    b.pos = [100, 0, 0]
    a.state = ST_PUNCH
    streaming(rig, [a, b], 0.2)
    cid = a.claim(CL["HIT"], b.seat, aux=0x40 >> 5, ref_frame=b.frame - 3)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(cid)
    res.check(v and v["name"] == "HIT" and v["seat"] == b.seat
              and v["a"] == a.seat and v["d"] == 2, "HIT verdict (%s)" % v)
    res.check(v and v["e"] != 0, "it names the spilled instance")
    sp = a.find("SPILL", seat=b.seat)
    res.check(len(sp) == 1 and sp[0]["c"] == 1 and sp[0]["d"] == 0,
              "SPILL cause hit, b now 0")
    res.check(ref.scores[b.seat] == 0, "ledger b 0")
    res.check(v and abs(v["x"] - 0) < FIX, "HIT carries the attacker pos")
    # second hit inside the immune window: INVULN
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 3)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["INVULN"], "second hit: INVULN")
    # victim's own invuln byte
    streaming(rig, [a, b], 1.2)
    b.invuln = 20
    streaming(rig, [a, b], 0.2)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["INVULN"], "victim invuln > 0")
    b.invuln = 0
    # out of reach
    b.pos = [600, 0, 0]
    streaming(rig, [a, b], 0.2)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["RANGE"], "600 units: RANGE")
    # lag compensation: b is far NOW but was close at the drawn frame
    b.pos = [100, 0, 0]
    streaming(rig, [a, b], 0.2)
    drawn = b.frame
    b.pos = [900, 0, 0]
    streaming(rig, [a, b], 0.1)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=drawn)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["name"] == "HIT", "compensated: b was in reach "
              "at the frame a drew")
    # too old a drawn frame
    streaming(rig, [a, b], 1.3)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 14)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["TOO_OLD"], "drawn 14 frames back: "
              "TOO_OLD")
    # not attacking
    a.state = ST_WALK
    streaming(rig, [a, b], 0.5)
    b.pos = [100, 0, 0]
    streaming(rig, [a, b], 0.2)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["STATE"], "walking: STATE")
    # clash: b punching, facing a (b at +x, a at origin: angle -x = 0xC000)
    a.state = ST_PUNCH
    b.state = ST_PUNCH
    b.yaw = -0x4000
    streaming(rig, [a, b], 0.3)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["name"] == "CLASH", "victim punching facing "
              "the attacker: CLASH")
    # facing away: a hit
    b.yaw = 0x4000
    streaming(rig, [a, b], 0.3)
    cid = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["name"] == "HIT", "punching away: HIT")
    # trade: both claim within one frame
    streaming(rig, [a, b], 1.5)
    b.yaw = 0
    a.yaw = 0
    a.state = b.state = ST_DIVE
    streaming(rig, [a, b], 0.3)
    tnow = rig.now
    ca = a.claim(CL["HIT"], b.seat, aux=0x400 >> 5, ref_frame=b.frame - 2,
                 stamp=ref.ref_ms(tnow, a.seat) - 20)
    cb = b.claim(CL["HIT"], a.seat, aux=0x400 >> 5, ref_frame=a.frame - 2,
                 stamp=ref.ref_ms(tnow, b.seat) - 10)
    streaming(rig, [a, b], 0.4)
    va, vb = a.verdict(ca), b.verdict(cb)
    res.check(va["name"] == "HIT" and vb["name"] == "HIT" and vb["b"] == 1,
              "trade: both stand (%s / %s)" % (va["name"], vb["name"]))
    # self / not in match
    cid = a.claim(CL["HIT"], a.seat, ref_frame=a.frame)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["b"] == DENY["SELF"], "hit self: SELF")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_hit_nc(mod):
    """HITSPILL1: the ROM's tongue raises no_control on the attacker itself;
    the referee must accept an attacker in that state and nothing else."""
    res = Result("hit no_control: a caught tongue attacks, nothing frozen does")
    R = mod.referee
    NC = 0x40
    ST_YOSHI, ST_TALK = 3, 48
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    b.pos, a.pos = [100, 0, 0], [0, 0, 0]

    def settle(sec=1.3):
        a.state, a.flags = ST_WAIT, 0x03
        streaming(rig, [a, b], sec)

    def hit(kind=CL["HIT"]):
        cid = a.claim(kind, b.seat, aux=2, ref_frame=b.frame - 2)
        streaming(rig, [a, b], 0.3)
        return a.verdict(cid)

    # 1. the honest tongue: state up clean, the catch raises the flag
    settle()
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.4)
    v = hit()
    res.check(v and v["name"] == "HIT" and v["a"] == a.seat,
              "caught tongue (flag raised inside the attack state): HIT (%s)" % v)
    # 2. the same state, flag already clean: still a HIT (no regression)
    settle()
    a.state = ST_YOSHI
    streaming(rig, [a, b], 0.3)
    v = hit()
    res.check(v and v["name"] == "HIT", "plain tongue: HIT (%s)" % v)
    # 3. no_control in a non-attack state: refused, even right after a tongue
    settle()
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.state, a.flags = ST_TALK, 0x03 | NC
    streaming(rig, [a, b], 0.2)
    v = hit()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "St_Talk with no_control inside the attack lookback: STATE (%s)" % v)
    # 4. forged: the flag raised by a frozen state and carried into the attack
    settle()
    a.state, a.flags = ST_TALK, 0x03 | NC
    streaming(rig, [a, b], 0.5)
    a.state = ST_YOSHI
    streaming(rig, [a, b], 0.3)
    v = hit()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "no_control raised in St_Talk, then St_YoshiPower: STATE (%s)" % v)
    # 5. forged: the flag up in an attack state that never raises it
    settle()
    a.state, a.flags = ST_PUNCH, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.3)
    v = hit()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "St_PunchKick with no_control: STATE (%s)" % v)
    # 6. bounded: parked in the tongue with the flag up for 1.7 s
    settle()
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 1.7)
    v = hit()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "flag up in St_YoshiPower for 51+ frames: STATE (%s)" % v)
    # 7. ... and a fresh run (flag down one frame, up again) qualifies again
    a.flags = 0x03
    streaming(rig, [a, b], 0.07)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.3)
    streaming(rig, [a, b], 1.0)
    a.flags = 0x03
    streaming(rig, [a, b], 0.07)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.3)
    v = hit()
    res.check(v and v["name"] == "HIT", "a fresh no_control run: HIT (%s)" % v)
    # 8. hurt flag with no_control in the tongue: refused
    settle()
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC | 0x20
    streaming(rig, [a, b], 0.3)
    v = hit()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "in_hurt + no_control in the tongue: STATE (%s)" % v)
    # 9. eating from the same state
    settle()
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.3)
    v = hit(CL["EAT"])
    res.check(v and v["name"] == "EATEN", "EAT from a caught tongue: EATEN (%s)" % v)
    cid = a.claim(CL["SPIT"], b.seat)
    streaming(rig, [a, b], 0.3)
    # 10. the exemption is for ATTACKS only: a grab / marker from it is refused
    settle(0.5)
    inst = a.find("STAR_WAKE")[-1]["a"]
    a.state, a.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b], 0.3)
    a.flags = 0x03 | NC
    streaming(rig, [a, b], 0.3)
    cid = a.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(cid)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "MARKER_BREAK with no_control in the tongue: STATE (%s)" % v)
    # 11. the helper, on hand-made histories: a seat's very first samples,
    #     a run that began in another state, the frame cap exactly
    def seat_with(rows):
        s = R.Seat(1, 0)
        for i, (st, fl) in enumerate(rows):
            smp = R.Sample()
            smp.frame, smp.rt, smp.state, smp.flags = 100 + i, i * 33, st, fl
            smp.x = smp.y = smp.z = smp.yaw = smp.step = 0
            smp.invuln = smp.power = smp.held = smp.eaten_by = 0
            s.hist.append(smp)
        return s
    ok = R.Referee._attack_nc_ok
    s = seat_with([(ST_YOSHI, NC)] * 5)
    res.check(not ok(s, s.hist[-1]), "no clean sample before the run: refused")
    s = seat_with([(ST_YOSHI, 0)] + [(ST_YOSHI, NC)] * 5)
    res.check(ok(s, s.hist[-1]), "clean sample, then a short run: accepted")
    s = seat_with([(ST_YOSHI, 0)] + [(ST_YOSHI, NC)] * R.ATTACK_NC_MAX_FRAMES)
    res.check(ok(s, s.hist[-1]), "run of exactly the cap: accepted")
    s = seat_with([(ST_YOSHI, 0)] + [(ST_YOSHI, NC)] * (R.ATTACK_NC_MAX_FRAMES + 2))
    res.check(not ok(s, s.hist[-1]), "run one frame past the cap: refused")
    s = seat_with([(ST_WAIT, 0), (ST_TALK, NC), (ST_YOSHI, NC)])
    res.check(not ok(s, s.hist[-1]), "run begun in St_Talk: refused")
    s = seat_with([(ST_WAIT, 0), (ST_YOSHI, NC), (ST_TALK, NC)])
    res.check(not ok(s, s.hist[-1]), "sample itself in St_Talk: refused")
    s = seat_with([(ST_WAIT, 0), (ST_YOSHI, NC)])
    other = seat_with([(ST_YOSHI, NC)]).hist[-1]
    res.check(not ok(s, other), "a sample not in the history: refused")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_reliability(mod):
    res = Result("reliability: resends, ack catch-up, snapshot, one verdict")
    rig, (a, b) = match(2, cap=3)       # room for the late joiner below
    streaming(rig, [a, b], 20.0)        # a 3-body match starts on 2 after 20 s
    ref = rig.ref("ARENA01")
    inst = a.find("STAR_WAKE")[-1]["a"]
    cid = a.claim(CL["MARKER_BREAK"], inst)
    a.claim(CL["MARKER_BREAK"], inst, cid=cid)   # resend while pending
    streaming(rig, [a, b], 0.3)
    v = [e for e in a.events if e["seat"] == a.seat and e["c"] == cid]
    res.check(len(v) == 1, "one verdict for a resent claim")
    # resend after the verdict left the window: directed resend (ten
    # stream events push it out; DENY / ACCEPTED are directed, not stream)
    for i in range(10):
        a.claim(CL["PIT"], 0xFF)
        ref._emit(ref.EV_JOIN if hasattr(ref, "EV_JOIN") else 11, 0xFF)
    streaming(rig, [a, b], 0.2)
    n0 = len(a.got)
    a.claim(CL["MARKER_BREAK"], inst, cid=cid)
    direct = [parse_synr(d) for d in a.got[n0:] if len(d) == 272]
    res.check(any(h["flags"] & 0x02 and any(e["c"] == cid and
                                            e["name"] == "MARKER_BROKEN"
                                            for e in h["events"])
                  for h in direct), "old verdict re-sent, directed")
    res.check(ref.n_verdicts == 11 + 2, "decided once (%d)" % ref.n_verdicts)
    # a console that stops applying (ack stuck) gets catch-up frames
    b_applied = b.applied
    real = b.receive
    b.receive = lambda d: None   # deaf for a while
    for i in range(12):
        ref._emit(11, 0xFF)     # stream events while b is deaf
    streaming(rig, [a, b], 0.3)
    b.receive = real
    b.applied = b_applied
    streaming(rig, [a, b], 0.8)
    res.check(b.applied == ref.newest_seq, "deaf console caught up "
              "(%d / %d)" % (b.applied, ref.newest_seq))
    res.check(any(h["flags"] & 0x04 for h in b.headers), "via RESEND frames")
    # a seat that never acks (ack 0) with more than 8 events: snapshot
    c = Console(rig, "ARENA01")
    c.syns_ack_zero = True
    streaming(rig, [a, b, c], 0.5)
    c.ready(cap=3, time_s=120)
    streaming(rig, [a, b, c], 0.5)
    joins = a.find("JOIN", seat=c.seat)
    res.check(joins and joins[-1]["a"] == 1, "late READY: JOIN mid-match")
    res.check(any(e["name"] == "START" and e["seq"] == 0 for e in c.snap),
              "snapshot replays START (seq 0)")
    res.check(any(e["name"] == "STAR_STATE" for e in c.snap),
              "snapshot carries the live stars")
    res.check(c.applied == ref.newest_seq or c.applied >= ref.newest_seq - 1,
              "joiner applied up to the stream head")
    res.check(ref.roster == {a.seat, b.seat, c.seat}, "roster grew")
    return res


def u_leave(mod):
    res = Result("leave: QUIT, silence, relay detach, last standing")
    rig, (a, b, c) = match(3)
    ref = rig.ref("ARENA01")
    inst = a.find("STAR_WAKE")[-1]["a"]
    c.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b, c], 0.2)
    c.claim(CL["GRAB"], inst)
    streaming(rig, [a, b, c], 0.3)
    inst = a.find("STAR_WAKE")[-1]["a"]
    c.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b, c], 0.2)
    c.claim(CL["GRAB"], inst)
    streaming(rig, [a, b, c], 0.3)
    res.check(ref.scores[c.seat] == 2, "c has 2")
    q = c.claim(CL["QUIT"], 0xFF)
    streaming(rig, [a, b], 0.3)
    lv = a.find("LEAVE", seat=c.seat)
    res.check(lv and lv[-1]["a"] == 1 and lv[-1]["d"] == 2,
              "QUIT -> LEAVE, 2 stars spilled")
    res.check(len(a.find("SPILL", seat=c.seat)) == 2, "two SPILL events")
    res.check(ref.scores[c.seat] == 0 and c.seat not in ref.roster,
              "out of the ledger")
    res.check(c.verdict(q) and c.verdict(q)["name"] == "ACCEPTED",
              "QUIT accepted")
    # silence: b stops sending for 3 s
    rig.consoles.remove(b)

    def keep_a():
        pass
    streaming(rig, [a], 3.5)
    lv = a.find("LEAVE", seat=b.seat)
    res.check(lv and lv[-1]["a"] == 2, "3 s silent -> LEAVE (silent)")
    over = a.find("MATCH_OVER")
    res.check(over and over[-1]["a"] == 3 and over[-1]["c"] == 1 << a.seat,
              "last standing -> MATCH_OVER, a wins")
    # relay detach (idle expiry) in another match
    rig2, (d, e) = match(2, code="ARENA03")
    rig2.consoles.remove(e)
    ep = rig2.relay.endpoints[e.addr]
    rig2.relay._detach(ep, "idle", rig2.now)
    streaming(rig2, [d], 0.3)
    lv = d.find("LEAVE", seat=e.seat)
    res.check(lv and lv[-1]["a"] == 3, "relay detach -> LEAVE (detach)")
    res.check(d.find("MATCH_OVER"), "and last standing ends it")
    return res


def u_rejoin(mod):
    res = Result("rejoin: a silent seat is reclaimed by number, a live one never; "
                 "no seat past the body count")
    # --- the quick rejoin: seat 3's console dies, a new process (new source
    # port) asks for seat 3 about 2 s later, while the relay still holds it
    rig, (a, b, c, d) = match(4, code="REJOIN01")
    code = "REJOIN01"
    ref = rig.ref(code)
    sess = rig.session(code)
    res.check(ref.body_count() == 4, "READY fixed the body count at 4")
    old_addr = d.addr
    streaming(rig, [a, b, c], 0.5)          # d silent for 0.5 s
    e = Console(rig, code, want=3, ping=False)
    res.check(e.seat is None and e.got[-1][5] == 4,
              "silent 0.5 s: want_seat 3 is refused, seat taken (status 4)")
    res.check(sess.seats.get(3) == old_addr, "seat 3 still held by its console")
    streaming(rig, [a, b, c], 2.0)          # now silent 2.5 s: still held (reclaim is 3.0 s)
    rig.consoles.remove(e)
    e = Console(rig, code, want=3, ping=False)
    res.check(e.seat is None and e.got[-1][5] == 4,
              "silent 2.5 s: want_seat 3 still refused (a 1.5-3 s outage keeps its seat)")
    rig.consoles.remove(e)
    streaming(rig, [a, b, c], 1.0)          # now silent 3.5 s
    e = Console(rig, code, want=3, ping=False)
    res.check(e.seat == 3 and e.got[-1][5] == 0 and e.got[-1][6] == 3,
              "silent 3.5 s: want_seat 3 -> seat 3 back, ACK status 0")
    res.check(sess.seats.get(3) == e.addr and old_addr not in
              rig.relay.endpoints, "the old holder is gone, the new one holds it")
    rig.consoles.remove(d)
    streaming(rig, [a, b, c, e], 0.3)
    lv = a.find("LEAVE", seat=3)
    res.check(lv, "the referee ran the old holder's leave (LEAVE event, "
              "in order before the new join)")
    res.check(3 not in ref.roster, "the old body is out of the match ledger")
    # the old console, if it was only lagging, is a stranger now and is told
    # no (its keepalive HELLO asks for seat 3, which is live again)
    d_back = Console(rig, code, want=3, ping=False)
    res.check(d_back.seat is None and d_back.got[-1][5] == 4,
              "the displaced console cannot take the seat back from a live one")
    rig.consoles.remove(d_back)
    # --- HIJACK: a live seat's number from another console is refused, over
    # and over, while the holder is heard (SYNS 30 Hz) or only keeps HELLOs
    rig.relay.stats_line()
    refused = 0
    for k in range(40):
        streaming(rig, [a, b, c, e], 0.25)
        h = Console(rig, code, want=1, ping=False)
        if h.seat is None and h.got[-1][5] == 4:
            refused += 1
        rig.consoles.remove(h)
    res.check(refused == 40 and sess.seats.get(1) == b.addr,
              "hijack x40 against a live seat (10 s of play): all refused, "
              "the holder keeps seat 1 (%d)" % refused)
    # only HELLO keepalives, 1 per second: still live
    for k in range(6):
        rig.send(b.addr, hello(code, 3, 1))
        rig.advance(1.0, step=0.05)
        h = Console(rig, code, want=1, ping=False)
        res.check(h.seat is None and h.got[-1][5] == 4,
                  "holder that only sends its 1 Hz HELLO is live (t+%d)" % k)
        rig.consoles.remove(h)
    # the boundary: heard 2.9 s ago = live, 3.1 s ago = reclaimable
    rig.send(b.addr, hello(code, 3, 1))
    rig.advance(2.9, step=0.05)
    h = Console(rig, code, want=1, ping=False)
    res.check(h.seat is None, "silent 2.9 s: not yet")
    rig.consoles.remove(h)
    rig.advance(0.2, step=0.05)
    h = Console(rig, code, want=1, ping=False)
    res.check(h.seat == 1, "silent 3.1 s: reclaimable")
    rig.consoles.remove(h)
    # a console may not ask for a seat by taking WANT_ANY out of the bodies
    streaming(rig, [a, c, e], 0.2)
    # --- no seat past the body count
    rig2, (p0, p1) = match(2, code="BODY0002")
    ref2 = rig2.ref("BODY0002")
    res.check(ref2.body_count() == 2, "2 bodies")
    x = Console(rig2, "BODY0002", want=0xFF, ping=False)
    res.check(x.seat is None and x.got[-1][5] == 1,
              "a third console asking for any seat in a 2-body arena: FULL "
              "(status 1), never seat 2")
    y = Console(rig2, "BODY0002", want=2, ping=False)
    res.check(y.seat is None and y.got[-1][5] == 1,
              "want_seat 2 in a 2-body arena: FULL (status 1)")
    z = Console(rig2, "BODY0002", want=15, ping=False)
    res.check(z.seat is None and z.got[-1][5] == 1, "want_seat 15: FULL")
    res.check(sorted(rig2.session("BODY0002").seats) == [0, 1],
              "seats still 0 and 1")
    # free seat 0 by a relay detach: any-seat gives 0, still not 2
    rig2.consoles.remove(p0)
    rig2.relay._detach(rig2.relay.endpoints[p0.addr], "idle", rig2.now)
    w = Console(rig2, "BODY0002", want=0xFF, ping=False)
    res.check(w.seat == 0, "any-seat with seat 0 free -> seat 0")
    w2 = Console(rig2, "BODY0002", want=0xFF, ping=False)
    res.check(w2.seat is None and w2.got[-1][5] == 1, "and the next is FULL")
    # before any READY the room is as open as it was (the body count is not
    # known yet)
    rig3 = Rig(mod)
    q = [Console(rig3, "BODY0003", ping=False) for _ in range(6)]
    res.check([k.seat for k in q] == list(range(6)) and
              rig3.ref("BODY0003").body_count() is None,
              "no READY yet: seats 0..5 given as before")
    # --- the sole member's console restarts: the room survives the reclaim
    rig4 = Rig(mod)
    s0 = Console(rig4, "SOLO0004")
    streaming(rig4, [s0], 0.5)
    rig4.advance(3.5)
    s1 = Console(rig4, "SOLO0004", want=0, ping=False)
    res.check(s1.seat == 0 and rig4.session("SOLO0004") is not None
              and "SOLO0004" in rig4.relay.arenas
              and rig4.session("SOLO0004").seats.get(0) == s1.addr,
              "a lone member restarting gets seat 0 and the room is kept")
    # --- hubs are untouched: a silent hub member is not displaced by number
    rig5 = Rig(mod)
    h0 = Console.__new__(Console)
    addr_a, addr_b = rig5.addr(), rig5.addr()
    rig5.relay.on_datagram(hello("HUB00005", 2, 0), addr_a, rig5.now)
    rig5.advance(5.0)
    rig5.wire.sent = []
    rig5.relay.on_datagram(hello("HUB00005", 2, 0), addr_b, rig5.now)
    out = [d for d, ad in rig5.wire.sent if ad == addr_b]
    res.check(out and out[-1][5] == 4,
              "a hub seat is not reclaimed by number (status 4)")
    return res


def u_ready(mod):
    res = Result("READY: config must agree; modes; bad fields; restart")
    rig = Rig(mod)
    a, b, c = (Console(rig, "READY001") for _ in range(3))
    streaming(rig, [a, b, c], 0.6)
    ca = a.ready(cap=3, digest=1)
    cb = b.ready(cap=3, digest=2)
    cc = c.ready(cap=3, digest=1, mode=1)
    streaming(rig, [a, b, c], 0.3)
    res.check(a.verdict(ca)["name"] == "ACCEPTED", "first READY accepted")
    res.check(b.verdict(cb)["b"] == DENY["MISMATCH"], "other digest: MISMATCH")
    res.check(c.verdict(cc)["b"] == DENY["UNSUPPORTED"], "mode 1: UNSUPPORTED")
    cbad = c.ready(cap=0, digest=1)
    streaming(rig, [a, b, c], 0.3)
    res.check(c.verdict(cbad)["b"] == DENY["BAD"], "capacity 0: BAD")
    ref = rig.ref("READY001")
    res.check(ref.state == 0, "still waiting (1 of 3 ready)")
    b.ready(cap=3, digest=1)
    streaming(rig, [a, b, c], 0.3)
    res.check(ref.state == 0, "2 of 3: waits for the third or 20 s")
    streaming(rig, [a, b, c], 20.0)
    res.check(ref.state in (1, 2) and ref.roster == {a.seat, b.seat},
              "after 20 s it starts with the two ready")
    # a claim before RUNNING: STALE
    rig2 = Rig(mod)
    d, e = (Console(rig2, "READY002") for _ in range(2))
    streaming(rig2, [d, e], 0.6)
    d.ready(); e.ready()
    streaming(rig2, [d, e], 0.3)
    early = d.claim(CL["GRAB"], 1)
    streaming(rig2, [d, e], 0.2)
    res.check(d.verdict(early)["b"] == DENY["STALE"], "claim in countdown: "
              "STALE")
    return res


def u_strikes(mod):
    res = Result("strikes: speed / teleport logged, sanctions, kick knob")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    sa = ref.seats[a.seat]
    s0 = sa.strikes_total

    def run_fast():
        a.pos[0] += 80          # 80 units a frame walking
    a.state = ST_WALK
    state = {"n": rig.now}

    def each():
        if rig.now >= state["n"]:
            state["n"] += 1 / 30.0
            run_fast()
            a.syns()
            b.syns()
    rig.advance(0.5, each=each)
    res.check(sa.strikes_total > s0, "80/frame walking: strikes (%d)"
              % (sa.strikes_total - s0))
    res.check(any("strike seat %d" % a.seat in m for m in mod.LOGGED),
              "logged")
    res.check(a.seat in ref.roster, "phase A: not kicked")
    # a slide at 100/frame is fine
    s1 = sa.strikes_total
    a.state = ST_SLIDE
    state["n"] = rig.now

    def each2():
        if rig.now >= state["n"]:
            state["n"] += 1 / 30.0
            a.pos[0] += 100
            a.syns()
            b.syns()
    rig.advance(0.3, each=each2)
    res.check(sa.strikes_total == s1, "butt slide 100/frame: no strike")
    # teleport after a PIT claim: sanctioned
    a.state = ST_WAIT
    a.claim(CL["PIT"], 0xFF)
    streaming(rig, [a, b], 0.1)
    s2 = sa.strikes_total
    a.flags |= 0x04
    a.pos = [5000, 0, 0]
    streaming(rig, [a, b], 0.1)
    a.flags &= ~0x04
    res.check(sa.strikes_total == s2, "teleport after PIT: no strike")
    streaming(rig, [a, b], 3.5)
    a.flags |= 0x04
    a.pos = [-5000, 0, 0]
    streaming(rig, [a, b], 0.05)
    a.flags &= ~0x04
    res.check(sa.strikes_total > s2, "teleport with no sanction: strike")
    # kick knob on
    rigk, (k1, k2) = match(2, env={"SM64DS_REFEREE_KICK": "1"},
                           code="KICK0001")
    refk = rigk.ref("KICK0001")
    for i in range(4):
        rigk.send(k1.addr, b"SYNR" + bytes(268))
    res.check(k1.addr not in rigk.relay.endpoints, "kick on: 3 strikes -> "
              "detached")
    return res


def u_eat(mod):
    res = Result("eat (provisional, A5): EATEN, spit, escape, leave")
    rig, (a, b) = match(2)
    a.state = 3     # St_YoshiPower
    a.pos, b.pos = [0, 0, 0], [80, 0, 0]
    streaming(rig, [a, b], 0.3)
    cid = a.claim(CL["EAT"], b.seat, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid)["name"] == "EATEN", "EATEN")
    cid2 = a.claim(CL["HIT"], b.seat, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(cid2)["name"] == "DENY", "can't hit the eaten one")
    sp = a.claim(CL["SPIT"], b.seat)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(sp)
    res.check(v and v["name"] == "RELEASED" and v["b"] == 1, "SPIT -> "
              "RELEASED 1")
    esc = b.claim(CL["ESCAPE"], a.seat)
    streaming(rig, [a, b], 0.3)
    res.check(b.verdict(esc)["b"] == DENY["GONE"], "escape when free: GONE")
    # an ESCAPE right after the EATEN is refused (no real escape is that fast);
    # the mouthful is unchanged, and a later ESCAPE is released
    streaming(rig, [a, b], 3.0)          # past the hit immunity of the first round
    cid3 = a.claim(CL["EAT"], b.seat, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.05)
    res.check(a.verdict(cid3) and a.verdict(cid3)["name"] == "EATEN", "eaten again")
    early = b.claim(CL["ESCAPE"], a.seat, cid=201)
    streaming(rig, [a, b], 0.1)
    ve = b.verdict(early)
    res.check(ve and ve["b"] == DENY["STATE"], "ESCAPE 0.1 s after the EATEN: DENY STATE (%s)" % ve)
    res.check(rig.ref("ARENA01").seats[b.seat].eaten_by == a.seat,
              "the early ESCAPE left the mouthful as it was")
    streaming(rig, [a, b], 0.6)
    late = b.claim(CL["ESCAPE"], a.seat, cid=202)
    streaming(rig, [a, b], 0.3)
    vl = b.verdict(late)
    res.check(vl and vl["name"] == "RELEASED" and vl["b"] == 3, "ESCAPE 0.7 s after: RELEASED 3 (%s)" % vl)
    return res


def u_grab(mod):
    res = Result("grab (GRAB1): CATCH, THROW, SWING, BREAK, leave, failsafe")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    sa, sb = ref.seats[a.seat], ref.seats[b.seat]
    a.pos, b.pos = [0, 0, 0], [100, 0, 0]
    a.look = 2
    a.state = ST_PUNCH
    streaming(rig, [a, b], 0.3)

    def catch(ref_back=2, who=None, victim=None):
        who = who or a
        victim = victim or b
        cid = who.claim(CL["CATCH"], victim.seat, ref_frame=victim.frame - ref_back)
        streaming(rig, [a, b], 0.3)
        return who.verdict(cid)

    # the victim is not stunned: refused (must-FAIL control for the accept below)
    v = catch()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["INVULN"],
              "victim in St_Wait: DENY INVULN (%s)" % v)
    # a Wario hit puts it in the stun
    h = a.claim(CL["HIT"], b.seat, aux=0x40 >> 5, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(h)["name"] == "HIT", "the hit lands")
    b.state = ST_HURT
    b.flags |= 0x20
    streaming(rig, [a, b], 0.3)
    # not a Wario
    a.look = 3
    streaming(rig, [a, b], 0.2)
    v = catch()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "attacker not Wario: DENY STATE (%s)" % v)
    a.look = 2
    # not attacking
    a.state = ST_WALK
    streaming(rig, [a, b], 0.5)
    v = catch()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "attacker walking: DENY STATE (%s)" % v)
    a.state = ST_PUNCH
    # out of reach
    b.pos = [700, 0, 0]
    streaming(rig, [a, b], 0.3)
    v = catch()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"],
              "700 units: DENY RANGE (%s)" % v)
    b.pos = [100, 0, 0]
    streaming(rig, [a, b], 0.3)
    # the good one
    cid = a.claim(CL["CATCH"], b.seat, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.12)
    v = a.verdict(cid)
    res.check(v and v["name"] == "GRABBED" and v["seat"] == b.seat
              and v["a"] == a.seat, "CATCH: GRABBED (%s)" % v)
    res.check(sb.grabbed_by == a.seat and sa.holding == b.seat, "pair recorded")
    # THROW too soon is refused, the hold stays (must-FAIL control for the throw below)
    early = a.claim(CL["THROW"], b.seat, aux=0x1234, ref_frame=0x30000)
    streaming(rig, [a, b], 0.1)
    ve = a.verdict(early)
    res.check(ve and ve["name"] == "DENY" and ve["b"] == DENY["STATE"],
              "THROW 0.1 s after the catch: DENY STATE (%s)" % ve)
    res.check(sb.grabbed_by == a.seat, "the early THROW left the hold as it was")
    # a hold is exclusive, and a held seat cannot be hit
    v = catch()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "second catch while holding: DENY STATE (%s)" % v)
    a.state, b.state = ST_HOLDLIGHT, ST_GRABBED
    b.flags &= ~0x20
    streaming(rig, [a, b], 0.2)
    ch = a.claim(CL["HIT"], b.seat, aux=2, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(ch)["name"] == "DENY", "a held seat cannot be hit")
    # a THROW from a seat that does not hold it, and a BREAK at the wrong seat
    streaming(rig, [a, b], 0.6)
    bad = b.claim(CL["THROW"], a.seat, aux=0, ref_frame=0x30000)
    streaming(rig, [a, b], 0.3)
    res.check(b.verdict(bad)["b"] == DENY["GONE"], "THROW by the victim: GONE")
    # a launch above the cap
    over = a.claim(CL["THROW"], b.seat, aux=0, ref_frame=GRAB_SPEED_MAX_T + 1)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(over)["b"] == DENY["BAD"], "launch over the cap: BAD")
    res.check(sb.grabbed_by == a.seat, "still held")
    # the real throw
    th = a.claim(CL["THROW"], b.seat, aux=0x1234, ref_frame=0x30000)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(th)
    res.check(v and v["name"] == "THROWN" and v["b"] == 1 and v["d"] == 0x1234
              and v["x"] == 0x30000 and v["seat"] == b.seat and v["a"] == a.seat,
              "THROW: THROWN how 1, yaw and speed carried (%s)" % v)
    res.check(sb.grabbed_by is None and sa.holding is None, "pair cleared")
    # throw again: GONE
    again = a.claim(CL["THROW"], b.seat, aux=0, ref_frame=0x10000)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(again)["b"] == DENY["GONE"], "THROW with no hold: GONE")
    # a resend of the verdict is the same event
    rig_events = len([e for e in a.events if e["name"] == "THROWN"])
    res.check(rig_events == 1, "one THROWN in the stream")

    # a second hold: SWING and BREAK
    def new_hold(n):
        b.state, b.flags = ST_WAIT, 0x03
        a.state = ST_PUNCH
        b.pos = [100, 0, 0]
        streaming(rig, [a, b], 1.4)      # past the hit immunity
        hh = a.claim(CL["HIT"], b.seat, aux=0x40 >> 5, ref_frame=b.frame - 2)
        streaming(rig, [a, b], 0.3)
        ok = a.verdict(hh)["name"] == "HIT"
        b.state, b.flags = ST_HURT, 0x23
        streaming(rig, [a, b], 0.3)
        c2 = a.claim(CL["CATCH"], b.seat, ref_frame=b.frame - 2)
        streaming(rig, [a, b], 0.3)
        v2 = a.verdict(c2)
        a.state, b.state = ST_HOLDLIGHT, ST_GRABBED
        b.flags = 0x03
        streaming(rig, [a, b], 0.7)
        res.check(ok and v2 and v2["name"] == "GRABBED", "hold %d" % n)

    new_hold(2)
    a.state = ST_SWING
    streaming(rig, [a, b], 0.2)
    sw = a.claim(CL["SWING"], b.seat, aux=0xC000, ref_frame=0x2a000)
    streaming(rig, [a, b], 0.3)
    v = a.verdict(sw)
    res.check(v and v["name"] == "THROWN" and v["b"] == 2 and v["d"] == 0xC000
              and v["x"] == 0x2a000, "SWING: THROWN how 2 (%s)" % v)
    new_hold(3)
    br = b.claim(CL["BREAK"], a.seat)
    streaming(rig, [a, b], 0.3)
    v = b.verdict(br)
    res.check(v and v["name"] == "THROWN" and v["b"] == 3 and v["seat"] == b.seat
              and v["a"] == a.seat, "BREAK: THROWN how 3 (%s)" % v)
    # an early BREAK is refused
    new_hold(4)
    # (the 0.7 s in new_hold already passed the minimum: make a fresh fast one)
    res.check(sb.grabbed_by == a.seat, "hold 4 is live")
    # the failsafe: the victim never reports St_Grabbed
    b.state = ST_WAIT
    streaming(rig, [a, b], 3.4)
    res.check(sb.grabbed_by is None and sa.holding is None,
              "failsafe: a hold the victim never shows is let go")
    forced = [e for e in a.events if e["name"] == "THROWN" and e["b"] == 4]
    res.check(len(forced) == 1 and forced[0]["c"] == 0, "THROWN how 4")
    # the holder leaves (QUIT)
    new_hold(5)
    a.claim(CL["QUIT"], 0xFF)
    streaming(rig, [b], 0.4)
    forced = [e for e in b.events if e["name"] == "THROWN" and e["b"] == 4]
    res.check(len(forced) == 2 and sb.grabbed_by is None,
              "holder QUIT: the victim is let go (%d)" % len(forced))
    res.check(ref.invariants() == [], "invariants %s" % ref.invariants())
    return res


GRAB_SPEED_MAX_T = 0x40000


def u_stomp(mod):
    res = Result("stomp (GRAB1, F9-d): STOMP onto a player, refusals, one per pair per 250 ms")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    a.pos, b.pos = [0, 60, 0], [20, 0, 0]
    a.flags = 0x01                 # airborne
    a.vy = -600
    a.state = 18                   # St_Jump
    b.state = ST_WAIT
    streaming(rig, [a, b], 0.3)

    def stomp(back=2):
        cid = a.claim(CL["STOMP"], b.seat, ref_frame=b.frame - back)
        streaming(rig, [a, b], 0.3)
        return a.verdict(cid)

    c1 = a.claim(CL["STOMP"], b.seat, ref_frame=b.frame - 2)
    c2 = a.claim(CL["STOMP"], b.seat, ref_frame=b.frame - 2)
    streaming(rig, [a, b], 0.4)
    v, v2 = a.verdict(c1), a.verdict(c2)
    res.check(v and v["name"] == "STOMPED" and v["seat"] == b.seat and v["a"] == a.seat,
              "STOMP: STOMPED (%s)" % v)
    res.check(ref.scores == [0] * 16, "no star moved")
    res.check(v2 and v2["name"] == "DENY" and v2["b"] == DENY["STATE"],
              "a second stomp inside 250 ms: DENY STATE (%s)" % v2)
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "STOMPED", "and one 0.4 s later is fine (%s)" % v)
    # must-FAIL controls
    streaming(rig, [a, b], 0.4)
    a.flags = 0x03                 # grounded
    streaming(rig, [a, b], 0.3)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"], "stomper on the ground: DENY STATE (%s)" % v)
    a.flags = 0x01
    a.vy = 300                     # rising
    streaming(rig, [a, b], 0.5)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"], "stomper rising: DENY STATE (%s)" % v)
    a.vy = -600
    a.pos = [0, 5, 0]              # not above (the ROM's 10 units)
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"], "not above: DENY RANGE (%s)" % v)
    a.pos = [0, 60, 0]
    b.pos = [400, 0, 0]
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"], "400 units away: DENY RANGE (%s)" % v)
    b.pos = [20, 0, 0]
    b.flags = 0x43                 # no control
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["INVULN"], "victim in no-control: DENY INVULN (%s)" % v)
    b.flags = 0x03
    a.state = 61                   # a ground pound is not a stomp
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"], "ground pound: DENY STATE (%s)" % v)
    a.state = 18
    streaming(rig, [a, b], 0.4)
    v = stomp()
    res.check(v and v["name"] == "STOMPED", "and a real one again (%s)" % v)
    sc = a.claim(CL["STOMP"], a.seat, ref_frame=a.frame)
    streaming(rig, [a, b], 0.3)
    res.check(a.verdict(sc)["b"] == DENY["SELF"], "stomp self: SELF")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_rematch(mod):
    res = Result("rematch in one session: linger, then a new match")
    rig, (a, b) = match(2, first_to_n=1, code="REMATCH1")
    ref = rig.ref("REMATCH1")
    inst = a.find("STAR_WAKE")[-1]["a"]
    a.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, [a, b], 0.2)
    a.claim(CL["GRAB"], inst)
    streaming(rig, [a, b], 0.3)
    res.check(ref.state == 3, "first match over")
    cr = a.ready(cap=2, digest=0xBEEF)
    streaming(rig, [a, b], 1.0)
    res.check(ref.state == 3 and a.verdict(cr) is None,
              "a new READY inside OVER_LINGER_MS waits (no verdict, no reset)")
    seq_before = ref.newest_seq
    streaming(rig, [a, b], 2.5)
    for c in (a, b):
        c.ready(cap=2, digest=0xBEEF)
    streaming(rig, [a, b], 0.5)
    res.check(ref.state in (1, 2), "after the linger the rematch starts")
    st = [e for e in b.events if e["name"] == "START"]
    res.check(len(st) == 2 and st[1]["seq"] > seq_before,
              "seq keeps running: b applies the new START in order (%s)"
              % [e["seq"] for e in st])
    return res


def u_hit_eat(mod):
    """FIELD4 F9-a: a HIT and an EAT between the same two players at the same
    instant used to both land (the victim of the pound was eaten AND its eater
    lost a star).  Exactly one of the two lands now, in every order, and the
    tongue wins a tie."""
    res = Result("hit vs eat: one outcome in every order, the tongue wins a tie")
    R = mod.referee
    ST_YOSHI, ST_POUND = R._IDX["St_YoshiPower"], R._IDX["St_GroundPound"]

    def pair(n=2):
        rig, cs = match(n)
        rig.no_pings = True
        ref = rig.ref("ARENA01")
        for c in cs:
            sc = ref.seats[c.seat]
            sc.rtts.clear()
            sc.rtts.append(125)
            sc.rtt_min = 125            # cap 87 ms, the referee's hold H
        a, b = cs[0], cs[1]
        # b holds a star, so a landed hit shows as a spill
        inst = a.find("STAR_WAKE")[-1]["a"]
        b.claim(CL["MARKER_BREAK"], inst)
        streaming(rig, cs, 0.2)
        b.claim(CL["GRAB"], inst)
        streaming(rig, cs, 0.3)
        streaming(rig, cs, 1.4)                     # (FIX9C: out of the star-get pose, which nothing hurts)
        a.pos, b.pos = [0, 0, 0], [100, 0, 0]
        a.state, b.state = ST_POUND, ST_YOSHI       # a pounds, b's tongue is out
        streaming(rig, cs, 0.3)
        return rig, cs, ref

    def outcome(a, b, ch, ce):
        vh, ve = a.verdict(ch), b.verdict(ce)
        return (vh["name"] if vh else None, ve["name"] if ve else None, vh, ve)

    for label, delta in (("EAT 10 ms after the HIT", 10),
                         ("EAT 10 ms before the HIT", -10),
                         ("EAT and HIT on the same stamp", 0)):
        rig, (a, b), ref = pair()
        t = rig.now
        ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2,
                     stamp=ref.ref_ms(t, a.seat) - 20)
        ce = b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2,
                     stamp=ref.ref_ms(t, b.seat) - 20 + delta)
        streaming(rig, [a, b], 0.5)
        hn, en, vh, ve = outcome(a, b, ch, ce)
        res.check(en == "EATEN" and hn == "DENY" and vh["b"] == DENY["INVULN"],
                  "%s: the tongue lands, the pound is refused INVULN (%s / %s)"
                  % (label, hn, en))
        res.check(ref.scores[b.seat] == 1 and not a.find("SPILL"),
                  "%s: the eater keeps its star, nothing spilled" % label)
        res.check(ref.seats[a.seat].eaten_by == b.seat, "%s: a is in b's "
                  "mouth" % label)
    # the EAT reaches the referee after the HIT was already decided (HIT stamp
    # first, EAT within TRADE_MS of it): the HIT stands and the EAT is refused,
    # so still exactly one lands (it used to ride the HIT-vs-HIT trade rule)
    rig, (a, b), ref = pair()
    t = rig.now
    ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2,
                 stamp=ref.ref_ms(t, a.seat))
    rig.advance(0.1)
    ce = b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2,
                 stamp=ref.ref_ms(t, b.seat) + 15)
    streaming(rig, [a, b], 0.5)
    hn, en, vh, ve = outcome(a, b, ch, ce)
    res.check(hn == "HIT" and en == "DENY" and ve["b"] == DENY["STATE"],
              "EAT arrives after the HIT landed: HIT stands, EAT STATE (%s / %s)"
              % (hn, en))
    res.check(ref.seats[a.seat].eaten_by is None and ref.scores[b.seat] == 0,
              "... a is not eaten, b lost its star")
    # well apart: the pound at one moment, the tongue 1.3 s later (the hit
    # immunity is over): both are ordinary events and both land
    streaming(rig, [a, b], 1.3)
    a.state, b.state = ST_POUND, ST_YOSHI
    streaming(rig, [a, b], 0.3)
    ce = b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2)
    streaming(rig, [a, b], 0.3)
    res.check(b.verdict(ce) and b.verdict(ce)["name"] == "EATEN",
              "a tongue 1.3 s after the hit it suffered: EATEN (no regression)")
    # a victim that holds a mouthful cannot be hit at all, and can again
    # after it spits
    rig, (a, b, c), ref = pair(3)
    c.pos = [100, 0, 100]
    b.state, b.flags = ST_YOSHI, 0x03
    streaming(rig, [a, b, c], 0.3)
    ce = b.claim(CL["EAT"], c.seat, ref_frame=c.frame - 2)
    streaming(rig, [a, b, c], 0.3)
    res.check(b.verdict(ce)["name"] == "EATEN", "b eats c")
    ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2)
    streaming(rig, [a, b, c], 0.3)
    v = a.verdict(ch)
    res.check(v["name"] == "DENY" and v["b"] == DENY["INVULN"],
              "a pound on an eater with a full mouth: INVULN (%s)" % v["name"])
    b.claim(CL["SPIT"], c.seat)
    streaming(rig, [a, b, c], 1.2)
    a.state = ST_POUND
    b.state = ST_WAIT
    streaming(rig, [a, b, c], 0.3)
    ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2)
    streaming(rig, [a, b, c], 0.3)
    res.check(a.verdict(ch)["name"] == "HIT", "after the spit the same pound "
              "lands (%s)" % a.verdict(ch)["name"])
    res.check(ref.invariants() == [], "invariants")
    return res


def u_star_flight(mod):
    """FIELD4 F9-b: a star in flight is judged at GRAB_R + FLIGHT_SLACK against
    the owner's newest SYNO; a star at rest, or one whose owner went quiet, at
    GRAB_R; a claim from far away is refused either way."""
    res = Result("star flight: a moving star gets slack, rest and cheats do not")
    R = mod.referee
    rig, (a, b) = match(2)
    mod = rig.mod               # (every Rig loads its own relay + referee)
    R = mod.referee
    ref = rig.ref("ARENA01")
    a.state = b.state = ST_WAIT
    a.pos = [0, 0, 0]

    def new_star(x, state):
        """a breaks the next marker and streams the star at (x, 0, 0)."""
        inst = a.find("STAR_WAKE")[-1]["a"]
        a.claim(CL["MARKER_BREAK"], inst)
        streaming(rig, [a, b], 0.3)
        rig.send(a.addr, S_SYNO.pack(b"SYNO", 1, 0, 1, inst, a.frame,
                                     x * FIX, 0, 0, 0, 0, 0, state, 0))
        return inst

    def grab(inst, at):
        b.pos = [at, 0, 0]
        streaming(rig, [a, b], 0.1)
        cid = b.claim(CL["GRAB"], inst)
        streaming(rig, [a, b], 0.3)
        return b.verdict(cid)
    # moving star, claimant 400 units off the referee's copy: inside the slack
    inst = new_star(0, 1)
    v = grab(inst, 400)
    res.check(v and v["name"] == "STAR_TAKEN", "moving star, 400 units: taken "
              "(%s)" % (v and v["name"]))
    # the cheat control: 600 units from a moving star is still RANGE
    inst = new_star(0, 1)
    v = grab(inst, 600)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"],
              "moving star, 600 units: RANGE (%s)" % (v and v["name"]))
    # the same 400 units from a star AT REST: RANGE (the plain radius)
    inst = new_star(0, 2)
    v = grab(inst, 400)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"],
              "resting star, 400 units: RANGE (%s)" % (v and v["name"]))
    v = grab(inst, 150)
    res.check(v and v["name"] == "STAR_TAKEN", "resting star, 150 units: taken")
    # a "moving" report older than FLIGHT_STALE_MS proves nothing
    inst = new_star(0, 1)
    streaming(rig, [a, b], getattr(R, "FLIGHT_STALE_MS", 1000) / 1000.0 + 0.3)
    v = grab(inst, 400)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"],
              "moving SYNO 1.3 s old, 400 units: RANGE (%s)" % (v and v["name"]))
    # the journal and the counters say what was refused and by how much
    lines = [m for m in mod.LOGGED if "deny GRAB seat %d" % b.seat in m
             and "RANGE" in m]
    res.check(lines and "dist=" in lines[0] and "limit=" in lines[0]
              and "flying=" in lines[0], "journal line names reason, distance, "
              "limit (%s)" % (lines[0] if lines else None))
    res.check(R.DENY_TOTALS.get((CL["GRAB"], DENY["RANGE"]), 0) >= 3,
              "DENY_TOTALS counts GRAB.RANGE (%s)" % R.deny_totals_text())
    tail = mod.RelayProtocol.stats_arena(rig.relay)
    res.check("arena_deny=" in tail and "GRAB.RANGE:" in tail,
              "the relay's stats line carries the per-reason counters")
    # a hurt claimant is still refused STATE, and the line says why
    inst = new_star(0, 2)
    b.state = ST_HURT
    streaming(rig, [a, b], 0.1)
    v = grab(inst, 50)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
              "touch while hurt: STATE (%s)" % (v and v["name"]))
    res.check(any("deny GRAB" in m and "STATE" in m and "state=St_Hurt" in m
                  for m in mod.LOGGED), "STATE line names the state")
    # the journal is rate-limited per session: a refused-claim spam at 20 / s
    # for 3 s writes about 4 lines a second (literal numbers, not the constant,
    # so raising DENY_LOG_PER_S cannot pass)
    n0 = len([m for m in mod.LOGGED if "deny GRAB" in m])
    state = {"n": rig.now}

    def spam():
        if rig.now >= state["n"]:
            state["n"] += 1 / 20.0
            b.claim(CL["GRAB"], inst)
            a.syns()
            b.syns()
    rig.advance(3.0, each=spam)
    n1 = len([m for m in mod.LOGGED if "deny GRAB" in m])
    res.check(R.DENY_LOG_PER_S == 4, "journal budget is 4 lines / s")
    res.check(1 <= n1 - n0 <= 4 * 4, "60 refused claims in 3 s wrote %d journal "
              "lines (at most 4 / s, 16 over the span)" % (n1 - n0))
    res.check(ref.invariants() == [], "invariants")
    return res


def u_eat_shield(mod):
    """REVREFFIX2 F1: a pending EAT claim shields its sender from a HIT only
    when the sender's own samples show the tongue out; a seat that stands in
    St_Wait and spams dummy EAT claims at the attacker blocks nothing."""
    res = Result("eat shield: dummy EAT claims block no honest hit, a real tongue still wins")
    R = mod.referee
    ST_POUND = R._IDX["St_GroundPound"]
    for rate in (10, 20):
        rig, (a, b) = match(2)
        ref = rig.ref("ARENA01")
        a.pos, b.pos = [0, 0, 0], [100, 0, 0]
        a.state, b.state = ST_POUND, ST_WAIT          # b has no tongue out
        streaming(rig, [a, b], 0.3)
        landed = blocked = other = 0
        # 12 honest pounds 1.3 s apart, a dummy EAT from b stamped within
        # TRADE_MS of each, plus a continuous dummy stream at `rate` / s
        state = {"n": rig.now, "f": rig.now}

        def dummies():
            if rig.now >= state["f"]:           # both consoles stream at 30 Hz
                state["f"] += 1.0 / 30.0
                a.syns()
                b.syns()
            if rig.now >= state["n"]:
                state["n"] += 1.0 / rate
                b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2)
        for i in range(12):
            t = rig.now
            ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2,
                         stamp=ref.ref_ms(t, a.seat) - 20)
            rig.advance(0.05, each=dummies)
            b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2,
                    stamp=ref.ref_ms(t, b.seat) - 10)
            rig.advance(1.3, each=dummies)
            v = a.verdict(ch)
            if v and v["name"] == "HIT":
                landed += 1
            elif v and v["name"] == "DENY" and v["b"] == DENY["INVULN"]:
                blocked += 1
            else:
                other += 1
        res.check(blocked == 0 and landed == 12,
                  "dummy EAT spam at %d / s: honest pounds landed %d blocked %d "
                  "other %d of 12" % (rate, landed, blocked, other))
    # a real tongue (samples in St_YoshiPower around the claim frame) still
    # wins the tie: covered by u_hit_eat; here the same shape with the tongue
    # up only in the samples of the claim frame
    ST_YOSHI = R._IDX["St_YoshiPower"]
    rig, (a, b) = match(2)
    rig.no_pings = True
    ref = rig.ref("ARENA01")
    for c in (a, b):
        sc = ref.seats[c.seat]
        sc.rtts.clear()
        sc.rtts.append(125)
        sc.rtt_min = 125
    a.pos, b.pos = [0, 0, 0], [100, 0, 0]
    a.state, b.state = ST_POUND, ST_YOSHI
    streaming(rig, [a, b], 0.3)
    t = rig.now
    ch = a.claim(CL["HIT"], b.seat, aux=0x1400 >> 5, ref_frame=b.frame - 2,
                 stamp=ref.ref_ms(t, a.seat) - 20)
    ce = b.claim(CL["EAT"], a.seat, ref_frame=a.frame - 2,
                 stamp=ref.ref_ms(t, b.seat) - 10)
    streaming(rig, [a, b], 0.5)
    res.check(b.verdict(ce)["name"] == "EATEN" and a.verdict(ch)["name"] == "DENY",
              "a real tongue wins the tie (%s / %s)" % (b.verdict(ce)["name"],
                                                         a.verdict(ch)["name"]))
    return res


def u_land_speed(mod):
    """FIELD4 F9-e: a wing-cap run into a landing carries 53-54 units a frame;
    St_Land is in the air class (limit 90) so it is no strike, while the same
    speed walking still is."""
    res = Result("speed: St_Land at 54/frame is not a strike, St_Walk is")
    R = mod.referee
    st_land = R._IDX["St_Land"]
    res.check(R.SPEED_CLASS[st_land] == "air", "St_Land is in the air class")
    rig, (a, b) = match(2)
    ref = rig.ref("ARENA01")
    sa = ref.seats[a.seat]
    state = {"n": rig.now}

    def each():
        if rig.now >= state["n"]:
            state["n"] += 1 / 30.0
            a.pos[0] += 54
            a.syns()
            b.syns()
    a.state = st_land
    s0 = sa.strikes_total
    rig.advance(0.5, each=each)
    res.check(sa.strikes_total == s0, "54/frame in St_Land: no strike (%d)"
              % (sa.strikes_total - s0))
    a.state = ST_WALK
    s0 = sa.strikes_total
    rig.advance(0.5, each=each)
    res.check(sa.strikes_total > s0, "54/frame in St_Walk: still a strike "
              "(%d)" % (sa.strikes_total - s0))
    a.state = st_land
    state["n"] = rig.now
    s0 = sa.strikes_total

    def each2():
        if rig.now >= state["n"]:
            state["n"] += 1 / 30.0
            a.pos[0] += 120
            a.syns()
            b.syns()
    rig.advance(0.5, each=each2)
    res.check(sa.strikes_total > s0, "120/frame in St_Land: a strike "
              "(the limit is raised, not removed)")
    return res



def u_caps(mod):
    res = Result("caps (VSREF3): CAP_TAKE contention, forged claims, CAP_DROP, respawn, snapshot")
    rf = mod.referee
    saved = dict(rf.CAP_MAPS)
    # three placed caps (slots 0, 1, 2): Mario look at (-200, 0), Wario look at (0, 300), Luigi at (200, 0)
    rf.CAP_MAPS[0] = [(0, -200, 0, 0, 0x004), (1, 0, 0, 300, 0x204), (2, 200, 0, 0, 0x104)]
    H0 = rf.Referee.cap_hash
    H = lambda x, z, slot=0: H0(slot, x, z)
    try:
        rig, (a, b) = match(2, mod=mod)
        ref = rig.ref("ARENA01")
        res.check(sorted(ref.caps) == [1, 2, 3], "the table makes ids 1..3 (%s)" % sorted(ref.caps))
        res.check([ref.caps[i].look for i in (1, 2, 3)] == [0, 2, 1], "looks come from param1")
        a.pos, b.pos = [-200, 0, 0], [-180, 0, 10]
        streaming(rig, [a, b], 0.2)

        def take(who, cid, aux=None, stamp=None, frame=None, wait=0.3):
            n = who.claim(CL["CAP_TAKE"], cid, aux=H(-200, 0) if aux is None else aux,
                          stamp=stamp, frame=frame)
            streaming(rig, [a, b], wait)
            return who.verdict(n)

        # ---- forged claims first: none of them touches the cap
        v = take(a, 0)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["BAD"], "cap id 0: DENY BAD (%s)" % v)
        v = take(a, 9)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["GONE"], "a cap that is not there (9): DENY GONE (%s)" % v)
        v = take(a, 250)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["GONE"], "cap 250 (never issued): DENY GONE (%s)" % v)
        v = take(a, 1, aux=H(-200, 0) ^ 0x55)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["BAD"], "wrong place hash: DENY BAD (%s)" % v)
        a.pos = [-200 + 700, 0, 0]
        streaming(rig, [a, b], 0.3)
        v = take(a, 1)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["RANGE"], "700 units from the cap: DENY RANGE (%s)" % v)
        a.pos = [-200, 0, 0]
        a.state = ST_HURT
        a.flags |= 0x20
        streaming(rig, [a, b], 0.3)
        v = take(a, 1)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"], "a hurt claimant: DENY STATE (%s)" % v)
        a.state = ST_WAIT
        a.flags &= ~0x20
        streaming(rig, [a, b], 0.3)
        res.check(not ref.caps[1].taken, "no forged claim took cap 1")
        res.check(not a.find("CAP_TAKEN") and not b.find("CAP_TAKEN"), "no CAP_TAKEN event yet")

        # ---- the honest take, then the loser
        v = take(a, 1)
        res.check(v and v["name"] == "CAP_TAKEN" and v["a"] == 1 and v["b"] == 0 and v["d"] == rf.CAP_RESPAWN_MS
                  and v["seat"] == a.seat, "a takes cap 1: CAP_TAKEN (%s)" % v)
        res.check(b.find("CAP_TAKEN", a=1, seat=a.seat), "the other console hears it too")
        v = take(b, 1)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["LOST"] and v["d"] == a.seat,
                  "b, after a: DENY LOST naming a (%s)" % v)
        streaming(rig, [a, b], 0.8)
        v = take(b, 1)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["GONE"], "a second later: DENY GONE (%s)" % v)
        # ---- it is back after CAP_RESPAWN_MS
        streaming(rig, [a, b], 1.0)
        v = take(b, 1)
        res.check(v and v["name"] == "CAP_TAKEN" and v["seat"] == b.seat, "back after the respawn window: b takes it (%s)" % v)

        # ---- FIX9C: a cap tongued in is claimed from the swallow / the put-on's no-control morph
        streaming(rig, [a, b], 1.7)
        a.pos = [-200, 0, 0]
        a.state = ST_NOCONTROL
        a.flags |= 0x40
        streaming(rig, [a, b], 0.3)
        v = take(a, 1)
        res.check(v and v["name"] == "CAP_TAKEN" and v["seat"] == a.seat,
                  "a claimant in the no-control put-on morph still takes the cap (%s)" % v)
        a.state = ST_WAIT
        a.flags &= ~0x40
        streaming(rig, [a, b], 0.3)

        # ---- a race: the earlier stamp wins even when its claim arrives second
        streaming(rig, [a, b], 2.2)
        sa = a.ref_now()
        ca = a.claim(CL["CAP_TAKE"], 1, aux=H(-200, 0), stamp=sa)
        cb = b.claim(CL["CAP_TAKE"], 1, aux=H(-200, 0), stamp=(b.ref_now() - 30) & 0xFFFFFFFF)
        streaming(rig, [a, b], 0.4)
        va, vb = a.verdict(ca), b.verdict(cb)
        res.check(vb and vb["name"] == "CAP_TAKEN" and va and va["name"] == "DENY" and va["b"] == DENY["LOST"]
                  and va["d"] == b.seat, "race: the earlier stamp (b) takes it, a is told LOST (%s / %s)" % (va, vb))
        n_taken = len([e for e in a.events if e["name"] == "CAP_TAKEN" and e["seq"] >= vb["seq"]])
        res.check(n_taken == 1, "exactly one CAP_TAKEN for that race (%d)" % n_taken)

        # ---- CAP_DROP: forged first
        a.look = 3
        streaming(rig, [a, b], 0.3)
        n = a.claim(CL["CAP_DROP"], 2, aux=0x1234)
        streaming(rig, [a, b], 0.3)
        v = a.verdict(n)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"], "CAP_DROP with no cap on: DENY STATE (%s)" % v)
        a.look = 2
        streaming(rig, [a, b], 0.3)
        n = a.claim(CL["CAP_DROP"], 2, aux=0x1234)
        streaming(rig, [a, b], 0.5)
        v = a.verdict(n)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
                  "CAP_DROP while not hurt (waited, then refused): DENY STATE (%s)" % v)
        n = a.claim(CL["CAP_DROP"], 3)
        streaming(rig, [a, b], 0.2)
        v = a.verdict(n)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["BAD"], "CAP_DROP look 3: DENY BAD (%s)" % v)
        a.state, a.flags = ST_HURT, a.flags | 0x20
        streaming(rig, [a, b], 0.1)
        n = a.claim(CL["CAP_DROP"], 1, aux=0x1234)
        streaming(rig, [a, b], 0.4)
        v = a.verdict(n)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["STATE"],
                  "CAP_DROP naming a cap that is not the one worn (look 1 vs 2): DENY STATE (%s)" % v)
        # the honest drop
        a.pos = [50, 0, 60]
        streaming(rig, [a, b], 0.2)
        n = a.claim(CL["CAP_DROP"], 2, aux=0x1234, ref_frame=5 | (7 << 16))
        streaming(rig, [a, b], 0.4)
        v = a.verdict(n)
        res.check(v and v["name"] == "CAP_DROPPED" and v["seat"] == a.seat and v["b"] == 2 and v["a"] >= 32
                  and v["d"] == 0x1234 and v["e"] == 5, "hurt wearer: CAP_DROPPED (%s)" % v)
        res.check(v and abs(v["x"] - 50 * FIX) <= 2 * FIX and abs(v["z"] - 60 * FIX) <= 2 * FIX
                  and v["y"] == 0x82000, "the pop point is the referee's own sample + 0x82000 (%s)" % v)
        drop_id = v["a"]
        res.check(b.find("CAP_DROPPED", a=drop_id), "the other console hears it")
        # a resend of the same claim gets the same verdict, never a second cap
        a.claim(CL["CAP_DROP"], 2, aux=0x1234, ref_frame=5 | (7 << 16), cid=n)
        streaming(rig, [a, b], 0.3)
        res.check(len(a.find("CAP_DROPPED")) == 1, "a resend makes no second cap")
        # a fresh claim straight after: refused (nothing worn, 1 s limit)
        n2 = a.claim(CL["CAP_DROP"], 2, aux=1)
        streaming(rig, [a, b], 0.4)
        v2 = a.verdict(n2)
        res.check(v2 and v2["name"] == "DENY" and v2["b"] == DENY["STATE"], "a second drop at once: DENY STATE (%s)" % v2)
        # the knocked-off cap: anyone near it may take it, once, and it never comes back
        a.state, a.flags, a.look = ST_WAIT, a.flags & ~0x20, 3
        b.pos = [60, 0, 60]
        streaming(rig, [a, b], 0.3)
        n3 = b.claim(CL["CAP_TAKE"], drop_id, aux=0)
        streaming(rig, [a, b], 0.3)
        v3 = b.verdict(n3)
        res.check(v3 and v3["name"] == "CAP_TAKEN" and v3["a"] == drop_id and v3["b"] == 2 and v3["d"] == 0,
                  "b takes the knocked-off cap: CAP_TAKEN, no respawn (%s)" % v3)
        n4 = a.claim(CL["CAP_TAKE"], drop_id, aux=0)
        streaming(rig, [a, b], 0.3)
        v4 = a.verdict(n4)
        res.check(v4 and v4["name"] == "DENY" and v4["b"] == DENY["GONE"], "a, afterwards: DENY GONE (%s)" % v4)
        res.check(drop_id not in ref.caps, "the knocked-off cap left the table")

        # ---- a knocked-off cap too far from the pop point, and one that ran out
        a.look = 2
        streaming(rig, [a, b], 0.3)
        a.state, a.flags = ST_HURT, a.flags | 0x20
        streaming(rig, [a, b], 0.2)
        streaming(rig, [a, b], 1.0)       # the one-per-second limit
        n = a.claim(CL["CAP_DROP"], 2, aux=7)
        streaming(rig, [a, b], 0.4)
        v = a.verdict(n)
        res.check(v and v["name"] == "CAP_DROPPED", "second honest drop: CAP_DROPPED (%s)" % v)
        id2 = v["a"] if v else 0
        a.state, a.flags, a.look = ST_WAIT, a.flags & ~0x20, 3
        b.pos = [2500, 0, 2500]
        streaming(rig, [a, b], 2.5)       # well past where a cap can slide to
        n5 = b.claim(CL["CAP_TAKE"], id2, aux=0)
        streaming(rig, [a, b], 0.3)
        v5 = b.verdict(n5)
        res.check(v5 and v5["name"] == "DENY" and v5["b"] == DENY["RANGE"], "far (3400 units) from the knocked-off cap: DENY RANGE (%s)" % v5)
        b.pos = [60, 0, 60]
        streaming(rig, [a, b], 5.0)       # it lies CAP_LIFE_MS (7 s) in all
        n6 = b.claim(CL["CAP_TAKE"], id2, aux=0)
        streaming(rig, [a, b], 0.3)
        v6 = b.verdict(n6)
        res.check(v6 and v6["name"] == "DENY" and v6["b"] == DENY["GONE"], "past its life: DENY GONE (%s)" % v6)

        # ---- a knocked-off cap slides down a slope: found 1000 units from its pop point, it is still takeable
        a.look = 2
        streaming(rig, [a, b], 0.3)
        a.state, a.flags = ST_HURT, a.flags | 0x20
        streaming(rig, [a, b], 1.2)
        n = a.claim(CL["CAP_DROP"], 2, aux=8)
        streaming(rig, [a, b], 0.4)
        v = a.verdict(n)
        res.check(v and v["name"] == "CAP_DROPPED", "third honest drop: CAP_DROPPED (%s)" % v)
        id3 = v["a"] if v else 0
        a.state, a.flags, a.look = ST_WAIT, a.flags & ~0x20, 3
        b.pos = [1100, 0, 60]
        streaming(rig, [a, b], 1.5)
        n7 = b.claim(CL["CAP_TAKE"], id3, aux=0)
        streaming(rig, [a, b], 0.3)
        v7 = b.verdict(n7)
        res.check(v7 and v7["name"] == "CAP_TAKEN" and v7["a"] == id3,
                  "1050 units from the pop point, 1.8 s later (it slid): CAP_TAKEN (%s)" % v7)
        b.pos = [60, 0, 60]
        streaming(rig, [a, b], 0.2)

        # ---- the snapshot a late console gets: a taken placed cap and a knocked-off one
        a.look = 2
        streaming(rig, [a, b], 0.3)
        a.state, a.flags = ST_HURT, a.flags | 0x20
        streaming(rig, [a, b], 1.2)
        n = a.claim(CL["CAP_DROP"], 2, aux=9)
        streaming(rig, [a, b], 0.4)
        a.state, a.flags, a.look = ST_WAIT, a.flags & ~0x20, 3
        b.pos = [-200, 0, 0]
        streaming(rig, [a, b], 0.3)
        n7 = b.claim(CL["CAP_TAKE"], 1, aux=H(-200, 0))
        streaming(rig, [a, b], 0.3)
        a.snap = []
        ref._snapshot(a.seat, ref.rt(rig.now))
        rig.deliver()
        kinds = sorted((e["a"], e["b"]) for e in a.snap if e["name"] == "CAP_STATE")
        res.check(len(kinds) == 2 and kinds[0][1] == 1 and kinds[1][1] == 2,
                  "snapshot: CAP_STATE for the taken cap and the knocked-off one (%s)" % kinds)
        res.check(all(e["seq"] == 0 for e in a.snap if e["name"] == "CAP_STATE"), "snapshot records are seq 0")

        # ---- a flood of claims: bounded, refused, no exception, invariants hold
        for i in range(300):
            a.claim(CL["CAP_TAKE"], (i % 40), aux=i & 0xFFFF)
            a.claim(CL["CAP_DROP"], i % 5, aux=i & 0xFFFF)
        streaming(rig, [a, b], 1.0)
        res.check(ref.invariants() == [], "invariants after a flood (%s)" % ref.invariants())
        res.check(len(ref.cap_wait) <= 16 * 16, "the CAP_DROP wait list is bounded (%d)" % len(ref.cap_wait))
    finally:
        rf.CAP_MAPS.clear()
        rf.CAP_MAPS.update(saved)
    return res


def u_star_tie(mod):
    res = Result("star tie (VSSTAR2): two touches a few ms apart, one award, the loser is denied")
    rig, (a, b) = match(2, code="STARTIE1")
    ref = rig.ref("STARTIE1")
    for trial in range(10):
        inst = a.find("STAR_WAKE")[-1]["a"]
        a.claim(CL["MARKER_BREAK"], inst)
        streaming(rig, [a, b], 0.3)
        tnow = rig.now
        gap = 1 + trial % 4              # b touched 1..4 ms after a
        sa0, sb0 = ref.scores[a.seat], ref.scores[b.seat]
        ca = a.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, a.seat) - 20)
        cb = b.claim(CL["GRAB"], inst, stamp=ref.ref_ms(tnow, b.seat) - 20 + gap)
        streaming(rig, [a, b], 0.4)
        va, vb = a.verdict(ca), b.verdict(cb)
        won = [v for v in (va, vb) if v and v["name"] == "STAR_TAKEN"]
        lost = [v for v in (va, vb) if v and v["name"] == "DENY"]
        res.check(len(won) == 1 and len(lost) == 1,
                  "trial %d: exactly one award and one deny (%s / %s)" % (trial, va, vb))
        res.check(ref.scores[a.seat] + ref.scores[b.seat] == sa0 + sb0 + 1,
                  "trial %d: the count moved by exactly one" % trial)
        res.check(lost and lost[0]["b"] == DENY["LOST"], "trial %d: loser denied LOST" % trial)
        # the loser also hears the award go to the other seat, so its console can
        # take the star away from view with no pose of its own
        loser, winner = (b, a) if va is not None and va["name"] == "STAR_TAKEN" else (a, b)
        ev = [e for e in loser.find("STAR_TAKEN") if e["a"] == inst]
        res.check(len(ev) == 1 and ev[0]["seat"] == winner.seat,
                  "trial %d: the loser is told seat %d took inst %d" % (trial, winner.seat, inst))
        # FIX9C immunity belongs to the winner only
        res.check(ref.seats[winner.seat].star_stamp is not None
                  and ref.seats[loser.seat].star_stamp != ref.seats[winner.seat].star_stamp,
                  "trial %d: pose immunity stamped for the winner only" % trial)
        ref.seats[a.seat].star_stamp = ref.seats[b.seat].star_stamp = None
    return res


# ================================================================ LUIGI
# Luigi Infection in arena mode (mode 2, lane REFLUIGI1). One test per rule
# Tango set on 11-09 (in/REFLUIGI1.md of run after093); arena_contract.py
# RULES has the rule text. The seed is SM64DS_REFEREE_SEED (12345 here), so
# the first Luigi is sorted(roster)[12345 % n].

LUIGI = 2
INF = dict(PICK=1, HIT=2, STOMP=3, TAG=4, HANDOFF=5, LATE_JOIN=6, RETURN=7)
OVER = dict(TIME_UP=1, FIRST_TO_N=2, LAST_STANDING=3, EVERYONE_LEFT=4,
            LUIGIS_WIN=5, SURVIVORS_WIN=6)


def lmatch(n=2, time_s=120, first_to_n=0, code="LUIGI001", mod=None,
           cap=None, env=None, run=True):
    """A rig with n consoles in a mode 2 match; run=True streams past the
    five-second countdown (the first Luigi is picked)."""
    rig = Rig(mod, env=env)
    cs = [Console(rig, code) for _ in range(n)]
    streaming(rig, cs, 0.6)
    for c in cs:
        c.ready(cap=cap or n, time_s=time_s, first_to_n=first_to_n,
                mode=LUIGI)
    streaming(rig, cs, 5.3 if run else 0.2)
    return rig, cs


def luigi_of(cs):
    """(the Luigi consoles, the survivor consoles) by the INFECTED stream
    the first console applied."""
    lu = set()
    for e in cs[0].events:
        if e["name"] == "INFECTED":
            lu.add(e["seat"])
        if e["name"] == "LEAVE":
            lu.discard(e["seat"])
    return ([c for c in cs if c.seat in lu],
            [c for c in cs if c.seat not in lu])


def _mask(seats):
    m = 0
    for s in seats:
        m |= 1 << s
    return m


def u_luigi_pick(mod):
    res = Result("luigi: mode 2 READY, 5 s countdown, first Luigi from the "
                 "seed at T0, deterministic")
    rig, cs = lmatch(3, run=False)
    ref = rig.ref("LUIGI001")
    a = cs[0]
    acc = [c.verdict(c.cid) for c in cs]
    res.check(all(v and v["name"] == "ACCEPTED" for v in acc),
              "mode 2 READY accepted by every seat (%s)" % acc)
    res.check(ref.state == 1, "countdown")
    left = ref.t0 - ref.rt(rig.now)
    res.check(4700 <= left <= 5000, "countdown is five seconds (%d ms left)"
              % left)
    st = a.find("START")
    res.check(st and (st[0]["z"] >> 8) & 0xFF == LUIGI,
              "START carries the rules word with mode 2")
    # the countdown: nothing is decided, nobody is frozen by the referee
    a.pos = [0, 0, 0]
    cs[1].pos = [50, 0, 0]
    a.state = ST_PUNCH
    streaming(rig, cs, 0.5)
    hit = a.claim(CL["HIT"], cs[1].seat, aux=2, ref_frame=cs[1].frame - 2)
    grab = a.claim(CL["GRAB"], 1)
    tag = a.claim(CL["TAG"], cs[1].seat, ref_frame=cs[1].frame - 2)
    fr = cs[1].frame
    cs[1].pos = [400, 0, 0]
    streaming(rig, cs, 0.3)
    for name, cid in (("HIT", hit), ("GRAB", grab), ("TAG", tag)):
        v = a.verdict(cid)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY["STALE"],
                  "%s in the countdown: DENY STALE (%s)" % (name, v))
    moved = [s for s in ref.seats[cs[1].seat].hist if s.frame > fr]
    res.check(moved and moved[-1].x == 400 * FIX,
              "a body moving in the countdown is taken as it is (%s)"
              % (moved[-1].x // FIX if moved else None))
    res.check(not a.find("INFECTED"), "no Luigi before T0")
    streaming(rig, cs, 4.6)
    res.check(ref.state == 2, "running after five seconds")
    inf = a.find("INFECTED")
    want = sorted(c.seat for c in cs)[12345 % 3]
    res.check(len(inf) == 1 and inf[0]["seat"] == want
              and inf[0]["a"] == 0xFF and inf[0]["b"] == INF["PICK"]
              and inf[0]["c"] == 0, "one INFECTED, the pick: seat %d (%s)"
              % (want, inf))
    res.check(inf and inf[0]["d"] == 1 << want and inf[0]["e"] == 2,
              "mask and survivors left")
    res.check(inf and inf[0]["t"] == ref.seat_wire(a.seat, ref.t0),
              "the pick is stamped T0")
    res.check(all([(e["seat"], e["b"]) for e in c.find("INFECTED")]
                  == [(want, INF["PICK"])] for c in cs),
              "every console hears the same pick")
    # deterministic from the seed and the roster, every n and several seeds
    for seed in (1, 2, 3, 7, 12345, 0xFFFFFFFF):
        for n in (2, 3, 4):
            m = load_relay({"SM64DS_REFEREE_SEED": str(seed)})
            picks = []
            for _ in range(2):
                r2, c2 = lmatch(n, mod=m, code="PICK%04d" % n)
                inf2 = c2[0].find("INFECTED")
                picks.append(inf2[0]["seat"] if inf2 else None)
            seats = sorted(c.seat for c in c2)
            res.check(picks[0] == picks[1] == seats[seed % n],
                      "seed %d, %d players: pick %s == seats[seed %% n] %d"
                      % (seed, n, picks, seats[seed % n]))
    return res


def u_luigi_immune(mod):
    res = Result("luigi: a Luigi is immune to hit, stomp, eat, grab, tag")
    rig, cs = lmatch(3, code="LUIGI002")
    ref = rig.ref("LUIGI002")
    (lu,), surv = luigi_of(cs)
    s1, s2 = surv
    lu.pos, s1.pos, s2.pos = [0, 0, 0], [60, 0, 0], [3000, 0, 0]
    s1.state = ST_PUNCH
    s1.look = 2                     # a Wario, so CATCH is shaped right
    streaming(rig, cs, 0.4)

    def deny(c, kind, target, reason, what, **kw):
        cid = c.claim(CL[kind], target, ref_frame=kw.get("rf", 0), aux=2)
        streaming(rig, cs, 0.3)
        v = c.verdict(cid)
        res.check(v and v["name"] == "DENY" and v["b"] == DENY[reason],
                  "%s: DENY %s (%s)" % (what, reason, v))

    rf = lambda: lu.frame - 2
    deny(s1, "HIT", lu.seat, "INVULN", "survivor HIT on the Luigi", rf=rf())
    s1.state, s1.flags, s1.vy, s1.pos = 18, 0x01, -600, [20, 60, 0]
    streaming(rig, cs, 0.3)
    deny(s1, "STOMP", lu.seat, "INVULN", "survivor STOMP on the Luigi",
         rf=rf())
    s1.flags, s1.vy, s1.pos, s1.state = 0x03, 0, [60, 0, 0], 3
    streaming(rig, cs, 0.3)
    deny(s1, "EAT", lu.seat, "INVULN", "survivor EAT on the Luigi", rf=rf())
    s1.state = ST_PUNCH
    lu.state = ST_HURT
    streaming(rig, cs, 0.3)
    deny(s1, "CATCH", lu.seat, "INVULN", "survivor CATCH (Wario) on the "
         "Luigi", rf=rf())
    lu.state = ST_WAIT
    deny(s1, "TAG", lu.seat, "INVULN", "survivor TAG on the Luigi", rf=rf())
    res.check(ref.scores == [0] * 16 and not cs[0].find("HIT")
              and not cs[0].find("STOMPED") and not cs[0].find("EATEN")
              and not cs[0].find("GRABBED"), "nothing landed on the Luigi")
    # the Luigi's own team moves: no eating, no catching
    lu.state = 3
    streaming(rig, cs, 0.3)
    deny(lu, "EAT", s1.seat, "TEAM", "the Luigi's EAT", rf=s1.frame - 2)
    lu.state, lu.look = ST_PUNCH, 2
    s1.state = ST_HURT
    streaming(rig, cs, 0.3)
    deny(lu, "CATCH", s1.seat, "TEAM", "the Luigi's CATCH", rf=s1.frame - 2)
    lu.look, s1.state = 1, ST_WAIT
    # Luigi on Luigi: tag s1 first, then the two Luigis hit each other
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(lu.verdict(cid)["name"] == "INFECTED", "s1 tagged")
    streaming(rig, cs, 0.3)
    deny(lu, "HIT", s1.seat, "INVULN", "Luigi HIT on a Luigi",
         rf=s1.frame - 2)
    deny(s1, "TAG", lu.seat, "INVULN", "Luigi TAG on a Luigi", rf=rf())
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_infect(mod):
    res = Result("luigi: a Luigi's hit / stomp / tag infects, no hurt, no "
                 "star; survivor vs survivor unchanged")
    rig, cs = lmatch(4, code="LUIGI003")
    ref = rig.ref("LUIGI003")
    (lu,), (s1, s2, s3) = luigi_of(cs)
    a0 = cs[0]
    # s1 gets a star (stars still spill between survivors)
    inst = a0.find("STAR_WAKE")[-1]["a"]
    s1.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, cs, 0.2)
    s1.claim(CL["GRAB"], inst)
    streaming(rig, cs, 2.0)                      # past the star-get pose
    res.check(ref.scores[s1.seat] == 1, "s1 has a star")
    # survivor vs survivor: the classic HIT and SPILL
    s2.pos, s1.pos = [0, 0, 0], [100, 0, 0]
    s2.state = ST_PUNCH
    streaming(rig, cs, 0.3)
    cid = s2.claim(CL["HIT"], s1.seat, aux=2, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    v = s2.verdict(cid)
    res.check(v and v["name"] == "HIT" and a0.find("SPILL", seat=s1.seat),
              "survivor HIT on a survivor: HIT + SPILL as classic (%s)" % v)
    s2.state = ST_WAIT
    # s3 gets a star, then the Luigi punches it: INFECTED, no spill
    inst = [e for e in a0.find("STAR_WAKE")][-1]["a"]
    s3.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, cs, 0.2)
    s3.claim(CL["GRAB"], inst)
    streaming(rig, cs, 0.3)
    res.check(ref.scores[s3.seat] == 1, "s3 has a star")
    lu.pos, s3.pos = [5000, 0, 0], [5100, 0, 0]
    lu.state = ST_PUNCH
    s3.invuln = 20                               # Classic's guard ignores it
    streaming(rig, cs, 0.3)
    n_spill = len(a0.find("SPILL"))
    cid = lu.claim(CL["HIT"], s3.seat, aux=2, ref_frame=s3.frame - 2)
    streaming(rig, cs, 0.3)
    v = lu.verdict(cid)
    res.check(v and v["name"] == "INFECTED" and v["seat"] == s3.seat
              and v["a"] == lu.seat and v["b"] == INF["HIT"]
              and v["c"] == cid, "Luigi HIT: INFECTED by the Luigi (%s)" % v)
    res.check(v and v["d"] == _mask([lu.seat, s3.seat]) and v["e"] == 2,
              "mask and survivors left")
    res.check(v and abs(v["x"] - 5000 * FIX) < FIX, "carries the tagger pos")
    res.check(len(a0.find("SPILL")) == n_spill and ref.scores[s3.seat] == 1,
              "no star left s3 (stars stay, decide nothing)")
    res.check(not a0.find("HIT", seat=s3.seat), "no HIT event: no hurt")
    res.check(s3.seat in ref.luigis, "s3 is on the team")
    s3.invuln = 0
    # out of reach and wrong state are still refused
    lu.pos, s2.pos = [0, 0, 8000], [900, 0, 8000]
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["HIT"], s2.seat, aux=2, ref_frame=s2.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(lu.verdict(cid)["b"] == DENY["RANGE"], "Luigi HIT from 900 "
              "units: DENY RANGE")
    lu.state = ST_WAIT
    s2.pos = [60, 0, 8000]
    streaming(rig, cs, 0.6)
    cid = lu.claim(CL["HIT"], s2.seat, aux=2, ref_frame=s2.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(lu.verdict(cid)["b"] == DENY["STATE"], "Luigi HIT while not "
              "attacking: DENY STATE (use TAG for a touch)")
    # STOMP infects
    lu.state, lu.flags, lu.vy = 18, 0x01, -600
    lu.pos, s2.pos = [0, 60, 8000], [20, 0, 8000]
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["STOMP"], s2.seat, ref_frame=s2.frame - 2)
    streaming(rig, cs, 0.3)
    v = lu.verdict(cid)
    res.check(v and v["name"] == "INFECTED" and v["b"] == INF["STOMP"]
              and v["seat"] == s2.seat, "Luigi STOMP: INFECTED (%s)" % v)
    res.check(not a0.find("STOMPED", seat=s2.seat), "no squash event")
    # TAG infects (body contact), refused out of contact reach
    lu.state, lu.flags, lu.vy = ST_WALK, 0x03, 0
    lu.pos, s1.pos = [0, 0, -8000], [200, 0, -8000]
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(lu.verdict(cid)["b"] == DENY["RANGE"], "TAG from 200 units: "
              "DENY RANGE")
    s1.pos = [70, 0, -8000]
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    v = lu.verdict(cid)
    res.check(v and v["name"] == "INFECTED" and v["b"] == INF["TAG"],
              "Luigi TAG: INFECTED (%s)" % v)
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_tag_rules(mod):
    res = Result("luigi: TAG only from a Luigi, only in mode 2; a tag frees "
                 "a mouthful first")
    rig, cs = lmatch(3, code="LUIGI004")
    (lu,), (s1, s2) = luigi_of(cs)
    s1.pos, s2.pos = [0, 0, 0], [60, 0, 0]
    streaming(rig, cs, 0.3)
    cid = s1.claim(CL["TAG"], s2.seat, ref_frame=s2.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(s1.verdict(cid)["b"] == DENY["TEAM"], "survivor TAG: DENY TEAM")
    # s1 eats s2, then the Luigi tags the eater: RELEASED (forced) first
    s1.state = 3                                  # St_YoshiPower
    streaming(rig, cs, 0.3)
    cid = s1.claim(CL["EAT"], s2.seat, ref_frame=s2.frame - 2)
    streaming(rig, cs, 0.3)
    res.check(s1.verdict(cid)["name"] == "EATEN", "s1 ate s2")
    lu.pos = [0, 0, 70]
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    v = lu.verdict(cid)
    rel = cs[0].find("RELEASED", seat=s2.seat, a=s1.seat, b=4)
    res.check(v and v["name"] == "INFECTED" and rel
              and rel[0]["seq"] < v["seq"], "eater tagged: RELEASED b 4, "
              "then INFECTED (%s / %s)" % (rel, v))
    # a survivor in a mouth cannot be tagged
    rig2, cs2 = lmatch(3, code="LUIGI005")
    (l2,), (t1, t2) = luigi_of(cs2)
    t1.pos, t2.pos, l2.pos = [0, 0, 0], [60, 0, 0], [60, 0, 60]
    t1.state = 3
    streaming(rig2, cs2, 0.3)
    t1.claim(CL["EAT"], t2.seat, ref_frame=t2.frame - 2)
    streaming(rig2, cs2, 0.3)
    cid = l2.claim(CL["TAG"], t2.seat, ref_frame=t2.frame - 2)
    streaming(rig2, cs2, 0.3)
    res.check(l2.verdict(cid)["b"] == DENY["INVULN"], "TAG on a survivor in "
              "a mouth: DENY INVULN")
    # mode 0: TAG is not a claim there
    rig3, (a, b) = match(2, code="LUIGI006")
    cid = a.claim(CL["TAG"], b.seat, ref_frame=b.frame - 2)
    streaming(rig3, [a, b], 0.3)
    v = a.verdict(cid)
    ra = rig3.ref("LUIGI006").seats[a.seat]
    res.check(v is None and ra.strikes_total == 1, "TAG in mode 0: an unknown "
              "kind as before mode 2 (no verdict, one strike) (%s, %d)"
              % (v, ra.strikes_total))
    res.check(not a.find("INFECTED"), "mode 0 never infects")
    return res


def u_luigi_caps(mod):
    res = Result("luigi: no caps for Luigis, no Luigi cap for survivors")
    rf = mod.referee
    rig, cs = lmatch(2, code="LUIGI007")
    ref = rig.ref("LUIGI007")
    (lu,), (sv,) = luigi_of(cs)
    caps = dict((slot, (x, y, z, (p >> 8) & 0xF))
                for slot, x, y, z, p in rf.CAP_MAPS[0])

    def take(c, slot):
        x, y, z, look = caps[slot]
        c.pos = [x, y, z]
        streaming(rig, cs, 0.3)
        cid = c.claim(CL["CAP_TAKE"], slot + 1,
                      aux=rf.Referee.cap_hash(slot, x, z))
        streaming(rig, cs, 0.3)
        return c.verdict(cid)

    mario = [k for k, v in caps.items() if v[3] == 0][0]
    luigi = [k for k, v in caps.items() if v[3] == 1][0]
    wario = [k for k, v in caps.items() if v[3] == 2][0]
    v = take(lu, mario)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["TEAM"],
              "Luigi takes the Mario cap: DENY TEAM (%s)" % v)
    v = take(lu, luigi)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["TEAM"],
              "Luigi takes the Luigi cap: DENY TEAM (%s)" % v)
    v = take(sv, luigi)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["TEAM"],
              "survivor takes the Luigi cap: DENY TEAM (%s)" % v)
    v = take(sv, wario)
    res.check(v and v["name"] == "CAP_TAKEN", "survivor takes the Wario cap: "
              "CAP_TAKEN (%s)" % v)
    lu.state = ST_HURT
    lu.look = 1
    streaming(rig, cs, 0.3)
    cid = lu.claim(CL["CAP_DROP"], 1, aux=0, ref_frame=0)
    streaming(rig, cs, 0.3)
    v = lu.verdict(cid)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["TEAM"],
              "Luigi CAP_DROP: DENY TEAM, no Luigi cap in the arena (%s)" % v)
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_wins(mod):
    res = Result("luigi: everyone infected = Luigis win; clock out with a "
                 "survivor = survivors win; stars decide nothing")
    rig, cs = lmatch(3, first_to_n=1, code="LUIGI008")
    ref = rig.ref("LUIGI008")
    (lu,), (s1, s2) = luigi_of(cs)
    a0 = cs[0]
    inst = a0.find("STAR_WAKE")[-1]["a"]
    s1.claim(CL["MARKER_BREAK"], inst)
    streaming(rig, cs, 0.2)
    s1.claim(CL["GRAB"], inst)
    streaming(rig, cs, 0.3)
    res.check(ref.scores[s1.seat] == 1 and ref.state == 2,
              "first_to_n 1 reached by a star: the match goes on")
    for s in (s1, s2):
        lu.pos, s.pos = [0, 0, 0], [60, 0, 0]
        streaming(rig, cs, 0.3)
        lu.claim(CL["TAG"], s.seat, ref_frame=s.frame - 2)
        streaming(rig, cs, 0.3)
        s.pos = [5000 + 1000 * s.seat, 0, 0]
    ov = a0.find("MATCH_OVER")
    res.check(ref.state == 3 and len(ov) == 1 and ov[0]["a"]
              == OVER["LUIGIS_WIN"], "all infected: MATCH_OVER LUIGIS_WIN "
              "(%s)" % ov)
    res.check(ov and ov[0]["c"] == _mask(c.seat for c in cs)
              and ov[0]["b"] == 3, "winners = every Luigi (the star "
              "leader is not singled out)")
    # time up with a survivor left
    rig, cs = lmatch(3, time_s=8, code="LUIGI009")
    ref = rig.ref("LUIGI009")
    (lu,), (s1, s2) = luigi_of(cs)
    lu.pos, s1.pos, s2.pos = [0, 0, 0], [60, 0, 0], [4000, 0, 0]
    streaming(rig, cs, 0.3)
    lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 4.0)
    res.check(ref.state == 2, "still running at 4.6 s of 8")
    streaming(rig, cs, 4.0)
    ov = cs[0].find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["SURVIVORS_WIN"]
              and ov[0]["c"] == _mask([s2.seat]) and ov[0]["b"] == 1,
              "clock out with s2 alive: SURVIVORS_WIN, winner s2 (%s)" % ov)
    res.check(ov and ov[0]["t"] == ref.seat_wire(cs[0].seat, ref.t0 + 8000),
              "the end is T0 + the clock (the countdown is not on it)")
    return res


def u_luigi_leave(mod):
    res = Result("luigi: the only Luigi leaves = new random Luigi; the last "
                 "survivor leaves = Luigis win")
    rig, cs = lmatch(4, code="LUIGI010")
    ref = rig.ref("LUIGI010")
    (lu,), surv = luigi_of(cs)
    lu.claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(lu)
    left = [c for c in cs if c is not lu]
    streaming(rig, left, 0.3)
    a0 = left[0]
    ho = a0.find("INFECTED", b=INF["HANDOFF"])
    want = sorted(c.seat for c in left)[12345 % 3]
    res.check(len(ho) == 1 and ho[0]["seat"] == want and ho[0]["a"] == 0xFF
              and ho[0]["d"] == 1 << want, "the only Luigi quit: HANDOFF to "
              "seat %d by the pick rule (%s)" % (want, ho))
    lv = a0.find("LEAVE", seat=lu.seat)
    res.check(lv and ho and lv[0]["seq"] < ho[0]["seq"], "LEAVE first, then "
              "the new Luigi")
    res.check(ref.state == 2 and ref.luigis == {want}, "the match goes on")
    # a silent Luigi (no QUIT) is the same
    rig, cs = lmatch(3, code="LUIGI011")
    ref = rig.ref("LUIGI011")
    (lu,), surv = luigi_of(cs)
    streaming(rig, surv, 3.6)                    # lu goes quiet
    ho = surv[0].find("INFECTED", b=INF["HANDOFF"])
    res.check(len(ho) == 1 and ref.state == 2, "a Luigi gone silent: "
              "HANDOFF (%s)" % ho)
    # the last survivor leaves: Luigis win
    rig, cs = lmatch(3, code="LUIGI012")
    ref = rig.ref("LUIGI012")
    (lu,), (s1, s2) = luigi_of(cs)
    lu.pos, s1.pos = [0, 0, 0], [60, 0, 0]
    streaming(rig, cs, 0.3)
    lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    s2.claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(s2)
    streaming(rig, [lu, s1], 0.3)
    ov = lu.find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["LUIGIS_WIN"]
              and ov[0]["c"] == _mask([lu.seat, s1.seat]),
              "the last survivor quit: LUIGIS_WIN (%s)" % ov)
    # two players, the Luigi leaves: the one left wins (no lone hand-off)
    rig, cs = lmatch(2, code="LUIGI013")
    (lu,), (sv,) = luigi_of(cs)
    lu.claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(lu)
    streaming(rig, [sv], 0.3)
    ov = sv.find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["SURVIVORS_WIN"]
              and ov[0]["c"] == 1 << sv.seat
              and not sv.find("INFECTED", b=INF["HANDOFF"]),
              "2 players, the Luigi quit: SURVIVORS_WIN for the one left (%s)"
              % ov)
    # two players, the survivor leaves: Luigis win
    rig, cs = lmatch(2, code="LUIGI014")
    (lu,), (sv,) = luigi_of(cs)
    sv.claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(sv)
    streaming(rig, [lu], 0.3)
    ov = lu.find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["LUIGIS_WIN"]
              and ov[0]["c"] == 1 << lu.seat, "2 players, the survivor quit: "
              "LUIGIS_WIN (%s)" % ov)
    # a leave in the countdown: nobody is a Luigi yet
    rig, cs = lmatch(3, code="LUIGI015", run=False)
    ref = rig.ref("LUIGI015")
    cs[2].claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(cs[2])
    streaming(rig, cs[:2], 5.2)
    inf = cs[0].find("INFECTED")
    want = sorted(c.seat for c in cs[:2])[12345 % 2]
    res.check(len(inf) == 1 and inf[0]["seat"] == want and inf[0]["b"]
              == INF["PICK"], "a quit in the countdown: the pick is over "
              "the two left (%s)" % inf)
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_join(mod):
    res = Result("luigi: a late joiner is a Luigi; before the pick it is in "
                 "the pick; the SNAPSHOT carries the team")
    # a rejoin mid-match: QUIT then READY again
    rig, cs = lmatch(3, code="LUIGI016")
    ref = rig.ref("LUIGI016")
    (lu,), (s1, s2) = luigi_of(cs)
    s2.claim(CL["QUIT"], 0xFF)
    streaming(rig, cs, 0.5)
    res.check(ref.state == 2 and s2.seat not in ref.roster, "s2 left, two "
              "play on")
    s2.snap = []
    s2.ready(cap=3, mode=LUIGI)
    streaming(rig, cs, 0.5)
    lj = cs[0].find("INFECTED", seat=s2.seat, b=INF["LATE_JOIN"])
    res.check(len(lj) == 1 and lj[0]["a"] == 0xFF and s2.seat in ref.luigis,
              "back mid-match: INFECTED LATE_JOIN (%s)" % lj)
    js = cs[0].find("JOIN", seat=s2.seat, a=1)
    res.check(js and lj and js[-1]["seq"] < lj[0]["seq"], "JOIN first")
    ts = [e for e in s2.snap if e["name"] == "TEAM_STATE"]
    res.check(ts and ts[0]["a"] == 1 and ts[0]["c"] == _mask([lu.seat,
                                                              s2.seat])
              and ts[0]["d"] == _mask(c.seat for c in cs),
              "its SNAPSHOT: TEAM_STATE with the Luigi mask (%s)" % ts)
    # a fresh console: capacity 3, two start after 20 s, the third comes
    # in during the countdown (in the pick) or after it (a Luigi)
    for late, code in ((False, "LUIGI017"), (True, "LUIGI018")):
        rig = Rig(mod)
        cs = [Console(rig, code) for _ in range(2)]
        streaming(rig, cs, 0.6)
        for c in cs:
            c.ready(cap=3, mode=LUIGI)
        streaming(rig, cs, 20.3)
        ref = rig.ref(code)
        res.check(ref.state == 1, "%s: two started, countdown" % code)
        if late:
            streaming(rig, cs, 5.2)
        c3 = Console(rig, code)
        cs.append(c3)
        streaming(rig, cs, 0.6)
        c3.ready(cap=3, mode=LUIGI)
        streaming(rig, cs, 0.4 if late else 5.2)
        inf = cs[0].find("INFECTED")
        if late:
            res.check([(e["seat"], e["b"]) for e in inf] ==
                      [(sorted(c.seat for c in cs[:2])[12345 % 2],
                        INF["PICK"]), (c3.seat, INF["LATE_JOIN"])],
                      "joined after the pick: a Luigi (%s)" % inf)
            ts = [e for e in c3.snap if e["name"] == "TEAM_STATE"]
            res.check(ts and ts[-1]["c"] & (1 << c3.seat), "the new console's "
                      "SNAPSHOT has itself on the team")
        else:
            res.check([(e["seat"], e["b"]) for e in inf] ==
                      [(sorted(c.seat for c in cs)[12345 % 3], INF["PICK"])],
                      "joined in the countdown: one pick over all three (%s)"
                      % inf)
        res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_16(mod):
    res = Result("luigi: 16 players, one pick, fifteen tags, Luigis win")
    rig, cs = lmatch(16, code="LUIGI019")
    ref = rig.ref("LUIGI019")
    res.check(len(ref.roster) == 16, "16 in the match")
    (lu,), surv = luigi_of(cs)
    res.check(lu.seat == sorted(c.seat for c in cs)[12345 % 16],
              "the pick is seats[seed % 16]")
    for i, c in enumerate(cs):
        c.pos = [2000 * i, 0, 0]
    streaming(rig, cs, 0.3)
    n = 0
    for s in surv:
        lu.pos = [s.pos[0] + 60, 0, 0]
        streaming(rig, cs, 0.2)
        cid = lu.claim(CL["TAG"], s.seat, ref_frame=s.frame - 2)
        streaming(rig, cs, 0.25)
        v = lu.verdict(cid)
        n += 1 if v and v["name"] == "INFECTED" else 0
    res.check(n == 15, "fifteen tags, fifteen INFECTED (%d)" % n)
    ov = cs[0].find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["LUIGIS_WIN"]
              and ov[0]["c"] == 0xFFFF and ov[0]["b"] == 16,
              "LUIGIS_WIN, all sixteen (%s)" % ov)
    last = cs[0].find("INFECTED")[-1]
    res.check(last["d"] == 0xFFFF and last["e"] == 0, "the last INFECTED: "
              "mask 0xffff, 0 survivors")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_mode0(mod):
    res = Result("luigi: mode 0 untouched (3 s countdown, no team events, "
                 "TIME_UP, mode 1 still refused)")
    rig, (a, b) = match(2, time_s=5, code="LUIGI020")
    ref = rig.ref("LUIGI020")
    res.check(not ref.luigis and not a.find("INFECTED"), "no Luigi in mode 0")
    st = a.find("START")[0]
    res.check((st["z"] >> 8) & 0xFF == 0, "rules word mode 0")
    a.snap = []
    ref._snapshot(a.seat, ref.rt(rig.now))
    rig.deliver()
    res.check(a.snap and not [e for e in a.snap if e["name"] == "TEAM_STATE"],
              "a mode 0 SNAPSHOT has no TEAM_STATE")
    streaming(rig, [a, b], 5.0)
    ov = a.find("MATCH_OVER")
    res.check(ov and ov[0]["a"] == OVER["TIME_UP"], "clock out: TIME_UP (%s)"
              % ov)
    rig = Rig(mod)
    c, c2 = Console(rig, "LUIGI021"), Console(rig, "LUIGI021")
    streaming(rig, [c, c2], 0.6)
    c.ready(cap=2)
    c2.ready(cap=2)
    streaming(rig, [c, c2], 0.2)
    ref = rig.ref("LUIGI021")
    left = ref.t0 - ref.rt(rig.now)
    res.check(2700 <= left <= 3000, "mode 0 countdown still 3 s (%d)" % left)
    rig3 = Rig(mod)
    k = Console(rig3, "LUIGI022")
    streaming(rig3, [k], 0.6)
    cid = k.ready(cap=2, mode=1)
    streaming(rig3, [k], 0.3)
    v = k.verdict(cid)
    res.check(v and v["b"] == DENY["UNSUPPORTED"], "mode 1 (King) still "
              "UNSUPPORTED (%s)" % v)
    cid = k.ready(cap=2, mode=3)
    streaming(rig3, [k], 0.3)
    res.check(k.verdict(cid)["b"] == DENY["UNSUPPORTED"], "mode 3: "
              "UNSUPPORTED")
    return res


def u_luigi_clock_out(mod):
    res = Result("luigi: after the clock runs out a leave picks no Luigi and "
                 "nobody joins (Classic's frozen end)")
    rig, cs = lmatch(4, time_s=8, code="LUIGI030")
    ref = rig.ref("LUIGI030")
    (lu,), (s1, s2, s3) = luigi_of(cs)
    s3.claim(CL["QUIT"], 0xFF)                  # a survivor leaves early
    streaming(rig, cs, 0.3)
    live = [lu, s1, s2, s3]
    while ref.rt(rig.now) < ref.t0 + ref.match_ms + 20:
        streaming(rig, live, 0.01)
    res.check(ref.state == 2, "clock out, the referee is holding (state %d)"
              % ref.state)
    lu.claim(CL["QUIT"], 0xFF)                  # the only Luigi leaves now
    s3.ready(cap=4, mode=LUIGI)                 # and s3 tries to come back
    streaming(rig, [s1, s2, s3], 0.5)
    res.check(not s1.find("INFECTED", b=INF["HANDOFF"]), "no HANDOFF after "
              "the clock ran out")
    res.check(not s1.find("INFECTED", seat=s3.seat), "the joiner in the hold "
              "is not made a Luigi")
    res.check(s3.seat not in ref.roster, "nor put in the match")
    ov = s1.find("MATCH_OVER")
    res.check(len(ov) == 1 and ov[0]["a"] == OVER["SURVIVORS_WIN"]
              and ov[0]["c"] == _mask([s1.seat, s2.seat]) and ov[0]["b"] == 2,
              "SURVIVORS_WIN for the two survivors who were there at the end "
              "(%s)" % ov)
    # control: the same quit before the clock runs out hands off
    rig, cs = lmatch(3, time_s=8, code="LUIGI031")
    (lu,), surv = luigi_of(cs)
    lu.claim(CL["QUIT"], 0xFF)
    rig.consoles.remove(lu)
    streaming(rig, surv, 0.3)
    res.check(surv[0].find("INFECTED", b=INF["HANDOFF"]), "control: before "
              "the clock the same quit hands off")
    return res


def u_luigi_solo(mod):
    res = Result("luigi: a one-player Luigi match is refused at READY")
    rig = Rig(mod)
    a = Console(rig, "LUIGI032")
    streaming(rig, [a], 0.6)
    cid = a.ready(cap=1, mode=LUIGI)
    streaming(rig, [a], 6.0)
    v = a.verdict(cid)
    ref = rig.ref("LUIGI032")
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["BAD"],
              "mode 2 capacity 1: DENY BAD (%s)" % v)
    res.check(ref.state == 0 and not a.find("START")
              and not a.find("INFECTED"), "no match starts, nobody is picked")
    rig = Rig(mod)
    b = Console(rig, "LUIGI033")
    streaming(rig, [b], 0.6)
    cid = b.ready(cap=1, mode=0)
    streaming(rig, [b], 0.3)
    res.check(b.verdict(cid)["name"] == "ACCEPTED", "mode 0 capacity 1 is "
              "accepted as before")
    return res


def u_luigi_early_tag(mod):
    res = Result("luigi: a TAG stamped before the pick is refused")
    rig, cs = lmatch(3, code="LUIGI034", run=False)
    ref = rig.ref("LUIGI034")
    for c in cs:
        c.pos = [0, 0, 0]
    while ref.state != 2 or ref.rt(rig.now) < ref.t0 + 20:
        streaming(rig, cs, 0.01)
    (lu,), (s1, s2) = luigi_of(cs)
    s1.pos = [60, 0, 0]
    early = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 1,
                     stamp=ref.seat_wire(lu.seat, ref.t0 - 50))
    streaming(rig, cs, 0.3)
    v = lu.verdict(early)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STALE"],
              "TAG stamped 50 ms before T0: DENY STALE (%s)" % v)
    res.check(s1.seat not in ref.luigis, "s1 is still a survivor")
    ok = lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.3)
    v = lu.verdict(ok)
    res.check(v and v["name"] == "INFECTED", "control: the same TAG stamped "
              "now infects (%s)" % v)
    # a newly infected Luigi cannot tag with a stamp from before its own
    # INFECTED. Its claim must reach the referee inside its back-dating cap of
    # that moment, so give s1 a long cap (rtt 200 -> cap 125 ms) after lu's
    # TAG is queued, and send s1's TAG the instant the INFECTED is made.
    rig, cs = lmatch(3, code="LUIGI039")
    ref = rig.ref("LUIGI039")
    (lu,), (s1, s2) = luigi_of(cs)
    lu.pos, s1.pos, s2.pos = [0, 0, 0], [60, 0, 0], [60, 0, 60]
    streaming(rig, cs, 0.3)
    lu.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    ss = ref.seats[s1.seat]
    ss.rtts.clear(); ss.rtts.append(200); ss.rtt_min = 200
    while s1.seat not in ref.luigis:
        rig.now += 0.001
        rig.relay.tick(rig.now)
        rig.deliver()
    t_inf = ref.luigi_at[s1.seat]
    cid = s1.claim(CL["TAG"], s2.seat, ref_frame=s2.frame - 1,
                   stamp=ref.seat_wire(s1.seat, int(t_inf) - 10))
    streaming(rig, cs, 0.3)
    v = s1.verdict(cid)
    res.check(v and v["name"] == "DENY" and v["b"] == DENY["STALE"],
              "a TAG stamped 10 ms before its tagger's own INFECTED: DENY "
              "STALE (%s)" % v)
    return res


def u_luigi_reconnect(mod):
    res = Result("luigi: a dropped player who comes back keeps its team; a "
                 "new one is a Luigi")
    # a survivor drops (silent) and comes back on its seat
    rig, cs = lmatch(3, code="LUIGI035")
    ref = rig.ref("LUIGI035")
    (lu,), (s1, s2) = luigi_of(cs)
    streaming(rig, [lu, s2], 3.6)               # s1 silent past PRESENCE_MS
    res.check(s1.seat not in ref.roster and ref.state == 2,
              "s1 dropped as silent, the match goes on")
    s1.snap = []
    s1.ready(cap=3, mode=LUIGI)
    streaming(rig, cs, 0.5)
    res.check(s1.seat in ref.roster and s1.seat not in ref.luigis,
              "s1 is back as a survivor")
    res.check(not lu.find("INFECTED", seat=s1.seat), "no INFECTED for it")
    ts = [e for e in s1.snap if e["name"] == "TEAM_STATE"]
    res.check(ts and not ts[-1]["c"] & (1 << s1.seat), "its SNAPSHOT's "
              "TEAM_STATE leaves it out of the Luigis (%s)" % ts)
    # a Luigi drops and comes back
    rig, cs = lmatch(4, code="LUIGI036")
    ref = rig.ref("LUIGI036")
    (lu,), surv = luigi_of(cs)
    streaming(rig, surv, 3.6)                   # the Luigi goes silent
    ho = surv[0].find("INFECTED", b=INF["HANDOFF"])
    res.check(len(ho) == 1, "the silent Luigi: HANDOFF meanwhile")
    lu.ready(cap=4, mode=LUIGI)
    streaming(rig, cs, 0.5)
    rt_ = surv[0].find("INFECTED", seat=lu.seat, b=INF["RETURN"])
    res.check(len(rt_) == 1 and lu.seat in ref.luigis, "the Luigi is back as "
              "a Luigi: INFECTED b RETURN (%s)" % rt_)
    res.check(not surv[0].find("INFECTED", seat=lu.seat,
                               b=INF["LATE_JOIN"]), "not LATE_JOIN")
    # a clean QUIT is a leave, not a drop: back = a new player = a Luigi
    s = [c for c in surv if c.seat not in ref.luigis][0]
    s.claim(CL["QUIT"], 0xFF)
    streaming(rig, cs, 0.4)
    s.ready(cap=4, mode=LUIGI)
    streaming(rig, cs, 0.5)
    res.check(surv[0].find("INFECTED", seat=s.seat, b=INF["LATE_JOIN"]),
              "a survivor who QUIT and joins again is a new player: LATE_JOIN")
    # a drop longer than the window: back = a Luigi
    rig, cs = lmatch(3, code="LUIGI037", mod=mod)
    ref = rig.ref("LUIGI037")
    (lu,), (s1, s2) = luigi_of(cs)
    keep = mod.referee.REJOIN_KEEP_MS
    mod.referee.REJOIN_KEEP_MS = 1000
    try:
        streaming(rig, [lu, s2], 5.0)
        s1.ready(cap=3, mode=LUIGI)
        streaming(rig, cs, 0.5)
    finally:
        mod.referee.REJOIN_KEEP_MS = keep
    res.check(lu.find("INFECTED", seat=s1.seat, b=INF["LATE_JOIN"]),
              "back after the window (1 s here): LATE_JOIN")
    res.check(ref.invariants() == [], "invariants")
    return res


def u_luigi_tag_anytime(mod):
    res = Result("luigi: Tango's 'tag anytime': hurt, dead, respawning, "
                 "squished survivors are tagged (differs from Classic)")
    rig, cs = lmatch(5, code="LUIGI038")
    ref = rig.ref("LUIGI038")
    (lu,), surv = luigi_of(cs)
    cases = [("St_Hurt", 7, 0x21), ("St_DeadHit", 12, 0x03),
             ("St_Respawn", 54, 0x43), ("St_Squish", 32, 0x03)]
    for i, (c, (name, st, fl)) in enumerate(zip(surv, cases)):
        c.pos = [3000 * (i + 1), 0, 0]
        c.state, c.flags = st, fl
    streaming(rig, cs, 0.3)
    for c, (name, st, fl) in zip(surv, cases):
        lu.pos = [c.pos[0] + 60, 0, 0]
        streaming(rig, cs, 0.2)
        cid = lu.claim(CL["TAG"], c.seat, ref_frame=c.frame - 2)
        streaming(rig, cs, 0.25)
        v = lu.verdict(cid)
        res.check(v and v["name"] == "INFECTED", "survivor in %s: INFECTED "
                  "(%s)" % (name, v))
    return res


def luigi_race(mod, v):
    """Two Luigis tag one survivor: exactly one INFECTED, credited to the
    earlier stamp (the classic contention rule). Returns the winner index."""
    rig, cs = lmatch(3, code="LRACE001", mod=mod)
    ref = rig.ref("LRACE001")
    (l0,), (s1, s2) = luigi_of(cs)
    l0.pos, s1.pos, s2.pos = [0, 0, 0], [60, 0, 0], [0, 0, 60]
    streaming(rig, cs, 0.3)
    l0.claim(CL["TAG"], s1.seat, ref_frame=s1.frame - 2)
    streaming(rig, cs, 0.4)
    a, b, victim = l0, s1, s2
    sa, sb = ref.seats[a.seat], ref.seats[b.seat]
    rig.no_pings = True
    sa.rtts.clear(); sa.rtts.append(v["rtt"][0]); sa.rtt_min = v["rtt"][0]
    sb.rtts.clear(); sb.rtts.append(v["rtt"][1]); sb.rtt_min = v["rtt"][1]
    ref.seats[victim.seat].rtts.clear()
    ref.seats[victim.seat].rtts.append(40)
    ref.seats[victim.seat].rtt_min = 40
    t_base = ref.rt(rig.now) + 100
    order = sorted([(v["arrive"][0], 0), (v["arrive"][1], 1)])
    cids = {}
    nxt = [rig.now]
    for arr, who in order:
        target = t_base + arr - 2000.0
        while ref.rt(rig.now) < target:
            rig.now += 0.001
            if rig.now >= nxt[0]:           # bodies keep streaming (30 Hz)
                nxt[0] += 1.0 / 30.0
                for k in cs:
                    k.syns()
            rig.relay.tick(rig.now)
            rig.deliver()
        rf = victim.frame - 2
        c = (a, b)[who]
        cids[who] = c.claim(CL["TAG"], victim.seat, ref_frame=rf,
                            stamp=ref.seat_wire(c.seat, t_base
                                                + v["stamp"][who] - 2000))
    streaming(rig, cs, 0.4)
    va, vb = a.verdict(cids[0]), b.verdict(cids[1])
    wins = [bool(va and va["name"] == "INFECTED"),
            bool(vb and vb["name"] == "INFECTED")]
    if sum(wins) != 1:
        return -1
    return 0 if wins[0] else 1


def run_luigi_vectors(mod, res, vec_doc, write):
    lvec = race_vectors(seed=13, races=8)
    for v in lvec:
        v["winner"] = luigi_race(mod, v)
    two = sum(1 for v in lvec if v["winner"] < 0)
    ok = sum(1 for v in lvec if v["winner"] == v["truth"])
    res.note("luigi tags: %d/%d correct (%.1f%%)" % (ok, len(lvec),
                                                     100.0 * ok / len(lvec)))
    res.check(two == 0, "no tag race with two or zero INFECTED (%d)" % two)
    res.check(ok >= 0.95 * len(lvec), "tag races >= 95%% correct")
    if not write:
        same = [(r["stamp"], r["arrive"], r["rtt"], r["winner"])
                for r in (vec_doc or {}).get("luigi_races", [])] == [
            (v["stamp"], v["arrive"], v["rtt"], v["winner"]) for v in lvec]
        res.check(same, "arena_vectors.json luigi_races match this referee")
    return lvec


def run_unit():
    mod = load_relay()
    tests = (u_contract, u_handshake, u_routes, u_match, u_timeup,
             u_contention, u_hits, u_hit_nc, u_reliability, u_leave, u_ready,
             u_strikes, u_eat, u_grab, u_stomp, u_rematch, u_rejoin, u_hit_eat, u_star_flight,
             u_eat_shield, u_land_speed, u_caps, u_star_tie,
             u_luigi_pick, u_luigi_immune, u_luigi_infect, u_luigi_tag_rules,
             u_luigi_caps, u_luigi_wins, u_luigi_leave, u_luigi_join,
             u_luigi_16, u_luigi_mode0, u_luigi_clock_out, u_luigi_solo,
             u_luigi_early_tag, u_luigi_reconnect, u_luigi_tag_anytime)
    results = []
    for t in tests:
        try:
            r = t(mod)
        except Exception as exc:
            import traceback
            r = Result(t.__name__)
            r.check(False, "exception: %s\n%s" % (exc, traceback.format_exc()))
        r.report()
        results.append(r)
    return results


# ================================================================ VECTORS
# Contention vectors shared with the game (A3's fairness rig reads the same
# file): each race has two consoles with an RTT pair, a true touch time per
# console, the stamp each console's clock estimate gives, and the time each
# claim reaches the referee; "winner" is what this referee decides, "truth"
# who touched first. Built from M3's model (contend_sim.py): 30 Hz frame
# grids, jitter |N(0,6)| per hop, route asymmetry N(0,6).


def race_vectors(seed=7, races=200, rtts=((40, 125), (40, 40), (125, 125),
                                          (70, 125), (40, 250))):
    rng = random.Random(seed)
    out = []
    for ra, rb in rtts:
        for i in range(races):
            gap = rng.uniform(-160, 160)
            base = 2000.0 + rng.uniform(0, 33.3)
            ta = base
            tb = base + gap
            # each console sees its touch on its own 30 Hz frame grid
            pa = rng.uniform(0, 33.333)
            pb = rng.uniform(0, 33.333)
            fa = math.ceil((ta - pa) / 33.333) * 33.333 + pa
            fb = math.ceil((tb - pb) / 33.333) * 33.333 + pb
            asym_a = rng.gauss(0, 6)
            asym_b = rng.gauss(0, 6)
            # clock offset error = half the route asymmetry
            sa = fa + asym_a / 2
            sb = fb + asym_b / 2
            aa = fa + ra / 2 + asym_a / 2 + abs(rng.gauss(0, 6))
            ab = fb + rb / 2 + asym_b / 2 + abs(rng.gauss(0, 6))
            out.append({"rtt": [ra, rb], "touch": [round(fa, 3),
                                                    round(fb, 3)],
                        "stamp": [int(sa), int(sb)],
                        "arrive": [round(aa, 3), round(ab, 3)],
                        "truth": 0 if fa < fb else 1})
    return out


def decide_race(mod, v, caps=False):
    """Run one race through a real referee; returns the winning console
    index (0 / 1).  caps=True: the two consoles race for one cap (CAP_TAKE)
    instead of a star (GRAB); the contention rule is the same."""
    rf = mod.referee
    saved = dict(rf.CAP_MAPS)
    if caps:
        rf.CAP_MAPS[0] = [(0, 0, 0, 0, 0x004)]
    try:
        return _decide_race(mod, v, caps)
    finally:
        rf.CAP_MAPS.clear()
        rf.CAP_MAPS.update(saved)


def _decide_race(mod, v, caps):
    rig, (a, b) = match(2, code="RACE0001", mod=mod)
    ref = rig.ref("RACE0001")
    sa, sb = ref.seats[a.seat], ref.seats[b.seat]
    rig.no_pings = True
    sa.rtts.clear(); sa.rtts.append(v["rtt"][0]); sa.rtt_min = v["rtt"][0]
    sb.rtts.clear(); sb.rtts.append(v["rtt"][1]); sb.rtt_min = v["rtt"][1]
    if caps:
        inst = 1
    else:
        inst = a.find("STAR_WAKE")[-1]["a"]
        a.claim(CL["MARKER_BREAK"], inst)
        streaming(rig, [a, b], 0.3)
    # race times are relative to "now" in referee ms
    t_base = ref.rt(rig.now) + 100
    order = sorted([(v["arrive"][0], 0), (v["arrive"][1], 1)])
    cids = {}
    for arr, who in order:
        target = t_base + arr - 2000.0
        while ref.rt(rig.now) < target:
            rig.now += 0.001
            rig.relay.tick(rig.now)
            rig.deliver()
        c = (a, b)[who]
        cids[who] = c.claim(CL["CAP_TAKE"] if caps else CL["GRAB"], inst,
                            aux=mod.referee.Referee.cap_hash(0, 0, 0) if caps else 0,
                            stamp=ref.seat_wire(c.seat, t_base
                                                + v["stamp"][who] - 2000))
    streaming(rig, [a, b], 0.4)
    va, vb = a.verdict(cids[0]), b.verdict(cids[1])
    won = "CAP_TAKEN" if caps else "STAR_TAKEN"
    wins = [va["name"] == won, vb["name"] == won]
    if sum(wins) != 1:
        return -1
    return 0 if wins[0] else 1


def run_vectors(mod, write=False, races=200):
    res = Result("contention vectors (M3 model, %d races per RTT pair)"
                 % races)
    vec = race_vectors(races=races)
    capvec = race_vectors(seed=11, races=40)
    try:
        vec_doc = json.load(open(VECTORS))
    except (OSError, ValueError):
        vec_doc = None
    lvec = run_luigi_vectors(mod, res, vec_doc, write)
    for v in capvec:
        v["winner"] = decide_race(mod, v, caps=True)
    by = {}
    two = 0
    for v in vec:
        w = decide_race(mod, v)
        v["winner"] = w
        if w < 0:
            two += 1
        k = "%d/%d" % tuple(v["rtt"])
        ok, n, a_wins = by.get(k, (0, 0, 0))
        by[k] = (ok + (w == v["truth"]), n + 1, a_wins + (w == 0))
    for k, (ok, n, aw) in sorted(by.items()):
        res.note("rtt %s: correct %d/%d = %.1f%%, A wins %.0f%%"
                 % (k, ok, n, 100.0 * ok / n, 100.0 * aw / n))
        res.check(ok >= 0.95 * n, "rtt %s >= 95%% correct" % k)
    res.check(two == 0, "no race with two or zero winners (%d)" % two)
    cap_two = sum(1 for v in capvec if v["winner"] < 0)
    cap_ok = sum(1 for v in capvec if v["winner"] == v["truth"])
    res.note("caps: %d/%d correct (%.1f%%)" % (cap_ok, len(capvec), 100.0 * cap_ok / len(capvec)))
    res.check(cap_two == 0, "no cap race with two or zero winners (%d)" % cap_two)
    res.check(cap_ok >= 0.95 * len(capvec), "cap races >= 95%% correct")
    if write:
        doc = {"about": "REFEREE1 contention vectors: two consoles race to "
                        "one star. Times in ms on one axis (the referee's "
                        "clock); stamp = each console's clock-estimate stamp "
                        "of its touch; arrive = when its claim reaches the "
                        "referee; truth = who touched first; winner = this "
                        "referee's verdict. Rule: arena_contract.json rules "
                        "1-2.",
               "races": vec}
        with open(VECTORS, "w", newline="\n") as fh:
            # one race per line, so the file reads (and diffs) as a table
            fh.write('{"about": %s,\n "races": [\n'
                     % json.dumps(doc["about"]))
            fh.write(",\n".join("  " + json.dumps(r, sort_keys=True)
                                for r in vec))
            fh.write("\n ],\n")
            fh.write(' "cap_races_about": "VSREF3: the same model, two consoles race to one cap (CAP_TAKE)",\n')
            fh.write(' "cap_races": [\n')
            fh.write(",\n".join("  " + json.dumps(r, sort_keys=True)
                                for r in capvec))
            fh.write("\n ],\n")
            fh.write(' "luigi_races_about": "REFLUIGI1: the same model, two '
                     'Luigis race to tag one survivor (TAG); winner = the one '
                     'whose INFECTED it is",\n')
            fh.write(' "luigi_races": [\n')
            fh.write(",\n".join("  " + json.dumps(r, sort_keys=True)
                                for r in lvec))
            fh.write("\n ]}\n")
        res.note("wrote %s (%d races)" % (os.path.basename(VECTORS),
                                          len(vec)))
    else:
        try:
            doc = json.load(open(VECTORS))
            same = [(r["stamp"], r["arrive"], r["rtt"], r["winner"])
                    for r in doc["races"]] == [
                (v["stamp"], v["arrive"], v["rtt"], v["winner"]) for v in vec]
            res.check(same, "arena_vectors.json matches this referee")
            same_c = [(r["stamp"], r["arrive"], r["rtt"], r["winner"])
                      for r in doc.get("cap_races", [])] == [
                (v["stamp"], v["arrive"], v["rtt"], v["winner"]) for v in capvec]
            res.check(same_c, "arena_vectors.json cap_races match this referee")
        except (OSError, ValueError, KeyError):
            res.check(False, "arena_vectors.json missing or unreadable")
    return res


# ================================================================ FUZZ


def run_fuzz(mod, rounds=150000, seed=1):
    """Mutated and random datagrams from arena seats, strangers and a hub
    beside them, with time advancing and ticks. Violations: an exception; a
    forward the arena table does not allow; a referee datagram that is not a
    272-byte SYNR / 20-byte SYNQ with byte 5 0xFE, or that goes to an
    unverified or unseated address; a reply to a stranger larger than its
    datagram; a broken referee invariant."""
    res = Result("fuzz: %d datagrams at arena + hub sessions" % rounds)
    rng = random.Random(seed)
    # (VSREF3) a cap table, so the cap claims below are judged against caps
    saved_caps = dict(mod.referee.CAP_MAPS)
    mod.referee.CAP_MAPS[0] = [(0, -200, 0, 0, 0x004), (1, 0, 0, 300, 0x204),
                               (2, 200, 0, 0, 0x104)]
    try:
        return _run_fuzz(mod, res, rounds, rng)
    finally:
        mod.referee.CAP_MAPS.clear()
        mod.referee.CAP_MAPS.update(saved_caps)


def _run_fuzz(mod, res, rounds, rng):
    rig = Rig(mod)
    viol = []
    cons = [Console(rig, "FUZZ0001") for _ in range(4)]       # honest
    attackers = [Console(rig, "FUZZ0001") for _ in range(2)]  # seated, hostile
    hub = [rig.addr() for _ in range(2)]
    for h in hub:
        rig.relay.on_datagram(hello("FUZZHUB1", 2), h, rig.now)
    rig.wire.sent = []
    streaming(rig, cons, 0.6)
    ref = rig.ref("FUZZ0001")
    # Instrument the referee: every event it ever makes, for the ledger
    # conservation and one-winner checks below.
    log = []
    cap_log = []          # (etype, cap id, stamp) for the cap conservation check
    real_emit = ref._emit

    def emit(etype, seat, a=0, b=0, c=0, d=0, e=0, t=0, x=0, y=0, z=0):
        log.append((etype, seat, a, b, c, d, e))
        if etype in (19, 20):
            cap_log.append((etype, a, t, seat))
        return real_emit(etype, seat, a, b, c, d, e, t, x, y, z)
    ref._emit = emit
    digest = [100]
    for c in cons:
        c.ready(cap=4, time_s=60, first_to_n=0, digest=digest[0])
    streaming(rig, cons, 3.5)
    strangers = [rig.addr() for _ in range(8)]
    templates = []
    for c in cons:
        templates.append(S_SYNS.pack(b"SYNS", 1, 0, 3, 3, 100, 0, 0, 0, 0, 0,
                                     0, 0, 1, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0))
    templates += [
        S_SYNE.pack(b"SYNE", 1, 0, rng.randrange(1, 12), 1, 5, 0, 0, 100, 90),
        S_PING.pack(b"SYNP", 2, 0, 0xFE, 0, 1, 0, 0xFFFF, 0),
        S_SYNO.pack(b"SYNO", 1, 0, 1, 1, 100, 0, 0, 0, 0, 0, 0, 2, 0),
        b"SYNR" + bytes(268), b"SYNQ" + bytes(16), b"SYNP" + bytes(8),
        b"SYNI" + bytes(28), b"SYNT" + bytes(20), b"SYNV" + bytes(172),
        b"SYNG" + bytes(40), hello("FUZZ0001", 3), hello("FUZZ0001", 3, 99),
    ]
    arena_addrs = set(c.addr for c in cons + attackers)
    fwd_ok = {}
    for tag, (route, kind) in mod.ARENA_KINDS.items():
        fwd_ok[tag] = (kind.exact, route)
    t_frame = rig.now
    for i in range(rounds):
        r = rng.random()
        rig.now += rng.random() * 0.002
        if rig.now - t_frame > 1 / 30.0:
            t_frame = rig.now
            c = rng.choice(cons)
            if c.seat is not None:
                c.pos[0] += rng.uniform(-20, 20)
                c.syns()
        if r < 0.06:
            # a well-formed claim with random fields from a console (the
            # referee's logic, not just its parser)
            c = rng.choice(cons)
            if c.seat is not None:
                kind = (rng.choice([2, 2, 3, 3, 3, 4, 4, 5, 6, 8, 9, 10, 11,
                                    12, 12, 13, 14, 15, 16, 16, 17, 17, 17,
                                    18, 18])
                        if rng.random() > 0.003 else 7)
                tgt = rng.choice([rng.randrange(0, 20),
                                  rng.choice(cons).seat or 0,
                                  rng.randrange(0, 4), rng.randrange(30, 40)])
                c.claim(kind, tgt, aux=rng.randrange(65536),
                        stamp=(ref.ref_ms(rig.now, c.seat)
                               - rng.randrange(-50, 400)) & 0xFFFFFFFF,
                        frame=max(0, c.frame - rng.randrange(0, 20)),
                        ref_frame=max(0, rng.choice(cons).frame
                                      - rng.randrange(0, 20)))
                c.state = rng.choice([14, 15, 59, 62, 3, 7, 35, 18, 16, 37, 65])
                c.look = rng.choice([2, 2, 3, 0, 1])
                if rng.random() < 0.3:
                    c.pos = [rng.choice([-200, 0, 200, 600]) + rng.uniform(-30, 30),
                             0, rng.choice([0, 300]) + rng.uniform(-30, 30)]
                c.invuln = rng.choice([0, 0, 0, 10])
            continue
        if r < 0.75:
            src = rng.choice(attackers).addr if rng.random() < 0.8 else \
                rng.choice(strangers + hub)
            d = bytearray(rng.choice(templates))
            for _ in range(rng.choice([0, 1, 2, 4, 8])):
                op = rng.random()
                if op < 0.6 and d:
                    d[rng.randrange(len(d))] = rng.randrange(256)
                elif op < 0.8:
                    d = d[:rng.randrange(len(d) + 1)]
                else:
                    d += bytes(rng.randrange(256)
                               for _ in range(rng.randrange(8)))
            d = bytes(d)
        else:
            src = rng.choice([x.addr for x in attackers] + strangers)
            d = bytes(rng.randrange(256) for _ in range(rng.choice(
                [rng.randrange(0, 300), 16, 20, 24, 32, 48, 272])))
        seated_before = dict((a, rig.relay.endpoints.get(a))
                             for a in [src])
        try:
            rig.relay.on_datagram(d, src, rig.now)
            if i % 7 == 0:
                rig.relay.tick(rig.now)
            if i % 500 == 0:
                rig.relay.sweep(rig.now)
        except Exception as exc:
            import traceback
            viol.append("exception %s at %d: %s" % (exc, i,
                                                    traceback.format_exc()))
            break
        out, rig.wire.sent = rig.wire.sent, []
        sess = rig.session("FUZZ0001")
        seated = set(sess.seats.values()) if sess else set()
        verified = set()
        if sess:
            for seat, s in sess.referee.seats.items():
                if s.verified and seat in sess.seats:
                    verified.add(sess.seats[seat])
        for data, addr in out:
            if seated_before[src] is None and addr == src and \
                    len(data) > len(d):
                viol.append("reply to a stranger larger than its datagram")
            tag = data[:4]
            if tag == b"SYNR":
                if len(data) != 272 or data[5] != 0xFE:
                    viol.append("malformed SYNR")
                if addr not in verified:
                    viol.append("SYNR to an unverified address")
            elif tag == b"SYNQ" and len(data) == 20:
                if data[5] != 0xFE or addr != src or len(d) != 20:
                    viol.append("bad referee pong")
            elif tag == b"SMRA":
                pass
            elif addr in arena_addrs or addr in seated:
                ent = fwd_ok.get(bytes(tag))
                if src in arena_addrs or addr in arena_addrs:
                    if ent is None or len(data) not in ent[0] or \
                            ent[1] in ("ref", "none"):
                        viol.append("arena forward outside the table: %r %d"
                                    % (tag, len(data)))
                    if tag == b"SYNS" and not ref.check_state(0, data):
                        viol.append("forwarded a SYNS that fails the field "
                                    "check")
            for c in cons:
                if c.addr == addr:
                    c.receive(data)
        if i % 1000 == 0 and sess:
            bad = sess.referee.invariants()
            if bad:
                viol.append("invariants: %s" % bad)
            # a match that ended (or never started) is replaced by a fresh
            # one, so the fuzz keeps exercising a RUNNING referee
            if sess.referee.state in (0, 3):
                digest[0] += 1
                for c in cons:
                    if c.seat is not None:
                        c.ready(cap=4, time_s=60, digest=digest[0])
                rig.deliver()
        if len(viol) > 20:
            break
        # keep the consoles alive (keepalive + pings) so the match runs on
        if i % 300 == 0:
            for c in cons:
                rig.relay.on_datagram(hello("FUZZ0001", 3, c.seat
                                            if c.seat is not None else 0xFF),
                                      c.addr, rig.now)
                c.ping()
            rig.deliver()
    # ledger conservation and one winner per match, from the event log
    matches = 0
    overs = 0
    score = {}
    taken = set()
    starts = 0
    for etype, seat, a_, b_, c_, d_, e_ in log:
        if etype == 1:                      # START: a new match
            starts += 1
            if score and any(v < 0 for v in score.values()):
                viol.append("negative running score")
            score = {}
            taken = set()
            overs = 0
            matches += 1
        elif etype == 4:                    # STAR_TAKEN
            score[seat] = score.get(seat, 0) + 1
            if b_ != score[seat] and b_ != 127:
                viol.append("STAR_TAKEN score %d != ledger %d" % (b_,
                                                                  score[seat]))
            key = (a_, len(taken))
            taken.add(key)
        elif etype == 8 and seat != 0xFF:   # SPILL
            score[seat] = score.get(seat, 0) - 1
            if d_ != score[seat]:
                viol.append("SPILL new score %d != ledger %d" % (d_,
                                                                 score[seat]))
        elif etype == 11 and a_ == 1:       # JOIN mid-match: starts at 0
            if score.get(seat, 0) != 0:
                viol.append("JOIN with a nonzero running score")
        elif etype == 9:
            overs += 1
            if overs > 1:
                viol.append("two MATCH_OVER in one match")
    # caps: a placed cap is never taken twice inside its respawn window and a
    # knocked-off cap is taken at most once
    last_take = {}
    dropped = set()
    for etype, cid, t_, seat_ in cap_log:
        if etype == 20:
            dropped.add(cid)
        elif cid >= 32:
            if cid not in dropped:
                viol.append("a knocked-off cap %d taken but never dropped" % cid)
            dropped.discard(cid)
        else:
            if cid in last_take and t_ - last_take[cid] < mod.referee.CAP_RESPAWN_MS                     and t_ >= last_take[cid]:
                viol.append("cap %d taken twice inside its respawn window" % cid)
            last_take[cid] = t_
    res.note("caps: %d CAP_TAKEN, %d CAP_DROPPED" % (
        sum(1 for e in cap_log if e[0] == 19), sum(1 for e in cap_log if e[0] == 20)))
    res.note("matches %d, events %d, claims decided %d, STAR_TAKEN %d, "
             "SPILL %d, HIT %d, DENY %d" % (
                 matches, len(log), ref.n_verdicts,
                 sum(1 for e in log if e[0] == 4),
                 sum(1 for e in log if e[0] == 8),
                 sum(1 for e in log if e[0] == 6),
                 sum(1 for e in log if e[0] == 5)))
    res.check(not viol, "0 violations (%d): %s" % (len(viol), viol[:5]))
    res.note("referee: %s; relay arena drops %s" % (
        rig.ref("FUZZ0001").stats() if rig.session("FUZZ0001") else "-",
        rig.relay.a_drop))
    return res


def main(argv):
    print("arena referee suite, python %s on %s, pid %d"
          % (sys.version.split()[0], sys.platform, os.getpid()))
    what = argv[0] if argv else "all"
    results = []
    if what in ("all", "unit"):
        results += run_unit()
    if what in ("all", "vectors"):
        mod = load_relay()
        r = run_vectors(mod, write=(what == "vectors"))
        r.report()
        results.append(r)
    if what in ("all", "fuzz"):
        n = int(argv[1]) if what == "fuzz" and len(argv) > 1 else (
            150000 if what == "fuzz" else 40000)
        mod = load_relay()
        r = run_fuzz(mod, n)
        r.report()
        results.append(r)
    failed = [r for r in results if not r.ok]
    print("\n%d tests, %d checks, %d failed"
          % (len(results), sum(r.n for r in results), len(failed)))
    for r in failed:
        print("FAILED: %s" % r.name)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
