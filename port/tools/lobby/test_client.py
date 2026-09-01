#!/usr/bin/env python3
"""Test client for sm64ds-lobby. No dependencies beyond the standard library.

    python3 test_client.py selftest              spawn a server, prove every
                                                 verb and every refusal
    python3 test_client.py selftest --url URL    run against a server already
                                                 up (e.g. the deployed one)
    python3 test_client.py soak [--seconds N]    hold a room open under load
    python3 test_client.py negatives --out DIR   write one file per captured
                                                 refusal, for the proof folder

WHY IT SPEAKS HTTP BY HAND. Most of what is worth proving here is a MALFORMED
request: a body with no Content-Length, a Content-Length that lies, a 5 KB
body, the wrong content type. A convenience wrapper fixes all of those on the
way out, so this file builds each request header by header through
http.client's low level API and never lets anything be corrected for it.

THE PORT IT BINDS. Derived from this process's pid and then verified by
binding, the way the port harnesses in this tree do it. It stays inside
20000..39999, which keeps it clear of 51765 (the comms loopback base, the
owner's own desk pair) and of 58434..58733.
"""

import argparse
import http.client
import json
import os
import random
import socket
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
SERVER = os.path.join(HERE, "app", "server.py")
PREFIX = "/port/lobby"

ROOM_ALPHABET = "ABCDEFGHJKMNPQRSTUVWXYZ23456789"


# ------------------------------------------------------------------- plumbing


def pick_port():
    """A base port derived from our own pid, then proven free by binding it."""
    base = 20000 + (os.getpid() * 7) % 20000
    for step in range(64):
        port = 20000 + ((base - 20000 + step) % 20000)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.bind(("127.0.0.1", port))
            return port
        except OSError:
            continue
        finally:
            s.close()
    raise SystemExit("no free port in 20000..39999")


class Client(object):
    """One HTTP conversation with the lobby, with nothing corrected for us."""

    def __init__(self, host, port, prefix=PREFIX, tls=False):
        self.host = host
        self.port = port
        self.prefix = prefix
        self.tls = tls

    def _conn(self):
        if self.tls:
            import ssl
            return http.client.HTTPSConnection(
                self.host, self.port, timeout=60, context=ssl.create_default_context())
        return http.client.HTTPConnection(self.host, self.port, timeout=60)

    def raw(self, method, path, body=None, ctype="application/json",
            content_length="auto", extra=()):
        """Send exactly the request described and return (status, text).

        content_length: "auto" for the real length, "omit" for no header at
        all, or an integer to announce a length that is not the truth.
        """
        conn = self._conn()
        try:
            conn.putrequest(method, path, skip_accept_encoding=True)
            if ctype is not None:
                conn.putheader("Content-Type", ctype)
            if content_length == "auto":
                if body is not None:
                    conn.putheader("Content-Length", str(len(body)))
            elif content_length == "omit":
                pass
            else:
                conn.putheader("Content-Length", str(content_length))
            for k, v in extra:
                conn.putheader(k, v)
            conn.endheaders()
            if body:
                conn.send(body)
            resp = conn.getresponse()
            return resp.status, resp.read().decode("utf-8", "replace")
        finally:
            conn.close()

    # A rate-shed 429 is not a functional failure -- it is the limiter doing its
    # job -- and this functional client is not the limiter's test (test_security
    # and the negatives are, and they drive 429s through raw() on purpose). So
    # post() rides past a 429 with a bounded backoff instead of returning it,
    # which keeps a caller like the selftest deterministic whether it runs
    # against its own spawned server or, at full tilt, against the shipped
    # 20 req/s limit on the deployed URL. The backoff total (~23 s) clears even
    # the 10 s bad-sender ignore window. raw() deliberately does none of this.
    def post(self, verb, obj, retry_429=True, **kw):
        body = json.dumps(obj).encode("utf-8")
        backoff = 0.5
        for attempt in range(7):
            status, text = self.raw("POST", self.prefix + "/" + verb, body, **kw)
            try:
                parsed = json.loads(text)
            except ValueError:
                parsed = {"_raw": text}
            if status == 429 and retry_429 and attempt < 6:
                time.sleep(backoff)
                backoff = min(8.0, backoff * 2)
                continue
            return status, parsed

    def get(self, verb):
        status, text = self.raw("GET", self.prefix + "/" + verb, None, ctype=None)
        try:
            return status, json.loads(text)
        except ValueError:
            return status, {"_raw": text}


class Spawned(object):
    """A server of our own, on a pid-derived port, torn down on the way out."""

    def __init__(self, port, env_extra=None):
        self.port = port
        env = dict(os.environ)
        env["LISTEN_PORT"] = str(port)
        env["LISTEN_ADDR"] = "127.0.0.1"
        env.update(env_extra or {})
        self.log_path = os.environ.get("LOBBY_TEST_LOG")
        self.logfile = open(self.log_path, "ab") if self.log_path else None
        self.proc = subprocess.Popen(
            [sys.executable, "-u", SERVER],
            env=env,
            stdout=self.logfile or subprocess.DEVNULL,
            stderr=subprocess.STDOUT)

    def wait_ready(self, timeout=15.0):
        c = Client("127.0.0.1", self.port)
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self.proc.poll() is not None:
                raise SystemExit("server exited early, rc=%s" % self.proc.returncode)
            try:
                status, body = c.get("health")
                if status == 200 and body.get("ok"):
                    return c
            except OSError:
                pass
            time.sleep(0.15)
        raise SystemExit("server never became healthy on port %d" % self.port)

    def stop(self):
        try:
            self.proc.terminate()
            self.proc.wait(timeout=10)
        except Exception:
            try:
                self.proc.kill()
            except Exception:
                pass
        if self.logfile:
            self.logfile.close()


# ---------------------------------------------------------------- assertions

PASSED = []
FAILED = []


def check(name, ok, detail=""):
    (PASSED if ok else FAILED).append(name)
    print("%-5s %s%s" % ("ok" if ok else "FAIL", name,
                         "" if ok else "   <- " + str(detail)))
    return ok


def expect(name, got, want_status, want_err=None):
    status, body = got
    ok = status == want_status
    if ok and want_err is not None:
        ok = body.get("error") == want_err
    return check(name, ok, "got %s %s, wanted %s %s"
                 % (status, body, want_status, want_err))


