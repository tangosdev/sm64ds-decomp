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
              "do_kick", "do_leave", "sweep"):
    setattr(S, _name, _locked(getattr(S, _name)))
S.VERBS = {"create": S.do_create, "join": S.do_join, "poll": S.do_poll,
           "chat": S.do_chat, "params": S.do_params, "kick": S.do_kick,
           "leave": S.do_leave}

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
              "seen", "chat_allowance", "chat_stamp", "last_addr"},
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
        S.GAME_MAX_PLAYERS = 4
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
    test_rate_buckets()
    test_server_caps()
    test_opacity()

    print("\n%d passed, %d failed" % (len(PASSED), len(FAILED)))
    for name in FAILED:
        print("  FAILED: %s" % name)
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
