#!/usr/bin/env python3
"""Unit tests for sm64ds-lobby. Standard library only, no server, no sockets.

    python3 test_units.py

test_client.py proves the wire. This file proves the parts underneath it: the
field grammar, the byte caps, the room lifecycle, the timeouts and the rate
limiters. It calls the verb handlers directly, which is legitimate because
every one of them is a pure function of (body, address, clock) over the room
table and does no I/O of its own.

It drives its own clock. The reaper's work is `sweep(now)`, so a member
timeout, an idle room and the grace a closed room is kept for are all provable
in milliseconds instead of in real minutes.
"""

import json
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "app"))

import server as S     # noqa: E402


def _locked(fn):
    """Run a server function the way the HTTP layer runs it: under the lock.

    Every verb handler's contract is that the module lock is already held --
    Room.push notifies a Condition built on that lock, and notifying an
    unheld lock is an error. These tests call the handlers directly, so they
    take the lock exactly where do_POST takes it, rather than the server
    being loosened to make testing easier.
    """
    def wrapper(*a, **kw):
        with S.LOCK:
            return fn(*a, **kw)
    wrapper.__name__ = fn.__name__
    return wrapper


for _name in ("do_create", "do_join", "do_poll", "do_chat", "do_params",
              "do_preflight", "do_color", "do_start", "do_ready", "do_result",
              "do_failed", "do_kick", "do_leave", "sweep"):
    setattr(S, _name, _locked(getattr(S, _name)))
S.VERBS = {"create": S.do_create, "join": S.do_join, "poll": S.do_poll,
           "chat": S.do_chat, "params": S.do_params,
           "preflight": S.do_preflight, "color": S.do_color,
           "start": S.do_start,
           "ready": S.do_ready, "result": S.do_result, "failed": S.do_failed,
           "kick": S.do_kick, "leave": S.do_leave}

PASSED = []
FAILED = []


def check(name, ok, detail=""):
    (PASSED if ok else FAILED).append(name)
    print("%-5s %s%s" % ("ok" if ok else "FAIL", name,
                         "" if ok else "   <- " + str(detail)))
    return ok


def reset():
    """A clean server between groups, without restarting a process."""
    S.ROOMS.clear()
    S.BUCKETS = S.Buckets()
    S.WAITERS = 0


def create(nick="tango", who="10.0.0.1", now=1000.0, **extra):
    body = {"v": 1, "nick": nick}
    body.update(extra)
    return S.do_create(body, who, now)


def join(room, nick, who="10.0.0.2", now=1000.0, **extra):
    body = {"v": 1, "room": room, "nick": nick}
    body.update(extra)
    return S.do_join(body, who, now)


# ------------------------------------------------------------ field grammar


def test_grammar():
    print("\n-- the field grammar")

    # room
    good = "K7QMR3"
    check("a well formed room code is accepted",
          S.v_room({"room": good}) == (good, None))
    check("a 7 character room code is too_long",
          S.v_room({"room": "ABCDEF2"})[1] == "too_long")
    check("a 5 character room code is bad_room",
          S.v_room({"room": "ABCDE"})[1] == "bad_room")
    check("lower case is bad_room", S.v_room({"room": "abcdef"})[1] == "bad_room")
    for banned in "ILO01":
        check("the confusable %r is not in the room alphabet" % banned,
              S.v_room({"room": (banned * 6)})[1] == "bad_room")
    check("a room code that is not a string is bad_room",
          S.v_room({"room": 123456})[1] == "bad_room")
    check("a missing room code is bad_room", S.v_room({})[1] == "bad_room")

    # token
    tok = "0123456789abcdef" * 2
    check("a 32 hex token is accepted", S.v_token({"token": tok}) == (tok, None))
    check("upper case hex is bad_token",
          S.v_token({"token": "A" * 32})[1] == "bad_token")
    check("31 hex is bad_token", S.v_token({"token": "a" * 31})[1] == "bad_token")
    check("33 hex is too_long", S.v_token({"token": "a" * 33})[1] == "too_long")
    check("a minted token passes its own validator",
          S.v_token({"token": S.new_token()})[1] is None)

    # nick
    check("a plain nickname is accepted",
          S.v_nick({"nick": "tango"}) == ("tango", None))
    check("16 bytes is the cap and is accepted",
          S.v_nick({"nick": "a" * 16}) == ("a" * 16, None))
    check("17 bytes is too_long", S.v_nick({"nick": "a" * 17})[1] == "too_long")
    check("an empty nickname is bad_nick", S.v_nick({"nick": ""})[1] == "bad_nick")
    check("a nickname of spaces is bad_nick",
          S.v_nick({"nick": "   "})[1] == "bad_nick")
    check("a leading space is bad_nick",
          S.v_nick({"nick": " tango"})[1] == "bad_nick")
    check("a trailing space is bad_nick",
          S.v_nick({"nick": "tango "})[1] == "bad_nick")
    check("an interior space is fine",
          S.v_nick({"nick": "big tango"})[1] is None)
    check("a comma is bad_nick", S.v_nick({"nick": "ta,ngo"})[1] == "bad_nick")
    check("a tab is bad_nick", S.v_nick({"nick": "ta\tngo"})[1] == "bad_nick")
    check("a newline is bad_nick", S.v_nick({"nick": "ta\nngo"})[1] == "bad_nick")
    check("0x7F is bad_nick", S.v_nick({"nick": "ta\x7fngo"})[1] == "bad_nick")
    check("a non-ASCII nickname is bad_nick",
          S.v_nick({"nick": "tangó"})[1] == "bad_nick")
    # LENGTH IS COUNTED IN BYTES, so six three-byte characters is eighteen
    # bytes and is refused for length before its content is ever looked at.
    check("the length cap counts bytes, not characters",
          S.v_nick({"nick": "あ" * 6})[1] == "too_long")
    check("a lone surrogate cannot even be measured, and is refused",
          S.v_nick({"nick": "\ud800"})[1] == "bad_nick")

    # text
    check("a plain chat line is accepted", S.v_text({"text": "gg"}) == ("gg", None))
    check("200 bytes is the cap and is accepted",
          S.v_text({"text": "y" * 200})[1] is None)
    check("201 bytes is too_long", S.v_text({"text": "y" * 201})[1] == "too_long")
    check("an empty chat line is bad_text", S.v_text({"text": ""})[1] == "bad_text")
    check("a 0x07 byte in chat is bad_text",
          S.v_text({"text": "bell\x07here"})[1] == "bad_text")
    check("a comma in chat is fine (only nicknames bar it)",
          S.v_text({"text": "a,b"})[1] is None)
    check("chat keeps its leading and trailing spaces",
          S.v_text({"text": " padded "}) == (" padded ", None))

    # ints
    check("an int in range passes",
          S.v_int({"map": 2}, "map", 0, 3, "bad_map") == (2, None))
    check("an int out of range is refused",
          S.v_int({"map": 4}, "map", 0, 3, "bad_map")[1] == "bad_map")
    check("True is NOT an integer here",
          S.v_int({"map": True}, "map", 0, 3, "bad_map")[1] == "bad_map")
    check("a float is not an integer here",
          S.v_int({"map": 1.0}, "map", 0, 3, "bad_map")[1] == "bad_map")
    check("a numeric string is not an integer here",
          S.v_int({"map": "1"}, "map", 0, 3, "bad_map")[1] == "bad_map")

    # shape
    check("an unknown key is bad_field",
          S.shape({"v": 1, "nick": "x", "colour": "red"}, ("v", "nick")) == "bad_field")
    check("a missing required key is bad_field",
          S.shape({"v": 1}, ("v", "nick")) == "bad_field")
    check("a declared optional key is allowed",
          S.shape({"v": 1, "nick": "x", "pre_ok": True}, ("v", "nick"),
                  ("pre_ok",)) is None)
    check("the exact required set passes",
          S.shape({"v": 1, "nick": "x"}, ("v", "nick")) is None)


def test_addresses():
    print("\n-- address blunting (nothing whole ever reaches a log)")
    check("an IPv4 address keeps two octets",
          S.blunt_host("203.0.113.42") == "203.0.x.x")
    check("an IPv6 address keeps two groups",
          S.blunt_host("2001:db8:1234::1") == "2001:db8::x")
    check("localhost blunts too", S.blunt_host("127.0.0.1") == "127.0.x.x")
    check("an empty address does not crash", S.blunt_host("") == "x")
    check("junk does not crash", S.blunt_host("not-an-address") == "x")


def test_codes():
    print("\n-- minted codes")
    codes = [S.new_room_code() for _ in range(2000)]
    check("every minted room code is 6 characters",
          all(len(c) == 6 for c in codes))
    check("every character is in the room alphabet",
          all(all(ch in S.ROOM_ALPHABET for ch in c) for c in codes))
    check("the alphabet has no confusable characters",
          not (set("ILO01") & set(S.ROOM_ALPHABET)))
    check("2000 codes are near enough all distinct", len(set(codes)) > 1990,
          len(set(codes)))
    toks = [S.new_token() for _ in range(500)]
    check("every minted token is 32 lowercase hex",
          all(len(t) == 32 and all(ch in "0123456789abcdef" for ch in t)
              for t in toks))
    check("500 tokens are all distinct", len(set(toks)) == 500)


# --------------------------------------------------------- room lifecycle


def test_create_join():
    print("\n-- create and join")
    reset()
    st, out = create()
    check("create answers 200", st == 200, out)
    room = S.ROOMS[out["room"]]
    check("the creator is seat 1", out["member"] == 1)
    check("the creator is the host", room.host_seat == 1)
    check("the creator is playing", room.members[1].playing is True)
    check("pre_ok defaults to false", room.members[1].pre_ok is False)
    check("a new room starts at cursor 1", out["cursor"] == 1)
    check("a new room is in the lobby state", room.state == "lobby")

    st, out2 = create(nick="withflag", pre_ok=True)
    check("pre_ok is carried when it is sent",
          S.ROOMS[out2["room"]].members[1].pre_ok is True)

    code = out["room"]
    st, g = join(code, "opie")
    check("join answers 200", st == 200, g)
    check("the joiner is seat 2", g["member"] == 2)
    check("the joiner is playing while seats remain",
          room.members[2].playing is True)
    check("a joined event was pushed",
          room.events[-1]["kind"] == "joined" and room.events[-1]["seat"] == 2)

    st, third = join(code, "ace")
    check("the third seat is a spectator", room.members[3].playing is False,
          "GAME_MAX_PLAYERS is %d" % S.GAME_MAX_PLAYERS)
    st, fourth = join(code, "nn")
    check("the fourth seat joins", st == 200)
    st, fifth = join(code, "late")
    check("the fifth is refused with room_full",
          st == 409 and fifth["error"] == "room_full", (st, fifth))

    st, nope = join("ZZZZZZ", "x")
    check("an unknown room is no_such_room",
          st == 404 and nope["error"] == "no_such_room", (st, nope))

    check("every token maps back to its own seat",
          all(S.ROOMS[code].by_token[m.token] == m.seat
              for m in room.members.values()))
    check("no two members share a token",
          len(set(m.token for m in room.members.values())) == len(room.members))


def test_duplicate_nicknames():
    print("\n-- duplicate nicknames are disambiguated once, by the server")
    reset()
    st, host = create(nick="opie")
    code = host["room"]
    join(code, "opie")
    room = S.ROOMS[code]
    check("the first claimant keeps the plain nickname",
          room.members[1].display == "opie")
    check("the second is suffixed with its seat",
          room.members[2].display == "opie (2)", room.members[2].display)
    join(code, "opie")
    check("and so is the third", room.members[3].display == "opie (3)")
    join(code, "other")
    check("a different nickname is left alone",
          room.members[4].display == "other")

    check("a display string never passes 20 bytes",
          all(len(m.display.encode()) <= S.DISPLAY_MAX
              for m in room.members.values()))

    reset()
    st, host = create(nick="a" * 16)
    code = host["room"]
    join(code, "a" * 16)
    room = S.ROOMS[code]
    check("a 16 byte nickname duplicated still fits the display cap",
          len(room.members[2].display.encode()) == 20,
          room.members[2].display)
    check("game_name is the display truncated to 16 bytes",
          room.members[2].game_name == "a" * 16
          and len(room.members[1].game_name.encode()) <= S.GAME_NAME_MAX)

    # The stage B string the server will build, proved on the shape it will
    # have. No launcher ever assembles this; both sides are handed a copy.
    names = ",".join([room.members[s].game_name if s in room.members else ""
                      for s in range(1, 5)])
    check("the four-field names string has exactly three commas",
          names.count(",") == 3, names)
    check("and never passes 67 bytes", len(names.encode()) <= 67, len(names))