# ------------------------------------------------------------------ selftest


def selftest(c, quick=False):
    # REGRESSION NOTE (2026-09-01). This runs the happy path at full speed from
    # one address, and near the end (selftest_kick's create/join/kick/poll
    # burst) that outruns the server's shipped 20 req/s limit. A rate-shed poll
    # returns 429 {"v":1,"error":...} with no "view", and a bare seen["view"]
    # then KeyErrors -- which made this test pass or fail on TIMING, not on
    # correctness (both the author's and the reviewer's 55/0 runs happened to
    # stay under 20/s; a fresh deploy check did not). THE SERVER IS CORRECT:
    # shedding a flooding client is the limiter working. Two things keep this
    # test honest about that instead of racing it: its own spawned server lifts
    # the rate limits (see main), and c.post() rides past a 429 with a bounded
    # backoff (see Client.post), which is what carries a `selftest --url` run
    # against the real, un-liftable limit on the deployed server.
    # -- health ----------------------------------------------------------
    status, health = c.get("health")
    check("health answers 200", status == 200, status)
    check("health carries rooms/members/waiters/revision",
          all(k in health for k in ("rooms", "members", "waiters", "revision")),
          health)
    rooms_before = health.get("rooms", 0)

    # -- create ----------------------------------------------------------
    status, host = c.post("create", {"v": 1, "nick": "tango", "pre_ok": True})
    check("create answers 200", status == 200, host)
    code = host.get("room", "")
    check("room code is 6 chars of the room alphabet",
          len(code) == 6 and all(ch in ROOM_ALPHABET for ch in code), code)
    check("token is 32 lowercase hex",
          len(host.get("token", "")) == 32
          and all(ch in "0123456789abcdef" for ch in host.get("token", "")),
          host.get("token"))
    check("creator is seat 1 and the host",
          host.get("member") == 1 and host["view"]["host"] == 1, host.get("view"))
    check("a new room is in the lobby state with map 0 / time",
          host["view"]["state"] == "lobby" and host["view"]["map"] == 0
          and host["view"]["win_mode"] == "time"
          and host["view"]["star_target"] is None, host["view"])
    check("health now shows one more room",
          c.get("health")[1]["rooms"] == rooms_before + 1)

    # -- join ------------------------------------------------------------
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "opie"})
    check("join answers 200", status == 200, guest)
    check("joiner is seat 2 and playing",
          guest.get("member") == 2
          and guest["view"]["members"][1]["playing"] is True, guest.get("view"))
    check("both seats are in the roster",
          [m["display"] for m in guest["view"]["members"]] == ["tango", "opie"],
          guest["view"]["members"])

    # -- the roster reaches the host through a poll -----------------------
    status, seen = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                   "cursor": host["cursor"], "wait": 0})
    check("host's poll carries the joined event",
          any(e["kind"] == "joined" and e["seat"] == 2 for e in seen["events"]),
          seen.get("events"))
    check("host's poll view shows two seats",
          len(seen["view"]["members"]) == 2, seen["view"])
    check("the host's own view says you=1", seen["view"]["you"] == 1)

    # -- chat, both ways --------------------------------------------------
    status, sent = c.post("chat", {"v": 1, "room": code, "token": host["token"],
                                   "text": "hey"})
    check("host can chat", status == 200, sent)
    status, seen2 = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                    "cursor": guest["cursor"], "wait": 0})
    chats = [e for e in seen2["events"] if e["kind"] == "chat"]
    check("the guest sees the host's line verbatim",
          len(chats) == 1 and chats[0]["text"] == "hey"
          and chats[0]["display"] == "tango", seen2.get("events"))

    status, _ = c.post("chat", {"v": 1, "room": code, "token": guest["token"],
                                "text": "gg"})
    check("guest can chat", status == 200)
    # From the cursor the host's OWN chat handed back, so the only thing this
    # poll can carry is what happened after it.
    status, seen3 = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                    "cursor": sent["cursor"], "wait": 0})
    chats = [e for e in seen3["events"] if e["kind"] == "chat"]
    check("the host sees the guest's line verbatim",
          len(chats) == 1 and chats[0]["text"] == "gg"
          and chats[0]["display"] == "opie", seen3.get("events"))

    # -- the long poll actually holds, and returns early on an event ------
    hold = {}

    def hold_poll():
        t0 = time.time()
        hold["r"] = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                    "cursor": seen3["cursor"], "wait": 20})
        hold["dt"] = time.time() - t0

    t = threading.Thread(target=hold_poll)
    t.start()
    time.sleep(1.0)
    c.post("chat", {"v": 1, "room": code, "token": guest["token"], "text": "wake"})
    t.join(timeout=25)
    check("a long poll returns as soon as there is something to say",
          hold.get("dt", 99) < 5.0, hold.get("dt"))
    check("and it returns the event that woke it",
          any(e["kind"] == "chat" and e["text"] == "wake"
              for e in hold["r"][1]["events"]), hold["r"][1].get("events"))

    if not quick:
        t0 = time.time()
        status, idle = c.post("poll", {"v": 1, "room": code,
                                       "token": host["token"],
                                       "cursor": hold["r"][1]["cursor"],
                                       "wait": 3})
        dt = time.time() - t0
        check("a long poll with nothing to say holds for its full wait",
              2.5 <= dt <= 4.5 and status == 200, dt)
        check("and answers with no events", idle["events"] == [], idle)

    # The host's cursor from here on. Polling from 0 would ask for a resync,
    # which by contract answers with the view and the retained CHAT rather
    # than with the event list, so it is the wrong instrument for the
    # roster-event assertions below.
    hcur = hold["r"][1]["cursor"] if not quick else seen3["cursor"]

    # -- params, and who may set them -------------------------------------
    expect("a guest cannot set params",
           c.post("params", {"v": 1, "room": code, "token": guest["token"],
                             "map": 2, "win_mode": "stars", "star_target": 3}),
           403, "not_host")
    status, _ = c.post("params", {"v": 1, "room": code, "token": host["token"],
                                  "map": 2, "win_mode": "stars", "star_target": 3})
    check("the host can set params", status == 200)
    status, after = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                    "cursor": hcur, "wait": 0})
    check("a params event reaches the other window",
          any(e["kind"] == "params" and e["map"] == 2
              and e["win_mode"] == "stars" and e["star_target"] == 3
              for e in after["events"]), after.get("events"))
    check("the params reach the room's view",
          after["view"]["map"] == 2 and after["view"]["win_mode"] == "stars"
          and after["view"]["star_target"] == 3, after["view"])
    expect("star_target is forbidden under win_mode time",
           c.post("params", {"v": 1, "room": code, "token": host["token"],
                             "map": 0, "win_mode": "time", "star_target": 3}),
           400, "bad_star_target")
    expect("star_target is required under win_mode stars",
           c.post("params", {"v": 1, "room": code, "token": host["token"],
                             "map": 0, "win_mode": "stars"}),
           400, "bad_star_target")
    expect("star_target is capped at 5",
           c.post("params", {"v": 1, "room": code, "token": host["token"],
                             "map": 0, "win_mode": "stars", "star_target": 6}),
           400, "bad_star_target")
    expect("map is 0..3",
           c.post("params", {"v": 1, "room": code, "token": host["token"],
                             "map": 4, "win_mode": "time"}),
           400, "bad_map")

    # -- duplicate nicknames are disambiguated once, by the server --------
    status, dup = c.post("join", {"v": 1, "room": code, "nick": "opie"})
    check("a third seat may join", status == 200, dup)
    check("the third seat is a spectator", dup["view"]["members"][2]["playing"] is False,
          dup["view"]["members"])
    check("the duplicate nickname is suffixed with its seat, once",
          [m["display"] for m in dup["view"]["members"]] == ["tango", "opie", "opie (3)"],
          dup["view"]["members"])

    # -- seats are capped --------------------------------------------------
    status, four = c.post("join", {"v": 1, "room": code, "nick": "ace"})
    check("a fourth seat may join", status == 200, four)
    expect("a fifth may not", c.post("join", {"v": 1, "room": code, "nick": "nn"}),
           409, "room_full")

    # -- leaving -----------------------------------------------------------
    hcur = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                           "cursor": hcur, "wait": 0})[1]["cursor"]
    status, _ = c.post("leave", {"v": 1, "room": code, "token": dup["token"]})
    check("a member can leave", status == 200)
    status, gone = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                   "cursor": hcur, "wait": 0})
    check("the room drops to three seats", len(gone["view"]["members"]) == 3,
          gone["view"]["members"])
    check("and a left event says who and why",
          any(e["kind"] == "left" and e["seat"] == 3 and e["why"] == "quit"
              for e in gone["events"]), gone["events"])
    hcur = gone["cursor"]

    # A playing seat leaving promotes the lowest spectator, and the promotion
    # arrives in the view rather than as a new event kind.
    status, _ = c.post("leave", {"v": 1, "room": code, "token": guest["token"]})
    check("a playing member can leave", status == 200)
    status, promoted = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                       "cursor": hcur, "wait": 0})
    seats = {m["seat"]: m for m in promoted["view"]["members"]}
    check("the lowest spectator is promoted into the free playing seat",
          seats.get(4, {}).get("playing") is True, promoted["view"]["members"])

    # -- resync ------------------------------------------------------------
    status, resync = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                     "cursor": 0, "wait": 0})
    check("a cursor of 0 on a busy room asks the client to replace its state",
          resync.get("resync") is True, resync)
    check("and the answer carries the retained chat",
          all(e["kind"] == "chat" for e in resync["events"])
          and len(resync["events"]) >= 3, resync["events"])

    # -- the host leaving closes the room ----------------------------------
    last = c.post("join", {"v": 1, "room": code, "nick": "watcher"})[1]
    status, _ = c.post("leave", {"v": 1, "room": code, "token": host["token"]})
    check("the host can leave", status == 200)
    status, closed = c.post("poll", {"v": 1, "room": code, "token": last["token"],
                                     "cursor": last["cursor"], "wait": 0})
    check("the room is closed, and the reason survives for the members",
          status == 200 and closed["view"]["state"] == "closed"
          and any(e["kind"] == "closed" and e["why"] == "host_left"
                  for e in closed["events"]), (status, closed))
    expect("a closed room refuses a new join",
           c.post("join", {"v": 1, "room": code, "nick": "late"}),
           404, "no_such_room")

    selftest_match(c)
    selftest_rematch(c)
    selftest_kick(c)
    return code


def selftest_match(c):
    """Stage B + C over the wire: arm, ready, the per-member go plan, result,
    and a rematch with a fresh comms code."""
    # -- start refusals, each in its own fresh room ----------------------
    status, host = c.post("create", {"v": 1, "nick": "tango", "pre_ok": True})
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "opie",
                                    "pre_ok": True})
    expect("a non-host cannot start",
           c.post("start", {"v": 1, "room": code, "token": guest["token"]}),
           403, "not_host")

    status, solo = c.post("create", {"v": 1, "nick": "alone", "pre_ok": True})
    expect("start with one playing seat is refused",
           c.post("start", {"v": 1, "room": solo["room"],
                            "token": solo["token"]}), 409, "not_enough_players")
    c.post("leave", {"v": 1, "room": solo["room"], "token": solo["token"]})

    status, h2 = c.post("create", {"v": 1, "nick": "h", "pre_ok": True})
    c.post("join", {"v": 1, "room": h2["room"], "nick": "g", "pre_ok": False})
    expect("start with a playing seat not pre_ok is refused",
           c.post("start", {"v": 1, "room": h2["room"], "token": h2["token"]}),
           409, "member_not_ready")
    c.post("leave", {"v": 1, "room": h2["room"], "token": h2["token"]})

    # -- the happy arm ---------------------------------------------------
    status, armed = c.post("start", {"v": 1, "room": code,
                                     "token": host["token"]})
    check("start arms the match", status == 200 and len(armed.get("match", "")) == 16,
          armed)
    match1 = armed["match"]
    status, seen = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                   "cursor": guest["cursor"], "wait": 0})
    arm_ev = [e for e in seen["events"] if e["kind"] == "arming"]
    check("the guest is told the room is arming, with a deadline",
          len(arm_ev) == 1 and arm_ev[0]["match"] == match1
          and arm_ev[0]["deadline_ms"] > 0, seen.get("events"))
    check("the view says state=arming", seen["view"]["state"] == "arming")

    # -- ready, and the per-member go plan -------------------------------
    status, _ = c.post("ready", {"v": 1, "room": code, "token": host["token"],
                                 "match": match1})
    check("the host readies", status == 200)
    status, hostpoll = c.post("poll", {"v": 1, "room": code,
                                       "token": host["token"],
                                       "cursor": armed["cursor"], "wait": 0})
    check("one ready is not enough: still arming",
          hostpoll["view"]["state"] == "arming", hostpoll["view"])
    status, _ = c.post("ready", {"v": 1, "room": code, "token": guest["token"],
                                 "match": match1})
    check("the guest readies -> go", status == 200)

    status, gp = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                 "cursor": seen["cursor"], "wait": 0})
    go = [e for e in gp["events"] if e["kind"] == "go"]
    check("the guest's go event carries a child plan: slot 1, 1500 ms delay",
          len(go) == 1 and go[0]["plan"]["role"] == "child"
          and go[0]["plan"]["slot"] == 1
          and go[0]["plan"]["spawn_delay_ms"] == 1500, go)
    gplan = go[0]["plan"]
    status, hp = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                 "cursor": hostpoll["cursor"], "wait": 0})
    hgo = [e for e in hp["events"] if e["kind"] == "go"]
    hplan = hgo[0]["plan"]
    check("the host's go event carries a parent plan: slot 0, no delay",
          hplan["role"] == "parent" and hplan["slot"] == 0
          and hplan["spawn_delay_ms"] == 0, hplan)
    check("both plans carry an 8-char comms code from the room alphabet, "
          "never a room code",
          len(gplan["code"]) == 8 and gplan["code"] == hplan["code"]
          and all(ch in ROOM_ALPHABET for ch in gplan["code"]), gplan["code"])
    check("both plans force players=2 (the join-race guarantee)",
          gplan["players"] == 2 and hplan["players"] == 2)
    check("both plans carry the byte-identical names 'tango,opie,,'",
          gplan["names"] == hplan["names"] == "tango,opie,,", gplan["names"])
    check("both plans carry the same relay address",
          gplan["relay"] == hplan["relay"] and ":" in gplan["relay"])

    # -- failed, in a fresh room -----------------------------------------
    status, fh = c.post("create", {"v": 1, "nick": "fh", "pre_ok": True})
    fcode = fh["room"]
    status, fg = c.post("join", {"v": 1, "room": fcode, "nick": "fg",
                                 "pre_ok": True})
    status, fa = c.post("start", {"v": 1, "room": fcode, "token": fh["token"]})
    status, ff = c.post("failed", {"v": 1, "room": fcode, "token": fg["token"],
                                   "match": fa["match"], "reason": "no_pairing"})
    check("a failed is accepted", status == 200, ff)
    status, fp = c.post("poll", {"v": 1, "room": fcode, "token": fh["token"],
                                 "cursor": fa["cursor"], "wait": 0})
    check("a failed event returns the room to lobby with the reason",
          fp["view"]["state"] == "lobby"
          and any(e["kind"] == "failed" and e["reason"] == "no_pairing"
                  for e in fp["events"]), fp.get("events"))
    c.post("leave", {"v": 1, "room": fcode, "token": fh["token"]})
    c.post("leave", {"v": 1, "room": code, "token": host["token"]})

    selftest_arming_roster(c)
    selftest_only_playing(c)
    selftest_preflight(c)