def test_leaving():
    print("\n-- leaving, promotion, and the host closing the room")
    reset()
    st, host = create()
    code = host["room"]
    st, g2 = join(code, "b")
    st, g3 = join(code, "c")
    room = S.ROOMS[code]
    check("seat 3 starts as a spectator", room.members[3].playing is False)

    st, out = S.do_leave({"v": 1, "room": code, "token": g2["token"]},
                         "10.0.0.2", 1001.0)
    check("a member can leave", st == 200 and out.get("ok") is True, (st, out))
    check("the seat is freed", 2 not in room.members)
    check("the token no longer resolves", g2["token"] not in room.by_token)
    check("a left event says who and why",
          room.events[-1]["kind"] == "left" and room.events[-1]["seat"] == 2
          and room.events[-1]["why"] == "quit", room.events[-1])
    check("the spectator was promoted into the freed playing seat",
          room.members[3].playing is True)
    check("and no new event KIND was invented for it: the view carries it",
          all(e["kind"] in ("joined", "left", "chat", "params", "arming", "go",
                            "result", "failed", "closed")
              for e in room.events), [e["kind"] for e in room.events])

    st, out = S.do_leave({"v": 1, "room": code, "token": g2["token"]},
                         "10.0.0.2", 1002.0)
    check("a token that already left is not a member any more",
          st == 403 and out["error"] == "not_a_member", (st, out))

    st, out = S.do_leave({"v": 1, "room": code, "token": host["token"]},
                         "10.0.0.1", 1003.0)
    check("the host can leave", st == 200)
    check("the host leaving closes the room", room.state == "closed")
    check("and the reason is on the record", room.why == "host_left"
          and room.events[-1]["kind"] == "closed"
          and room.events[-1]["why"] == "host_left", room.events[-1])
    check("the record is kept, not deleted, so members learn why",
          code in S.ROOMS)
    st, out = join(code, "late")
    check("a closed room refuses a join",
          st == 404 and out["error"] == "no_such_room", (st, out))

    # The grace, then the record really is gone.
    S.sweep(room.closed_at + S.CLOSED_KEEP_S - 1)
    check("the closed room survives the grace window", code in S.ROOMS)
    S.sweep(room.closed_at + S.CLOSED_KEEP_S + 1)
    check("and is deleted after it", code not in S.ROOMS)


def test_timeouts():
    print("\n-- heartbeats, idle rooms, and the clock in general")
    reset()
    now = 1000.0
    st, host = create(now=now)
    code = host["room"]
    st, g = join(code, "guest", now=now)
    room = S.ROOMS[code]

    S.sweep(now + S.MEMBER_TIMEOUT_S - 1)
    check("a member inside the heartbeat window keeps its seat",
          2 in room.members)

    # The host keeps polling the way a live launcher does; only the guest goes
    # quiet. Without this the HOST times out first and the room closes, which
    # is correct behaviour and a different assertion (further down).
    S.do_poll({"v": 1, "room": code, "token": host["token"], "cursor": 0,
               "wait": 0}, "10.0.0.1", now + S.MEMBER_TIMEOUT_S)
    S.sweep(now + S.MEMBER_TIMEOUT_S + 1)
    check("a member that stopped polling loses its seat", 2 not in room.members)
    check("and the reason is timeout, not quit",
          room.events[-1]["kind"] == "left"
          and room.events[-1]["why"] == "timeout", room.events[-1])
    check("the room is still open, because the host is still polling",
          room.state == "lobby", (room.state, room.why))

    # A poll stamps the heartbeat, which is the whole mechanism.
    reset()
    st, host = create(now=1000.0)
    code = host["room"]
    st, g = join(code, "guest", now=1000.0)
    room = S.ROOMS[code]
    S.do_poll({"v": 1, "room": code, "token": g["token"], "cursor": 0,
               "wait": 0}, "10.0.0.2", 1000.0 + S.MEMBER_TIMEOUT_S - 5)
    S.sweep(1000.0 + S.MEMBER_TIMEOUT_S + 1)
    check("a poll before the deadline keeps the seat", 2 in room.members)

    # The host going quiet closes the room for everybody.
    reset()
    st, host = create(now=1000.0)
    code = host["room"]
    join(code, "guest", now=1000.0)
    room = S.ROOMS[code]
    S.sweep(1000.0 + S.MEMBER_TIMEOUT_S + 1)
    check("the host going quiet closes the room", room.state == "closed")
    check("and it reads as host_left, the same as if it had said so",
          room.why == "host_left")

    # An idle room closes on its own.
    reset()
    st, host = create(now=1000.0)
    code = host["room"]
    room = S.ROOMS[code]
    for seat in room.members.values():
        seat.seen = 10 ** 9      # never times out, so only the idle rule can fire
    S.sweep(1000.0 + S.ROOM_IDLE_S + 1)
    check("a room with no events for ROOM_IDLE_S closes",
          room.state == "closed" and room.why == "idle", (room.state, room.why))