def selftest_preflight(c):
    """A member correcting its own pre-flight in place, over the wire.

    The live failure: a player who unpacked their ROM with the Multiplayer
    window open was stuck "not ready" until they closed it, reopened it, made a
    new room and rejoined.
    """
    status, host = c.post("create", {"v": 1, "nick": "fh", "pre_ok": True})
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "fg",
                                    "pre_ok": False})
    check("the guest joins not-ready",
          guest["view"]["members"][1]["pre_ok"] is False,
          guest["view"]["members"])
    expect("so the host cannot start",
           c.post("start", {"v": 1, "room": code, "token": host["token"]}),
           409, "member_not_ready")

    status, up = c.post("preflight", {"v": 1, "room": code,
                                      "token": guest["token"], "pre_ok": True})
    check("the guest can say it is ready now, in place", status == 200, up)

    status, seen = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                   "cursor": host["cursor"], "wait": 0})
    check("the HOST's roster shows the guest ready, with no rejoin",
          seen["view"]["members"][1]["pre_ok"] is True, seen["view"]["members"])
    check("and an event carried it, so a long poll would have woken",
          any(e["kind"] == "preflight" and e["seat"] == 2
              and e["pre_ok"] is True for e in seen["events"]), seen["events"])

    status, _ = c.post("start", {"v": 1, "room": code, "token": host["token"]})
    check("NOW the host can start, no new room needed", status == 200)

    # Grammar and authority.
    expect("a non-boolean pre_ok is refused",
           c.post("preflight", {"v": 1, "room": code, "token": host["token"],
                                "pre_ok": "yes"}), 400, "bad_field")
    expect("there is no seat argument: a member speaks only for itself",
           c.post("preflight", {"v": 1, "room": code, "token": host["token"],
                                "pre_ok": True, "seat": 2}), 400, "bad_field")
    expect("a token holding no seat is refused",
           c.post("preflight", {"v": 1, "room": code, "token": "0" * 32,
                                "pre_ok": True}), 403, "not_a_member")
    c.post("leave", {"v": 1, "room": code, "token": host["token"]})


def selftest_arming_roster(c):
    """A roster that moves during arming must not reach go, over the wire.

    The plan freezes at start, so "everybody said they can" has to mean the seats
    that were frozen. A guest closing its launcher during the twenty-second
    countdown is ordinary behaviour.
    """
    status, host = c.post("create", {"v": 1, "nick": "ah", "pre_ok": True})
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "ag",
                                    "pre_ok": True})
    status, armed = c.post("start", {"v": 1, "room": code, "token": host["token"]})
    match = armed["match"]
    status, _ = c.post("leave", {"v": 1, "room": code, "token": guest["token"]})
    check("a guest can leave during the arming window", status == 200)
    status, _ = c.post("ready", {"v": 1, "room": code, "token": host["token"],
                                 "match": match})
    status, seen = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                   "cursor": armed["cursor"], "wait": 0})
    check("the host's own ready does NOT send the depleted room to go",
          seen["view"]["state"] == "arming", seen["view"]["state"])
    check("and no go event was ever pushed",
          not any(e["kind"] == "go" for e in seen.get("events", [])),
          seen.get("events"))
    check("the room holds one member, so no plan for a departed player exists",
          len(seen["view"]["members"]) == 1, seen["view"]["members"])
    c.post("leave", {"v": 1, "room": code, "token": host["token"]})


def selftest_only_playing(c):
    """ready / failed / result are restricted to a seated PLAYING member."""
    status, host = c.post("create", {"v": 1, "nick": "ph", "pre_ok": True})
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "pg",
                                    "pre_ok": True})
    status, spec = c.post("join", {"v": 1, "room": code, "nick": "ps",
                                   "pre_ok": True})
    check("the third seat is a spectator",
          spec["view"]["members"][2]["playing"] is False,
          spec["view"]["members"])
    status, armed = c.post("start", {"v": 1, "room": code, "token": host["token"]})
    match = armed["match"]

    expect("a spectator cannot ready",
           c.post("ready", {"v": 1, "room": code, "token": spec["token"],
                            "match": match}), 403, "not_playing")
    expect("a spectator cannot abort the match with failed",
           c.post("failed", {"v": 1, "room": code, "token": spec["token"],
                             "match": match, "reason": "no_pairing"}),
           403, "not_playing")

    # Take it live, then try to end it as the spectator.
    c.post("ready", {"v": 1, "room": code, "token": host["token"], "match": match})
    c.post("ready", {"v": 1, "room": code, "token": guest["token"], "match": match})
    expect("a spectator cannot end the match with result, nor write the scoreline",
           c.post("result", {"v": 1, "room": code, "token": spec["token"],
                             "match": match, "win": "star-target",
                             "scores": [9, 0, 0, 0]}), 403, "not_playing")
    status, seen = c.post("poll", {"v": 1, "room": code, "token": host["token"],
                                   "cursor": armed["cursor"], "wait": 0})
    check("the match is still the room's own, unaborted and unresolved",
          seen["view"]["state"] in ("go", "in_match")
          and seen["view"]["match"] == match, seen["view"])
    check("no result event was pushed by the spectator",
          not any(e["kind"] == "result" for e in seen.get("events", [])),
          seen.get("events"))

    status, _ = c.post("result", {"v": 1, "room": code, "token": guest["token"],
                                  "match": match, "win": "time-up",
                                  "scores": [1, 2, 0, 0]})
    check("a PLAYING member's result still ends it", status == 200)
    c.post("leave", {"v": 1, "room": code, "token": host["token"]})


def selftest_rematch(c):
    """Stage C over the wire: a match plays to a result, the room comes back to
    lobby with the params intact, and a rematch uses a FRESH comms code."""
    status, host = c.post("create", {"v": 1, "nick": "tango", "pre_ok": True})
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "opie",
                                    "pre_ok": True})
    c.post("params", {"v": 1, "room": code, "token": host["token"],
                      "map": 3, "win_mode": "stars", "star_target": 3})
    status, armed = c.post("start", {"v": 1, "room": code, "token": host["token"]})
    match1 = armed["match"]
    c.post("ready", {"v": 1, "room": code, "token": host["token"], "match": match1})
    c.post("ready", {"v": 1, "room": code, "token": guest["token"], "match": match1})
    status, gp = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                 "cursor": guest["cursor"], "wait": 0})
    go = [e for e in gp["events"] if e["kind"] == "go"]
    code1 = go[0]["plan"]["code"]

    # A result validates its fields.
    expect("a bad win value is refused",
           c.post("result", {"v": 1, "room": code, "token": host["token"],
                             "match": match1, "win": "nope",
                             "scores": [3, 0, 0, 0]}), 400, "bad_win")
    expect("scores that are not four ints are refused",
           c.post("result", {"v": 1, "room": code, "token": host["token"],
                             "match": match1, "win": "time-up",
                             "scores": [3, 0, 0]}), 400, "bad_scores")

    # The real result.
    status, res = c.post("result", {"v": 1, "room": code, "token": host["token"],
                                    "match": match1, "win": "star-target",
                                    "scores": [3, 1, 0, 0]})
    check("a result is accepted", status == 200, res)
    status, back = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                   "cursor": gp["cursor"], "wait": 0})
    res_ev = [e for e in back["events"] if e["kind"] == "result"]
    check("a result event reaches the room with win and scores",
          len(res_ev) == 1 and res_ev[0]["win"] == "star-target"
          and res_ev[0]["scores"] == [3, 1, 0, 0], back.get("events"))
    check("the room is back in lobby, roster intact, params intact",
          back["view"]["state"] == "lobby"
          and len(back["view"]["members"]) == 2
          and back["view"]["map"] == 3
          and back["view"]["star_target"] == 3, back["view"])
    expect("a second result for the finished match is idempotent, not stale",
           c.post("result", {"v": 1, "room": code, "token": guest["token"],
                             "match": match1, "win": "star-target",
                             "scores": [3, 1, 0, 0]}), 200)

    # The rematch, and THE FRESH COMMS CODE.
    status, armed2 = c.post("start", {"v": 1, "room": code, "token": host["token"]})
    match2 = armed2["match"]
    check("a rematch arms with a different match id", match2 != match1, match2)
    c.post("ready", {"v": 1, "room": code, "token": host["token"], "match": match2})
    c.post("ready", {"v": 1, "room": code, "token": guest["token"], "match": match2})
    status, gp2 = c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                                  "cursor": back["cursor"], "wait": 0})
    go2 = [e for e in gp2["events"] if e["kind"] == "go"]
    check("THE REMATCH USES A DIFFERENT COMMS CODE than the first match",
          go2 and go2[0]["plan"]["code"] != code1,
          (code1, go2[0]["plan"]["code"] if go2 else None))
    # A draw result is accepted (the launcher posts it for an unparseable marker).
    status, dr = c.post("result", {"v": 1, "room": code, "token": host["token"],
                                   "match": match2, "win": "draw",
                                   "scores": [0, 0, 0, 0]})
    check("a draw result is accepted and returns to lobby",
          status == 200, dr)
    c.post("leave", {"v": 1, "room": code, "token": host["token"]})