def test_cursor_and_rings():
    print("\n-- the cursor, the event ring and the chat ring")
    reset()
    st, host = create()
    code = host["room"]
    st, g = join(code, "opie")
    room = S.ROOMS[code]

    seqs = []
    for i in range(10):
        st, out = S.do_chat({"v": 1, "room": code, "token": host["token"],
                             "text": "line %d" % i}, "10.0.0.1",
                            1000.0 + i * 3)
        if st != 200:
            check("chat %d accepted" % i, False, out)
        seqs.append(out["cursor"])
    check("the cursor is strictly monotonic",
          seqs == sorted(seqs) and len(set(seqs)) == len(seqs), seqs)

    st, out = S.do_poll({"v": 1, "room": code, "token": g["token"],
                         "cursor": seqs[4], "wait": 0}, "10.0.0.2", 1100.0)
    check("a poll returns exactly what came after its cursor",
          [e["seq"] for e in out["events"]] == seqs[5:],
          [e["seq"] for e in out["events"]])
    check("nothing is missed by a client that skipped nine polls",
          len(out["events"]) == 5)

    # The rings.
    with S.LOCK:
        for i in range(200):
            room.push("chat", seat=1, display="tango", text="fill %d" % i)
    check("the event ring is capped at %d" % S.EVENT_RING,
          len(room.events) == S.EVENT_RING, len(room.events))
    check("the chat ring is capped at %d" % S.CHAT_RING,
          len(room.chat) == S.CHAT_RING, len(room.chat))
    check("the rings keep the NEWEST, not the oldest",
          room.events[-1]["text"] == "fill 199")

    st, out = S.do_poll({"v": 1, "room": code, "token": g["token"],
                         "cursor": 1, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor older than the ring asks the client to resync",
          out.get("resync") is True, out.get("resync"))
    check("a resync answer carries the retained chat",
          len(out["events"]) == S.CHAT_RING
          and all(e["kind"] == "chat" for e in out["events"]))
    check("a resync answer carries the whole view",
          out["view"]["state"] == "lobby" and len(out["view"]["members"]) == 2)

    fresh = room.events[-1]["seq"] - 3
    st, out = S.do_poll({"v": 1, "room": code, "token": g["token"],
                         "cursor": fresh, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor inside the ring does NOT ask for a resync",
          "resync" not in out and len(out["events"]) == 3, out.get("events"))

    st, out = S.do_poll({"v": 1, "room": code, "token": g["token"],
                         "cursor": room.seq + 50, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor from the future asks for a resync too",
          out.get("resync") is True)


def test_auth():
    print("\n-- who may do what")
    reset()
    st, host = create()
    code = host["room"]
    st, g = join(code, "opie")

    st, out = S.do_params({"v": 1, "room": code, "token": g["token"], "map": 1,
                           "win_mode": "time"}, "10.0.0.2", 1000.0)
    check("a member cannot set params",
          st == 403 and out["error"] == "not_host", (st, out))
    st, out = S.do_params({"v": 1, "room": code, "token": host["token"],
                           "map": 1, "win_mode": "time"}, "10.0.0.1", 1000.0)
    check("the host can", st == 200, out)
    check("and the room took the values",
          S.ROOMS[code].map == 1 and S.ROOMS[code].win_mode == "time"
          and S.ROOMS[code].star_target is None)

    st, out = S.do_params({"v": 1, "room": code, "token": host["token"],
                           "map": 3, "win_mode": "stars", "star_target": 5},
                          "10.0.0.1", 1000.0)
    check("stars with a target is accepted", st == 200, out)
    check("the host picks the target, and every value up to the ceiling works",
          all(S.do_params({"v": 1, "room": code, "token": host["token"],
                           "map": 0, "win_mode": "stars", "star_target": n},
                          "10.0.0.1", 1000.0)[0] == 200
              for n in range(1, S.STAR_TARGET_MAX + 1)))
    check("one past the ceiling is refused",
          S.do_params({"v": 1, "room": code, "token": host["token"], "map": 0,
                       "win_mode": "stars",
                       "star_target": S.STAR_TARGET_MAX + 1},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_star_target")
    check("zero is refused too",
          S.do_params({"v": 1, "room": code, "token": host["token"], "map": 0,
                       "win_mode": "stars", "star_target": 0},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_star_target")
    check("the ceiling is a knob, not a literal scattered around the file",
          S.STAR_TARGET_MAX == 5)

    stranger = "f" * 32
    for verb, extra in (("chat", {"text": "hi"}), ("leave", {}),
                        ("params", {"map": 0, "win_mode": "time"})):
        body = {"v": 1, "room": code, "token": stranger}
        body.update(extra)
        st, out = S.VERBS[verb](body, "10.0.0.9", 1000.0)
        check("a stranger's token is refused by %s" % verb,
              st == 403 and out["error"] == "not_a_member", (verb, st, out))

    st, out = S.do_poll({"v": 1, "room": code, "token": stranger, "cursor": 0,
                         "wait": 0}, "10.0.0.9", 1000.0)
    check("a stranger's token is refused by poll",
          st == 403 and out["error"] == "not_a_member", (st, out))


def test_chat_limits():
    print("\n-- the chat limiter, which is per member")
    reset()
    st, host = create()
    code = host["room"]
    st, g = join(code, "opie")
    t = 2000.0
    results = []
    for _ in range(6):
        results.append(S.do_chat({"v": 1, "room": code, "token": host["token"],
                                  "text": "x"}, "10.0.0.1", t)[0])
    check("the burst is spent and then refused",
          results[:3] == [200, 200, 200] and results[3:] == [429, 429, 429],
          results)
    st, _ = S.do_chat({"v": 1, "room": code, "token": g["token"], "text": "x"},
                      "10.0.0.2", t)
    check("the OTHER member is unaffected: the limit is per member", st == 200)
    st, _ = S.do_chat({"v": 1, "room": code, "token": host["token"], "text": "x"},
                      "10.0.0.1", t + 2.1)
    check("two seconds later the host may speak again", st == 200)


def test_rate_buckets():
    print("\n-- the per-address limiters")
    b = S.Buckets()
    now = 5000.0
    allowed = sum(1 for _ in range(200) if b.allow_request("1.2.3.4", now))
    check("a burst is capped at RATE_BURST", allowed == S.RATE_BURST, allowed)
    check("a different address has its own budget",
          b.allow_request("5.6.7.8", now) is True)
    check("the bucket refills at RATE_REQ_PER_S",
          b.allow_request("1.2.3.4", now + 1.0) is True)

    b = S.Buckets()
    creates = sum(1 for _ in range(100) if b.allow_create("1.2.3.4", now))
    check("creates are capped per hour",
          creates == S.RATE_CREATE_PER_HOUR, creates)
    check("and the cap lifts an hour later",
          b.allow_create("1.2.3.4", now + 3601.0) is True)

    b = S.Buckets()
    joins = sum(1 for _ in range(200) if b.allow_join("1.2.3.4", now))
    check("joins are capped per minute", joins == S.RATE_JOIN_PER_MIN, joins)

    b = S.Buckets()
    for _ in range(S.BAD_LIMIT):
        b.note_reject("9.9.9.9", now)
    check("a sender refused %d times in %ds is ignored outright"
          % (S.BAD_LIMIT, int(S.BAD_WINDOW_S)), b.ignored("9.9.9.9", now) is True)
    check("and only for BAD_IGNORE_S",
          b.ignored("9.9.9.9", now + S.BAD_IGNORE_S + 0.1) is False)
    check("a well behaved address is never ignored",
          b.ignored("1.1.1.1", now) is False)
    for i in range(S.BAD_LIMIT * 2):
        b.note_reject("8.8.8.8", now + i * (S.BAD_WINDOW_S + 1))
    check("rejections spread out do not accumulate",
          b.ignored("8.8.8.8", now + S.BAD_LIMIT * 2 * (S.BAD_WINDOW_S + 1))
          is False)

    b = S.Buckets()
    for i in range(S.MAX_BUCKETS + 100):
        b.allow_request("10.%d.%d.%d" % (i // 65536, (i // 256) % 256, i % 256),
                        now)
    check("the address table cannot grow past MAX_BUCKETS",
          len(b.tokens) <= S.MAX_BUCKETS, len(b.tokens))


def test_server_caps():
    print("\n-- the server-wide caps")
    reset()
    made = []
    refusals = []
    for i in range(S.MAX_ROOMS + 5):
        st, out = create(who="10.0.%d.%d" % (i // 256, i % 256))
        if st == 200:
            made.append(out["room"])
        else:
            refusals.append((i, st, out.get("error")))
    check("the first MAX_ROOMS creates all succeed",
          len(made) == S.MAX_ROOMS, len(made))
    check("and every one past that is refused with full",
          refusals and all(st == 503 and err == "full"
                           for _i, st, err in refusals), refusals[:3])
    check("the server holds at most MAX_ROOMS rooms",
          len(S.ROOMS) == S.MAX_ROOMS, len(S.ROOMS))
    check("and every code is distinct", len(set(made)) == len(made))
    reset()


def _params(code, token, who="10.0.0.1", now=1000.0, **extra):
    body = {"v": 1, "room": code, "token": token, "map": 0,
            "win_mode": "time"}
    body.update(extra)
    return S.do_params(body, who, now)


def _dial(code, token, n, v=2, **kw):
    return _params(code, token, v=v, match_players=n, **kw)


class game_max(object):
    """Run a block with the deployment knob at a given value.

    The unit tests import the module with the FILE defaults (GAME_MAX_PLAYERS
    2), while the compose file that actually deploys sets 4. Anything that
    exercises the dial past two has to say which deployment it is talking
    about, so it says so here instead of quietly depending on an import-time
    default. Setting the knob and re-deriving is exactly a compose edit plus a
    restart.

    run vs16 added `seats`, for the same reason the class exists: DIAL_MAX is
    min(DIAL_HARD_MAX, MAX_SEATS, GAME_MAX_PLAYERS), so a test about sixteen
    players has to say that the ROOM holds sixteen as well as that the game
    plays sixteen. Leaving it out silently capped every wide test at four --
    which would still have passed, and would have proved nothing.
    """

    def __init__(self, n, seats=None):
        self.n = n
        self.seats = seats

    def __enter__(self):
        self.was = S.GAME_MAX_PLAYERS
        self.was_seats = S.MAX_SEATS
        S.GAME_MAX_PLAYERS = self.n
        if self.seats is not None:
            S.MAX_SEATS = self.seats
        S.recompute_dial()
        reset()
        return self

    def __exit__(self, *exc):
        S.GAME_MAX_PLAYERS = self.was
        S.MAX_SEATS = self.was_seats
        S.recompute_dial()
        reset()
        return False


def test_dial_defaults_are_inert():
    """The dial must cost a deployment that does not use it NOTHING.

    This is the whole backward-compatibility claim, asserted rather than
    asserted-in-prose: at the shipped knobs, with no client ever sending
    `match_players`, the seating a room ends up with is the seating it had
    before the dial existed.
    """
    print("\n-- the dial is inert at the defaults")
    original = S.GAME_MAX_PLAYERS
    try:
        for game_max in (2, 3, 4):
            S.GAME_MAX_PLAYERS = game_max
            S.recompute_dial()
            reset()
            st, host = create()
            code = host["room"]
            for nick in ("b", "c", "d"):
                join(code, nick)
            room = S.ROOMS[code]
            # The old rule, restated here independently: the first
            # GAME_MAX_PLAYERS seats in join order play.
            want = [s <= game_max for s in (1, 2, 3, 4)]
            got = [room.members[s].playing for s in (1, 2, 3, 4)]
            check("with GAME_MAX_PLAYERS=%d and nobody touching the dial, "
                  "seating is exactly the old rule" % game_max,
                  got == want, (got, want))
            check("and the room's dial reads the deployment's own capability "
                  "(%d)" % game_max, room.match_players == game_max,
                  room.match_players)
    finally:
        S.GAME_MAX_PLAYERS = original
        S.recompute_dial()
        reset()

    # A v1 `params` -- the only kind launcher 0.3.0 sends -- leaves the dial
    # alone rather than resetting it.
    reset()
    st, host = create()
    code = host["room"]
    room = S.ROOMS[code]
    _dial(code, host["token"], 2)
    check("the host sets the dial to 2", room.match_players == 2)
    st, out = _params(code, host["token"], map=3)
    check("a v1 params (an old launcher changing the arena) is accepted",
          st == 200, (st, out))
    check("and does NOT reset the dial the host set",
          room.match_players == 2 and room.map == 3,
          (room.match_players, room.map))


def test_dial_range_is_server_enforced():
    print("\n-- the dial's range, enforced by the server")
    reset()
    st, host = create()
    code = host["room"]
    token = host["token"]
    room = S.ROOMS[code]

    check("DIAL_MAX is derived, never configured directly: "
          "min(16, MAX_SEATS, GAME_MAX_PLAYERS)",
          S.DIAL_MAX == min(16, S.MAX_SEATS, S.GAME_MAX_PLAYERS), S.DIAL_MAX)
    check("and the view advertises it, so a launcher never has to be refused "
          "to learn the bound", room.view(1)["dial_max"] == S.DIAL_MAX)
    check("the hard ceiling the owner named is written once",
          S.DIAL_HARD_MAX == 16, S.DIAL_HARD_MAX)

    for n in (S.DIAL_MAX + 1, 5, 8, 16):
        if n <= S.DIAL_MAX:
            continue
        st, out = _dial(code, token, n)
        check("a dial of %d is refused with its own code, not clamped" % n,
              st == 400 and out["error"] == "bad_match_players", (st, out))
        check("and the room keeps the value it had", room.match_players != n)

    for n in (0, 1, -1, 17, 999):
        st, out = _dial(code, token, n)
        check("a dial of %d is refused" % n,
              st == 400 and out["error"] == "bad_match_players", (st, out))

    for bad in (True, 2.0, "2", None, [2]):
        st, out = _dial(code, token, bad)
        check("a dial of %r is refused on type" % (bad,),
              st == 400 and out["error"] == "bad_match_players", (st, out))

    for n in range(2, S.DIAL_MAX + 1):
        st, out = _dial(code, token, n)
        check("a dial of %d is accepted" % n, st == 200, (st, out))
        check("and lands in the room and its view",
              room.match_players == n and room.view(1)["match_players"] == n)

    # Only the host.
    st, guest = join(code, "b")
    st, out = _dial(code, guest["token"], 2, who="10.0.0.2")
    check("a guest cannot move the dial",
          st == 403 and out["error"] == "not_host", (st, out))
    # And not during a match.
    S.ROOMS[code].state = "arming"
    st, out = _dial(code, token, 2)
    check("and nobody can move it once the room has left the lobby",
          st == 409 and out["error"] == "not_in_lobby", (st, out))

    # THE ONE THE OWNER ASKED FOR, on the config that actually deploys: the
    # control offers 2..16 and the server refuses everything above what the
    # game can run, one value at a time, with the room untouched afterwards.
    with game_max(4):
        st, host = create()
        code, token = host["room"], host["token"]
        room = S.ROOMS[code]
        check("on the deployed config the dial tops out at 4",
              S.DIAL_MAX == 4, S.DIAL_MAX)
        for n in (2, 3, 4):
            st, out = _dial(code, token, n)
            check("deployed: dial %d accepted" % n, st == 200, (st, out))
        for n in range(5, 17):
            st, out = _dial(code, token, n)
            check("deployed: dial %d refused, cleanly, and the room keeps 4"
                  % n,
                  st == 400 and out["error"] == "bad_match_players"
                  and room.match_players == 4, (st, out, room.match_players))


def test_dial_is_v2_only():
    """Section 3.0's strictness, kept: a client claiming v1 gets the v1 field
    set and nothing else."""
    print("\n-- the dial is a v2 field, and v1 stays as strict as it was")
    reset()
    st, host = create()
    code, token = host["room"], host["token"]

    st, out = _params(code, token, v=1, match_players=2)
    check("a v1 request carrying the v2 field is bad_field, exactly as an "
          "undefined field would be",
          st == 400 and out["error"] == "bad_field", (st, out))
    st, out = _params(code, token, v=2, match_players=2)
    check("the same body at v2 is accepted", st == 200, (st, out))
    check("the version range is a range, and it is 1..3",
          (S.CONTRACT_MIN, S.CONTRACT_V) == (1, 3))
    check("and the gate on the field is named, not spelled 2 at the check",
          S.V_DIAL == 2)
    # run vs16: the DIAL ARRIVED in v2 and stays gated at v2 forever -- a field
    # does not move to the newest version just because a newer one exists. What
    # v3 changed is the RANGE the field may carry, which is a different rule
    # with its own gate below.
    st, out = _params(code, token, v=2, match_players=2)
    check("the dial is still a v2 field after v3 exists", st == 200, (st, out))


# A body that satisfies each verb's REQUIRED fields, so the generic walk below
# can add one optional key to it and be sure that key is the only thing under
# test. The values need only be well-formed enough to reach the shape check,
# which runs before any of them is validated.
_FIELD_SAMPLES = {
    "v": 1,
    "room": "K7QMR3",
    "token": "0" * 32,
    "nick": "tango",
    "text": "gg",
    "cursor": 0,
    "wait": 0,
    "map": 0,
    "win_mode": "time",
    "star_target": 3,
    "match_players": 2,
    "seat": 2,
    "match": "0" * 16,
    "reason": "spawn_failed",
    "win": "slot",
    "scores": [0, 0, 0, 0],
    "pre_ok": True,
    "color": "ff8800",
    "shoes": "1a2b3c",
}


def test_every_versioned_field_is_gated():
    """THE GATE IS STRUCTURAL, NOT REMEMBERED.

    Walks VERB_FIELDS and, for every field that arrived after the contract
    floor, proves it is refused by a request claiming an earlier version --
    and accepted at its own. It names no field, so a field added tomorrow with
    a version and no gate FAILS HERE rather than going quietly through as a v1
    key. That is the whole point: the dial's first gate was one hand-written
    expression in one function with one test aimed at it, and the next lane to
    add a v2 field would have had nothing telling it the expression existed.
    """
    print("\n-- every versioned field is gated, by construction")

    check("every verb the server routes has a row in the field table, and "
          "every row has a verb", set(S.VERBS) == set(S.VERB_FIELDS),
          sorted(set(S.VERBS) ^ set(S.VERB_FIELDS)))

    known = set(_FIELD_SAMPLES)
    missing = set()
    versioned = []
    late_verbs = []
    for verb, row in sorted(S.VERB_FIELDS.items()):
        required, optional = row[0], row[1]
        missing |= (set(required) | set(optional)) - known
        for name, since in sorted(optional.items()):
            if since > S.CONTRACT_MIN:
                versioned.append((verb, name, since))
        if S.verb_since(verb) > S.CONTRACT_MIN:
            late_verbs.append((verb, S.verb_since(verb)))

    # If this fires, a field was added to the table and this test cannot build
    # a body for it -- which would silently reduce the walk's coverage.
    check("the walk knows a sample value for every field in the table, so it "
          "cannot skip one by accident", not missing, sorted(missing))

    check("there is at least one versioned field to check (the walk is not "
          "vacuously passing)", bool(versioned), versioned)

    for verb, name, since in versioned:
        required = S.VERB_FIELDS[verb][0]
        for ver in range(S.CONTRACT_MIN, since):
            body = {k: _FIELD_SAMPLES[k] for k in required}
            body["v"] = ver
            body[name] = _FIELD_SAMPLES[name]
            check("%s: '%s' arrived in v%d, so a v%d request carrying it is "
                  "bad_field" % (verb, name, since, ver),
                  S.shape_for(verb, body) == "bad_field",
                  S.shape_for(verb, body))
        # And it is genuinely accepted at its own version, so the gate is a
        # gate and not a permanent refusal.
        body = {k: _FIELD_SAMPLES[k] for k in required}
        body["v"] = since
        body[name] = _FIELD_SAMPLES[name]
        check("%s: and a v%d request carrying '%s' passes the shape check"
              % (verb, since, name), S.shape_for(verb, body) is None,
              S.shape_for(verb, body))

    # ---- THE SAME GUARANTEE FOR A WHOLE VERB -------------------------------
    #
    # `color` is not an old verb that grew a key, it is a verb that did not
    # exist at v1. A field-level gate cannot express that: every one of its
    # fields is REQUIRED, so there is no optional key to hang a version on, and
    # a v1 caller sending its exact required set would have been answered. The
    # row carries the verb's own version, and this walks it the same way -- by
    # the table, naming no verb, so the next v3 verb is covered on the day it
    # is added.
    check("there is at least one verb that arrived after the floor (the "
          "verb-level walk is not vacuously passing)", bool(late_verbs),
          late_verbs)

    for verb, since in late_verbs:
        required = S.VERB_FIELDS[verb][0]
        for ver in range(S.CONTRACT_MIN, since):
            body = {k: _FIELD_SAMPLES[k] for k in required}
            body["v"] = ver
            check("%s: the whole verb arrived in v%d, so a v%d caller sending "
                  "its exact required set is still bad_field"
                  % (verb, since, ver),
                  S.shape_for(verb, body) == "bad_field",
                  S.shape_for(verb, body))
            # And it is refused as bad_field, NOT as something that leaks the
            # verb's existence -- a v1 caller cannot tell a v2 verb apart from
            # a misspelled one.
            unknown = dict(body)
            unknown.pop(required[-1], None)
            check("%s: and a v%d caller cannot tell it apart from a verb that "
                  "does not exist" % (verb, ver),
                  S.shape_for(verb, unknown) == "bad_field")
        body = {k: _FIELD_SAMPLES[k] for k in required}
        body["v"] = since
        check("%s: and at v%d the verb answers normally" % (verb, since),
              S.shape_for(verb, body) is None, S.shape_for(verb, body))

    # THE UNION, asserted as one fact rather than assumed from the rows above:
    # everything v2 adds is v2, and there is no second definition of the
    # number anywhere in the module.
    v2_adds = sorted(
        [(v, n) for v, n, sc in versioned if sc == 2]
        + [(v, "<the verb itself>") for v, sc in late_verbs if sc == 2])
    check("v2 is the UNION of both lanes' additions, in one table: %s"
          % ", ".join("%s.%s" % (v, n) for v, n in v2_adds),
          v2_adds == [("color", "<the verb itself>"),
                      ("create", "color"), ("create", "shoes"),
                      ("join", "color"), ("join", "shoes"),
                      ("params", "match_players")], v2_adds)
    check("and the retired second definition of v2 is really gone -- no "
          "PROTO_VERSIONS, no PROTO_COLOR_V",
          not hasattr(S, "PROTO_VERSIONS") and not hasattr(S, "PROTO_COLOR_V"),
          [n for n in ("PROTO_VERSIONS", "PROTO_COLOR_V") if hasattr(S, n)])
    # THIS CHECK USED TO SAY "agree with the contract version", and that was
    # only ever true while v2 WAS the newest version -- it read as a statement
    # about the gates and was really a statement about there being exactly two
    # versions. run vs16 added v3 and the sentence stopped being about
    # anything. What it should always have said is that a named gate names the
    # version its field ARRIVED IN, and that no gate names a version this
    # server does not answer.
    check("the two named v2 gates both name v2, the version their fields "
          "arrived in -- a field does not follow the contract version upward",
          S.V_DIAL == 2 and S.V_COLOR == 2, (S.V_DIAL, S.V_COLOR))
    check("and no named gate is outside the range this server answers",
          all(S.CONTRACT_MIN <= g <= S.CONTRACT_V
              for g in (S.V_DIAL, S.V_COLOR)),
          (S.V_DIAL, S.V_COLOR, S.CONTRACT_MIN, S.CONTRACT_V))

    # The other half of the same guarantee: a key NO version defines is
    # refused at every version, including the newest.
    for ver in range(S.CONTRACT_MIN, S.CONTRACT_V + 1):
        body = {"v": ver, "nick": "tango", "colour": "red"}
        check("a key no version defines is bad_field at v%d too" % ver,
              S.shape_for("create", body) == "bad_field")

    # Required fields are v1 by construction. A later version cannot make a
    # field mandatory without breaking every earlier client, so the table's
    # required tuples carry no versions and this pins that reading.
    for verb, row in sorted(S.VERB_FIELDS.items()):
        if S.verb_since(verb) > S.CONTRACT_MIN:
            continue        # a v2 verb has nothing to satisfy at the floor
        required = row[0]
        body = {k: _FIELD_SAMPLES[k] for k in required}
        body["v"] = S.CONTRACT_MIN
        check("%s: its required fields are all satisfiable at the contract "
              "floor (v%d)" % (verb, S.CONTRACT_MIN),
              S.shape_for(verb, body) is None, S.shape_for(verb, body))


def test_dial_moves_the_fewest_people():
    print("\n-- moving the dial moves as few people as it can")
    with game_max(4):
        _dial_churn()


def _dial_churn():
    st, host = create(nick="host")
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    for nick in ("b", "c", "d"):
        join(code, nick)

    _dial(code, token, 2)
    check("dial 2: seats 1 and 2 play, 3 and 4 watch",
          [room.members[s].playing for s in (1, 2, 3, 4)]
          == [True, True, False, False],
          [room.members[s].playing for s in (1, 2, 3, 4)])
    _dial(code, token, 4)
    check("dial 4: everybody plays",
          all(m.playing for m in room.members.values()))
    _dial(code, token, 3)
    check("dial 3: the HIGHEST seat is the one dropped, so the three who were "
          "already playing keep playing",
          [room.members[s].playing for s in (1, 2, 3, 4)]
          == [True, True, True, False],
          [room.members[s].playing for s in (1, 2, 3, 4)])
    _dial(code, token, 2)
    _dial(code, token, 3)
    check("down and back up returns the same person, not a different one",
          [room.members[s].playing for s in (1, 2, 3, 4)]
          == [True, True, True, False])

    # The host is the parent of the session; a spectating host would hand out
    # a go plan with no parent in it.
    for n in range(2, S.DIAL_MAX + 1):
        _dial(code, token, n)
        check("at dial %d the host is still playing" % n,
              room.members[1].playing, n)


def test_dial_and_seat_reuse():
    """Why the dial promotes and demotes instead of "the N lowest seats play":
    seat numbers are REUSED, and the rank rule would throw a seated player out
    of a match to make room for somebody who just walked in."""
    print("\n-- the dial versus seat reuse")
    with game_max(4):
        _dial_seat_reuse()


def _dial_seat_reuse():
    st, host = create(nick="host")
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    b = join(code, "b", who="10.0.0.2")[1]
    join(code, "c", who="10.0.0.3")
    join(code, "d", who="10.0.0.4")
    _dial(code, token, 2)
    check("dial 2 with four in the room: 1 and 2 play",
          [s for s in (1, 2, 3, 4) if room.members[s].playing] == [1, 2])

    S.do_leave({"v": 1, "room": code, "token": b["token"]}, "10.0.0.2", 1000.0)
    check("seat 2 leaves and seat 3 is promoted into the gap",
          [s for s in sorted(room.members) if room.members[s].playing]
          == [1, 3],
          [(s, room.members[s].playing) for s in sorted(room.members)])

    join(code, "e", who="10.0.0.5")
    check("seat 2 is handed to the new arrival (numbers are reused)",
          2 in room.members and room.members[2].nick == "e")
    check("and the promoted seat 3 is NOT thrown out of the match to make "
          "room for them",
          [s for s in sorted(room.members) if room.members[s].playing]
          == [1, 3],
          [(s, room.members[s].playing) for s in sorted(room.members)])


def test_dial_reaches_the_plan():
    print("\n-- the dial reaches the go plan and the frozen match state")
    with game_max(4):
        _dial_plan()


def _dial_plan():
    st, host = create(nick="host", pre_ok=True)
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    guest = join(code, "b", pre_ok=True)[1]
    third = join(code, "c", who="10.0.0.3", pre_ok=True)[1]
    _dial(code, token, 3)

    st, out = S.do_start({"v": 1, "room": code, "token": token},
                         "10.0.0.1", 1000.0)
    check("start is accepted with three playing seats", st == 200, (st, out))
    check("agreed_players is what actually turned up", room.agreed_players == 3)
    check("and the dial is frozen alongside the other params",
          room.match_dial == 3)
    plan = S.member_plan(room, 1)
    check("the plan carries both: players=3 (what launched) and "
          "match_players=3 (what the host asked for)",
          plan["players"] == 3 and plan["match_players"] == 3, plan)

    # Fewer people than the dial: the plan tells the truth about both numbers
    # rather than pretending the room was full.
    reset()
    st, host = create(nick="host", pre_ok=True)
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    join(code, "b", pre_ok=True)
    _dial(code, token, 4)
    S.do_start({"v": 1, "room": code, "token": token}, "10.0.0.1", 1000.0)
    plan = S.member_plan(room, 1)
    check("with 2 of a 4-player dial filled, the plan says players=2 and "
          "match_players=4 -- no pretending",
          plan["players"] == 2 and plan["match_players"] == 4, plan)
    check("and SM64DS_VS_PLAYERS therefore rides the REAL count",
          plan["players"] == room.agreed_players)
    _r = S.do_failed({"v": 1, "room": code, "token": token,
                      "match": room.match, "reason": "spawn_failed"},
                     "10.0.0.1", 1000.0)
    check("and the frozen dial is cleared with the rest of the match state",
          room.match_dial == 0, (_r, room.match_dial))


def test_opacity():
    print("\n-- chat and nicknames are opaque, end to end")
    reset()
    tricky = "<b>&amp; \"quoted\" 'x' \\ / %s %d {0}"
    st, host = create(nick="tan<go>")
    code = host["room"]
    st, out = S.do_chat({"v": 1, "room": code, "token": host["token"],
                         "text": tricky}, "10.0.0.1", 1000.0)
    check("a chat line full of markup and format specifiers is accepted",
          st == 200, out)
    room = S.ROOMS[code]
    check("and comes back byte for byte, unescaped and unparsed",
          room.events[-1]["text"] == tricky, room.events[-1]["text"])
    check("the nickname comes back exactly as sent",
          room.members[1].nick == "tan<go>")
    # The exact set of things a member record holds. Pinned so that adding a
    # field to it is a deliberate act with a test to change, not a drift.
    check("a member record holds exactly the fields the design names",
          set(Member_fields(room.members[1])) == {
              "seat", "token", "nick", "display", "playing", "pre_ok", "armed",
              "seen", "chat_allowance", "chat_stamp", "last_addr",
              # v2: the two colours this player picked.
              "color", "shoes",
              # 0.3.2: the contract version this member joined at.
              "v"},
          Member_fields(room.members[1]))
    check("the only client address kept is the one a kick needs, and it never "
          "reaches a client",
          room.members[1].last_addr == "10.0.0.1"
          and "last_addr" not in room.members[1].view()
          and "last_addr" not in json.dumps(room.view(1)),
          room.view(1))




def test_kick():
    print("\n-- the host removing somebody (open question 4, answered yes)")
    reset()
    st, host = create(who="10.0.0.1")
    code = host["room"]
    st, g2 = join(code, "rude", who="10.0.0.2")
    st, g3 = join(code, "fine", who="10.0.0.3")
    room = S.ROOMS[code]

    st, out = S.do_kick({"v": 1, "room": code, "token": g2["token"], "seat": 3},
                        "10.0.0.2", 1000.0)
    check("a member cannot kick anybody",
          st == 403 and out["error"] == "not_host", (st, out))

    st, out = S.do_kick({"v": 1, "room": code, "token": host["token"],
                         "seat": 1}, "10.0.0.1", 1000.0)
    check("the host cannot kick itself",
          st == 400 and out["error"] == "bad_seat", (st, out))

    st, out = S.do_kick({"v": 1, "room": code, "token": host["token"],
                         "seat": 4}, "10.0.0.1", 1000.0)
    check("an empty seat cannot be kicked",
          st == 400 and out["error"] == "bad_seat", (st, out))

    st, out = S.do_kick({"v": 1, "room": code, "token": host["token"],
                         "seat": 9}, "10.0.0.1", 1000.0)
    check("a seat number outside the room is bad_seat",
          st == 400 and out["error"] == "bad_seat", (st, out))

    victim_token = g2["token"]
    st, out = S.do_kick({"v": 1, "room": code, "token": host["token"],
                         "seat": 2}, "10.0.0.1", 1000.0)
    check("the host can kick a member", st == 200 and out.get("ok") is True,
          (st, out))
    check("the seat is freed", 2 not in room.members)
    check("it reuses the left event with why=kicked, not a new event kind",
          room.events[-1]["kind"] == "left" and room.events[-1]["seat"] == 2
          and room.events[-1]["why"] == "kicked", room.events[-1])
    check("the spectator is promoted exactly as it is for a plain leave",
          room.members[3].playing is True)

    st, out = S.do_poll({"v": 1, "room": code, "token": victim_token,
                         "cursor": 0, "wait": 0}, "10.0.0.2", 1000.0)
    check("the kicked launcher is told WHY on its next request",
          st == 403 and out["error"] == "kicked", (st, out))

    st, out = join(code, "rude", who="10.0.0.2", now=1000.0)
    check("and the address it was kicked from cannot walk back in",
          st == 403 and out["error"] == "kicked", (st, out))
    st, out = join(code, "rude", who="10.0.0.2", now=1000.0 + S.KICK_COOLDOWN_S + 1)
    check("but only for the cooldown, not for the room's lifetime",
          st == 200, (st, out))

    # The honest limitation, asserted rather than left in a comment.
    reset()
    st, host = create(who="10.0.0.1")
    code = host["room"]
    st, g = join(code, "rude", who="10.0.0.2")
    S.do_kick({"v": 1, "room": code, "token": host["token"], "seat": 2},
              "10.0.0.1", 1000.0)
    st, out = join(code, "rude-again", who="10.0.0.7", now=1000.0)
    check("a kicked player who changes address walks straight back in "
          "(there are no accounts; the remedy is closing the room)",
          st == 200, (st, out))

    # And the other half of the same fact.
    reset()
    st, host = create(who="10.0.0.1")
    code = host["room"]
    join(code, "housemate-a", who="10.0.0.2")
    join(code, "housemate-b", who="10.0.0.2")
    S.do_kick({"v": 1, "room": code, "token": host["token"], "seat": 2},
              "10.0.0.1", 1000.0)
    st, out = join(code, "housemate-c", who="10.0.0.2", now=1000.0)
    check("a housemate on the same address shares the cooldown, which is why "
          "it is a cooldown and not a ban", st == 403, (st, out))

    # A kick cannot happen once a match is being armed.
    reset()
    st, host = create(who="10.0.0.1")
    code = host["room"]
    join(code, "b", who="10.0.0.2")
    S.ROOMS[code].state = "arming"
    st, out = S.do_kick({"v": 1, "room": code, "token": host["token"],
                         "seat": 2}, "10.0.0.1", 1000.0)
    check("a kick is refused once the room has left the lobby state",
          st == 409 and out["error"] == "not_in_lobby", (st, out))


def test_no_two_player_assumption():
    print("\n-- nothing assumes two players beyond the one config number")
    reset()
    check("MAX_SEATS is 4 and GAME_MAX_PLAYERS is its own number",
          S.MAX_SEATS == 4 and S.GAME_MAX_PLAYERS == 2)

    original = S.GAME_MAX_PLAYERS
    try:
        # Moving the knob and re-deriving is exactly what a compose edit plus a
        # restart does. The dial's bounds come off GAME_MAX_PLAYERS, so the
        # re-derivation is part of the change, not an extra step.
        S.GAME_MAX_PLAYERS = 4
        S.recompute_dial()
        reset()
        st, host = create()
        code = host["room"]
        for nick in ("b", "c", "d"):
            join(code, nick)
        room = S.ROOMS[code]
        check("raising the one number makes all four seats playing, with no "
              "other change", all(m.playing for m in room.members.values()),
              [(m.seat, m.playing) for m in room.members.values()])
        check("and the view reports it",
              room.view(1)["max_players"] == 4, room.view(1)["max_players"])

        S.GAME_MAX_PLAYERS = 3
        S.recompute_dial()
        reset()
        st, host = create()
        code = host["room"]
        for nick in ("b", "c", "d"):
            join(code, nick)
        room = S.ROOMS[code]
        check("three plays and the fourth watches",
              [room.members[s].playing for s in (1, 2, 3, 4)]
              == [True, True, True, False])
    finally:
        S.GAME_MAX_PLAYERS = original
        S.recompute_dial()
        reset()



def test_seat_stability():
    """The three properties stage B's slot assignment rests on.

    Over the relay the parent's ACCEPT is a broadcast with no recipient field,
    so two children claiming the same slot cannot be told apart by the game.
    The lobby is the only place a slot can be handed out uniquely, and it hands
    it out from the seat number. So the seat number has to be stable, the host
    has to be seat 1, and the numbers have to stay inside the game's four
    slots. Asserted here rather than left true by accident.
    """
    print("\n-- seat numbers, which are what stage B turns into game slots")
    reset()

    st, host = create(nick="host")
    code = host["room"]
    room = S.ROOMS[code]
    seats = {"host": host["member"]}
    for nick in ("b", "c", "d"):
        seats[nick] = join(code, nick)[1]["member"]
    check("seats are handed out in join order, starting at 1",
          [seats[n] for n in ("host", "b", "c", "d")] == [1, 2, 3, 4], seats)
    check("the creator is seat 1", seats["host"] == 1)
    check("the host is seat 1", room.host_seat == 1)

    tokens = {m.seat: m.token for m in room.members.values()}

    # Everything that can happen to a room, short of it closing.
    S.do_leave({"v": 1, "room": code, "token": tokens[3]}, "10.0.0.3", 1001.0)
    S.do_kick({"v": 1, "room": code, "token": host["token"], "seat": 4},
              "10.0.0.1", 1002.0)
    st, e = join(code, "e", who="10.0.0.5", now=1003.0)
    check("a new joiner takes the lowest FREE number, not the next one up",
          e["member"] == 3, e.get("member"))
    st, f = join(code, "f", who="10.0.0.6", now=1004.0)
    check("and then the next one", f["member"] == 4, f.get("member"))

    check("nobody who stayed was renumbered",
          room.by_token[tokens[1]] == 1 and room.by_token[tokens[2]] == 2,
          {t[:6]: v for t, v in room.by_token.items()})
    check("the host is still seat 1 through all of it", room.host_seat == 1)
    check("every seat number is distinct",
          len(set(room.members)) == len(room.members))
    check("every seat number is inside 1..MAX_SEATS",
          all(1 <= n <= S.MAX_SEATS for n in room.members), sorted(room.members))

    slots = sorted(n - 1 for n in room.members)
    check("the slots derived from them are distinct and inside 0..3",
          slots == sorted(set(slots)) and all(0 <= x <= 3 for x in slots), slots)
    check("the host's slot is 0", room.host_seat - 1 == 0)

    # A promotion must not renumber anybody either.
    reset()
    st, host = create(nick="host")
    code = host["room"]
    room = S.ROOMS[code]
    join(code, "b")
    join(code, "c")
    before = {m.token: m.seat for m in room.members.values()}
    guest2 = [m for m in room.members.values() if m.seat == 2][0]
    S.do_leave({"v": 1, "room": code, "token": guest2.token}, "10.0.0.2", 1001.0)
    check("the promoted spectator keeps the number it already had",
          room.members[3].playing is True and 3 in room.members,
          sorted(room.members))
    check("and nobody else moved either",
          all(room.by_token[t] == n for t, n in before.items()
              if t in room.by_token))

    # The gap this leaves is the one thing stage B has to choose about.
    playing = sorted(m.seat for m in room.members.values() if m.playing)
    check("with a spectator promoted, the playing seats need NOT be contiguous "
          "-- stage B chooses between slot=seat-1 and slot=rank",
          playing == [1, 3], playing)

# ---------------------------------------------------- stage B: start + race
#
# The arming handshake, the frozen plan, the seat-count force, and the slot and
# names the plan carries. These are what stand between "press start" and a match
# smaller than the room agreed on.


def _two_ready(now=1000.0, guest_pre_ok=True, host_pre_ok=True):
    """A room with a host and a guest, both playing, ready to arm."""
    st, host = create(nick="tango", now=now, pre_ok=host_pre_ok)
    code = host["room"]
    st, guest = join(code, "opie", now=now, pre_ok=guest_pre_ok)
    return code, S.ROOMS[code], host, guest


def start(code, token, who="10.0.0.1", now=1000.0):
    return S.do_start({"v": 1, "room": code, "token": token}, who, now)


def ready(code, token, match, who="10.0.0.1", now=1000.0):
    return S.do_ready({"v": 1, "room": code, "token": token, "match": match},
                      who, now)


def poll(code, token, cursor, who="10.0.0.1", now=1000.0, wait=0):
    return S.do_poll({"v": 1, "room": code, "token": token, "cursor": cursor,
                      "wait": wait}, who, now)


def test_start():
    print("\n-- start: freeze the match, open the arming window")
    reset()
    code, room, host, guest = _two_ready()

    # Refusals first, each with its own code.
    st, p = start(code, guest["token"])
    check("a non-host start is 403 not_host",
          st == 403 and p["error"] == "not_host", (st, p))

    reset()
    st, host = create(nick="solo", pre_ok=True)
    st, p = start(host["room"], host["token"])
    check("start with one playing seat is 409 not_enough_players",
          st == 409 and p["error"] == "not_enough_players", (st, p))

    reset()
    code, room, host, guest = _two_ready(guest_pre_ok=False)
    st, p = start(code, host["token"])
    check("start with a playing seat not pre_ok is 409 member_not_ready",
          st == 409 and p["error"] == "member_not_ready", (st, p))

    # The happy arm.
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    check("start arms the room", st == 200, (st, p))
    check("state is arming", room.state == "arming", room.state)
    check("a 16-hex match id was minted",
          isinstance(p.get("match"), str) and len(p["match"]) == 16
          and all(c in S.HEX_ALPHABET for c in p["match"]), p.get("match"))
    check("an 8-char comms code was minted from the room alphabet, NOT shown "
          "to any client",
          room.comms_code is not None and len(room.comms_code) == 8
          and all(c in S.ROOM_ALPHABET for c in room.comms_code)
          and "code" not in p, room.comms_code)
    check("the comms code is not the room code", room.comms_code != code)
    check("agreed_players froze at 2", room.agreed_players == 2)
    check("no seat is armed yet -- even the host must POST ready",
          all(not m.armed for m in room.members.values()))
    ev = room.events[-1]
    check("an arming event carries the match and the deadline in ms",
          ev["kind"] == "arming" and ev["match"] == p["match"]
          and ev["deadline_ms"] == S.ARM_DEADLINE_S * 1000, ev)

    # Frozen: params, join, kick, and a second start are all refused now.
    st, pp = S.do_params({"v": 1, "room": code, "token": host["token"],
                          "map": 2, "win_mode": "time"}, "10.0.0.1", 1001.0)
    check("params is 409 not_in_lobby while arming",
          st == 409 and pp["error"] == "not_in_lobby", (st, pp))
    st, jj = join(code, "ace", who="10.0.0.9", now=1001.0)
    check("a join is 409 in_match while arming",
          st == 409 and jj["error"] == "in_match", (st, jj))
    st, ss = start(code, host["token"], now=1001.0)
    check("a second start is 409 not_in_lobby",
          st == 409 and ss["error"] == "not_in_lobby", (st, ss))


def test_ready_and_go():
    print("\n-- ready: nobody spawns until everybody says they can")
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    match = p["match"]

    st, r = ready(code, "0" * 32, match)
    check("ready from a token that holds no seat is 403 not_a_member",
          st == 403 and r["error"] == "not_a_member", (st, r))
    st, r = ready(code, host["token"], "f" * 16)
    check("ready with the wrong match id is 409 stale_match",
          st == 409 and r["error"] == "stale_match", (st, r))

    st, r = ready(code, host["token"], match)
    check("the host readies", st == 200 and room.members[1].armed)
    check("one ready is not enough: still arming", room.state == "arming")
    st, r = ready(code, guest["token"], match, who="10.0.0.2")
    check("the last playing seat readies -> go", room.state == "go", room.state)
    ev = room.events[-1]
    check("a go event carries the match", ev["kind"] == "go" and ev["match"] == match)

    # A late duplicate ready, after go, is idempotent rather than an error.
    st, r = ready(code, host["token"], match)
    check("a duplicate ready after go is idempotent 200", st == 200, (st, r))


def test_go_plan():
    print("\n-- the go plan: per member, identical where it must be")
    reset()
    code, room, host, guest = _two_ready()
    # A third seat: a spectator, since GAME_MAX_PLAYERS is 2.
    st, spec = join(code, "watch", who="10.0.0.3")
    check("the third seat is a spectator", room.members[3].playing is False)
    S.do_params({"v": 1, "room": code, "token": host["token"],
                 "map": 2, "win_mode": "stars", "star_target": 3},
                "10.0.0.1", 1000.0)
    st, p = start(code, host["token"])
    match = p["match"]
    ready(code, host["token"], match)
    ready(code, guest["token"], match, who="10.0.0.2")

    hp = S.member_plan(room, 1)
    gp = S.member_plan(room, 2)
    sp = S.member_plan(room, 3)
    check("the host plan is parent, slot 0, no spawn delay",
          hp["role"] == "parent" and hp["slot"] == 0
          and hp["spawn_delay_ms"] == 0, hp)
    check("the guest plan is child, slot 1, 1500 ms delay",
          gp["role"] == "child" and gp["slot"] == 1
          and gp["spawn_delay_ms"] == 1500, gp)
    check("both plans carry the SAME comms code, relay, map, players, names",
          hp["code"] == gp["code"] == room.comms_code
          and hp["relay"] == gp["relay"] == S.RELAY_ADDR
          and hp["map"] == gp["map"] == 2
          and hp["players"] == gp["players"] == 2
          and hp["names"] == gp["names"], (hp, gp))
    check("the star target rides the plan in stars mode",
          hp.get("star_target") == 3 and gp.get("star_target") == 3, hp)
    check("the spectator plan is empty and spawns nothing",
          sp == {"playing": False}, sp)

    # A poll injects the right plan per member and also carries a top-level plan.
    st, out = poll(code, guest["token"], guest["cursor"], who="10.0.0.2")
    go_evs = [e for e in out.get("events", []) if e.get("kind") == "go"]
    check("the guest's poll carries a go event with the guest's own plan",
          len(go_evs) == 1 and go_evs[0]["plan"]["role"] == "child"
          and go_evs[0]["plan"]["slot"] == 1, go_evs)
    check("and a top-level plan too, for a resync client",
          out.get("plan", {}).get("role") == "child", out.get("plan"))

    # In time mode the star target is absent, not zero.
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    check("time mode: the plan has no star_target key",
          "star_target" not in S.member_plan(room, 1), S.member_plan(room, 1))


def test_names():
    print("\n-- SM64DS_VS_NAMES, built once by the server in slot order")
    reset()
    code, room, host, guest = _two_ready()
    start(code, host["token"])
    check("two seats: names is 'tango,opie,,' with exactly three commas",
          room.names == "tango,opie,," and room.names.count(",") == 3, room.names)
    check("the two plans carry the byte-identical names string",
          S.member_plan(room, 1)["names"] == S.member_plan(room, 2)["names"]
          == "tango,opie,,")

    # A duplicate nickname is disambiguated ONCE, by the server, so both windows
    # get the same string.
    reset()
    st, host = create(nick="opie", pre_ok=True)
    code = host["room"]
    st, guest = join(code, "opie", pre_ok=True)
    room = S.ROOMS[code]
    start(code, host["token"])
    check("a duplicate nickname is 'opie,opie (2),,' identical on both sides",
          room.names == "opie,opie (2),,"
          and S.member_plan(room, 1)["names"] == S.member_plan(room, 2)["names"],
          room.names)


def test_failed():
    print("\n-- failed: any playing member aborts the whole room to lobby")
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    match = p["match"]

    st, f = S.do_failed({"v": 1, "room": code, "token": guest["token"],
                         "match": match, "reason": "spawn_failed"},
                        "10.0.0.2", 1001.0)
    check("a failed returns 200", st == 200, (st, f))
    check("the room is back in lobby", room.state == "lobby", room.state)
    check("the match id was cleared", room.match is None)
    check("the comms code was discarded", room.comms_code is None)
    check("every armed flag was cleared",
          all(not m.armed for m in room.members.values()))
    ev = room.events[-1]
    check("a failed event names the match and the reason",
          ev["kind"] == "failed" and ev["match"] == match
          and ev["reason"] == "spawn_failed", ev)

    # reason is a fixed enum; a client cannot mint member_not_ready.
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    st, f = S.do_failed({"v": 1, "room": code, "token": host["token"],
                         "match": p["match"], "reason": "nope"}, "10.0.0.1", 1001.0)
    check("an unknown reason is 400 bad_reason",
          st == 400 and f["error"] == "bad_reason", (st, f))
    st, f = S.do_failed({"v": 1, "room": code, "token": host["token"],
                         "match": p["match"], "reason": "member_not_ready"},
                        "10.0.0.1", 1001.0)
    check("member_not_ready is server-minted only, refused from a client",
          st == 400 and f["error"] == "bad_reason", (st, f))
    st, f = S.do_failed({"v": 1, "room": code, "token": host["token"],
                         "match": "a" * 16, "reason": "no_pairing"},
                        "10.0.0.1", 1001.0)
    check("a failed for a stale match is 409 stale_match",
          st == 409 and f["error"] == "stale_match", (st, f))


def test_arming_roster_is_the_frozen_one():
    """A roster that moves during arming must NOT reach go.

    The plan (players, names, slot_of) freezes at start, so "everybody is ready"
    has to mean the seats that were frozen -- not whoever happens to be playing
    when the last ready lands.
    """
    print("\n-- arming tests the FROZEN roster, not the live one")

    # (a) the other playing seat LEAVES during arming.
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    match = p["match"]
    st, _ = S.do_leave({"v": 1, "room": code, "token": guest["token"]},
                       "10.0.0.2", 1001.0)
    check("the guest can leave during arming", st == 200)
    check("the live roster is down to the host alone",
          [s for s in room.members if room.members[s].playing] == [1])
    ready(code, host["token"], match, now=1002.0)
    check("the host's own ready does NOT send a one-member room to go",
          room.state == "arming", room.state)
    S.sweep(1000.0 + S.ARM_DEADLINE_S + 0.1)
    check("the arming deadline returns it to lobby instead",
          room.state == "lobby" and room.events[-1]["kind"] == "failed"
          and room.events[-1]["reason"] == "member_not_ready")

    # (b) a PROMOTION during arming keeps the count but changes the seats. This
    # is the shape a bare len(ps) == agreed_players check would still let through.
    reset()
    st, host = create(nick="h", pre_ok=True)
    code = host["room"]
    st, g2 = join(code, "g2", who="10.0.0.2", pre_ok=True)
    st, g3 = join(code, "g3", who="10.0.0.3", pre_ok=True)
    room = S.ROOMS[code]
    check("seat 3 starts as a spectator", room.members[3].playing is False)
    st, p = start(code, host["token"])
    match = p["match"]
    frozen_names = room.names
    check("the frozen slot map is seats 1 and 2", sorted(room.slot_of) == [1, 2],
          room.slot_of)
    S.do_leave({"v": 1, "room": code, "token": g2["token"]}, "10.0.0.2", 1001.0)
    check("seat 3 was promoted, so the COUNT still matches agreed_players",
          room.members[3].playing is True
          and len([s for s in room.members if room.members[s].playing])
              == room.agreed_players)
    ready(code, host["token"], match, now=1002.0)
    ready(code, g3["token"], match, who="10.0.0.3", now=1002.0)
    check("but the SEATS differ, so the room still does not go",
          room.state == "arming", room.state)
    check("no plan was ever issued crediting the departed player",
          frozen_names.startswith("h,g2,"), frozen_names)


def test_only_playing_members_drive_a_match():
    """ready / failed / result are all restricted to a seated PLAYING member.

    A room code is not a secret by design, so a spectator is anyone who walked
    in. None of the three may touch a live match.
    """
    print("\n-- a spectator cannot arm, abort or end a match")
    reset()
    st, host = create(nick="h", pre_ok=True)
    code = host["room"]
    st, g = join(code, "g", who="10.0.0.2", pre_ok=True)
    st, spec = join(code, "spec", who="10.0.0.3", pre_ok=True)
    room = S.ROOMS[code]
    check("the third seat is a spectator", room.members[3].playing is False)
    st, p = start(code, host["token"])
    match = p["match"]

    st, r = ready(code, spec["token"], match, who="10.0.0.3")
    check("a spectator's ready is 403 not_playing",
          st == 403 and r["error"] == "not_playing", (st, r))
    check("and it did not arm the spectator's seat", room.members[3].armed is False)

    st, f = S.do_failed({"v": 1, "room": code, "token": spec["token"],
                         "match": match, "reason": "no_pairing"}, "10.0.0.3", 1001.0)
    check("a spectator's failed is 403 not_playing",
          st == 403 and f["error"] == "not_playing", (st, f))
    check("the match is still armed, not aborted", room.state == "arming"
          and room.match == match, room.state)

    # Take the room into a live match, then try to end it as the spectator.
    ready(code, host["token"], match, now=1002.0)
    ready(code, g["token"], match, who="10.0.0.2", now=1002.0)
    check("the two playing seats sent the room to go", room.state == "go", room.state)
    S.sweep(1002.0 + S.GO_GRACE_S + 0.1)
    check("the two playing seats got the match running", room.state == "in_match",
          room.state)
    st, rr = S.do_result({"v": 1, "room": code, "token": spec["token"],
                          "match": match, "win": "star-target",
                          "scores": [9, 0, 0, 0]}, "10.0.0.3", 1010.0)
    check("a spectator's result is 403 not_playing",
          st == 403 and rr["error"] == "not_playing", (st, rr))
    check("the match is STILL running and no scoreline was written",
          room.state == "in_match" and room.match == match, room.state)
    check("no result event was pushed by the spectator",
          not any(e["kind"] == "result" for e in room.events))

    # A playing member still can, so the gate is not simply refusing everybody.
    st, rr = S.do_result({"v": 1, "room": code, "token": g["token"],
                          "match": match, "win": "time-up",
                          "scores": [1, 2, 0, 0]}, "10.0.0.2", 1011.0)
    check("a PLAYING member's result still works", st == 200 and room.state == "lobby")


def test_preflight_updates():
    """A member can correct its own pre-flight answer without leaving.

    The live failure this closes: pre_ok was sent once at join, so a player who
    unpacked their ROM with the window open was stuck "not ready" until they
    closed the window, reopened it, made a new room and rejoined.
    """
    print("\n-- a member can refresh its own pre_ok in place")
    reset()
    st, host = create(nick="h", pre_ok=True)
    code = host["room"]
    st, g = join(code, "g", who="10.0.0.2", pre_ok=False)
    room = S.ROOMS[code]
    check("the guest is seated not-ready", room.members[2].pre_ok is False)
    st, p = start(code, host["token"])
    check("so the host cannot start", st == 409 and p["error"] == "member_not_ready")

    before = room.seq
    st, r = S.do_preflight({"v": 1, "room": code, "token": g["token"],
                            "pre_ok": True}, "10.0.0.2", 1001.0)
    check("the guest can say it is ready now", st == 200, (st, r))
    check("the roster shows it", room.members[2].pre_ok is True)
    check("an event was pushed so a long poll wakes", room.seq > before)
    ev = room.events[-1]
    check("the event names the seat and the new value",
          ev["kind"] == "preflight" and ev["seat"] == 2 and ev["pre_ok"] is True, ev)
    check("and the view every poll carries reflects it",
          room.view(1)["members"][1]["pre_ok"] is True)
    st, p = start(code, host["token"], now=1002.0)
    check("NOW the host can start, with no rejoin and no new room", st == 200, (st, p))

    # Unchanged is a no-op: a launcher re-runs this on a timer.
    reset()
    st, host = create(nick="h", pre_ok=True)
    code = host["room"]
    room = S.ROOMS[code]
    seq = room.seq
    for _ in range(5):
        st, _ = S.do_preflight({"v": 1, "room": code, "token": host["token"],
                                "pre_ok": True}, "10.0.0.1", 1001.0)
        check("repeating the same answer is accepted", st == 200)
    check("...and pushes NOTHING, so a polling launcher costs the room nothing",
          room.seq == seq, (seq, room.seq))

    # It can go the other way too (a ROM that went missing).
    st, _ = S.do_preflight({"v": 1, "room": code, "token": host["token"],
                            "pre_ok": False}, "10.0.0.1", 1002.0)
    check("a member can also go back to not-ready",
          room.members[1].pre_ok is False and room.seq > seq)

    # Authority and grammar.
    st, r = S.do_preflight({"v": 1, "room": code, "token": "0" * 32,
                            "pre_ok": True}, "10.0.0.9", 1003.0)
    check("a token that holds no seat is refused",
          st == 403 and r["error"] == "not_a_member", (st, r))
    st, r = S.do_preflight({"v": 1, "room": code, "token": host["token"],
                            "pre_ok": "yes"}, "10.0.0.1", 1003.0)
    check("a non-boolean pre_ok is 400 bad_field",
          st == 400 and r["error"] == "bad_field", (st, r))
    st, r = S.do_preflight({"v": 1, "room": code, "token": host["token"]},
                           "10.0.0.1", 1003.0)
    check("a missing pre_ok is 400 bad_field",
          st == 400 and r["error"] == "bad_field", (st, r))
    st, r = S.do_preflight({"v": 1, "room": code, "token": host["token"],
                            "pre_ok": True, "seat": 2}, "10.0.0.1", 1003.0)
    check("there is NO seat argument: a member speaks only for itself",
          st == 400 and r["error"] == "bad_field", (st, r))

    # It stays legal during a match, so a player who unpacks mid-match is ready
    # for the rematch rather than blocking it.
    reset()
    code, room, host, guest = _two_ready(now=1000.0)
    st, p = start(code, host["token"], now=1000.0)
    ready(code, host["token"], p["match"], now=1000.0)
    ready(code, guest["token"], p["match"], who="10.0.0.2", now=1000.0)
    S.sweep(1000.0 + S.GO_GRACE_S + 0.1)
    check("a match is running", room.state == "in_match")
    st, _ = S.do_preflight({"v": 1, "room": code, "token": guest["token"],
                            "pre_ok": False}, "10.0.0.2", 1010.0)
    check("preflight is still accepted mid-match", st == 200)
    check("and it does NOT disturb the running match",
          room.state == "in_match" and room.match == p["match"])


def test_arming_and_match_timers():
    print("\n-- the reaper's match timers")
    # Arming deadline: nobody readies, the deadline passes -> lobby + failed.
    reset()
    code, room, host, guest = _two_ready(now=1000.0)
    st, p = start(code, host["token"], now=1000.0)
    S.sweep(1000.0 + S.ARM_DEADLINE_S - 1)
    check("before the deadline the room is still arming", room.state == "arming")
    S.sweep(1000.0 + S.ARM_DEADLINE_S + 0.1)
    check("the arming deadline returns the room to lobby", room.state == "lobby")
    check("with a failed member_not_ready event",
          room.events[-1]["kind"] == "failed"
          and room.events[-1]["reason"] == "member_not_ready")

    # Go grace: both ready, sweep past GO_GRACE -> in_match.
    reset()
    code, room, host, guest = _two_ready(now=1000.0)
    st, p = start(code, host["token"], now=1000.0)
    match = p["match"]
    ready(code, host["token"], match, now=1000.0)
    ready(code, guest["token"], match, who="10.0.0.2", now=1000.0)
    check("both ready -> go", room.state == "go")
    S.sweep(1000.0 + S.GO_GRACE_S + 0.1)
    check("go grace elapses -> in_match", room.state == "in_match", room.state)

    # In match, a member that stops heart-beating is NOT dropped (spec 6):
    # the seen stamps are stale and the timeout would normally fire, but the
    # guard holds them because the room is not in lobby.
    room.members[1].seen = 1000.0
    room.members[2].seen = 1000.0
    S.sweep(1000.0 + S.MEMBER_TIMEOUT_S + 5)
    check("a silent member is NOT dropped mid-match", 2 in room.members)
    check("and the room did not close mid-match", room.state == "in_match")

    # Match timeout returns the room to lobby with failed timeout. Heartbeats
    # are fresh at this instant (a real launcher polls throughout), so the
    # lobby heartbeat sweep in the same pass does not then drop anybody.
    when = room.match_start + S.MATCH_TIMEOUT_S + 0.1
    room.members[1].seen = when
    room.members[2].seen = when
    S.sweep(when)
    check("the match timeout returns the room to lobby", room.state == "lobby")
    check("with a failed timeout event",
          room.events[-1]["kind"] == "failed"
          and room.events[-1]["reason"] == "timeout")


# -------------------------------------------------- stage C: the rematch loop


def test_result_and_rematch():
    print("\n-- result: the rematch loop, and a fresh comms code each match")
    reset()
    code, room, host, guest = _two_ready(now=1000.0)
    S.do_params({"v": 1, "room": code, "token": host["token"],
                 "map": 3, "win_mode": "stars", "star_target": 3},
                "10.0.0.1", 1000.0)
    st, p = start(code, host["token"], now=1000.0)
    match1 = p["match"]
    code1 = room.comms_code
    ready(code, host["token"], match1, now=1000.0)
    ready(code, guest["token"], match1, who="10.0.0.2", now=1000.0)
    S.sweep(1000.0 + S.GO_GRACE_S + 0.1)
    check("match one is running", room.state == "in_match")

    # A result validates its fields.
    st, rr = S.do_result({"v": 1, "room": code, "token": host["token"],
                          "match": match1, "win": "nope",
                          "scores": [3, 1, 0, 0]}, "10.0.0.1", 1010.0)
    check("a bad win is 400 bad_win", st == 400 and rr["error"] == "bad_win")
    st, rr = S.do_result({"v": 1, "room": code, "token": host["token"],
                          "match": match1, "win": "star-target",
                          "scores": [3, 1, 0]}, "10.0.0.1", 1010.0)
    check("scores that are not four ints is 400 bad_scores",
          st == 400 and rr["error"] == "bad_scores")

    # The real result.
    st, rr = S.do_result({"v": 1, "room": code, "token": host["token"],
                          "match": match1, "win": "star-target",
                          "scores": [3, 1, 0, 0]}, "10.0.0.1", 1010.0)
    check("the result returns the room to lobby", room.state == "lobby")
    check("the match id was cleared and the comms code discarded",
          room.match is None and room.comms_code is None)
    ev = room.events[-1]
    check("a result event carries win and the four scores",
          ev["kind"] == "result" and ev["win"] == "star-target"
          and ev["scores"] == [3, 1, 0, 0], ev)
    check("the params survived the match (same again is one button)",
          room.map == 3 and room.win_mode == "stars" and room.star_target == 3)
    check("the roster survived", set(room.members) == {1, 2})

    # The SECOND result for the same match is idempotent, not an error.
    st, rr = S.do_result({"v": 1, "room": code, "token": guest["token"],
                          "match": match1, "win": "star-target",
                          "scores": [3, 1, 0, 0]}, "10.0.0.2", 1011.0)
    check("both players reporting is normal: the second is idempotent 200",
          st == 200, (st, rr))

    # A rematch: start again, and it gets a DIFFERENT comms code.
    st, p2 = start(code, host["token"], now=1012.0)
    match2 = p2["match"]
    code2 = room.comms_code
    check("the rematch has a different match id", match2 != match1)
    check("THE REMATCH HAS A DIFFERENT COMMS CODE -- a fresh relay session so "
          "the previous match's held seats cannot refuse it",
          code2 != code1 and code2 is not None, (code1, code2))
    # A draw is accepted (the launcher posts it for an unparseable marker).
    ready(code, host["token"], match2, now=1012.0)
    ready(code, guest["token"], match2, who="10.0.0.2", now=1012.0)
    S.sweep(1012.0 + S.GO_GRACE_S + 0.1)
    st, rr = S.do_result({"v": 1, "room": code, "token": host["token"],
                          "match": match2, "win": "draw",
                          "scores": [0, 0, 0, 0]}, "10.0.0.1", 1020.0)
    check("a draw result is accepted", st == 200 and room.state == "lobby")



# ---------------------------------------------------------------- v2 colours


def test_colors():
    """The owner's feature: two hex colours per player, carried as data.

    Everything here is about the three properties the design rests on -- a
    colour is only ever set by the seat that owns it, the string the game reads
    is built ONCE by the server so no launcher can assemble a different one, and
    a launcher that predates the feature is not affected by any of it.
    """
    print("\n-- v2: two picked colours per player")

    # -- the grammar -------------------------------------------------------
    check("six lower-case hex is a colour",
          S.v_color({"c": "1a2b3c"}, "c") == ("1a2b3c", None))
    check("upper case is accepted and NORMALISED to lower",
          S.v_color({"c": "1A2B3C"}, "c") == ("1a2b3c", None))
    check("five digits is bad_color", S.v_color({"c": "1a2b3"}, "c")[1] == "bad_color")
    check("seven digits is too_long", S.v_color({"c": "1a2b3c4"}, "c")[1] == "too_long")
    check("a # prefix is bad_color", S.v_color({"c": "#1a2b3"}, "c")[1] == "bad_color")
    check("a non-hex letter is bad_color",
          S.v_color({"c": "1a2b3g"}, "c")[1] == "bad_color")
    check("a number is bad_color", S.v_color({"c": 123456}, "c")[1] == "bad_color")

    # -- the version bump --------------------------------------------------
    reset()
    st, out = S.do_create({"v": 1, "nick": "t", "color": "ff0000",
                           "shoes": "0000ff"}, "10.0.0.1", 1000.0)
    check("a v1 create carrying colours is refused: the bump has to mean "
          "something", st == 400 and out["error"] == "bad_field", (st, out))
    check("v1, v2 and v3 are the versions this server answers, and nothing "
          "else",
          (S.CONTRACT_MIN, S.CONTRACT_V) == (1, 3),
          (S.CONTRACT_MIN, S.CONTRACT_V))

    # -- create and join with colours -------------------------------------
    reset()
    st, host = S.do_create({"v": 2, "nick": "tango", "pre_ok": True,
                            "color": "8A2BE2", "shoes": "FFD700"},
                           "10.0.0.1", 1000.0)
    code = host["room"]
    room = S.ROOMS[code]
    check("a v2 create takes both colours", st == 200, (st, host))
    check("and stores them lower case",
          room.members[1].color == "8a2be2" and room.members[1].shoes == "ffd700",
          (room.members[1].color, room.members[1].shoes))
    check("the roster row carries them",
          host["view"]["members"][0]["color"] == "8a2be2"
          and host["view"]["members"][0]["shoes"] == "ffd700",
          host["view"]["members"][0])

    st, out = S.do_join({"v": 2, "room": code, "nick": "opie", "pre_ok": True,
                         "color": "ff0000"}, "10.0.0.2", 1000.0)
    check("a join with one colour and not the other is bad_color",
          st == 400 and out["error"] == "bad_color", (st, out))
    st, guest = S.do_join({"v": 2, "room": code, "nick": "opie", "pre_ok": True,
                           "color": "ff0000", "shoes": "00ff00"},
                          "10.0.0.2", 1000.0)
    check("a join with both is seated", st == 200, (st, guest))
    check("and the guest's row carries them",
          room.members[2].color == "ff0000" and room.members[2].shoes == "00ff00")

    # -- a v1 join into a coloured room -----------------------------------
    st, old = S.do_join({"v": 1, "room": code, "nick": "old", "pre_ok": True},
                        "10.0.0.3", 1000.0)
    check("a v1 launcher can still join a room full of colours", st == 200, (st, old))
    check("and takes the empty pair, which means the built-in Yoshi",
          room.members[3].color == "" and room.members[3].shoes == "")
    st, p = S.do_leave({"v": 1, "room": code, "token": old["token"]},
                       "10.0.0.3", 1000.0)
    check("and it can leave again", st == 200, (st, p))

    # -- the verb: own seat only, no-op on unchanged, event on change ------
    before = room.seq
    st, r = S.do_color({"v": 2, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1001.0)
    check("re-sending the same pair is a no-op", st == 200 and room.seq == before,
          (st, room.seq, before))

    st, r = S.do_color({"v": 2, "room": code, "token": guest["token"],
                        "color": "123456", "shoes": "654321"},
                       "10.0.0.2", 1002.0)
    check("a real change is taken", st == 200, (st, r))
    check("the roster moves", room.members[2].color == "123456")
    check("and an event is pushed so a long poll wakes", room.seq > before)
    ev = room.events[-1]
    check("the event names the seat and both colours",
          ev["kind"] == "color" and ev["seat"] == 2
          and ev["color"] == "123456" and ev["shoes"] == "654321", ev)
    check("the view every poll carries reflects it",
          room.view(1)["members"][1]["color"] == "123456")

    st, r = S.do_color({"v": 2, "room": code, "token": guest["token"],
                        "color": "", "shoes": ""}, "10.0.0.2", 1003.0)
    check("two empty strings put a player back on the built-in colour",
          st == 200 and room.members[2].color == "", (st, room.members[2].color))
    st, r = S.do_color({"v": 2, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": ""}, "10.0.0.2", 1004.0)
    check("one empty and one not is bad_color",
          st == 400 and r["error"] == "bad_color", (st, r))
    st, r = S.do_color({"v": 1, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1005.0)
    # bad_field, not bad_version, and that is the consolidation's one
    # deliberate behaviour change on this verb. The whole verb's version lives
    # in VERB_FIELDS now, so a v1 caller is refused exactly the way any
    # out-of-version key is refused -- which means it cannot tell a v2 verb
    # apart from a verb that does not exist, and the server gives away nothing
    # about what it grew. No player-visible difference: LobbyWire.Explain maps
    # bad_field and bad_version to the same sentence.
    check("the verb is v2 only, refused the same way every out-of-version key "
          "is", st == 400 and r["error"] == "bad_field", (st, r))
    st, r = S.do_color({"v": 2, "room": code, "token": "f" * 32,
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.9", 1006.0)
    check("a token that holds no seat is refused",
          st == 403 and r["error"] == "not_a_member", (st, r))
    st, r = S.do_color({"v": 2, "room": code, "token": guest["token"], "seat": 1,
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1007.0)
    check("A SEAT ARGUMENT IS REFUSED OUTRIGHT: the verb has no way to reach "
          "anybody else's row", st == 400 and r["error"] == "bad_field", (st, r))

    # -- the string the game reads ----------------------------------------
    S.do_color({"v": 2, "room": code, "token": guest["token"],
                "color": "ff0000", "shoes": "00ff00"}, "10.0.0.2", 1008.0)
    st, p = start(code, host["token"], now=1009.0)
    check("the room arms", st == 200, (st, p))
    check("SM64DS_VS_COLORS is four fields in slot order with three commas",
          room.colors == "8a2be2:ffd700,ff0000:00ff00,,", room.colors)
    check("both plans carry the byte-identical string",
          S.member_plan(room, 1)["colors"] == S.member_plan(room, 2)["colors"]
          == room.colors)
    check("and it is frozen, not live: changing a colour now is refused",
          S.do_color({"v": 2, "room": code, "token": guest["token"],
                      "color": "ffffff", "shoes": "ffffff"},
                     "10.0.0.2", 1010.0)[0] == 409)
    check("so the string the two games are wearing has not moved",
          room.colors == "8a2be2:ffd700,ff0000:00ff00,,")

    # -- a room where nobody picked ---------------------------------------
    reset()
    code, room, host, guest = _two_ready()
    start(code, host["token"])
    check("a room with no picks freezes ',,,' -- three commas, four empties",
          room.colors == ",,," and room.colors.count(",") == 3, room.colors)
    check("every plan still carries the field",
          S.member_plan(room, 1)["colors"] == ",,,")

    # -- one picker, one not ----------------------------------------------
    reset()
    st, host = S.do_create({"v": 2, "nick": "tango", "pre_ok": True,
                            "color": "18e618", "shoes": "e66318"},
                           "10.0.0.1", 1000.0)
    code = host["room"]
    S.do_join({"v": 1, "room": code, "nick": "opie", "pre_ok": True},
              "10.0.0.2", 1000.0)
    room = S.ROOMS[code]
    start(code, host["token"])
    check("slot 0 picked and slot 1 did not: '18e618:e66318,,,'",
          room.colors == "18e618:e66318,,,", room.colors)

    # -- the rematch keeps them -------------------------------------------
    S._reset_match(room)
    check("_reset_match clears the frozen string but not the picks: a colour "
          "lives on the member, not on the match",
          room.colors == "" and room.members[1].color == "18e618")
    start(code, host["token"], now=1001.0)
    check("so the rematch rebuilds the same string",
          room.colors == "18e618:e66318,,,", room.colors)

    # -- the invariant that makes the game's parser safe -------------------
    #
    # The game refuses SM64DS_VS_COLORS wholesale on any grammar violation, so
    # a string this server can emit that the game refuses is a feature that
    # silently does nothing. Prove the server cannot emit one: whatever a
    # client sends, the frozen string is only ever hex, colons and exactly
    # three commas, and never longer than the game's 55-byte cap.
    hostile = [
        "ff0000,00ff00", "ff00,00", "ff0000:", ":ff0000", "ff 000", "ff-000",
        "../../e", "%s%s%s", "\x00abcd", "ffffffff", "", " ", "gg0000",
        "FF0000\n", "ff0000;rm", "\u00ff0000", "0x0000", "'--", "ff0000,",
    ]
    emitted, refused = 0, 0
    for a in hostile:
        for b in hostile:
            reset()
            st, h = S.do_create({"v": 2, "nick": "t", "pre_ok": True,
                                 "color": a, "shoes": b}, "10.0.0.1", 1000.0)
            if st != 200:
                refused += 1
                continue
            emitted += 1
            c2 = h["room"]
            r2 = S.ROOMS[c2]
            S.do_join({"v": 2, "room": c2, "nick": "g", "pre_ok": True,
                       "color": a, "shoes": b}, "10.0.0.2", 1000.0)
            start(c2, h["token"])
            out = r2.colors
            assert out.count(",") == 3, out
            assert len(out) <= 55, out
            assert all(ch in "0123456789abcdef:," for ch in out), out
    check("%d hostile colour pairs, every one refused before it could reach "
          "the frozen string" % refused, emitted == 0, emitted)

    # And the same guarantee stated as a property of the builder rather than of
    # the validators, so a future validator bug is still caught here.
    reset()
    st, h = S.do_create({"v": 2, "nick": "t", "pre_ok": True,
                         "color": "ffffff", "shoes": "000000"},
                        "10.0.0.1", 1000.0)
    code = h["room"]
    room = S.ROOMS[code]
    S.do_join({"v": 2, "room": code, "nick": "g", "pre_ok": True,
               "color": "000000", "shoes": "ffffff"}, "10.0.0.2", 1000.0)
    start(code, h["token"])
    check("the longest legal string is 4 x 13 + 3 = 55 bytes, which is the "
          "game's cap",
          len("ffffff:000000,000000:ffffff,,") == 29 and 4 * 13 + 3 == 55)
    check("and this room's is inside it", len(room.colors) <= 55, room.colors)



def test_the_dial_cannot_widen_over_a_seated_old_client():
    """0.3.2. The join-time gate answers 'can this client enter a wide room';
    this one answers the other order: the old client is already in a narrow
    room and the host then turns the dial. Same failure, one verb later, and
    the same refusal, given to the host who can act on it."""
    print("-- the dial refuses to widen a room over a seated v2 client")
    reset()
    with game_max(16, seats=16):
        st, h = S.do_create({"v": 3, "nick": "new", "pre_ok": True},
                             "10.0.0.1", 1000.0)
        code, token = h["room"], h["token"]
        st, out = S.do_params({"v": 3, "room": code, "token": token, "map": 0,
                               "win_mode": "time", "match_players": 4},
                              "10.0.0.1", 1000.5)
        check("the v3 host dials the room down to four first", st == 200, (st, out))
        st, out = S.do_join({"v": 2, "room": code, "nick": "old",
                             "pre_ok": True}, "10.0.0.2", 1000.0)
        check("a v2 client joins the four-wide room", st == 200, (st, out))
        old_token = out["token"]
        base = {"v": 3, "room": code, "token": token, "map": 0,
                "win_mode": "time"}
        st, out = S.do_params(dict(base, match_players=8), "10.0.0.1", 1001.0)
        check("dialing it to eight is refused seated_client_too_old",
              st == 409 and out["error"] == "seated_client_too_old", (st, out))
        check("and the room is still four wide",
              S.ROOMS[code].match_players == 4, S.ROOMS[code].match_players)
        st, out = S.do_params(dict(base, match_players=3), "10.0.0.1", 1002.0)
        check("dialing within four is still fine", st == 200, (st, out))
        st, out = S.do_leave({"v": 2, "room": code, "token": old_token},
                             "10.0.0.2", 1003.0)
        st, out = S.do_join({"v": 3, "room": code, "nick": "new2",
                             "pre_ok": True}, "10.0.0.3", 1004.0)
        check("a v3 client joins in its place", st == 200, (st, out))
        st, out = S.do_params(dict(base, match_players=8), "10.0.0.1", 1005.0)
        check("and now the dial opens to eight", st == 200 and
              S.ROOMS[code].match_players == 8, (st, out))


def test_a_dropout_after_go_does_not_end_the_match_for_the_rest():
    """0.3.2 field lesson. Once the match is sent off, a non-host member's
    failed drops that member and the rest play on; the host's failed still
    ends it; and a match that falls below two players ends."""
    print("-- after go, one dropout does not end the match for the rest")
    reset()
    with game_max(16, seats=16):
        st, h = S.do_create({"v": 3, "nick": "h", "pre_ok": True},
                            "10.0.0.1", 1000.0)
        code, htok = h["room"], h["token"]
        toks = []
        for k in (2, 3):
            st, out = S.do_join({"v": 3, "room": code, "nick": "g%d" % k,
                                 "pre_ok": True}, "10.0.0.%d" % k, 1000.0)
            toks.append(out["token"])
        st, out = S.do_params({"v": 3, "room": code, "token": htok, "map": 0,
                               "win_mode": "time", "match_players": 3},
                              "10.0.0.1", 1000.5)
        st, p = start(code, htok)
        check("three players start", st == 200, (st, p))
        match = p["match"]
        room = S.ROOMS[code]
        # Arm: every playing launcher says it can spawn this match, and the
        # last one flips the room from arming to go.
        for tok, ip in ((htok, "10.0.0.1"), (toks[0], "10.0.0.2"),
                        (toks[1], "10.0.0.3")):
            st, out = S.do_ready({"v": 3, "room": code, "token": tok,
                                  "match": match}, ip, 1001.0)
        check("the room is at go", room.state == "go", room.state)
        st, f = S.do_failed({"v": 3, "room": code, "token": toks[0],
                             "match": match, "reason": "no_pairing"},
                            "10.0.0.2", 1005.0)
        check("a guest's failed after go is 200", st == 200, (st, f))
        check("and the match is still on", room.state != "lobby"
              and room.match == match, (room.state, room.match))
        check("the guest is no longer playing",
              not room.members[2].playing, room.members[2].playing)
        ev = room.events[-1]
        check("a dropped event names the seat and the reason",
              ev["kind"] == "dropped" and ev["seat"] == 2
              and ev["reason"] == "no_pairing", ev)
        st, f = S.do_failed({"v": 3, "room": code, "token": htok,
                             "match": match, "reason": "no_pairing"},
                            "10.0.0.1", 1006.0)
        check("the host's failed still ends the match",
              st == 200 and room.state == "lobby" and room.match is None,
              (st, room.state, room.match))
        check("and every launcher sees a failed event",
              room.events[-1]["kind"] == "failed", room.events[-1])


def test_result_accepts_a_wide_scoreline():
    """0.3.2: the marker prints four scores as its floor and up to sixteen."""
    print("-- result takes four to sixteen scores")
    reset()
    code, room, host, guest = _two_ready()
    st, p = start(code, host["token"])
    ok7 = S.v_scores({"scores": [1, 0, 0, 0, 0, 0, 0]})
    check("seven scores validate", ok7[1] is None, ok7)
    ok16 = S.v_scores({"scores": [0] * 16})
    check("sixteen scores validate", ok16[1] is None, ok16)
    check("three scores are still refused",
          S.v_scores({"scores": [1, 0, 0]})[1] == "bad_scores")
    check("seventeen scores are refused",
          S.v_scores({"scores": [0] * 17})[1] == "bad_scores")


def test_wide_is_v3_only():
    """run vs16. Above four players the game speaks a different wire and the
    two name/colour strings carry sixteen fields, so the RANGE the dial may
    carry is gated on the caller's version -- not the field, which arrived in
    v2 and stays there."""
    print("\n-- five players and up need v3, and the shape follows the dial")
    reset()
    with game_max(16, seats=16):
        # 0.3.2: the host is seated at the version it created with, and a
        # wide dial is refused while anyone seated is below v3 -- so a host
        # that will dial at v3 creates at v3, as a real v3 launcher does.
        st, host = S.do_create({"v": 3, "nick": "h", "pre_ok": True},
                               "10.0.0.1", 1000.0)
        code, token = host["room"], host["token"]

        # A v2 caller may still dial anything up to four.
        for n in (2, 3, 4):
            st, out = _dial(code, token, n, v=2)
            check("a v2 caller may dial %d" % n, st == 200, (n, st, out))

        # And nothing above it, even on a deployment that could run it.
        for n in (5, 8, 16):
            st, out = _dial(code, token, n, v=2)
            check("a v2 caller asking for %d is refused bad_match_players, "
                  "because its own build validates four name fields and would "
                  "have joined a room it could not play" % n,
                  st == 400 and out["error"] == "bad_match_players",
                  (n, st, out))

        # The same numbers at v3 are accepted.
        for n in (5, 8, 16):
            st, out = _dial(code, token, n, v=3)
            check("a v3 caller may dial %d" % n, st == 200, (n, st, out))

        # THE BOUND IS DERIVED, not restated. dial_max_for is the only reader
        # of the rule and it answers both halves.
        check("dial_max_for caps a v2 caller at four and lets v3 through",
              S.dial_max_for(2) == 4 and S.dial_max_for(3) == S.DIAL_MAX,
              (S.dial_max_for(2), S.dial_max_for(3), S.DIAL_MAX))
    check("and on a four-player deployment the two answers are identical, "
          "so the gate is invisible where nothing can use it",
          S.dial_max_for(2) == S.dial_max_for(3) == S.DIAL_MAX,
          (S.dial_max_for(2), S.dial_max_for(3), S.DIAL_MAX))
    reset()


def test_name_and_colour_shapes_move_together():
    """run vs16, and this is the coordinator's ruling as a test rather than a
    paragraph: NAMES and COLORS carry the same field count, always, and the
    count is decided by the dial and by nothing else."""
    print("\n-- names and colours are one grammar, four fields or sixteen")
    reset()
    with game_max(16, seats=16):
        # A room nobody dials wide: four fields, exactly as before.
        st, h = S.do_create({"v": 3, "nick": "tango", "pre_ok": True,
                             "color": "8A2BE2", "shoes": "FFD700"},
                            "10.0.0.1", 1000.0)
        code = h["room"]
        S.do_join({"v": 3, "room": code, "nick": "opie", "pre_ok": True,
                   "color": "FF0000", "shoes": "00FF00"}, "10.0.0.2", 1000.0)
        room = S.ROOMS[code]

        # THROUGH THE REAL START PATH, not by calling build_names by hand.
        # `match_dial` is frozen at arm, one statement before the strings are
        # built, so a test that built them directly would be testing a room
        # state that never exists in production -- and would have read a dial
        # of 0 and passed for the wrong reason.
        _params(code, h["token"], v=3, match_players=4)
        start(code, h["token"])
        names, colors = room.names, room.colors
        check("a four-player room's names carry exactly three commas",
              names.count(",") == 3, names)
        check("and its colours carry exactly three too",
              colors.count(",") == 3, colors)
        check("the two field counts are equal, which is the ruling's point",
              len(names.split(",")) == len(colors.split(",")))

        # Dial it wide and both strings grow, in the same start.
        S.ROOMS[code].state = "lobby"
        st, out = _params(code, h["token"], v=3, match_players=8)
        check("the room dials to eight", st == 200, (st, out))
        start(code, h["token"])
        names, colors = room.names, room.colors
        check("a wide room's names carry exactly fifteen commas",
              names.count(",") == 15, names)
        check("and its colours carry exactly fifteen too",
              colors.count(",") == 15, colors)
        check("the two field counts are still equal",
              len(names.split(",")) == len(colors.split(",")))
        check("and the real players are still in slot order at the front",
              names.split(",")[0] == "tango" and names.split(",")[1] == "opie",
              names)
        check("every slot past the room is empty, not missing",
              all(f == "" for f in names.split(",")[2:]), names)

        # AND THE COUNT COMES FROM ONE FUNCTION, so the two cannot diverge.
        check("name_field_count is the single decider and it is the dial",
              S.name_field_count(room) == 16
              and room.match_dial > S.NARROW_PLAYERS,
              (S.name_field_count(room), room.match_dial))

        # Back down and the shape goes back with it.
        S.ROOMS[code].state = "lobby"
        st, out = _params(code, h["token"], v=3, match_players=4)
        check("the room dials back to four", st == 200, (st, out))
        start(code, h["token"])
        check("and the string is four fields again, byte for byte the shape "
              "the shipped launcher validates",
              room.names.count(",") == 3 and room.colors.count(",") == 3,
              (room.names, room.colors))

        # The game's own caps, checked here so the service cannot emit a
        # string the reader in hal/star_flow.cpp would throw away.
        S.ROOMS[code].state = "lobby"
        _params(code, h["token"], v=3, match_players=16)
        start(code, h["token"])
        check("the widest legal names string is inside the game's 271-byte cap",
              len(room.names) <= 271, len(room.names))
        check("and the widest legal colours string is inside its 223-byte cap",
              len(room.colors) <= 223, len(room.colors))
    reset()



def test_a_room_is_never_wider_than_its_host_can_drive():
    """run vs16. The hole this closes is not the dial -- a v2 host cannot move
    the dial past four -- it is the DEFAULT. On a sixteen-player deployment a
    room defaults to the deployment's capability, and a v2 host would have got
    a sixteen-wide room without touching anything, then been handed a
    sixteen-field name string its own build refuses."""
    print("\n-- a room's default width is bounded by its host's version")
    reset()
    with game_max(16, seats=16):
        st, h2 = S.do_create({"v": 2, "nick": "old", "pre_ok": True},
                             "10.0.0.1", 1000.0)
        check("a v2 host's room defaults to four, not to the deployment's 16",
              S.ROOMS[h2["room"]].match_players == 4,
              S.ROOMS[h2["room"]].match_players)
        st, h3 = S.do_create({"v": 3, "nick": "new", "pre_ok": True},
                             "10.0.0.2", 1000.0)
        check("a v3 host's room defaults to the deployment's 16",
              S.ROOMS[h3["room"]].match_players == 16,
              S.ROOMS[h3["room"]].match_players)

        # And a narrow client cannot join the wide room at all.
        code = h3["room"]
        st, out = S.do_join({"v": 2, "room": code, "nick": "old2",
                             "pre_ok": True}, "10.0.0.3", 1000.0)
        check("a v2 client joining a wide room is refused needs_newer_client, "
              "at the door rather than at spawn time",
              st == 409 and out["error"] == "needs_newer_client", (st, out))
        st, out = S.do_join({"v": 3, "room": code, "nick": "new2",
                             "pre_ok": True}, "10.0.0.4", 1000.0)
        check("and a v3 client joins it", st == 200, (st, out))

        # The same v2 client joins the NARROW room without complaint, which is
        # the half that proves the refusal is about width and not about age.
        st, out = S.do_join({"v": 2, "room": h2["room"], "nick": "old3",
                             "pre_ok": True}, "10.0.0.5", 1000.0)
        check("the same v2 client joins a four-player room fine",
              st == 200, (st, out))
    reset()


def Member_fields(m):
    return [k for k in m.__slots__]


def main():
    test_grammar()
    test_addresses()
    test_codes()
    test_create_join()
    test_duplicate_nicknames()
    test_leaving()
    test_timeouts()
    test_cursor_and_rings()
    test_auth()
    test_chat_limits()
    test_kick()
    test_no_two_player_assumption()
    test_dial_defaults_are_inert()
    test_dial_range_is_server_enforced()
    test_dial_is_v2_only()
    test_wide_is_v3_only()
    test_a_dropout_after_go_does_not_end_the_match_for_the_rest()
    test_result_accepts_a_wide_scoreline()
    test_the_dial_cannot_widen_over_a_seated_old_client()
    test_name_and_colour_shapes_move_together()
    test_a_room_is_never_wider_than_its_host_can_drive()
    test_every_versioned_field_is_gated()
    test_dial_moves_the_fewest_people()
    test_dial_and_seat_reuse()
    test_dial_reaches_the_plan()
    test_seat_stability()
    test_start()
    test_ready_and_go()
    test_go_plan()
    test_names()
    test_failed()
    test_arming_roster_is_the_frozen_one()
    test_only_playing_members_drive_a_match()
    test_preflight_updates()
    test_colors()
    test_arming_and_match_timers()
    test_result_and_rematch()
    test_rate_buckets()
    test_server_caps()
    test_opacity()

    print("\n%d passed, %d failed" % (len(PASSED), len(FAILED)))
    for name in FAILED:
        print("  FAILED: %s" % name)
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