def selftest_kick(c):
    """The host removing somebody, over the wire.

    LAST, and in a room of its own. The kick cooldown is keyed on the client
    address, and every client in this harness is 127.0.0.1, so a kick anywhere
    earlier would lock this process out of the room it was testing. Kick
    records are per room, so a fresh room contains the damage.
    """
    status, host = c.post("create", {"v": 1, "nick": "hostk"})
    check("a room for the kick test", status == 200, host)
    code = host["room"]
    status, guest = c.post("join", {"v": 1, "room": code, "nick": "rude"})
    check("somebody to remove", status == 200, guest)

    expect("a member cannot kick",
           c.post("kick", {"v": 1, "room": code, "token": guest["token"],
                           "seat": 1}), 403, "not_host")
    expect("the host cannot kick itself",
           c.post("kick", {"v": 1, "room": code, "token": host["token"],
                           "seat": 1}), 400, "bad_seat")
    expect("an empty seat cannot be kicked",
           c.post("kick", {"v": 1, "room": code, "token": host["token"],
                           "seat": 4}), 400, "bad_seat")

    status, out = c.post("kick", {"v": 1, "room": code,
                                  "token": host["token"], "seat": 2})
    check("the host can remove a member", status == 200, out)
    status, seen = c.post("poll", {"v": 1, "room": code,
                                   "token": host["token"],
                                   "cursor": host["cursor"], "wait": 0})
    check("the roster drops to one seat", len(seen["view"]["members"]) == 1,
          seen["view"]["members"])
    check("and a left event says it was a kick",
          any(e["kind"] == "left" and e["seat"] == 2 and e["why"] == "kicked"
              for e in seen["events"]), seen["events"])

    expect("the removed launcher is told why, not just refused",
           c.post("poll", {"v": 1, "room": code, "token": guest["token"],
                           "cursor": 0, "wait": 0}), 403, "kicked")
    expect("and it cannot walk straight back in with the same code",
           c.post("join", {"v": 1, "room": code, "nick": "rude"}),
           403, "kicked")

    c.post("leave", {"v": 1, "room": code, "token": host["token"]})


# ----------------------------------------------------------------- negatives
#
# The stage A proof list, each one a real request and its real answer.


def negatives(c, out_dir=None):
    """Every refusal the stage A proof list names, plus the rest of the
    transport law, each one a real request and its real answer.

    IT PACES ITSELF ON PURPOSE. The shipped configuration refuses a sender
    outright for ten seconds once it has been refused ten times inside ten
    seconds, which is exactly what a suite of nothing but refusals looks like.
    So each refusal below waits a beat first, and the bad-sender rule gets its
    own capture rather than corrupting every other one.
    """
    captures = []
    PACE = 1.3

    st, host = c.post("create", {"v": 1, "nick": "tango"})
    if st != 200:
        raise SystemExit("could not create a room for the negatives: %s" % host)
    room = host["room"]
    st, guest = c.post("join", {"v": 1, "room": room, "nick": "opie"})
    if st != 200:
        raise SystemExit("could not join: %s" % guest)

    def cap(name, req, sent, status, answer, note=""):
        captures.append({"name": name, "request": req, "sent": sent,
                         "status": status, "answer": answer, "note": note})

    def rej(name, verb, obj, want_status, want_err, note, raw_body=None,
            method="POST", path=None, **kw):
        """One paced refusal: send it, assert it, capture it."""
        time.sleep(PACE)
        body = raw_body if raw_body is not None else json.dumps(obj).encode()
        target = path or (PREFIX + "/" + verb)
        status, text = c.raw(method, target, body, **kw)
        try:
            parsed = json.loads(text)
        except ValueError:
            parsed = {"_raw": text}
        ok = status == want_status and (want_err is None
                                        or parsed.get("error") == want_err)
        check(name, ok, "got %s %s, wanted %s %s"
              % (status, parsed, want_status, want_err))
        shown = body.decode("utf-8", "replace") if body else ""
        if len(shown) > 300:
            shown = "<%d bytes>" % len(body)
        cap(name, "%s %s" % (method, target), shown, status, text, note)
        return status, parsed

    # ---- the seven the stage A proof list names --------------------------

    rej("unknown key -> 400 bad_field", "chat",
        {"v": 1, "room": room, "token": host["token"], "text": "hi",
         "colour": "red"}, 400, "bad_field",
        "strictness is deliberate. The launcher and the server ship together "
        "and are versioned together, so a field that appears without a version "
        "bump is a bug or an attack and there is no third option.")

    big = json.dumps({"v": 1, "room": room, "token": host["token"],
                      "text": "x" * 5000}).encode()
    rej("a 5 KB body -> 413", "chat", None, 413, "too_large",
        "refused on the announced Content-Length, before a byte of the body is "
        "read. The announced body is still drained off the socket first, or "
        "the next request on this keep-alive connection would parse the middle "
        "of it as a request line.", raw_body=big)

    rej("POST /port/lobby/xyz -> 404 unknown_verb", "xyz", {"v": 1},
        404, "unknown_verb",
        "the verb list is closed. The server writes one log line for this and "
        "nothing else.")

    rej("a 17 byte nickname -> 400", "create", {"v": 1, "nick": "a" * 17},
        400, "too_long",
        "a string field's LENGTH IN BYTES is checked before its content, so an "
        "oversized field never reaches a grammar loop at all. Section 3.0 of "
        "the spec gives that rule its own code, too_long, rather than the "
        "field's own bad_nick.")

    rej("a chat line with a 0x07 byte in it -> 400 bad_text", "chat",
        {"v": 1, "room": room, "token": host["token"],
         "text": "bell" + chr(7) + "here"}, 400, "bad_text",
        "chat is 0x20..0x7E and nothing else, so the launcher can render it "
        "into a read-only TextBox as plain text with no control characters, no "
        "markup and no link detection to consider.")

    rej("a member's token on params -> 403 not_host", "params",
        {"v": 1, "room": room, "token": guest["token"], "map": 2,
         "win_mode": "time"}, 403, "not_host",
        "the room code is only permission to walk in. The 128 bit member token "
        "is what proves which seat you are and whether you are the host.")

    # The rate limit, and the bad-sender rule behind it. Deliberately last of
    # the seven, because it leaves this address in the penalty box.
    codes = {}
    t0 = time.time()
    for _ in range(200):
        st, _tx = c.raw("POST", PREFIX + "/poll",
                        json.dumps({"v": 1, "room": room,
                                    "token": host["token"],
                                    "cursor": 0, "wait": 0}).encode())
        codes[st] = codes.get(st, 0) + 1
        if time.time() - t0 > 10:
            break
    dt = time.time() - t0
    check("200 requests in 10 s from one address -> 429", codes.get(429, 0) > 0,
          codes)
    check("and the bad-sender rule then refuses that address outright",
          codes.get(429, 0) >= 10, codes)
    cap("rate limit and the bad-sender rule",
        "POST %s/poll x%d" % (PREFIX, sum(codes.values())),
        "%d polls in %.1f s" % (sum(codes.values()), dt),
        429, json.dumps(codes),
        "status counts across the burst. The limiter is a token bucket at 20 "
        "requests a second with a burst of 40; on top of it, a sender refused "
        "ten times inside ten seconds is refused outright for the next ten, "
        "the same shape relay.py uses.")
    print("   (waiting out the bad-sender window)")
    time.sleep(12)

    # ---- the rest of the transport law -----------------------------------

    rej("no Content-Length -> 411", "create", {"v": 1, "nick": "x"},
        411, "length_required",
        "the length is parsed before any rejection is written. A rejection "
        "that does not know how many bytes are owed cannot drain them, and an "
        "undrained body makes one rejected request appear twice.",
        content_length="omit")

    rej("the wrong content type -> 415", "create", {"v": 1, "nick": "x"},
        415, "bad_content_type", "application/json, or nothing.",
        ctype="text/plain")

    rej("GET on a POST verb -> 404", "create", None, 404, "unknown_verb",
        "GET is served for /port/lobby/health and for nothing else.",
        raw_body=None, method="GET", ctype=None)

    rej("a query string -> 404", "poll", {"v": 1}, 404, "unknown_verb",
        "the URL is exactly /port/lobby/<verb>, never with a query string, so "
        "nothing a client sends can end up in Caddy's access log.",
        path=PREFIX + "/poll?room=X")

    rej("a body that is not JSON -> 400 bad_shape", "create", None,
        400, "bad_shape", "", raw_body=b"{not json")

    rej("an array at the top level -> 400 bad_shape", "create", None,
        400, "bad_shape", "one object, never an array, never a bare value.",
        raw_body=b'[{"v":1}]')

    rej("a version that is not 1 -> 400 bad_version", "create",
        {"v": 2, "nick": "x"}, 400, "bad_version",
        "adding a field is a version bump, so the version is checked before "
        "the fields are.")

    rej("a missing version -> 400 bad_version", "create", {"nick": "x"},
        400, "bad_version", "")

    rej("a lowercase room code -> 400 bad_room", "join",
        {"v": 1, "room": "abc123", "nick": "x"}, 400, "bad_room",
        "six characters of ABCDEFGHJKMNPQRSTUVWXYZ23456789, which carries no "
        "I, no L, no O, no 0 and no 1, because the code is read aloud.")

    rej("an unused room code -> 404 no_such_room", "join",
        {"v": 1, "room": "ZZZZZZ", "nick": "x"}, 404, "no_such_room", "")

    rej("a well formed token nobody holds -> 403", "chat",
        {"v": 1, "room": room, "token": "f" * 32, "text": "hi"},
        403, "not_a_member",
        "the token is 128 bits of system randomness; guessing one is not a "
        "thing that happens.")

    rej("a comma in a nickname -> 400 bad_nick", "create",
        {"v": 1, "nick": "ta,ngo"}, 400, "bad_nick",
        "comma separates the four nicknames in the one environment variable "
        "that carries them into the game, so it is barred from a nickname and "
        "from nowhere else. Chat keeps its commas.")

    rej("a leading space in a nickname -> 400 bad_nick", "create",
        {"v": 1, "nick": " tango"}, 400, "bad_nick", "")

    rej("a nickname that is not a string -> 400 bad_nick", "create",
        {"v": 1, "nick": 7}, 400, "bad_nick", "")

    rej("a missing required key -> 400 bad_field", "chat",
        {"v": 1, "room": room, "token": host["token"]}, 400, "bad_field", "")

    rej("a 201 byte chat line -> 400 too_long", "chat",
        {"v": 1, "room": room, "token": host["token"], "text": "y" * 201},
        400, "too_long", "200 bytes, counted as bytes and not as characters.")

    rej("star_target under win_mode time -> 400", "params",
        {"v": 1, "room": room, "token": host["token"], "map": 0,
         "win_mode": "time", "star_target": 3}, 400, "bad_star_target",
        "required if and only if the win mode is stars. One shape, not two.")

    rej("a map outside 0..3 -> 400 bad_map", "params",
        {"v": 1, "room": room, "token": host["token"], "map": 9,
         "win_mode": "time"}, 400, "bad_map", "the ROM has four VS maps.")

    rej("a member's token on kick -> 403 not_host", "kick",
        {"v": 1, "room": room, "token": guest["token"], "seat": 1},
        403, "not_host",
        "kick is host-only, the same way params is. Removing somebody is the "
        "owner's answer to the spec's open question 4.")

    rej("the host kicking itself -> 400 bad_seat", "kick",
        {"v": 1, "room": room, "token": host["token"], "seat": 1},
        400, "bad_seat",
        "the host closes the room instead; there is no way to leave a room "
        "hostless.")

    # ---- the chat limiter, which is per MEMBER rather than per address ----
    print("   (letting the chat allowance refill)")
    time.sleep(8)
    got = []
    for i in range(6):
        st, _ = c.raw("POST", PREFIX + "/chat",
                      json.dumps({"v": 1, "room": room, "token": host["token"],
                                  "text": "flood %d" % i}).encode())
        got.append(st)
    check("chat flooding is refused once the burst is spent",
          got.count(200) <= 4 and 429 in got, got)
    cap("chat flood", "POST %s/chat x6" % PREFIX, "six lines with no pause",
        429, json.dumps(got),
        "one line per two seconds per MEMBER with three saved up. The launcher "
        "greys its Send button for two seconds rather than ever showing this "
        "to a player.")

    c.post("leave", {"v": 1, "room": room, "token": host["token"]})

    if out_dir:
        write_captures(captures, out_dir)
    return captures


def write_captures(captures, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    index = []
    for i, capture in enumerate(captures, 1):
        safe = "".join(ch if ch.isalnum() else "_" for ch in capture["name"])
        name = "%02d_%s.txt" % (i, safe[:60])
        with open(os.path.join(out_dir, name), "w", encoding="utf-8") as f:
            f.write("NEGATIVE PROOF: %s\n" % capture["name"])
            f.write("=" * 70 + "\n\n")
            f.write("REQUEST\n  %s\n" % capture["request"])
            f.write("  body: %s\n\n" % capture["sent"])
            f.write("ANSWER\n  HTTP %s\n  %s\n" % (capture["status"],
                                                      capture["answer"]))
            if capture["note"]:
                f.write("\nWHY\n  %s\n" % capture["note"])
        index.append("%-52s HTTP %s" % (capture["name"], capture["status"]))
    with open(os.path.join(out_dir, "00_index.txt"), "w", encoding="utf-8") as f:
        f.write("sm64ds-lobby stage A negative proofs\n")
        f.write("captured %s\n\n"
                % time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()))
        f.write("\n".join(index) + "\n")
    print("\nwrote %d files to %s" % (len(captures) + 1, out_dir))


# ---------------------------------------------------------------------- soak


def soak(c, seconds, members):
    status, host = c.post("create", {"v": 1, "nick": "soakhost"})
    if status != 200:
        raise SystemExit("could not create a room: %s" % host)
    room = host["room"]
    people = [host]
    for i in range(members - 1):
        st, m = c.post("join", {"v": 1, "room": room, "nick": "soak%d" % (i + 2)})
        if st != 200:
            raise SystemExit("could not join: %s" % m)
        people.append(m)

    stop = threading.Event()
    counts = {"poll": 0, "chat": 0, "err": 0}
    lock = threading.Lock()

    def worker(me):
        cursor = me["cursor"]
        while not stop.is_set():
            try:
                st, body = c.post("poll", {"v": 1, "room": room,
                                           "token": me["token"],
                                           "cursor": cursor, "wait": 5})
                if st == 200:
                    cursor = body["cursor"]
                    with lock:
                        counts["poll"] += 1
                else:
                    with lock:
                        counts["err"] += 1
                if random.random() < 0.25:
                    st, _ = c.post("chat", {"v": 1, "room": room,
                                            "token": me["token"],
                                            "text": "soak %.3f" % time.time()})
                    with lock:
                        if st == 200:
                            counts["chat"] += 1
                time.sleep(random.uniform(0.5, 2.5))
            except OSError:
                with lock:
                    counts["err"] += 1
                time.sleep(0.5)

    threads = [threading.Thread(target=worker, args=(m,)) for m in people]
    for t in threads:
        t.start()
    t0 = time.time()
    while time.time() - t0 < seconds:
        time.sleep(2)
        print("  %5.0fs  %s  health=%s" % (time.time() - t0, counts,
                                           c.get("health")[1]))
    stop.set()
    for t in threads:
        t.join(timeout=15)
    c.post("leave", {"v": 1, "room": room, "token": host["token"]})
    print("soak done: %s" % counts)
    return counts["err"] == 0


# ---------------------------------------------------------------------- main


def parse_url(url):
    """http[s]://host[:port][/prefix] -> (host, port, prefix, tls)."""
    tls = url.startswith("https://")
    rest = url.split("://", 1)[1] if "://" in url else url
    if "/" in rest:
        hostport, path = rest.split("/", 1)
        prefix = "/" + path.rstrip("/")
    else:
        hostport, prefix = rest, PREFIX
    if ":" in hostport:
        host, port = hostport.rsplit(":", 1)
        port = int(port)
    else:
        host, port = hostport, 443 if tls else 80
    return host, port, prefix, tls


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=("selftest", "soak", "negatives"))
    ap.add_argument("--url", default=None,
                    help="a server already running; default is to spawn one")
    ap.add_argument("--seconds", type=int, default=30)
    ap.add_argument("--members", type=int, default=3)
    ap.add_argument("--out", default=None, help="where to write captures")
    ap.add_argument("--quick", action="store_true")
    args = ap.parse_args()

    spawned = None
    if args.url:
        host, port, prefix, tls = parse_url(args.url)
        c = Client(host, port, prefix, tls)
    else:
        port = pick_port()
        print("spawning a server on 127.0.0.1:%d" % port)
        extra = {}
        if args.mode == "selftest":
            # The selftest exercises the HAPPY PATH at full speed from one
            # address, which is exactly what the request rate limiter and the
            # bad-sender rule exist to shed. Neither is what the selftest tests
            # (the negatives and test_security cover them at shipped values), so
            # both are lifted on the selftest's OWN spawned server. This keeps
            # the primary run fast and deterministic; a `selftest --url` run
            # against a real server cannot set these and leans on post()'s 429
            # backoff instead.
            extra["RATE_REQ_PER_S"] = "1000000"
            extra["RATE_BURST"] = "1000000"
            extra["BAD_LIMIT"] = "1000000"
            print("  (rate limits lifted on the selftest's own server; "
                  "`negatives` and `test_security` prove them at shipped values)")
        spawned = Spawned(port, extra)
        c = spawned.wait_ready()

    try:
        if args.mode == "selftest":
            selftest(c, quick=args.quick)
        elif args.mode == "negatives":
            negatives(c, args.out)
        else:
            soak(c, args.seconds, args.members)
    finally:
        if spawned:
            spawned.stop()

    print("\n%d passed, %d failed" % (len(PASSED), len(FAILED)))
    if FAILED:
        for name in FAILED:
            print("  FAILED: %s" % name)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
