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
              "do_failed", "do_kick", "do_ban", "do_leave", "do_world",
              "do_world_ping", "do_settings", "sweep"):
    setattr(S, _name, _locked(getattr(S, _name)))
S.VERBS = {"create": S.do_create, "join": S.do_join, "poll": S.do_poll,
           "chat": S.do_chat, "params": S.do_params,
           "preflight": S.do_preflight, "color": S.do_color,
           "start": S.do_start, "settings": S.do_settings,
           "ready": S.do_ready, "result": S.do_result, "failed": S.do_failed,
           "kick": S.do_kick, "ban": S.do_ban, "leave": S.do_leave,
           "world": S.do_world, "world_ping": S.do_world_ping}

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
    S.WORLDS.clear()
    S.BUCKETS = S.Buckets()
    S.WAITERS = 0


GAME = "t-1.0"      # the game build id every test room plays (v5)


def create(nick="tango", who="10.0.0.1", now=1000.0, **extra):
    body = {"v": 5, "nick": nick, "game": GAME}
    body.update(extra)
    return S.do_create(body, who, now)


def join(room, nick, who="10.0.0.2", now=1000.0, **extra):
    body = {"v": 5, "room": room, "nick": nick, "game": GAME}
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
          S.shape({"v": 5, "nick": "x", "colour": "red"}, ("v", "nick")) == "bad_field")
    check("a missing required key is bad_field",
          S.shape({"v": 5}, ("v", "nick")) == "bad_field")
    check("a declared optional key is allowed",
          S.shape({"v": 5, "nick": "x", "pre_ok": True}, ("v", "nick"),
                  ("pre_ok",)) is None)
    check("the exact required set passes",
          S.shape({"v": 5, "nick": "x"}, ("v", "nick")) is None)


def test_addresses():
    print("\n-- address blunting (no part of an address ever reaches a log)")
    check("an IPv4 address leaves nothing", S.blunt_host("203.0.113.42") == "client")
    check("an IPv6 address leaves nothing", S.blunt_host("2001:db8:1234::1") == "client")
    check("localhost leaves nothing", S.blunt_host("127.0.0.1") == "client")
    check("an empty address does not crash", S.blunt_host("") == "client")
    check("junk does not crash", S.blunt_host("not-an-address") == "client")


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

    st, out = S.do_leave({"v": 5, "room": code, "token": g2["token"]},
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

    st, out = S.do_leave({"v": 5, "room": code, "token": g2["token"]},
                         "10.0.0.2", 1002.0)
    check("a token that already left is not a member any more",
          st == 403 and out["error"] == "not_a_member", (st, out))

    st, out = S.do_leave({"v": 5, "room": code, "token": host["token"]},
                         "10.0.0.1", 1003.0)
    check("the host can leave", st == 200)
    # run mp1 L-J1: a VS host leaving passes the host on; the room closes only
    # when its LAST member leaves.
    check("the host leaving a room with people in it does NOT close it",
          room.state == "lobby" and room.host_seat == 3, (room.state,
                                                          room.host_seat))
    check("a host event names the new host",
          room.events[-1]["kind"] == "host" and room.events[-1]["seat"] == 3,
          room.events[-1])
    st, out = S.do_leave({"v": 5, "room": code, "token": g3["token"]},
                         "10.0.0.3", 1003.5)
    check("the last member (the new host) leaving closes the room",
          room.state == "closed")
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
    S.do_poll({"v": 5, "room": code, "token": host["token"], "cursor": 0,
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
    S.do_poll({"v": 5, "room": code, "token": g["token"], "cursor": 0,
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
        st, out = S.do_chat({"v": 5, "room": code, "token": host["token"],
                             "text": "line %d" % i}, "10.0.0.1",
                            1000.0 + i * 3)
        if st != 200:
            check("chat %d accepted" % i, False, out)
        seqs.append(out["cursor"])
    check("the cursor is strictly monotonic",
          seqs == sorted(seqs) and len(set(seqs)) == len(seqs), seqs)

    st, out = S.do_poll({"v": 5, "room": code, "token": g["token"],
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

    st, out = S.do_poll({"v": 5, "room": code, "token": g["token"],
                         "cursor": 1, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor older than the ring asks the client to resync",
          out.get("resync") is True, out.get("resync"))
    check("a resync answer carries the retained chat",
          len(out["events"]) == S.CHAT_RING
          and all(e["kind"] == "chat" for e in out["events"]))
    check("a resync answer carries the whole view",
          out["view"]["state"] == "lobby" and len(out["view"]["members"]) == 2)

    fresh = room.events[-1]["seq"] - 3
    st, out = S.do_poll({"v": 5, "room": code, "token": g["token"],
                         "cursor": fresh, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor inside the ring does NOT ask for a resync",
          "resync" not in out and len(out["events"]) == 3, out.get("events"))

    st, out = S.do_poll({"v": 5, "room": code, "token": g["token"],
                         "cursor": room.seq + 50, "wait": 0}, "10.0.0.2", 1100.0)
    check("a cursor from the future asks for a resync too",
          out.get("resync") is True)


def test_auth():
    print("\n-- who may do what")
    reset()
    st, host = create()
    code = host["room"]
    st, g = join(code, "opie")

    st, out = S.do_params({"v": 5, "room": code, "token": g["token"], "map": 1,
                           "win_mode": "time"}, "10.0.0.2", 1000.0)
    check("a member cannot set params",
          st == 403 and out["error"] == "not_host", (st, out))
    st, out = S.do_params({"v": 5, "room": code, "token": host["token"],
                           "map": 1, "win_mode": "time"}, "10.0.0.1", 1000.0)
    check("the host can", st == 200, out)
    check("and the room took the values",
          S.ROOMS[code].map == 1 and S.ROOMS[code].win_mode == "time"
          and S.ROOMS[code].star_target is None)

    st, out = S.do_params({"v": 5, "room": code, "token": host["token"],
                           "map": 3, "win_mode": "stars", "star_target": 5},
                          "10.0.0.1", 1000.0)
    check("stars with a target is accepted", st == 200, out)
    check("the host picks the target, and every value up to the ceiling works",
          all(S.do_params({"v": 5, "room": code, "token": host["token"],
                           "map": 0, "win_mode": "stars", "star_target": n},
                          "10.0.0.1", 1000.0)[0] == 200
              for n in range(1, S.STAR_TARGET_MAX + 1)))
    check("one past the ceiling is refused",
          S.do_params({"v": 5, "room": code, "token": host["token"], "map": 0,
                       "win_mode": "stars",
                       "star_target": S.STAR_TARGET_MAX + 1},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_star_target")
    check("zero is refused too",
          S.do_params({"v": 5, "room": code, "token": host["token"], "map": 0,
                       "win_mode": "stars", "star_target": 0},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_star_target")
    check("the ceiling is a knob, not a literal scattered around the file",
          S.STAR_TARGET_MAX == 5)

    stranger = "f" * 32
    for verb, extra in (("chat", {"text": "hi"}), ("leave", {}),
                        ("params", {"map": 0, "win_mode": "time"})):
        body = {"v": 5, "room": code, "token": stranger}
        body.update(extra)
        st, out = S.VERBS[verb](body, "10.0.0.9", 1000.0)
        check("a stranger's token is refused by %s" % verb,
              st == 403 and out["error"] == "not_a_member", (verb, st, out))

    st, out = S.do_poll({"v": 5, "room": code, "token": stranger, "cursor": 0,
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
        results.append(S.do_chat({"v": 5, "room": code, "token": host["token"],
                                  "text": "x"}, "10.0.0.1", t)[0])
    check("the burst is spent and then refused",
          results[:3] == [200, 200, 200] and results[3:] == [429, 429, 429],
          results)
    st, _ = S.do_chat({"v": 5, "room": code, "token": g["token"], "text": "x"},
                      "10.0.0.2", t)
    check("the OTHER member is unaffected: the limit is per member", st == 200)
    st, _ = S.do_chat({"v": 5, "room": code, "token": host["token"], "text": "x"},
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
    body = {"v": 5, "room": code, "token": token, "map": 0,
            "win_mode": "time"}
    body.update(extra)
    return S.do_params(body, who, now)


def _dial(code, token, n, v=5, **kw):
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

    # A params that does not carry the dial (the host changing the arena)
    # leaves the dial alone rather than resetting it.
    reset()
    st, host = create()
    code = host["room"]
    room = S.ROOMS[code]
    _dial(code, host["token"], 2)
    check("the host sets the dial to 2", room.match_players == 2)
    st, out = _params(code, host["token"], map=3)
    check("a params without the dial (the host changing the arena) is accepted",
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


def test_the_v5_floor():
    """Contract v5 (lobby 1.0) is the floor; v6 (party and coop rooms) is the
    ceiling and is additive.

    Launchers 0.3.x..0.5.x speak v1..v4 and map `bad_version` to "Check for an
    update", so the floor is how an old launcher is kept out of a 1.0 room
    instead of spawning a game that cannot pair with it.
    """
    print("\n-- contract v5 is the floor and the ceiling")
    check("the version range is exactly 5..7",
          (S.CONTRACT_MIN, S.CONTRACT_V) == (5, 7),
          (S.CONTRACT_MIN, S.CONTRACT_V))
    for verb, row in sorted(S.VERB_FIELDS.items()):
        body = {k: _FIELD_SAMPLES[k] for k in row[0]}
        for ver in (1, 2, 3, 4, 8):
            body["v"] = ver
            check("%s: a v%d body is bad_version" % (verb, ver),
                  S.shape_for(verb, body) == "bad_version",
                  S.shape_for(verb, body))
    reset()
    st, out = S.do_create({"v": 4, "nick": "old"}, "10.0.0.1", 1000.0)
    check("a v4 create (launcher 0.5.x) is 400 bad_version",
          st == 400 and out["error"] == "bad_version", (st, out))
    check("and it made no room", not S.ROOMS, list(S.ROOMS))

    # -- the game build field -----------------------------------------------
    st, out = S.do_create({"v": 5, "nick": "t"}, "10.0.0.1", 1000.0)
    check("a v5 create without `game` is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))
    for bad, code in (("", "bad_game"), ("a" * 25, "too_long"),
                      ("1.0 beta", "bad_game"), ("1.0/x", "bad_game"),
                      ("1.0\n", "bad_game"), ("\u00e9", "bad_game"),
                      (10, "bad_game"), (None, "bad_game")):
        st, out = S.do_create({"v": 5, "nick": "t", "game": bad},
                              "10.0.0.1", 1000.0)
        check("game %r is refused %s" % (bad, code),
              st == 400 and out["error"] == code, (st, out))
    for good in ("1.0.0", "a" * 24, "0.5.4-rc_2", "X"):
        check("game %r is a legal build id" % good,
              S.v_game({"game": good}) == (good, None), S.v_game({"game": good}))

    reset()
    st, host = S.do_create({"v": 5, "nick": "tango", "game": "1.0.0"},
                           "10.0.0.1", 1000.0)
    code = host["room"]
    check("the room remembers its creator's build",
          S.ROOMS[code].game == "1.0.0", S.ROOMS[code].game)
    st, out = S.do_join({"v": 5, "room": code, "nick": "opie",
                         "game": "1.0.1"}, "10.0.0.2", 1000.0)
    check("a join from another build is 409 game_mismatch",
          st == 409 and out["error"] == "game_mismatch", (st, out))
    check("and seats nobody", len(S.ROOMS[code].members) == 1)
    st, out = S.do_join({"v": 5, "room": code, "nick": "opie"},
                        "10.0.0.2", 1000.0)
    check("a join without `game` is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))
    st, out = S.do_join({"v": 5, "room": code, "nick": "opie",
                         "game": "1.0.0"}, "10.0.0.2", 1000.0)
    check("the same build joins", st == 200, (st, out))
    # A full room still says game_mismatch to the wrong build: the reason that
    # is actually theirs, not "room full".
    with game_max(2, seats=2):
        st, h = S.do_create({"v": 5, "nick": "a", "game": "1.0.0"},
                            "10.0.0.1", 1000.0)
        S.do_join({"v": 5, "room": h["room"], "nick": "b", "game": "1.0.0"},
                  "10.0.0.2", 1000.0)
        st, out = S.do_join({"v": 5, "room": h["room"], "nick": "c",
                             "game": "0.9"}, "10.0.0.3", 1000.0)
        check("a full room answers the wrong build game_mismatch, not "
              "room_full", st == 409 and out["error"] == "game_mismatch",
              (st, out))


# A body that satisfies each verb's REQUIRED fields, so the generic walk below
# can add one optional key to it and be sure that key is the only thing under
# test. The values need only be well-formed enough to reach the shape check,
# which runs before any of them is validated.
_FIELD_SAMPLES = {
    "v": 5,
    "room": "K7QMR3",
    "token": "0" * 32,
    "nick": "tango",
    "text": "gg",
    "cursor": 0,
    "wait": 0,
    "map": 0,
    "win_mode": "time",
    "star_target": 3,
    "king_target": 30,
    "match_players": 2,
    "seat": 2,
    "match": "0" * 16,
    "reason": "spawn_failed",
    "win": "slot",
    "scores": [0, 0, 0, 0],
    "pre_ok": True,
    "color": "ff8800",
    "shoes": "1a2b3c",
    "session_type": "adventure",
    "game": "1.0.0",
    "luigi": 1,
    "voice": 1,
    "netmode": "arena",
    "install": "0123456789abcdef0123456789abcdef",
    "code": "K7QMR3AB",
    "skip": "K7QMR3AB",
    "save_name": "My file",
    "settings": {"collision": 0},
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

    # Since v5 every field in the table arrived at or below the floor, so the
    # list below is empty today; the walk stays so the next version's fields
    # are gated on the day they are added.

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

    check("and no field or verb names a version this server does not answer",
          all(sc <= S.CONTRACT_V for _, _, sc in versioned)
          and all(sc <= S.CONTRACT_V for _, sc in late_verbs)
          and all(S.CONTRACT_MIN >= g for g in (S.V_DIAL, S.V_COLOR,
                                                S.V_SESSION)),
          (versioned, late_verbs))
    check("and the retired second definition of v2 is really gone -- no "
          "PROTO_VERSIONS, no PROTO_COLOR_V",
          not hasattr(S, "PROTO_VERSIONS") and not hasattr(S, "PROTO_COLOR_V"),
          [n for n in ("PROTO_VERSIONS", "PROTO_COLOR_V") if hasattr(S, n)])

    # The other half of the same guarantee: a key NO version defines is
    # refused at every version, including the newest.
    for ver in range(S.CONTRACT_MIN, S.CONTRACT_V + 1):
        body = {"v": ver, "nick": "tango", "game": "1.0.0", "colour": "red"}
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

    S.do_leave({"v": 5, "room": code, "token": b["token"]}, "10.0.0.2", 1000.0)
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

    st, out = S.do_start({"v": 5, "room": code, "token": token},
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
    S.do_start({"v": 5, "room": code, "token": token}, "10.0.0.1", 1000.0)
    plan = S.member_plan(room, 1)
    check("with 2 of a 4-player dial filled, the plan says players=2 and "
          "match_players=4 -- no pretending",
          plan["players"] == 2 and plan["match_players"] == 4, plan)
    check("and SM64DS_VS_PLAYERS therefore rides the REAL count",
          plan["players"] == room.agreed_players)
    _r = S.do_failed({"v": 5, "room": code, "token": token,
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
    st, out = S.do_chat({"v": 5, "room": code, "token": host["token"],
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
              # v7: the game install's random id, kept only for a ban.
              "install"},
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

    st, out = S.do_kick({"v": 5, "room": code, "token": g2["token"], "seat": 3},
                        "10.0.0.2", 1000.0)
    check("a member cannot kick anybody",
          st == 403 and out["error"] == "not_host", (st, out))

    st, out = S.do_kick({"v": 5, "room": code, "token": host["token"],
                         "seat": 1}, "10.0.0.1", 1000.0)
    check("the host cannot kick itself",
          st == 400 and out["error"] == "bad_seat", (st, out))

    st, out = S.do_kick({"v": 5, "room": code, "token": host["token"],
                         "seat": 4}, "10.0.0.1", 1000.0)
    check("an empty seat cannot be kicked",
          st == 400 and out["error"] == "bad_seat", (st, out))

    st, out = S.do_kick({"v": 5, "room": code, "token": host["token"],
                         "seat": 9}, "10.0.0.1", 1000.0)
    check("a seat number outside the room is bad_seat",
          st == 400 and out["error"] == "bad_seat", (st, out))

    victim_token = g2["token"]
    st, out = S.do_kick({"v": 5, "room": code, "token": host["token"],
                         "seat": 2}, "10.0.0.1", 1000.0)
    check("the host can kick a member", st == 200 and out.get("ok") is True,
          (st, out))
    check("the seat is freed", 2 not in room.members)
    check("it reuses the left event with why=kicked, not a new event kind",
          room.events[-1]["kind"] == "left" and room.events[-1]["seat"] == 2
          and room.events[-1]["why"] == "kicked", room.events[-1])
    check("the spectator is promoted exactly as it is for a plain leave",
          room.members[3].playing is True)

    st, out = S.do_poll({"v": 5, "room": code, "token": victim_token,
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
    S.do_kick({"v": 5, "room": code, "token": host["token"], "seat": 2},
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
    S.do_kick({"v": 5, "room": code, "token": host["token"], "seat": 2},
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
    st, out = S.do_kick({"v": 5, "room": code, "token": host["token"],
                         "seat": 2}, "10.0.0.1", 1000.0)
    check("a kick is refused once the room has left the lobby state",
          st == 409 and out["error"] == "not_in_lobby", (st, out))


INST_A = "a" * 32
INST_B = "b" * 32
INST_C = "c" * 32


def _ban(code, token, seat, who="10.0.0.1", now=1000.0, v=7):
    return S.do_ban({"v": v, "room": code, "token": token, "seat": seat},
                    who, now)


def test_ban():
    print("\n-- the host banning somebody for the room's life (v7, LOBBYBAN1)")
    reset()
    st, host = create(who="10.0.0.1", v=7, install=INST_A)
    code = host["room"]
    st, g2 = join(code, "rude", who="10.0.0.2", v=7, install=INST_B)
    st, g3 = join(code, "fine", who="10.0.0.3", v=7, install=INST_C)
    room = S.ROOMS[code]
    check("the install id is stored per member and never reaches a client",
          room.members[2].install == INST_B
          and INST_B not in json.dumps(room.view(1))
          and "install" not in room.members[2].view(), room.view(1))

    st, out = _ban(code, g2["token"], 3, who="10.0.0.2")
    check("a member who is not the host cannot ban",
          st == 403 and out["error"] == "not_host", (st, out))
    st, out = _ban(code, host["token"], 1)
    check("the host cannot ban itself",
          st == 400 and out["error"] == "bad_seat", (st, out))
    st, out = _ban(code, host["token"], 4)
    check("an empty seat cannot be banned",
          st == 400 and out["error"] == "bad_seat", (st, out))
    st, out = _ban(code, host["token"], 2, v=6)
    check("a v6 caller cannot reach the ban verb (bad_field, like any v7 verb)",
          st == 400 and out["error"] == "bad_field", (st, out))

    st, out = _ban(code, host["token"], 2)
    check("the host can ban a member", st == 200 and out.get("ok") is True,
          (st, out))
    check("the seat is freed with the same left event a kick sends",
          2 not in room.members and room.events[-1]["kind"] == "left"
          and room.events[-1]["why"] == "kicked", room.events[-1])
    check("the other friend is unaffected", 3 in room.members)

    st, out = S.do_poll({"v": 7, "room": code, "token": g2["token"],
                         "cursor": 0, "wait": 0}, "10.0.0.2", 1000.0)
    check("the banned v7 game is told `banned` on its next request (the token)",
          st == 403 and out["error"] == "banned", (st, out))
    st, out = S.do_chat({"v": 7, "room": code, "token": g2["token"],
                         "text": "hi"}, "10.0.0.9", 1000.0)
    check("its token stays refused from any address",
          st == 403 and out["error"] == "banned", (st, out))
    for ov in (5, 6):
        st, out = S.do_poll({"v": ov, "room": code, "token": g2["token"],
                             "cursor": 0, "wait": 0}, "10.0.0.2", 1000.0)
        check("a v%d poll on a banned token is told `kicked` (a word it knows)"
              % ov, st == 403 and out["error"] == "kicked", (st, out))
        st, out = S.do_leave({"v": ov, "room": code, "token": g2["token"]},
                             "10.0.0.2", 1000.0)
        check("a v%d leave on a banned token is told `kicked` too" % ov,
              st == 403 and out["error"] == "kicked", (st, out))

    st, out = join(code, "rude", who="10.0.0.2", v=7, install=INST_B)
    check("same address, same install (a restarted game): refused banned",
          st == 403 and out["error"] == "banned", (st, out))
    st, out = join(code, "rude", who="10.0.0.2", v=7, install="d" * 32)
    check("same address, a new install (the install file deleted): refused",
          st == 403 and out["error"] == "banned", (st, out))
    st, out = join(code, "rude", who="10.0.0.2", v=6)
    check("same address from an old client with no install: refused `kicked`",
          st == 403 and out["error"] == "kicked", (st, out))
    st, out = join(code, "rude", who="10.0.0.2", v=5)
    check("and a v5 client the same way",
          st == 403 and out["error"] == "kicked", (st, out))
    st, out = join(code, "rude", who="10.0.0.77", v=7, install=INST_B)
    check("a new address, the same install: refused",
          st == 403 and out["error"] == "banned", (st, out))

    later = 1000.0 + S.KICK_COOLDOWN_S + 3600
    st, out = join(code, "rude", who="10.0.0.2", now=later, v=7,
                   install=INST_B)
    check("the ban outlives the kick cooldown",
          st == 403 and out["error"] == "banned", (st, out))
    st, out = join(code, "newfriend", who="10.0.0.5", now=later, v=7,
                   install="e" * 32)
    check("somebody else still joins the room", st == 200, (st, out))

    # A new room by the same host is clean.
    S.do_leave({"v": 6, "room": code, "token": host["token"]}, "10.0.0.1",
               later)
    st, host2 = create(who="10.0.0.1", now=later, v=7, install=INST_A)
    st, out = join(host2["room"], "rude", who="10.0.0.2", now=later, v=7,
                   install=INST_B)
    check("a new room starts clean: the banned player joins it", st == 200,
          (st, out))

    # Kick still behaves exactly as before, beside a ban.
    reset()
    st, host = create(who="10.0.0.1", v=7, install=INST_A)
    code = host["room"]
    join(code, "kicked", who="10.0.0.2", v=7, install=INST_B)
    S.do_kick({"v": 6, "room": code, "token": host["token"], "seat": 2},
              "10.0.0.1", 1000.0)
    st, out = join(code, "kicked", who="10.0.0.2", v=7, install=INST_B)
    check("a kick is still a kick (refused `kicked`, not `banned`)",
          st == 403 and out["error"] == "kicked", (st, out))
    st, out = join(code, "kicked", who="10.0.0.2", v=7, install=INST_B,
                   now=1000.0 + S.KICK_COOLDOWN_S + 1)
    check("and lets the player back after the cooldown", st == 200, (st, out))

    # Only in the lobby state, like kick.
    reset()
    st, host = create(who="10.0.0.1", v=7)
    code = host["room"]
    join(code, "b", who="10.0.0.2", v=7)
    S.ROOMS[code].state = "arming"
    st, out = _ban(code, host["token"], 2)
    check("a ban is refused once the room has left the lobby state",
          st == 409 and out["error"] == "not_in_lobby", (st, out))

    # The install field: optional, v7 only, shape checked.
    reset()
    st, out = create(who="10.0.0.1", v=6, install=INST_A)
    check("a v6 create carrying install is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))
    for bad in ("A" * 32, "a" * 15, "a" * 65, "zz" * 10, 12345, "../x" * 5):
        st, out = create(who="10.0.0.1", v=7, install=bad)
        check("install %r is refused bad_install" % (bad,),
              st == 400 and out["error"] == "bad_install", (st, out))
    st, out = create(who="10.0.0.1", v=7)
    check("a v7 create without install is fine (it is optional)", st == 200,
          (st, out))
    st, j = join(out["room"], "x", who="10.0.0.2", v=7, install="bad!")
    check("a join with a bad install is refused bad_install",
          st == 400 and j["error"] == "bad_install", (st, j))

    # A v5/v6 client (the 1.0 game, today's launcher) still creates and joins.
    reset()
    st, host = create(who="10.0.0.1", v=6, session_type="coop",
                      save_name="Coop 1")
    st2, g = join(host["room"], "old", who="10.0.0.2", v=6)
    check("a v6 client with no install creates and joins as before",
          st == 200 and st2 == 200, (st, host, st2, g))
    # And a banned old client is told what it already understands.
    _ban(host["room"], host["token"], 2)
    st, out = S.do_poll({"v": 6, "room": host["room"], "token": g["token"],
                         "cursor": 0, "wait": 0}, "10.0.0.2", 1000.0)
    check("a banned old client's next poll says `kicked`, so it leaves",
          st == 403 and out["error"] == "kicked", (st, out))
    st, out = join(host["room"], "old", who="10.0.0.2", v=6)
    check("a banned old client's join is refused (403 kicked)",
          st == 403 and out["error"] == "kicked", (st, out))
    later = 1000.0 + S.KICK_COOLDOWN_S + 3600
    st, out = join(host["room"], "old", who="10.0.0.2", v=6, now=later)
    check("and stays refused after the kick cooldown (it is still a ban)",
          st == 403 and out["error"] == "kicked", (st, out))

    # The cap on what one room keeps.
    reset()
    old = S.BAN_MAX
    S.BAN_MAX = 2
    try:
        st, host = create(who="10.0.0.1", v=7)
        code = host["room"]
        results = []
        for i in range(3):
            st, g = join(code, "p%d" % i, who="10.0.1.%d" % i, v=7)
            results.append(_ban(code, host["token"], g["member"]))
        check("a room keeps at most BAN_MAX bans, then says too_many_bans",
              [r[0] for r in results] == [200, 200, 409]
              and results[2][1]["error"] == "too_many_bans", results)
    finally:
        S.BAN_MAX = old


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
    S.do_leave({"v": 5, "room": code, "token": tokens[3]}, "10.0.0.3", 1001.0)
    S.do_kick({"v": 5, "room": code, "token": host["token"], "seat": 4},
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
    S.do_leave({"v": 5, "room": code, "token": guest2.token}, "10.0.0.2", 1001.0)
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
    return S.do_start({"v": 5, "room": code, "token": token}, who, now)


def ready(code, token, match, who="10.0.0.1", now=1000.0):
    return S.do_ready({"v": 5, "room": code, "token": token, "match": match},
                      who, now)


def poll(code, token, cursor, who="10.0.0.1", now=1000.0, wait=0):
    return S.do_poll({"v": 5, "room": code, "token": token, "cursor": cursor,
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
    st, pp = S.do_params({"v": 5, "room": code, "token": host["token"],
                          "map": 2, "win_mode": "time"}, "10.0.0.1", 1001.0)
    check("params is 409 not_in_lobby while arming",
          st == 409 and pp["error"] == "not_in_lobby", (st, pp))
    st, jj = join(code, "ace", who="10.0.0.9", now=1001.0)
    check("a join while arming is a WAITING seat (L-J1; was 409 in_match)",
          st == 200 and room.members[jj["member"]].playing is False
          and jj["member"] not in room.slot_of, (st, jj))
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


def test_session_type():
    print("\n-- session types: vs, adventure, and party refused")
    reset()
    st, host = create(nick="host")
    check("a plain create is a vs room", st == 200
          and S.ROOMS[host["room"]].session_type == "vs", (st, host))
    check("and its view says so", host["view"]["session_type"] == "vs")
    check("a vs create answer carries no plan (VS plans come at go)",
          "plan" not in host, host)
    for kind in ("party", "coop"):
        reset()
        st, out = create(nick="host", session_type=kind)
        check("a v5 create of a %s room is bad_session_type (Check for an "
              "update), not a room" % kind,
              st == 400 and out["error"] == "bad_session_type"
              and not S.ROOMS, (st, out))
    for bad in ("bogus", "", 1, None, "Party", "coop "):
        reset()
        st, out = create(nick="host", session_type=bad)
        check("session_type %r is bad_session_type" % (bad,),
              st == 400 and out["error"] == "bad_session_type", (st, out))
        check("and made no room", not S.ROOMS)

    # A vs room's plan carries the type, so a launcher never has to guess.
    reset()
    st, host = create(nick="host", pre_ok=True)
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    join(code, "b", pre_ok=True)
    start(code, token)
    check("a vs room's plan says session_type=vs",
          S.member_plan(room, 1)["session_type"] == "vs")


def test_adventure_room():
    """v5 adventure rooms (design section 5): one hub code for the room's
    life, no start gate, a plan at create/join and on every poll, the room
    outlives its creator and closes after ten minutes empty."""
    print("\n-- v5 adventure rooms: a hub code, no start gate")
    reset()
    st, host = create(nick="host", session_type="adventure")
    code, token = host["room"], host["token"]
    room = S.ROOMS[code]
    check("an adventure create makes an adventure room", st == 200
          and room.session_type == "adventure", (st, host))
    plan = host.get("plan", {})
    check("the create answer carries the plan at once: no start gate",
          plan == {"playing": True, "session_type": "adventure",
                   "code": room.comms_code, "relay": S.RELAY_ADDR,
                   "relay_v": 2, "want_seat": 255}, plan)
    check("the hub code is 8 characters of the room alphabet and is not the "
          "room code", len(plan["code"]) == 8 and plan["code"] != code
          and all(ch in S.ROOM_ALPHABET for ch in plan["code"]), plan["code"])
    check("the plan names no level and no VS field (the title boots)",
          not any(k in plan for k in ("map", "players", "names", "slot",
                                       "luigi", "seed", "digest")), plan)

    guests = []
    for k in range(3):
        st, g = join(code, "g%d" % k, who="10.0.1.%d" % k, pre_ok=bool(k % 2))
        guests.append(g)
    check("every joiner gets the SAME hub code, pre_ok or not",
          all(g["plan"]["code"] == plan["code"] for g in guests),
          [g.get("plan") for g in guests])
    check("and everybody is playing: an adventure room has no dial",
          all(m.playing for m in room.members.values()))
    st, out = poll(code, guests[0]["token"], 0, who="10.0.1.0")
    check("every poll answer carries the plan",
          out.get("plan") == guests[0]["plan"], out.get("plan"))
    st, out = start(code, token)
    check("start is refused in an adventure room: 409 not_vs",
          st == 409 and out["error"] == "not_vs", (st, out))
    check("and the room is still in lobby with the same code",
          room.state == "lobby" and room.comms_code == plan["code"])
    st, out = _params(code, token, match_players=2)
    check("the dial on an adventure room is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))

    # THE ROOM OUTLIVES ITS CREATOR.
    st, out = S.do_leave({"v": 5, "room": code, "token": token},
                         "10.0.0.1", 1001.0)
    check("the creator leaves", st == 200, (st, out))
    check("the room stays open", room.state == "lobby", room.state)
    check("the host role passes to the lowest seat still in",
          room.host_seat == 2, room.host_seat)
    ev = room.events[-1]
    check("with a host event", ev["kind"] == "host" and ev["seat"] == 2, ev)
    check("and the hub code has not changed",
          room.comms_code == plan["code"])
    for g, k in zip(guests, range(3)):
        S.do_leave({"v": 5, "room": code, "token": g["token"]},
                   "10.0.1.%d" % k, 1002.0)
    check("the last member leaves and the room is still open, empty",
          room.state == "lobby" and not room.members, room.state)
    S.sweep(1003.0)
    check("the reaper starts the empty clock and keeps the room",
          room.state == "lobby" and room.empty_since == 1003.0,
          (room.state, room.empty_since))
    S.sweep(1003.0 + S.ADV_EMPTY_S - 1)
    check("one second short of ADV_EMPTY_S it is still open",
          room.state == "lobby")
    st, back = join(code, "back", who="10.0.2.1", now=1003.0 + S.ADV_EMPTY_S - 1)
    check("somebody comes back: seated, host, same hub code",
          st == 200 and room.host_seat == back["member"]
          and back["plan"]["code"] == plan["code"], (st, back))
    S.sweep(1003.0 + S.ADV_EMPTY_S + 5)
    check("a room with a member in it does not close on the empty clock",
          room.state == "lobby" and room.empty_since is None)
    S.do_leave({"v": 5, "room": code, "token": back["token"]}, "10.0.2.1",
               2000.0)
    S.sweep(2000.0)
    S.sweep(2000.0 + S.ADV_EMPTY_S)
    check("empty for ADV_EMPTY_S: closed as idle",
          room.state == "closed" and room.why == "idle", (room.state, room.why))

    # EXEMPT FROM ROOM_IDLE_S: members out playing push no events for hours.
    reset()
    st, host = create(nick="host", session_type="adventure")
    room = S.ROOMS[host["room"]]
    t = 1000.0
    while t < 1000.0 + S.ROOM_IDLE_S + 120:
        t += 20.0
        poll(host["room"], host["token"], room.seq, now=t)
        S.sweep(t)
    check("an adventure room whose member keeps polling outlives ROOM_IDLE_S",
          room.state == "lobby", room.state)

    # The VS host rule since run mp1 L-J1: a VS host leaving passes the host
    # on like an adventure room does (the match, if one is on, ends first; see
    # test_the_host_leaving_passes_the_host_on). A VS room closes only when
    # its last member leaves, with no ADV_EMPTY_S wait.
    reset()
    st, host = create(nick="host")
    join(host["room"], "b")
    S.do_leave({"v": 5, "room": host["room"], "token": host["token"]},
               "10.0.0.1", 1001.0)
    check("a VS host leaving passes the host to seat 2 (was: closes the room)",
          S.ROOMS[host["room"]].state == "lobby"
          and S.ROOMS[host["room"]].host_seat == 2)


def test_go_plan():
    print("\n-- the go plan: per member, identical where it must be")
    reset()
    code, room, host, guest = _two_ready()
    # A third seat: a spectator, since GAME_MAX_PLAYERS is 2.
    st, spec = join(code, "watch", who="10.0.0.3")
    check("the third seat is a spectator", room.members[3].playing is False)
    S.do_params({"v": 5, "room": code, "token": host["token"],
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


def test_session_digest():
    """The digest the game's ACCEPT check reproduces (design 2.3). The vectors
    are the lane's published contract; the game side must match them."""
    print("\n-- the session digest: canonical string and FNV-1a")
    check("FNV-1a of nothing is the offset basis",
          S.fnv1a32(b"") == 2166136261, S.fnv1a32(b""))
    vectors = [
        ((0, 2, "time", None, None, 0, 0),
         "0|2|time|0|0|0|0|lockstep|2", 1172137709),
        ((1, 4, "stars", 3, None, 1, 3735928559),
         "1|4|stars|3|0|1|3735928559|lockstep|2", 909114140),
        ((2, 16, "king", None, 30, 1, 4294967295),
         "2|16|king|0|30|1|4294967295|lockstep|3", 2033119464),
    ]
    for args, text, want in vectors:
        check("canonical %s" % text, S.session_canonical(*args) == text,
              S.session_canonical(*args))
        check("digest of %s is %d" % (text, want),
              S.session_digest(*args) == want, S.session_digest(*args))
    check("the wire field follows the head count: 4 players is wire 2",
          S.session_canonical(0, 4, "time", None, None, 0, 0).endswith("|2"))
    check("and 5 players is wire 3",
          S.session_canonical(0, 5, "time", None, None, 0, 0).endswith("|3"))
    check("a stale star target outside stars mode does not reach the string",
          S.session_canonical(0, 2, "time", 3, None, 0, 0)
          == "0|2|time|0|0|0|0|lockstep|2")
    check("with Luigi off the seed digests as 0 (the game's rule: an off "
          "room does not depend on it)",
          S.session_canonical(0, 2, "time", None, None, 0, 7)
          == "0|2|time|0|0|0|0|lockstep|2")
    # NETCORE1's published examples (out/NETCORE1/session_check_spec.txt),
    # so the lobby and the game's ACCEPT check are pinned to one rule.
    for args, text in (
            ((0, 4, "stars", 1, None, 0, 99), "0|4|stars|1|0|0|0|lockstep|2"),
            ((2, 16, "time", None, None, 1, 12345),
             "2|16|time|0|0|1|12345|lockstep|3"),
            ((1, 8, "king", None, 30, 0, 5), "1|8|king|0|30|0|0|lockstep|3")):
        check("NETCORE1 example %s" % text,
              S.session_canonical(*args) == text, S.session_canonical(*args))
    a = S.session_digest(0, 2, "time", None, None, 1, 1)
    b = S.session_digest(0, 2, "time", None, None, 1, 2)
    c = S.session_digest(0, 2, "time", None, None, 0, 1)
    check("with Luigi on, one seed apart is a different digest, and Luigi off "
          "is a third", len({a, b, c}) == 3, (a, b, c))


def test_vs_one_settings():
    """v5 VS settings: Luigi Infection and the voice switch on `params`, the
    seed minted at `start`, and the six new plan fields."""
    print("\n-- v5 VS settings: luigi, voice, seed, digest, relay seat")
    reset()
    code, room, host, guest = _two_ready()
    tok = host["token"]
    check("a fresh VS room has Luigi off and voice off (voice is shelved)",
          room.luigi == 0 and room.voice == 0, (room.luigi, room.voice))
    view = room.view(1)
    check("and the view says so", view["luigi"] == 0 and view["voice"] == 0,
          view)
    for key in ("luigi", "voice"):
        for bad in (2, -1, True, False, "1", 1.0, None):
            st, out = _params(code, tok, **{key: bad})
            check("%s=%r is 400 bad_field" % (key, bad),
                  st == 400 and out["error"] == "bad_field", (st, out))
    st, out = _params(code, tok, luigi=1, voice=1)
    check("the host sets luigi=1 and asks for voice=1: voice stays 0 (shelved)",
          st == 200 and room.luigi == 1 and room.voice == 0,
          (st, out, room.luigi, room.voice))
    ev = room.events[-1]
    check("the params event carries both",
          ev["kind"] == "params" and ev["luigi"] == 1 and ev["voice"] == 0, ev)
    st, out = _params(code, tok, map=2)
    check("a params without them keeps them",
          st == 200 and room.luigi == 1 and room.voice == 0,
          (room.luigi, room.voice))
    st, out = _params(code, guest["token"], who="10.0.0.2", luigi=0)
    check("a guest cannot set them", st == 403 and out["error"] == "not_host",
          (st, out))

    st, p = start(code, tok)
    check("the room arms", st == 200, (st, p))
    hp, gp = S.member_plan(room, 1), S.member_plan(room, 2)
    check("both plans carry luigi=1 voice=0",
          hp["luigi"] == gp["luigi"] == 1 and hp["voice"] == gp["voice"] == 0,
          (hp, gp))
    check("the seed is a u32 and identical in every member's plan",
          isinstance(hp["seed"], int) and 0 <= hp["seed"] <= 0xFFFFFFFF
          and hp["seed"] == gp["seed"] == room.match_seed, (hp["seed"],
                                                           gp["seed"]))
    check("relay_v is 2 in both", hp["relay_v"] == gp["relay_v"] == 2)
    check("want_seat is the slot: parent 0, child 1",
          hp["want_seat"] == 0 and gp["want_seat"] == 1, (hp, gp))
    want = S.session_digest(hp["map"], hp["players"], "time", None, None,
                            hp["luigi"], hp["seed"])
    check("the digest is the canonical digest of the plan's own fields",
          hp["digest"] == gp["digest"] == want, (hp["digest"], want))
    st, out = _params(code, tok, luigi=0)
    check("and nothing moves once armed", st == 409, (st, out))

    # THE SEED IS PER MATCH. Twenty rematches, twenty seeds from the system
    # RNG: all distinct (a 1-in-10^7 chance of a false failure), each one the
    # same in both plans.
    seeds, digests, same = [], set(), True
    for k in range(20):
        S._reset_match(room)
        st, p = start(code, tok, now=1001.0 + k)
        hp, gp = S.member_plan(room, 1), S.member_plan(room, 2)
        same = same and hp["seed"] == gp["seed"] and hp["digest"] == gp["digest"]
        seeds.append(hp["seed"])
        digests.add(hp["digest"])
    check("twenty matches, twenty different seeds", len(set(seeds)) == 20,
          seeds)
    check("and so twenty different digests", len(digests) == 20, digests)
    check("and every match's seed and digest identical in every plan", same)
    S._reset_match(room)
    check("the reset clears the frozen seed and settings but not the room's",
          room.match_seed == 0 and room.match_luigi == 0
          and room.luigi == 1 and room.voice == 0)

    # A stars match digests its target.
    reset()
    code, room, host, guest = _two_ready()
    _params(code, host["token"], map=1, win_mode="stars", star_target=3)
    start(code, host["token"])
    hp = S.member_plan(room, 1)
    check("a stars match digests its target",
          hp["digest"] == S.session_digest(1, 2, "stars", 3, None, 0,
                                           hp["seed"]), hp)

    # An adventure room has no match to put Luigi or voice in.
    reset()
    st, host = create(nick="adv", session_type="adventure", pre_ok=True)
    code = host["room"]
    for key in ("luigi", "voice"):
        for val in (0, 1):
            st, out = _params(code, host["token"], **{key: val})
            check("%s=%d on an adventure room is 400 bad_field" % (key, val),
                  st == 400 and out["error"] == "bad_field", (st, out))
    check("and the room did not take it",
          S.ROOMS[code].luigi == 0 and S.ROOMS[code].voice == 0)


def world(nick="w", game=None, who=None, now=1000.0, **extra):
    body = {"v": 5, "nick": nick, "game": game or GAME}
    body.update(extra)
    return S.do_world(body, who or "10.9.%d.%d" % (len(nick), ord(nick[-1])),
                      now)


def world_ping(code, token, who="10.9.0.1", now=1000.0):
    return S.do_world_ping({"v": 5, "code": code, "token": token}, who, now)


class game_ids(object):
    """Run a block with the GAME_IDS allow list at a given value (a compose
    edit plus a restart)."""

    def __init__(self, ids):
        self.ids = tuple(ids)

    def __enter__(self):
        self.was = S.GAME_IDS
        S.GAME_IDS = self.ids
        reset()

    def __exit__(self, *exc):
        S.GAME_IDS = self.was
        reset()
        return False


def test_game_allow_list():
    """Review REVLOBBY5 S2, part 1: only the real game builds are served."""
    print("\n-- the build allow list (GAME_IDS)")
    ids, dropped = S.parse_game_ids(" 1.0.0-0123abcd, 1.0.1 ,,1.0.0-0123abcd,"
                                    "bad id,%s" % ("x" * 25))
    check("GAME_IDS parses, trims, dedupes, and drops non-build-ids",
          ids == ("1.0.0-0123abcd", "1.0.1")
          and dropped == ["bad id", "x" * 25], (ids, dropped))
    check("an unset GAME_IDS is an empty list",
          S.parse_game_ids(None) == ((), []))
    with game_ids(()):
        st, out = create(game="anything-1")
        check("empty list: a code room takes any well-formed build",
              st == 200, (st, out))
        check("empty list: the world pool is OFF (the safe default)",
              not S.world_open() and world(game="anything-1")[1].get("error")
              == "world_off")
    with game_ids((GAME,)):
        check("with a list and relay v2 the pool is open", S.world_open())
        for verb in ("create", "world"):
            st, out = (create(game="made-up-7") if verb == "create"
                       else world(game="made-up-7"))
            check("%s with a build not on the list is 409 unknown_game" % verb,
                  st == 409 and out["error"] == "unknown_game", (st, out))
        check("and none of that made a room or a world",
              not S.ROOMS and not S.WORLDS, (list(S.ROOMS), list(S.WORLDS)))
        st, host = create()
        was = S.GAME_IDS
        S.GAME_IDS = ("other",)
        st, out = join(host["room"], "g")
        S.GAME_IDS = was
        check("join with a build that left the list is 409 unknown_game",
              st == 409 and out["error"] == "unknown_game", (st, out))
        st, out = world()
        check("a listed build gets a world", st == 200, (st, out))
        st, out = world(game="bad id")
        check("grammar still comes first: 400 bad_game",
              st == 400 and out["error"] == "bad_game", (st, out))
        was = S.RELAY_V
        S.RELAY_V = 1
        check("RELAY_V 1 still turns the pool off", not S.world_open())
        S.RELAY_V = was


def test_world_address_cap():
    """Review REVLOBBY5 S2, part 2: one address holds at most
    WORLD_ADDR_TOKENS live world tokens, however it asks."""
    print("\n-- one address, at most WORLD_ADDR_TOKENS world members")
    with game_ids((GAME,)):
        cap = S.WORLD_ADDR_TOKENS
        check("an empty world is dropped after 30 s, not ten minutes",
              S.WORLD_EMPTY_S == 30, S.WORLD_EMPTY_S)
        check("the shipped cap is 4 (a household behind one connection)",
              cap == 4, cap)
        home = "198.51.100.40"
        toks = []
        for k in range(4):
            st, out = world("h%d" % k, who=home, now=1000.0 + k)
            toks.append((st, out))
        check("a household of 4 on one address: all 4 get world 1",
              all(st == 200 and o["world_no"] == 1 for st, o in toks), toks)
        code = toks[0][1]["code"]
        pings = [world_ping(code, o["token"], who=home, now=1010.0)[0]
                 for _, o in toks]
        check("and all 4 keep pinging fine", pings == [200] * 4, pings)
        check("world 1 counts 4", S.WORLDS[code].count(1010.0) == 4)
        # A fifth ask from the same address (one of them relaunched): the
        # least recently heard token is retired, never a refusal.
        world_ping(code, toks[1][1]["token"], who=home, now=1011.0)
        world_ping(code, toks[2][1]["token"], who=home, now=1011.0)
        world_ping(code, toks[3][1]["token"], who=home, now=1011.0)
        st, fifth = world("h4", who=home, now=1012.0)
        check("a fifth ask from the address is served (a relaunch works)",
              st == 200 and fifth["world_no"] == 1, (st, fifth))
        check("and world 1 still counts 4 from that address",
              S.WORLDS[code].count(1012.0) == 4, S.WORLDS[code].count(1012.0))
        st, out = world_ping(code, toks[0][1]["token"], who=home, now=1013.0)
        check("the retired token was the least recently heard one: 403",
              st == 403 and out["error"] == "not_a_member", (st, out))

        # THE PADDING ATTACK (review H9b): one address, the real build, many
        # calls, pinging everything it was given.
        reset()
        evil = "203.0.113.66"
        got = []
        t = 2000.0
        for k in range(200):
            t += 1.0
            st, out = S.do_world({"v": 5, "nick": "x", "game": GAME}, evil, t)
            if st == 200:
                got.append(out)
            for o in got:
                world_ping(o["code"], o["token"], who=evil, now=t)
        live = sum(w.count(t) for w in S.WORLDS.values())
        check("200 world calls from ONE address, every token pinged: it never "
              "holds more than %d live members" % cap, live <= cap, live)
        st, real = world("real", who="198.51.100.9", now=t)
        check("a real player from another address still gets world 1 with "
              "room to spare", st == 200 and real["world_no"] == 1
              and real["members"] <= cap + 1, (st, real))
        check("and the pool never grew past one world", len(S.WORLDS) == 1,
              len(S.WORLDS))
        # skip cannot spread one address over many worlds either
        reset()
        codes = set()
        last = None
        for k in range(40):
            body = {"v": 5, "nick": "x", "game": GAME}
            if last:
                body["skip"] = last
            st, out = S.do_world(body, evil, 3000.0 + k)
            if st == 200:
                last = out["code"]
                codes.add(last)
        live = sum(w.count(3040.0) for w in S.WORLDS.values())
        check("40 calls with skip from one address: at most %d live members, "
              "at most 2 worlds" % cap, live <= cap and len(S.WORLDS) <= 2,
              (live, len(S.WORLDS)))


def test_world_pool():
    with game_ids((GAME, "0.9.9") + tuple("b%d" % k for k in range(10))):
        _world_pool()


def _world_pool():
    """v5 world rooms (design section 5): an always-on pool per game build,
    sixteen members a world, fill then overflow, freshness by world_ping."""
    print("\n-- v5 world rooms: fill 16, overflow, expiry")
    reset()
    st, w1 = world("a")
    check("the first world answer is 200", st == 200, (st, w1))
    check("it is world 1 with a hub plan: code, relay, relay_v 2, any seat",
          w1.get("world_no") == 1 and len(w1.get("code", "")) == 8
          and w1.get("relay") == S.RELAY_ADDR and w1.get("relay_v") == 2
          and w1.get("want_seat") == 255 and len(w1.get("token", "")) == 32,
          w1)
    check("the hub code is from the room alphabet",
          all(ch in S.ROOM_ALPHABET for ch in w1["code"]))
    toks = [w1["token"]]
    for k in range(1, S.WORLD_SEATS):
        st, out = world("p%d" % k, who="10.9.1.%d" % k)
        same = st == 200 and out["code"] == w1["code"] and out["world_no"] == 1
        if not same:
            break
        toks.append(out["token"])
    check("members 2..16 all land in world 1", len(toks) == S.WORLD_SEATS,
          len(toks))
    st, w2 = world("q17", who="10.9.2.1")
    check("the 17th goes to a NEW world 2 with its own code",
          st == 200 and w2["world_no"] == 2 and w2["code"] != w1["code"],
          (st, w2))
    check("and world 1 holds exactly WORLD_SEATS (32 since WORLD32)",
          S.WORLDS[w1["code"]].count(1000.0) == S.WORLD_SEATS)

    # A different game build has its own pool, starting at 1.
    st, o = world("old", game="0.9.9", who="10.9.3.1")
    check("another build gets its own world 1",
          st == 200 and o["world_no"] == 1 and o["code"] not in
          (w1["code"], w2["code"]), (st, o))

    # FRESHNESS. Members of world 1 that keep pinging stay; the rest expire.
    t = 1000.0
    for step in range(4):
        t += 10.0
        for tok in toks[:10]:
            st, _ = world_ping(w1["code"], tok, now=t)
        world_ping(w2["code"], w2["token"], now=t)
        S.sweep(t)
    check("pinging members stay counted, silent ones expire after 30 s",
          S.WORLDS[w1["code"]].count(t) == 10, S.WORLDS[w1["code"]].count(t))
    st, out = world_ping(w1["code"], toks[12], now=t)
    check("an expired member's ping is 403 not_a_member (ask world again)",
          st == 403 and out["error"] == "not_a_member", (st, out))
    st, out = world_ping(w1["code"], toks[0], now=t)
    check("a live member's ping is 200 with the world number and head count",
          st == 200 and out["world_no"] == 1 and out["members"] == 10,
          (st, out))
    st, back = world("back", who="10.9.4.1", now=t)
    check("with seats free again, a newcomer fills world 1 before world 2",
          st == 200 and back["world_no"] == 1, (st, back))

    # SKIP: the relay said world 1 is full (a race the lobby cannot see).
    st, sk = world("skipper", who="10.9.4.2", now=t, skip=w1["code"])
    check("skip=world 1 answers the next world with room (world 2)",
          st == 200 and sk["world_no"] == 2, (st, sk))
    st, out = world("bad", who="10.9.4.3", now=t, skip="short")
    check("a malformed skip is 400 bad_code",
          st == 400 and out["error"] == "bad_code", (st, out))

    # EMPTY WORLDS ARE DROPPED after WORLD_EMPTY_S, and the number comes back.
    t2 = t + S.WORLD_MEMBER_S + 1
    S.sweep(t2)
    check("once every member is silent, the world is empty but kept",
          w2["code"] in S.WORLDS and not S.WORLDS[w2["code"]].members)
    S.sweep(t2 + S.WORLD_EMPTY_S)
    check("and dropped WORLD_EMPTY_S later", w2["code"] not in S.WORLDS
          and w1["code"] not in S.WORLDS, sorted(S.WORLDS))
    st, again = world("again", who="10.9.5.1", now=t2 + S.WORLD_EMPTY_S + 1)
    check("the next arrival opens world 1 again, with a fresh code",
          st == 200 and again["world_no"] == 1
          and again["code"] not in (w1["code"], w2["code"]), (st, again))
    st, out = world_ping(w1["code"], toks[0], now=t2 + S.WORLD_EMPTY_S + 1)
    check("a ping to a dropped world is 404 no_such_world",
          st == 404 and out["error"] == "no_such_world", (st, out))

    # GRAMMAR AND BOUNDS.
    reset()
    for body, code in (
            ({"v": 5, "nick": "x"}, "bad_field"),
            ({"v": 5, "nick": "x", "game": "a b"}, "bad_game"),
            ({"v": 5, "nick": "x,y", "game": GAME}, "bad_nick"),
            ({"v": 5, "nick": "x", "game": GAME, "color": "ff0000"},
             "bad_color"),
            ({"v": 5, "nick": "x", "game": GAME, "room": "K7QMR3"},
             "bad_field"),
            ({"v": 4, "nick": "x", "game": GAME}, "bad_version")):
        st, out = S.do_world(body, "10.9.6.1", 1000.0)
        check("world %s -> 400 %s" % (sorted(body), code),
              st == 400 and out["error"] == code, (st, out))
    check("and none of that opened a world", not S.WORLDS)
    st, w = world("colours", color="FF0000", shoes="00ff00")
    check("a world call with a colour pair is fine", st == 200, (st, w))
    check("and the world keeps no nick and no colour: per token a time and "
          "the issuing address (for the per-address cap), nothing else",
          S.WORLDS[w["code"]].members == {w["token"]: [1000.0, "10.9.7.115"]}
          and set(vars(S.WORLDS[w["code"]])) == {"game", "no", "code",
                                                  "members", "empty_since"},
          vars(S.WORLDS[w["code"]]))
    for body, code in (({"v": 5, "code": "abc", "token": "0" * 32}, "bad_code"),
                       ({"v": 5, "code": w["code"], "token": "x"}, "bad_token"),
                       ({"v": 5, "code": w["code"]}, "bad_field")):
        st, out = S.do_world_ping(body, "10.9.6.1", 1000.0)
        check("world_ping %s -> 400 %s" % (sorted(body), code),
              st == 400 and out["error"] == code, (st, out))
    st, out = world_ping(w["code"], "f" * 32)
    check("a token nobody holds is 403 not_a_member",
          st == 403 and out["error"] == "not_a_member", (st, out))

    reset()
    was = S.MAX_WORLDS
    S.MAX_WORLDS = 3
    try:
        codes = []
        for k in range(3):
            st, out = world("m%d" % k, game="b%d" % k)
            codes.append(st)
        st, out = world("m9", game="b9")
        check("MAX_WORLDS bounds the whole pool: the fourth build is 503 full",
              codes == [200] * 3 and st == 503 and out["error"] == "full",
              (codes, st, out))
    finally:
        S.MAX_WORLDS = was
    reset()
    was = S.RELAY_V
    S.RELAY_V = 1
    try:
        st, out = world("v1")
        check("on a v1 relay the world pool is off: 503 world_off",
              st == 503 and out["error"] == "world_off", (st, out))
    finally:
        S.RELAY_V = was
    reset()


def test_king_of_the_star():
    print("\n-- king of the star: win_mode and king_target, required-iff-king")
    reset()
    st, host = create()
    code = host["room"]
    tok = host["token"]

    # king with a target is accepted, and the room takes the values.
    st, out = S.do_params({"v": 5, "room": code, "token": tok, "map": 2,
                           "win_mode": "king", "king_target": 30},
                          "10.0.0.1", 1000.0)
    check("king with a target is accepted", st == 200, out)
    check("the room took king mode and the target",
          S.ROOMS[code].win_mode == "king"
          and S.ROOMS[code].king_target == 30
          and S.ROOMS[code].star_target is None, S.ROOMS[code].__dict__)

    # the default is a real number the launcher can pre-fill.
    check("the launcher default is 30", S.KING_TARGET_DEFAULT == 30)

    # every value up to the ceiling works; one past it, and zero, are refused.
    check("one is the floor and is accepted",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king", "king_target": 1},
                      "10.0.0.1", 1000.0)[0] == 200)
    check("the ceiling is accepted",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king", "king_target": S.KING_TARGET_MAX},
                      "10.0.0.1", 1000.0)[0] == 200)
    check("one past the ceiling is refused with its own code",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king",
                       "king_target": S.KING_TARGET_MAX + 1},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_king_target")
    check("zero is refused too",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king", "king_target": 0},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_king_target")

    # required iff king: king with no target is refused.
    check("king with no target is refused",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king"},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_king_target")
    # forbidden otherwise: king_target on a non-king mode is refused.
    check("king_target in time mode is refused",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "time", "king_target": 30},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_king_target")
    check("king_target in stars mode is refused",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "stars", "star_target": 3,
                       "king_target": 30},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_king_target")
    # and star_target on king is refused for the mirror reason.
    check("star_target in king mode is refused",
          S.do_params({"v": 5, "room": code, "token": tok, "map": 0,
                       "win_mode": "king", "king_target": 30,
                       "star_target": 3},
                      "10.0.0.1", 1000.0)[1]["error"] == "bad_star_target")

    # the room view advertises the field.
    v = S.ROOMS[code].view(1)
    check("the room view carries king_target", "king_target" in v, v)

    # it freezes into the match and rides the plan, and only in king mode.
    reset()
    code, room, host, guest = _two_ready()
    S.do_params({"v": 5, "room": code, "token": host["token"],
                 "map": 1, "win_mode": "king", "king_target": 25},
                "10.0.0.1", 1000.0)
    st, p = start(code, host["token"])
    check("king_target froze into the match",
          room.match_win_mode == "king" and room.match_king_target == 25,
          (room.match_win_mode, room.match_king_target))
    hp = S.member_plan(room, 1)
    gp = S.member_plan(room, 2)
    check("the king target rides every playing plan",
          hp.get("king_target") == 25 and gp.get("king_target") == 25, (hp, gp))
    check("king mode carries no star_target",
          "star_target" not in hp, hp)

    # a non-king plan omits king_target entirely.
    reset()
    code, room, host, guest = _two_ready()
    start(code, host["token"])
    check("time mode: the plan has no king_target key",
          "king_target" not in S.member_plan(room, 1), S.member_plan(room, 1))


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

    st, f = S.do_failed({"v": 5, "room": code, "token": guest["token"],
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
    st, f = S.do_failed({"v": 5, "room": code, "token": host["token"],
                         "match": p["match"], "reason": "nope"}, "10.0.0.1", 1001.0)
    check("an unknown reason is 400 bad_reason",
          st == 400 and f["error"] == "bad_reason", (st, f))
    st, f = S.do_failed({"v": 5, "room": code, "token": host["token"],
                         "match": p["match"], "reason": "member_not_ready"},
                        "10.0.0.1", 1001.0)
    check("member_not_ready is server-minted only, refused from a client",
          st == 400 and f["error"] == "bad_reason", (st, f))
    st, f = S.do_failed({"v": 5, "room": code, "token": host["token"],
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
    st, _ = S.do_leave({"v": 5, "room": code, "token": guest["token"]},
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

    # (b) a playing seat leaving during arming with a spectator in the room.
    # Before run mp1 L-J1 the spectator was PROMOTED here, which kept the count
    # and changed the seats (the shape a bare len(ps) == agreed_players check
    # would let through). Now nobody is promoted into a frozen match at all.
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
    S.do_leave({"v": 5, "room": code, "token": g2["token"]}, "10.0.0.2", 1001.0)
    check("seat 3 is NOT promoted into the arming match (L-J1)",
          room.members[3].playing is False, room.members[3].playing)
    ready(code, host["token"], match, now=1002.0)
    st, r = ready(code, g3["token"], match, who="10.0.0.3", now=1002.0)
    check("the spectator's ready is 403 not_playing",
          st == 403 and r["error"] == "not_playing", (st, r))
    check("and the room does not go with the frozen seat missing",
          room.state == "arming", room.state)
    check("no plan was ever issued crediting the departed player",
          frozen_names.startswith("h,g2,"), frozen_names)
    S.sweep(1000.0 + S.ARM_DEADLINE_S + 0.1)
    check("the arming deadline returns it to lobby, where seat 3 is promoted",
          room.state == "lobby" and room.members[3].playing is True,
          (room.state, room.members[3].playing))


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

    st, f = S.do_failed({"v": 5, "room": code, "token": spec["token"],
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
    st, rr = S.do_result({"v": 5, "room": code, "token": spec["token"],
                          "match": match, "win": "star-target",
                          "scores": [9, 0, 0, 0]}, "10.0.0.3", 1010.0)
    check("a spectator's result is 403 not_playing",
          st == 403 and rr["error"] == "not_playing", (st, rr))
    check("the match is STILL running and no scoreline was written",
          room.state == "in_match" and room.match == match, room.state)
    check("no result event was pushed by the spectator",
          not any(e["kind"] == "result" for e in room.events))

    # A playing member still can, so the gate is not simply refusing everybody.
    st, rr = S.do_result({"v": 5, "room": code, "token": g["token"],
                          "match": match, "win": "time-up",
                          "scores": [1, 2, 0, 0]}, "10.0.0.2", 1011.0)
    # run mp1 L-J1: a non-host's result is a vote. Accepted (200), and on its
    # own it does not end a match the host is still in (G5); the host's does.
    check("a PLAYING guest's result is accepted as a vote",
          st == 200 and room.state == "in_match", (st, room.state))
    st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
                          "match": match, "win": "time-up",
                          "scores": [1, 2, 0, 0]}, "10.0.0.1", 1011.5)
    check("a PLAYING member's result still works: the host's ends it",
          st == 200 and room.state == "lobby")


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
    st, r = S.do_preflight({"v": 5, "room": code, "token": g["token"],
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
        st, _ = S.do_preflight({"v": 5, "room": code, "token": host["token"],
                                "pre_ok": True}, "10.0.0.1", 1001.0)
        check("repeating the same answer is accepted", st == 200)
    check("...and pushes NOTHING, so a polling launcher costs the room nothing",
          room.seq == seq, (seq, room.seq))

    # It can go the other way too (a ROM that went missing).
    st, _ = S.do_preflight({"v": 5, "room": code, "token": host["token"],
                            "pre_ok": False}, "10.0.0.1", 1002.0)
    check("a member can also go back to not-ready",
          room.members[1].pre_ok is False and room.seq > seq)

    # Authority and grammar.
    st, r = S.do_preflight({"v": 5, "room": code, "token": "0" * 32,
                            "pre_ok": True}, "10.0.0.9", 1003.0)
    check("a token that holds no seat is refused",
          st == 403 and r["error"] == "not_a_member", (st, r))
    st, r = S.do_preflight({"v": 5, "room": code, "token": host["token"],
                            "pre_ok": "yes"}, "10.0.0.1", 1003.0)
    check("a non-boolean pre_ok is 400 bad_field",
          st == 400 and r["error"] == "bad_field", (st, r))
    st, r = S.do_preflight({"v": 5, "room": code, "token": host["token"]},
                           "10.0.0.1", 1003.0)
    check("a missing pre_ok is 400 bad_field",
          st == 400 and r["error"] == "bad_field", (st, r))
    st, r = S.do_preflight({"v": 5, "room": code, "token": host["token"],
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
    st, _ = S.do_preflight({"v": 5, "room": code, "token": guest["token"],
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
    S.do_params({"v": 5, "room": code, "token": host["token"],
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
    st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
                          "match": match1, "win": "nope",
                          "scores": [3, 1, 0, 0]}, "10.0.0.1", 1010.0)
    check("a bad win is 400 bad_win", st == 400 and rr["error"] == "bad_win")
    st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
                          "match": match1, "win": "star-target",
                          "scores": [3, 1, 0]}, "10.0.0.1", 1010.0)
    check("scores that are not four ints is 400 bad_scores",
          st == 400 and rr["error"] == "bad_scores")

    # The real result.
    st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
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
    st, rr = S.do_result({"v": 5, "room": code, "token": guest["token"],
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
    st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
                          "match": match2, "win": "draw",
                          "scores": [0, 0, 0, 0]}, "10.0.0.1", 1020.0)
    check("a draw result is accepted", st == 200 and room.state == "lobby")

    # run mp1: every end word the game's marker prints is a result, echoed in
    # the event as posted (King of the Star, and Luigi Infection's two team
    # ends, which the launcher names as a team).
    for k, win in enumerate(("king-target", "luigi-all-infected",
                             "survivor-timeout")):
        t = 1030.0 + 20 * k
        st, p3 = start(code, host["token"], now=t)
        m = p3["match"]
        ready(code, host["token"], m, now=t)
        ready(code, guest["token"], m, who="10.0.0.2", now=t)
        S.sweep(t + S.GO_GRACE_S + 0.1)
        st, rr = S.do_result({"v": 5, "room": code, "token": host["token"],
                              "match": m, "win": win,
                              "scores": [2, 0, 0, 0]}, "10.0.0.1", t + 5)
        ev = room.events[-1]
        check("a %s result is accepted and echoed" % win,
              st == 200 and room.state == "lobby" and ev["kind"] == "result"
              and ev["win"] == win, (st, rr, ev.get("win")))



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

    # -- create and join with colours -------------------------------------
    reset()
    st, host = S.do_create({"v": 5, "game": GAME, "nick": "tango", "pre_ok": True,
                            "color": "8A2BE2", "shoes": "FFD700"},
                           "10.0.0.1", 1000.0)
    code = host["room"]
    room = S.ROOMS[code]
    check("a create takes both colours", st == 200, (st, host))
    check("and stores them lower case",
          room.members[1].color == "8a2be2" and room.members[1].shoes == "ffd700",
          (room.members[1].color, room.members[1].shoes))
    check("the roster row carries them",
          host["view"]["members"][0]["color"] == "8a2be2"
          and host["view"]["members"][0]["shoes"] == "ffd700",
          host["view"]["members"][0])

    st, out = S.do_join({"v": 5, "game": GAME, "room": code, "nick": "opie", "pre_ok": True,
                         "color": "ff0000"}, "10.0.0.2", 1000.0)
    check("a join with one colour and not the other is bad_color",
          st == 400 and out["error"] == "bad_color", (st, out))
    st, guest = S.do_join({"v": 5, "game": GAME, "room": code, "nick": "opie", "pre_ok": True,
                           "color": "ff0000", "shoes": "00ff00"},
                          "10.0.0.2", 1000.0)
    check("a join with both is seated", st == 200, (st, guest))
    check("and the guest's row carries them",
          room.members[2].color == "ff0000" and room.members[2].shoes == "00ff00")

    # -- a join that picked nothing, into a coloured room --------------------
    st, old = S.do_join({"v": 5, "game": GAME, "room": code, "nick": "old", "pre_ok": True},
                        "10.0.0.3", 1000.0)
    check("a player who picked nothing joins a room full of colours",
          st == 200, (st, old))
    check("and takes the empty pair, which means the built-in Yoshi",
          room.members[3].color == "" and room.members[3].shoes == "")
    st, p = S.do_leave({"v": 5, "room": code, "token": old["token"]},
                       "10.0.0.3", 1000.0)
    check("and it can leave again", st == 200, (st, p))

    # -- the verb: own seat only, no-op on unchanged, event on change ------
    before = room.seq
    st, r = S.do_color({"v": 5, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1001.0)
    check("re-sending the same pair is a no-op", st == 200 and room.seq == before,
          (st, room.seq, before))

    st, r = S.do_color({"v": 5, "room": code, "token": guest["token"],
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

    st, r = S.do_color({"v": 5, "room": code, "token": guest["token"],
                        "color": "", "shoes": ""}, "10.0.0.2", 1003.0)
    check("two empty strings put a player back on the built-in colour",
          st == 200 and room.members[2].color == "", (st, room.members[2].color))
    st, r = S.do_color({"v": 5, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": ""}, "10.0.0.2", 1004.0)
    check("one empty and one not is bad_color",
          st == 400 and r["error"] == "bad_color", (st, r))
    st, r = S.do_color({"v": 4, "room": code, "token": guest["token"],
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1005.0)
    check("a pre-v5 caller is refused bad_version like every other verb",
          st == 400 and r["error"] == "bad_version", (st, r))
    st, r = S.do_color({"v": 5, "room": code, "token": "f" * 32,
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.9", 1006.0)
    check("a token that holds no seat is refused",
          st == 403 and r["error"] == "not_a_member", (st, r))
    st, r = S.do_color({"v": 5, "room": code, "token": guest["token"], "seat": 1,
                        "color": "ff0000", "shoes": "00ff00"},
                       "10.0.0.2", 1007.0)
    check("A SEAT ARGUMENT IS REFUSED OUTRIGHT: the verb has no way to reach "
          "anybody else's row", st == 400 and r["error"] == "bad_field", (st, r))

    # -- the string the game reads ----------------------------------------
    S.do_color({"v": 5, "room": code, "token": guest["token"],
                "color": "ff0000", "shoes": "00ff00"}, "10.0.0.2", 1008.0)
    st, p = start(code, host["token"], now=1009.0)
    check("the room arms", st == 200, (st, p))
    check("SM64DS_VS_COLORS is four fields in slot order with three commas",
          room.colors == "8a2be2:ffd700,ff0000:00ff00,,", room.colors)
    check("both plans carry the byte-identical string",
          S.member_plan(room, 1)["colors"] == S.member_plan(room, 2)["colors"]
          == room.colors)
    check("and it is frozen, not live: changing a colour now is refused",
          S.do_color({"v": 5, "room": code, "token": guest["token"],
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
    st, host = S.do_create({"v": 5, "game": GAME, "nick": "tango", "pre_ok": True,
                            "color": "18e618", "shoes": "e66318"},
                           "10.0.0.1", 1000.0)
    code = host["room"]
    S.do_join({"v": 5, "game": GAME, "room": code, "nick": "opie", "pre_ok": True},
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
            st, h = S.do_create({"v": 5, "game": GAME, "nick": "t", "pre_ok": True,
                                 "color": a, "shoes": b}, "10.0.0.1", 1000.0)
            if st != 200:
                refused += 1
                continue
            emitted += 1
            c2 = h["room"]
            r2 = S.ROOMS[c2]
            S.do_join({"v": 5, "game": GAME, "room": c2, "nick": "g", "pre_ok": True,
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
    st, h = S.do_create({"v": 5, "game": GAME, "nick": "t", "pre_ok": True,
                         "color": "ffffff", "shoes": "000000"},
                        "10.0.0.1", 1000.0)
    code = h["room"]
    room = S.ROOMS[code]
    S.do_join({"v": 5, "game": GAME, "room": code, "nick": "g", "pre_ok": True,
               "color": "000000", "shoes": "ffffff"}, "10.0.0.2", 1000.0)
    start(code, h["token"])
    check("the longest legal string is 4 x 13 + 3 = 55 bytes, which is the "
          "game's cap",
          len("ffffff:000000,000000:ffffff,,") == 29 and 4 * 13 + 3 == 55)
    check("and this room's is inside it", len(room.colors) <= 55, room.colors)



def test_a_dropout_after_go_does_not_end_the_match_for_the_rest():
    """0.3.2 field lesson. Once the match is sent off, a non-host member's
    failed drops that member and the rest play on; the host's failed still
    ends it; and a match that falls below two players ends."""
    print("-- after go, one dropout does not end the match for the rest")
    reset()
    with game_max(16, seats=16):
        st, h = S.do_create({"v": 5, "game": GAME, "nick": "h", "pre_ok": True},
                            "10.0.0.1", 1000.0)
        code, htok = h["room"], h["token"]
        toks = []
        for k in (2, 3):
            st, out = S.do_join({"v": 5, "game": GAME, "room": code, "nick": "g%d" % k,
                                 "pre_ok": True}, "10.0.0.%d" % k, 1000.0)
            toks.append(out["token"])
        st, out = S.do_params({"v": 5, "room": code, "token": htok, "map": 0,
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
            st, out = S.do_ready({"v": 5, "room": code, "token": tok,
                                  "match": match}, ip, 1001.0)
        check("the room is at go", room.state == "go", room.state)
        st, f = S.do_failed({"v": 5, "room": code, "token": toks[0],
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
        st, f = S.do_failed({"v": 5, "room": code, "token": htok,
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


def test_wide_rooms_need_no_version_gate():
    """Before v5 a v2 caller was held to four players (its build validated
    four name fields). The floor retired those callers, so the dial's bound
    is the deployment's alone and a fresh room defaults to all of it."""
    print("\n-- above the v5 floor the dial's bound is the deployment's")
    reset()
    with game_max(16, seats=16):
        st, host = create(pre_ok=True)
        code, token = host["room"], host["token"]
        check("a fresh room on a sixteen-player deployment defaults to 16",
              S.ROOMS[code].match_players == 16, S.ROOMS[code].match_players)
        for n in (2, 4, 5, 8, 16):
            st, out = _dial(code, token, n)
            check("the host may dial %d" % n, st == 200, (n, st, out))
        check("and the narrow-client gates are gone, not merely unreachable",
              not hasattr(S, "dial_max_for") and not hasattr(S, "WIDE_MIN_V"))
    reset()


def test_name_and_colour_shapes_move_together():
    """run vs16, and this is the coordinator's ruling as a test rather than a
    paragraph: NAMES and COLORS carry the same field count, always, and the
    count is decided by the dial and by nothing else."""
    print("\n-- names and colours are one grammar, four fields or sixteen")
    reset()
    with game_max(16, seats=16):
        # A room nobody dials wide: four fields, exactly as before.
        st, h = S.do_create({"v": 5, "game": GAME, "nick": "tango", "pre_ok": True,
                             "color": "8A2BE2", "shoes": "FFD700"},
                            "10.0.0.1", 1000.0)
        code = h["room"]
        S.do_join({"v": 5, "game": GAME, "room": code, "nick": "opie", "pre_ok": True,
                   "color": "FF0000", "shoes": "00FF00"}, "10.0.0.2", 1000.0)
        room = S.ROOMS[code]

        # THROUGH THE REAL START PATH, not by calling build_names by hand.
        # `match_dial` is frozen at arm, one statement before the strings are
        # built, so a test that built them directly would be testing a room
        # state that never exists in production -- and would have read a dial
        # of 0 and passed for the wrong reason.
        _params(code, h["token"], v=5, match_players=4)
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
        st, out = _params(code, h["token"], v=5, match_players=8)
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
        st, out = _params(code, h["token"], v=5, match_players=4)
        check("the room dials back to four", st == 200, (st, out))
        start(code, h["token"])
        check("and the string is four fields again, byte for byte the shape "
              "the shipped launcher validates",
              room.names.count(",") == 3 and room.colors.count(",") == 3,
              (room.names, room.colors))

        # The game's own caps, checked here so the service cannot emit a
        # string the reader in hal/star_flow.cpp would throw away.
        S.ROOMS[code].state = "lobby"
        _params(code, h["token"], v=5, match_players=16)
        start(code, h["token"])
        check("the widest legal names string is inside the game's 271-byte cap",
              len(room.names) <= 271, len(room.names))
        check("and the widest legal colours string is inside its 223-byte cap",
              len(room.colors) <= 223, len(room.colors))
    reset()



# ------------------------------------------ run mp1 L-J1: join at the next match
#
# Tango's rule (09-29): a VS joiner waits in the room while a match runs and is
# in from the next match; nobody is moved into or out of a running match's
# roster; the host leaving ends a running match for everybody and the room
# carries on with a new host; one player's result does not end the match for
# the room.


def _running(n=3, now=1000.0, state="in_match"):
    """A room of n playing seats (host = seat 1), taken to `state`."""
    st, h = S.do_create({"v": 5, "game": GAME, "nick": "h", "pre_ok": True},
                        "10.0.0.1", now)
    code = h["room"]
    toks = [h["token"]]
    for k in range(2, n + 1):
        st, out = S.do_join({"v": 5, "game": GAME, "room": code,
                             "nick": "g%d" % k, "pre_ok": True},
                            "10.0.0.%d" % k, now)
        toks.append(out["token"])
    room = S.ROOMS[code]
    # (No dial call: a room made inside game_max(k) is dialled to k already.)
    st, p = start(code, toks[0], now=now)
    match = p["match"]
    if state in ("go", "in_match"):
        for i, tok in enumerate(toks):
            ready(code, tok, match, who="10.0.0.%d" % (i + 1), now=now)
    if state == "in_match":
        S.sweep(now + S.GO_GRACE_S + 0.1)
    return code, room, toks, match


def _result(code, tok, match, scores=(1, 0, 0, 0), win="time-up",
            who="10.0.0.1", now=1100.0):
    return S.do_result({"v": 5, "room": code, "token": tok, "match": match,
                        "win": win, "scores": list(scores)}, who, now)


def test_a_dropped_player_plays_the_next_match():
    print("\n-- L-J1: a player dropped from a match is back in for the next one")
    reset()
    with game_max(4, seats=4):
        code, room, toks, match = _running(3)
        check("three players are in a running match",
              room.state == "in_match" and room.playing_count() == 3,
              (room.state, room.playing_count()))
        st, f = S.do_failed({"v": 5, "room": code, "token": toks[1],
                             "match": match, "reason": "user_cancelled"},
                            "10.0.0.2", 1010.0)
        check("a guest's failed drops that seat and the match runs on",
              st == 200 and room.members[2].playing is False
              and room.state == "in_match", (st, room.state))
        _result(code, toks[0], match, scores=(2, 0, 1, 0))
        check("the result returns the room to lobby", room.state == "lobby",
              room.state)
        check("the dropped seat is PLAYING again for the next match (G7)",
              room.members[2].playing is True, room.members[2].playing)
        check("and the other two were not moved",
              room.members[1].playing and room.members[3].playing)
        st, p = start(code, toks[0], now=1200.0)
        check("the rematch freezes all three again",
              st == 200 and sorted(room.slot_of) == [1, 2, 3], room.slot_of)

    # A failed that ends the match (the last guest out of a 2P match) goes back
    # through the same reset, so that guest is back in too.
    reset()
    code, room, toks, match = _running(2)
    S.do_failed({"v": 5, "room": code, "token": toks[1], "match": match,
                 "reason": "no_pairing"}, "10.0.0.2", 1010.0)
    check("a 2P match that drops below two ends",
          room.state == "lobby" and room.events[-1]["kind"] == "failed",
          room.state)
    check("and the guest who failed is playing again in the lobby",
          room.members[2].playing is True)

    # The dial still rules: a room dialled below its head count keeps its
    # extra members watching through a match end.
    reset()
    st, h = create(nick="h", pre_ok=True)
    code = h["room"]
    join(code, "g2", who="10.0.0.2", pre_ok=True)
    st, w = join(code, "w3", who="10.0.0.3", pre_ok=True)
    room = S.ROOMS[code]
    st, p = start(code, h["token"])
    ready(code, h["token"], p["match"])
    ready(code, room.members[2].token, p["match"], who="10.0.0.2")
    _result(code, h["token"], p["match"])
    check("a watcher past the dial (2) is still watching after the match",
          room.members[3].playing is False and room.playing_count() == 2,
          room.playing_count())


def test_no_promotion_into_a_running_match():
    """plan.txt C4: a playing seat leaving during go/in_match used to run the
    dial and promote a spectator, whose go plan then said child, slot 0."""
    print("\n-- L-J1: a leave during a match promotes nobody")
    for state in ("go", "in_match"):
        reset()
        st, h = create(nick="h", pre_ok=True)
        code = h["room"]
        st, g2 = join(code, "g2", who="10.0.0.2", pre_ok=True)
        st, w3 = join(code, "w3", who="10.0.0.3", pre_ok=True)
        room = S.ROOMS[code]
        st, p = start(code, h["token"])
        ready(code, h["token"], p["match"])
        ready(code, g2["token"], p["match"], who="10.0.0.2")
        if state == "in_match":
            S.sweep(1000.0 + S.GO_GRACE_S + 0.1)
        check("[%s] the room is running with seat 3 watching" % state,
              room.state == state and room.members[3].playing is False,
              room.state)
        S.do_leave({"v": 5, "room": code, "token": g2["token"]},
                   "10.0.0.2", 1010.0)
        check("[%s] seat 2 leaving promotes nobody" % state,
              room.members[3].playing is False and room.playing_count() == 1,
              room.playing_count())
        st, out = poll(code, w3["token"], 1, who="10.0.0.3", now=1010.0)
        plans = [e["plan"] for e in out["events"] if e["kind"] == "go"]
        if "plan" in out:
            plans.append(out["plan"])
        check("[%s] the spectator's plans are all {playing: false}" % state,
              plans and all(pl == {"playing": False} for pl in plans), plans)
        check("[%s] and nothing it is handed carries the match's relay code"
              % state, room.comms_code not in json.dumps(out), "leaked")
        _result(code, h["token"], p["match"], now=1100.0)
        check("[%s] at the match end the vacancy is filled: seat 3 plays" % state,
              room.state == "lobby" and room.members[3].playing is True,
              (room.state, room.members[3].playing))


def test_the_plan_guard():
    """A seat missing from slot_of never gets a slot, least of all slot 0."""
    print("\n-- L-J1: the plan guard (no default slot 0)")
    reset()
    st, h = create(nick="h", pre_ok=True)
    code = h["room"]
    st, g2 = join(code, "g2", who="10.0.0.2", pre_ok=True)
    st, w3 = join(code, "w3", who="10.0.0.3", pre_ok=True)
    room = S.ROOMS[code]
    st, p = start(code, h["token"])
    match = p["match"]
    # Forge the C4 state directly (a seat playing that the match never froze),
    # so the guard is proved on its own and not only through drop_member.
    room.members[3].playing = True
    check("a playing seat outside slot_of is handed no plan",
          S.member_plan(room, 3) == {"playing": False}, S.member_plan(room, 3))
    st, r = ready(code, w3["token"], match, who="10.0.0.3")
    check("and its ready is 403 not_playing",
          st == 403 and r["error"] == "not_playing", (st, r))
    # (try_go compares the live playing set to the frozen one, so the forged
    # seat would stall arming: forge it again once the pair is at go.)
    room.members[3].playing = False
    ready(code, h["token"], match)
    ready(code, g2["token"], match, who="10.0.0.2")
    check("the frozen pair still goes", room.state == "go", room.state)
    room.members[3].playing = True
    st, out = poll(code, w3["token"], 1, who="10.0.0.3", now=1001.0)
    goplans = [e["plan"] for e in out["events"] if e["kind"] == "go"]
    check("its go event and top-level plan are {playing: false}",
          goplans == [{"playing": False}] and out.get("plan") == {"playing": False},
          (goplans, out.get("plan")))
    check("the frozen seats keep their own slots: host 0, seat 2 slot 1",
          S.member_plan(room, 1)["slot"] == 0
          and S.member_plan(room, 2)["slot"] == 1
          and S.member_plan(room, 2)["want_seat"] == 1)
    st, f = S.do_failed({"v": 5, "room": code, "token": w3["token"],
                         "match": match, "reason": "no_pairing"},
                        "10.0.0.3", 1002.0)
    check("its failed is 403 not_playing and the match runs on",
          st == 403 and f["error"] == "not_playing" and room.state == "go",
          (st, f, room.state))
    st, rr = _result(code, w3["token"], match, who="10.0.0.3")
    check("its result is 403 not_playing and the match runs on",
          st == 403 and rr["error"] == "not_playing" and room.state == "go",
          (st, rr, room.state))
    room.members[3].playing = False
    _result(code, h["token"], match)
    check("the match ends on the host's result", room.state == "lobby")
    # An old `go` event replayed from the ring after the match ended.
    st, out = poll(code, g2["token"], 1, who="10.0.0.2", now=1200.0)
    goplans = [e["plan"] for e in out["events"] if e["kind"] == "go"]
    check("a go event replayed in lobby carries {playing: false}, not slot 0 "
          "with no code", goplans == [{"playing": False}], goplans)


def test_join_while_a_match_runs_is_a_waiting_seat():
    print("\n-- L-J1: joining a room whose match is on = a waiting seat")
    for state in ("arming", "go", "in_match"):
        reset()
        with game_max(4, seats=4):
            code, room, toks, match = _running(3, state=state)
            names, slots = room.names, dict(room.slot_of)
            st, j = join(code, "late", who="10.0.0.9", now=1010.0, pre_ok=True)
            seat = j.get("member")
            check("[%s] the join answers 200 with a seat" % state,
                  st == 200 and seat == 4, (st, j))
            check("[%s] the seat is waiting (playing false), the room still %s"
                  % (state, state),
                  room.members[seat].playing is False and room.state == state,
                  (room.members[seat].playing, room.state))
            check("[%s] the join answer carries no plan" % state,
                  "plan" not in j, sorted(j))
            check("[%s] the view shows the waiting seat to everybody" % state,
                  any(m["seat"] == seat and m["playing"] is False
                      for m in j["view"]["members"]))
            check("[%s] the frozen match did not move: names, slots, count"
                  % state,
                  room.names == names and room.slot_of == slots
                  and room.agreed_players == 3, (room.slot_of, room.names))
            st, out = poll(code, j["token"], 1, who="10.0.0.9", now=1011.0)
            plans = [e["plan"] for e in out["events"] if e["kind"] == "go"]
            if "plan" in out:
                plans.append(out["plan"])
            check("[%s] every plan it is handed is {playing: false}" % state,
                  all(pl == {"playing": False} for pl in plans), plans)
            check("[%s] no answer to it carries the match's relay code" % state,
                  room.comms_code not in json.dumps(out)
                  and room.comms_code not in json.dumps(j), "leaked")
            st, c = S.do_chat({"v": 5, "room": code, "token": j["token"],
                               "text": "hi"}, "10.0.0.9", 1012.0)
            check("[%s] it can chat in the room" % state, st == 200, (st, c))

            # Hostile: a waiting seat cannot ready, fail, end, start, set up
            # or kick, and has no field that picks a slot.
            st, r = ready(code, j["token"], match, who="10.0.0.9", now=1013.0)
            check("[%s] waiting ready -> 403 not_playing" % state,
                  st == 403 and r["error"] == "not_playing", (st, r))
            st, f = S.do_failed({"v": 5, "room": code, "token": j["token"],
                                 "match": match, "reason": "no_pairing"},
                                "10.0.0.9", 1013.0)
            check("[%s] waiting failed -> 403 not_playing" % state,
                  st == 403 and f["error"] == "not_playing", (st, f))
            st, rr = _result(code, j["token"], match, who="10.0.0.9",
                             scores=(0, 0, 0, 9))
            check("[%s] waiting result -> 403 not_playing, match unchanged"
                  % state, st == 403 and rr["error"] == "not_playing"
                  and room.state == state and room.match == match, (st, rr))
            st, s2 = start(code, j["token"], who="10.0.0.9", now=1013.0)
            check("[%s] waiting start -> 403 not_host" % state,
                  st == 403 and s2["error"] == "not_host", (st, s2))
            st, pp = S.do_params({"v": 5, "room": code, "token": j["token"],
                                  "map": 1, "win_mode": "time"},
                                 "10.0.0.9", 1013.0)
            check("[%s] waiting params -> 403 not_host" % state,
                  st == 403 and pp["error"] == "not_host", (st, pp))
            st, kk = S.do_kick({"v": 5, "room": code, "token": j["token"],
                                "seat": 1}, "10.0.0.9", 1013.0)
            check("[%s] waiting kick -> 403 not_host" % state,
                  st == 403 and kk["error"] == "not_host", (st, kk))
            for key, val in (("slot", 0), ("want_seat", 0), ("playing", True),
                             ("role", "parent")):
                st, bad = join(code, "x", who="10.0.0.10", now=1014.0,
                               **{key: val})
                check("[%s] a join naming `%s` is 400 bad_field" % (state, key),
                      st == 400 and bad["error"] == "bad_field", (st, bad))
            check("[%s] still one waiting seat, still frozen" % state,
                  len(room.members) == 4 and room.slot_of == slots)

            # The next match: the waiting seat is in it.
            if state == "arming":
                S.sweep(1000.0 + S.ARM_DEADLINE_S + 0.1)
                check("[arming] the deadline ends the arm", room.state == "lobby")
            else:
                _result(code, toks[0], match, scores=(1, 2, 3, 0))
            check("[%s] back in lobby the waiting seat is PLAYING" % state,
                  room.state == "lobby" and room.members[seat].playing is True,
                  (room.state, room.members[seat].playing))
            S.do_params({"v": 5, "room": code, "token": toks[0], "map": 0,
                         "win_mode": "time", "match_players": 4},
                        "10.0.0.1", 1101.0)
            st, p2 = start(code, toks[0], now=1102.0)
            check("[%s] the next match freezes it into a slot" % state,
                  st == 200 and room.slot_of.get(seat) == 3
                  and room.agreed_players == 4, (st, room.slot_of))
            check("[%s] and its name is in the next names string" % state,
                  room.names.split(",")[3] == "late", room.names)


def test_waiting_join_floods_hit_the_caps():
    print("\n-- L-J1: a flood of waiting joins hits the existing caps")
    reset()
    with game_max(16, seats=16):
        code, room, toks, match = _running(2)
        # From one address: the per-address join budget.
        answers = []
        for i in range(S.RATE_JOIN_PER_MIN + 5):
            st, out = join(code, "f%d" % i, who="10.6.6.6", now=1010.0)
            answers.append(st)
        check("one address: MAX_SEATS seats then room_full, never past 16",
              len(room.members) == S.MAX_SEATS == 16, len(room.members))
        check("and the join budget answers 429 past %d a minute"
              % S.RATE_JOIN_PER_MIN,
              answers.count(429) == 5 and 429 not in answers[:S.RATE_JOIN_PER_MIN],
              {s: answers.count(s) for s in set(answers)})
        check("every waiting seat is out of the match",
              room.playing_count() == 2 and sorted(room.slot_of) == [1, 2],
              room.playing_count())
        st, out = join(code, "one-more", who="10.7.7.7", now=1011.0)
        check("a fresh address meets room_full, not a 17th seat",
              st == 409 and out["error"] == "room_full", (st, out))
        _result(code, toks[0], match, scores=(1, 0, 0, 0))
        check("at the match end the dial (16) seats all 16",
              room.playing_count() == 16, room.playing_count())


def test_the_host_leaving_passes_the_host_on():
    print("\n-- L-J1: a VS host leaving passes the host on; a match on ends")
    # (1) In lobby.
    reset()
    st, h = create(nick="h", pre_ok=True)
    code = h["room"]
    st, g2 = join(code, "g2", who="10.0.0.2", pre_ok=True)
    st, w3 = join(code, "w3", who="10.0.0.3", pre_ok=True)
    room = S.ROOMS[code]
    S.do_leave({"v": 5, "room": code, "token": h["token"]}, "10.0.0.1", 1001.0)
    check("[lobby] the room stays open", room.state == "lobby", room.state)
    check("[lobby] the host is now seat 2 (the lowest playing seat)",
          room.host_seat == 2, room.host_seat)
    check("[lobby] the watcher filled the host's playing seat",
          room.members[3].playing is True, room.members[3].playing)
    check("[lobby] events: left then host",
          [e["kind"] for e in room.events[-2:]] == ["left", "host"]
          and room.events[-1]["seat"] == 2, room.events[-2:])
    st, out = poll(code, h["token"], 1, now=1002.0)
    check("[lobby] the old host's token is no member any more",
          st == 403 and out["error"] == "not_a_member", (st, out))
    st, p = start(code, g2["token"], who="10.0.0.2", now=1003.0)
    check("[lobby] the new host can start", st == 200, (st, p))
    check("[lobby] and is the parent in slot 0",
          S.member_plan(room, 2)["role"] == "parent"
          and S.member_plan(room, 2)["slot"] == 0
          and S.member_plan(room, 3)["role"] == "child"
          and S.member_plan(room, 3)["slot"] == 1,
          (S.member_plan(room, 2), S.member_plan(room, 3)))
    check("[lobby] the names string is in slot order",
          room.names.split(",")[:2] == ["g2", "w3"], room.names)

    # (2) Mid-match, in every frozen state, with a waiting joiner.
    for state in ("arming", "go", "in_match"):
        reset()
        with game_max(4, seats=4):
            code, room, toks, match = _running(3, state=state)
            st, j = join(code, "late", who="10.0.0.9", now=1010.0, pre_ok=True)
            S.do_leave({"v": 5, "room": code, "token": toks[0]},
                       "10.0.0.1", 1011.0)
            kinds = [e["kind"] for e in room.events[-3:]]
            check("[%s] the room survives, back in lobby" % state,
                  room.state == "lobby" and room.match is None
                  and room.comms_code is None, (room.state, room.why))
            check("[%s] events: left, failed host_left, host" % state,
                  kinds == ["left", "failed", "host"]
                  and room.events[-2]["reason"] == "host_left"
                  and room.events[-2]["match"] == match
                  and room.events[-1]["seat"] == 2, room.events[-3:])
            check("[%s] the new host is seat 2" % state, room.host_seat == 2)
            check("[%s] the waiting joiner and both guests play next" % state,
                  sorted(s for s in room.members if room.members[s].playing)
                  == [2, 3, 4], {s: m.playing for s, m in room.members.items()})
            st, r = _result(code, toks[1], match, who="10.0.0.2")
            check("[%s] a late result for the ended match is 409 stale" % state,
                  st == 409 and r["error"] == "stale_match", (st, r))
            st, p = start(code, toks[1], who="10.0.0.2", now=1020.0)
            check("[%s] the new host starts the next match, parent slot 0"
                  % state, st == 200 and room.slot_of == {2: 0, 3: 1, 4: 2}
                  and S.member_plan(room, 2)["role"] == "parent",
                  (st, room.slot_of))

    # (3) The host is slot 0 even when a lower seat number plays.
    reset()
    with game_max(4, seats=4):
        st, h = create(nick="h", pre_ok=True)
        code = h["room"]
        st, g2 = join(code, "g2", who="10.0.0.2", pre_ok=True)
        st, g3 = join(code, "g3", who="10.0.0.3", pre_ok=True)
        room = S.ROOMS[code]
        S.do_leave({"v": 5, "room": code, "token": h["token"]}, "10.0.0.1", 1001.0)
        st, n1 = join(code, "new1", who="10.0.0.4", now=1002.0, pre_ok=True)
        check("a later joiner takes the freed seat 1 and plays",
              n1["member"] == 1 and room.members[1].playing is True, n1)
        st, p = start(code, g2["token"], who="10.0.0.2", now=1003.0)
        check("the host (seat 2) is slot 0 and seat 1 is a child in slot 1",
              room.slot_of == {2: 0, 1: 1, 3: 2}
              and S.member_plan(room, 2)["role"] == "parent"
              and S.member_plan(room, 1)["role"] == "child"
              and S.member_plan(room, 1)["want_seat"] == 1, room.slot_of)
        check("and the names string follows the slots",
              room.names.split(",")[:3] == ["g2", "new1", "g3"], room.names)

    # (4) host_left is the server's word: a client cannot send it.
    reset()
    code, room, toks, match = _running(2)
    st, f = S.do_failed({"v": 5, "room": code, "token": toks[1], "match": match,
                         "reason": "host_left"}, "10.0.0.2", 1010.0)
    check("a client's failed with reason host_left is 400 bad_reason",
          st == 400 and f["error"] == "bad_reason" and room.state == "in_match",
          (st, f, room.state))

    # (5) A host that times out in lobby passes the host on too.
    reset()
    st, h = create(nick="h", now=1000.0)
    code = h["room"]
    st, g2 = join(code, "g2", now=1000.0)
    room = S.ROOMS[code]
    room.members[2].seen = 1000.0 + S.MEMBER_TIMEOUT_S
    S.sweep(1000.0 + S.MEMBER_TIMEOUT_S + 1)
    check("a silent host is dropped and the room passes to seat 2",
          room.state == "lobby" and room.host_seat == 2
          and 1 not in room.members, (room.state, room.host_seat))

    # (6) The old host coming back is a new member, not the host.
    st, back = join(code, "h", who="10.0.0.1", now=1100.0)
    check("the old host rejoining gets a seat, not the host role",
          st == 200 and room.host_seat == 2 and back["member"] == 1, back)


def test_a_quitters_result_does_not_end_the_match():
    """plan.txt G5, server half: first-result-wins let one quitter's fake
    draw end the match for the room."""
    print("\n-- L-J1: one player's result does not end the match for the room")
    DRAW = (0, 0, 0, 0)
    REAL = (3, 1, 2, 0)

    # (1) 2P: the guest quits and its launcher posts a draw.
    reset()
    code, room, toks, match = _running(2)
    st, r = _result(code, toks[1], match, scores=DRAW, win="draw",
                    who="10.0.0.2")
    check("[2P] the guest's lone draw is 200 and the match runs on",
          st == 200 and room.state == "in_match" and room.match == match,
          (st, room.state))
    check("[2P] and no result event was pushed",
          not any(e["kind"] == "result" for e in room.events))
    st, r = _result(code, toks[0], match, scores=(1, 0, 0, 0))
    check("[2P] the host's result ends it with the HOST's scoreline",
          st == 200 and room.state == "lobby"
          and room.events[-1]["kind"] == "result"
          and room.events[-1]["scores"] == [1, 0, 0, 0], room.events[-1])
    st, r = _result(code, toks[1], match, scores=DRAW, win="draw",
                    who="10.0.0.2", now=1101.0)
    check("[2P] a result after the end is idempotent 200", st == 200)

    with game_max(4, seats=4):
        # (2) 4P: p2 quits (draw), p3 and p4 report the real result, the host
        # says nothing (its launcher died): 2 of 4 live is not a majority...
        reset()
        code, room, toks, match = _running(4)
        _result(code, toks[1], match, scores=DRAW, win="draw", who="10.0.0.2")
        _result(code, toks[2], match, scores=REAL, who="10.0.0.3")
        _result(code, toks[3], match, scores=REAL, who="10.0.0.4")
        check("[4P] quitter's draw + two real votes of four live: still on",
              room.state == "in_match", room.state)
        # ...until the quitter's `failed` drops it (what L-J2's launcher sends):
        # 2 of 3 live agree.
        S.do_failed({"v": 5, "room": code, "token": toks[1], "match": match,
                     "reason": "user_cancelled"}, "10.0.0.2", 1110.0)
        check("[4P] the quitter's failed drops it and the two real votes settle",
              room.state == "lobby" and room.events[-1]["kind"] == "result"
              and room.events[-1]["scores"] == list(REAL), room.events[-2:])
        check("[4P] and the quitter is back in for the next match",
              room.members[2].playing is True)

        # (3) Three of four non-host seats agree: settled without the host.
        reset()
        code, room, toks, match = _running(4)
        for k in (1, 2):
            _result(code, toks[k], match, scores=REAL, who="10.0.0.%d" % (k + 1))
        check("[4P] two agreeing votes of four: on", room.state == "in_match")
        _result(code, toks[3], match, scores=REAL, who="10.0.0.4")
        check("[4P] three agreeing votes of four live: over, their scoreline",
              room.state == "lobby"
              and room.events[-1]["scores"] == list(REAL), room.events[-1])

        # (4) Votes that disagree never settle; a seat's first vote stands.
        reset()
        code, room, toks, match = _running(4)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2")
        _result(code, toks[2], match, scores=DRAW, win="draw", who="10.0.0.3")
        for _ in range(5):
            _result(code, toks[1], match, scores=DRAW, win="draw",
                    who="10.0.0.2")
        _result(code, toks[3], match, scores=(9, 9, 9, 9), who="10.0.0.4")
        check("[4P] three different votes never settle, re-votes change "
              "nothing", room.state == "in_match"
              and room.result_votes[2] == ("time-up", REAL),
              (room.state, room.result_votes))
        st, r = _result(code, toks[0], match, scores=REAL)
        check("[4P] the host's result ends it", room.state == "lobby"
              and room.events[-1]["scores"] == list(REAL))

        # (5) A leave that shrinks the live roster re-counts the votes.
        reset()
        code, room, toks, match = _running(4)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2")
        _result(code, toks[2], match, scores=REAL, who="10.0.0.3")
        check("[4P] two of four: on", room.state == "in_match")
        S.do_leave({"v": 5, "room": code, "token": toks[3]}, "10.0.0.4", 1110.0)
        check("[4P] p4 leaves: two of three live agree, the match is over",
              room.state == "lobby" and room.events[-1]["kind"] == "result",
              room.events[-1])

        # (6) Hostile: a dropped seat cannot vote; a vote before `go` is not a
        # thing (arming is not a match yet).
        reset()
        code, room, toks, match = _running(4)
        S.do_failed({"v": 5, "room": code, "token": toks[3], "match": match,
                     "reason": "no_pairing"}, "10.0.0.4", 1010.0)
        st, r = _result(code, toks[3], match, scores=REAL, who="10.0.0.4")
        check("[4P] a dropped seat's result is 403 not_playing",
              st == 403 and r["error"] == "not_playing", (st, r))
        check("[4P] and it was not counted", 4 not in room.result_votes)


def test_the_new_host_played_the_match():
    """REVLOBBYJOIN1 NOTE B: the new host is picked from the seats that were
    playing the match the host's leaving ended, not from whoever the
    end-of-match dial seated (a joiner who waited on a freed low seat)."""
    print("\n-- L-J1B: a host leaving mid-match passes to a seat that played it")
    with game_max(4, seats=4):
        for state in ("arming", "go", "in_match"):
            reset()
            code, room, toks, match = _running(4, state=state)
            S.do_leave({"v": 5, "room": code, "token": toks[1]},
                       "10.0.0.2", 1010.0)
            st, late = join(code, "late", who="10.0.0.9", now=1011.0,
                            pre_ok=True)
            check("[%s] the joiner waits on the freed seat 2" % state,
                  st == 200 and late["member"] == 2
                  and room.members[2].playing is False, (st, late))
            S.do_leave({"v": 5, "room": code, "token": toks[0]},
                       "10.0.0.1", 1012.0)
            check("[%s] the new host is seat 3, who played the match, not "
                  "the joiner on seat 2" % state,
                  room.state == "lobby" and room.host_seat == 3
                  and room.events[-1]["kind"] == "host"
                  and room.events[-1]["seat"] == 3,
                  (room.host_seat, room.events[-1]))
            check("[%s] the joiner still plays the next match" % state,
                  room.members[2].playing is True
                  and sorted(s for s in room.members
                             if room.members[s].playing) == [2, 3, 4],
                  {s: m.playing for s, m in room.members.items()})
            st, p = start(code, toks[2], who="10.0.0.3", now=1020.0)
            check("[%s] the new host starts as the parent in slot 0" % state,
                  st == 200 and room.slot_of == {3: 0, 2: 1, 4: 2}
                  and S.member_plan(room, 3)["role"] == "parent",
                  (st, room.slot_of))

        # A host leaving in lobby: no match, the rule is unchanged.
        reset()
        code, room, toks, match = _running(3, state="in_match")
        _result(code, toks[0], match)
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1", 1200.0)
        check("[lobby] no match on: the lowest playing seat hosts, as before",
              room.host_seat == 2, room.host_seat)

        # Every seat that played has gone: the lowest playing seat hosts.
        reset()
        code, room, toks, match = _running(2, state="in_match")
        st, w = join(code, "w", who="10.0.0.9", now=1010.0, pre_ok=True)
        S.do_leave({"v": 5, "room": code, "token": toks[1]}, "10.0.0.2", 1011.0)
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1", 1012.0)
        check("[none left] only the waiter is left: it hosts and plays",
              room.state == "lobby" and room.host_seat == w["member"]
              and room.members[w["member"]].playing is True,
              (room.state, room.host_seat))


def test_every_guest_agrees_and_the_host_is_silent():
    """REVLOBBYJOIN1 section 3: when every live non-host seat has posted the
    SAME result and the host has sent nothing since the first of those votes
    for HOST_SILENT_S (60 s), the match is over: settle it with that vote
    instead of waiting out MATCH_TIMEOUT_S. Only a live pair needs this (with
    three or more live seats, all the guests agreeing is already a
    majority)."""
    print("\n-- L-J1B: all guests agree and the host is silent 60 s: settled")
    REAL = (3, 1, 0, 0)
    DRAW = (0, 0, 0, 0)
    Q = getattr(S, "HOST_SILENT_S", None) or 60
    check("the bound is 60 s by default",
          getattr(S, "HOST_SILENT_S", None) == 60, Q)

    # (1) 2P, the host's launcher died at MATCH OVER: the guest's real result
    # settles 60 s after it came in, not 20 minutes after the start.
    reset()
    code, room, toks, match = _running(2)
    t0 = 1100.0
    _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0)
    S.sweep(t0 + Q - 1)
    check("[2P] 59 s after the guest's vote: still on",
          room.state == "in_match", room.state)
    # (The guest's launcher is alive and polling; the host's is dead.)
    poll(code, toks[1], room.seq, who="10.0.0.2", now=t0 + Q)
    S.sweep(t0 + Q + 0.1)
    res = [e for e in room.events if e["kind"] == "result"]
    check("[2P] 60 s after it, host silent: over, the guest's scoreline",
          room.state == "lobby" and len(res) == 1
          and res[0]["match"] == match
          and res[0]["scores"] == list(REAL), room.events[-3:])
    check("[2P] back in lobby the dead host is dropped and the guest hosts",
          1 not in room.members and room.host_seat == 2
          and [e["kind"] for e in room.events[-3:]]
          == ["result", "left", "host"], room.events[-3:])

    # (2) The host is alive (it polled after the vote): no settle by this
    # rule, however long. Its own result will come.
    reset()
    code, room, toks, match = _running(2)
    _result(code, toks[1], match, scores=DRAW, win="draw", who="10.0.0.2",
            now=t0)
    poll(code, toks[0], room.seq, now=t0 + 30)
    S.sweep(t0 + 30 + Q + 1)
    check("[2P] a host that spoke after the vote: still on",
          room.state == "in_match", room.state)
    # ... and a host that polled before the vote only is silent after it.
    reset()
    code, room, toks, match = _running(2)
    poll(code, toks[0], room.seq, now=t0 - 5)
    _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0)
    S.sweep(t0 + Q + 0.1)
    check("[2P] a host last heard BEFORE the vote counts as silent",
          any(e["kind"] == "result" and e["match"] == match
              for e in room.events), room.events[-3:])

    with game_max(4, seats=4):
        # (3) Disagreeing guests never settle by this rule.
        reset()
        code, room, toks, match = _running(3)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0)
        _result(code, toks[2], match, scores=DRAW, win="draw",
                who="10.0.0.3", now=t0)
        S.sweep(t0 + Q + 60)
        check("[3P] two guests, two different results, host silent: on",
              room.state == "in_match", room.state)

        # (4) A guest that has not voted blocks it.
        reset()
        code, room, toks, match = _running(4)
        S.do_failed({"v": 5, "room": code, "token": toks[3], "match": match,
                     "reason": "no_pairing"}, "10.0.0.4", t0 - 10)
        S.do_failed({"v": 5, "room": code, "token": toks[2], "match": match,
                     "reason": "no_pairing"}, "10.0.0.3", t0 - 10)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0)
        S.sweep(t0 + Q + 0.1)
        check("[4P->2 live] the one live guest agrees, host silent: over",
              any(e["kind"] == "result" and e["match"] == match
                  for e in room.events), room.events[-3:])

        # (5) A waiting joiner cannot feed this rule (it cannot vote), and a
        # guest that has not voted blocks it.
        reset()
        code, room, toks, match = _running(3)
        st, w = join(code, "w", who="10.0.0.9", now=t0 - 1, pre_ok=True)
        st, r = _result(code, w["token"], match, scores=REAL,
                        who="10.0.0.9", now=t0)
        check("[3P] the waiting seat's vote is 403",
              st == 403 and r["error"] == "not_playing", (st, r))
        S.sweep(t0 + Q + 60)
        check("[3P] no guest voted: on", room.state == "in_match", room.state)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0 + 100)
        S.sweep(t0 + 100 + Q + 60)
        check("[3P] one of two guests voted, host silent: on",
              room.state == "in_match", room.state)

        # (6) The clock is the FIRST of the agreeing votes still counted,
        # and it starts over with the next match.
        reset()
        code, room, toks, match = _running(2)
        _result(code, toks[1], match, scores=REAL, who="10.0.0.2", now=t0)
        _result(code, toks[0], match, scores=REAL, now=t0 + 1)
        st, p = start(code, toks[0], now=t0 + 2)
        m2 = p["match"]
        for i, tok in enumerate(toks):
            ready(code, tok, m2, who="10.0.0.%d" % (i + 1), now=t0 + 2)
        S.sweep(t0 + 2 + S.GO_GRACE_S + 0.1)
        S.sweep(t0 + Q + 10)
        check("[next match] no vote carried over: on",
              room.state == "in_match" and room.match == m2,
              (room.state, room.match))


def test_server_line_width():
    """REVLOBBYJOIN1 hygiene nit: assign_slots lost a line break and made the
    only line of server.py over 100 characters."""
    print("\n-- L-J1B: server.py keeps its lines at 100 characters or fewer")
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "app", "server.py")
    with open(path, encoding="utf-8") as f:
        long = [(n, len(line.rstrip("\r\n")))
                for n, line in enumerate(f, 1)
                if len(line.rstrip("\r\n")) > 100]
    check("no line of server.py is over 100 characters", not long, long)


def test_host_silent_floor():
    """REVLOBBYJOIN1-B note D: HOST_SILENT_S accepted 10 from the env, under
    the launcher's 20 s poll hold, and set that low it read a live host as
    silent. Its floor is 45, the member heartbeat (MEMBER_TIMEOUT_S)."""
    print("\n-- L-J1C: HOST_SILENT_S cannot be set under 45 s")
    import subprocess
    app = os.path.join(os.path.dirname(os.path.abspath(__file__)), "app")
    got = {}
    for raw in ("10", "44", "45", "90", "99999", "junk"):
        env = dict(os.environ, HOST_SILENT_S=raw)
        out = subprocess.run(
            [sys.executable, "-c",
             "import server; print(server.HOST_SILENT_S)"],
            cwd=app, env=env, capture_output=True, text=True, timeout=60)
        got[raw] = out.stdout.strip().splitlines()[-1] if out.stdout else out.stderr
    check("HOST_SILENT_S=10 and =44 read as 45 (the floor)",
          got["10"] == "45" and got["44"] == "45", got)
    check("45 and 90 are kept, 99999 is clamped to 3600, junk is the default 60",
          (got["45"], got["90"], got["99999"], got["junk"])
          == ("45", "90", "3600", "60"), got)
    check("the floor is not under the member heartbeat (MEMBER_TIMEOUT_S 45)",
          int(got["10"]) >= 45, got)


def test_a_throttled_host_is_not_silent():
    """REVLOBBYJOIN1-B note C: host and guest behind ONE public address (a
    household NAT). A modified guest floods that address, the rate limiter
    refuses everything from it -- the live host's polls included, BEFORE
    they stamp `seen` -- and its fake result settled at +60 s. Silence the
    limiter caused is not silence: the host-silent clock counts from the
    later of the first vote and the host address's last refusal."""
    print("\n-- L-J1C: a host whose address is being refused is not silent")
    FAKE = (0, 9, 0, 0)
    Q = S.HOST_SILENT_S
    t0 = 1100.0
    NAT = "10.0.0.1"

    def flood(addr, t_from, t_to):
        # What the HTTP layer does for each refused request (the guest's
        # flood and the host's own polls alike): note_reject on the address.
        t = t_from
        while t <= t_to:
            S.BUCKETS.note_reject(addr, t)
            t += 0.5

    reset()
    code, room, toks, match = _running(2)
    _result(code, toks[1], match, scores=FAKE, who=NAT, now=t0)
    flood(NAT, t0 + 0.5, t0 + 75)
    S.sweep(t0 + Q + 1)
    check("[NAT] the address is refused through +75 s: at +%ds still on"
          % (Q + 1), room.state == "in_match" and room.match == match,
          room.state)
    S.sweep(t0 + 75 + Q - 1)
    check("[NAT] %d s after the last refusal: still on" % (Q - 1),
          room.state == "in_match", room.state)
    # The host really is dead: once its address has been left alone for
    # HOST_SILENT_S the rule works as before, so it is never stuck for good.
    # (The guest's launcher is alive and polling.)
    poll(code, toks[1], room.seq, who=NAT, now=t0 + 75 + Q)
    S.sweep(t0 + 75 + Q + 0.5)
    check("[NAT] %d s after the last refusal, host silent: over" % Q,
          any(e["kind"] == "result" and e["match"] == match
              for e in room.events), room.state)

    # A refusal of SOME OTHER address does not hold the pair's match.
    reset()
    code, room, toks, match = _running(2)
    _result(code, toks[1], match, scores=FAKE, who="10.0.0.2", now=t0)
    flood("10.0.0.77", t0, t0 + 75)
    poll(code, toks[1], room.seq, who="10.0.0.2", now=t0 + Q)
    S.sweep(t0 + Q + 0.5)
    check("[other address refused] the pair still settles at +%ds" % Q,
          room.state == "lobby", room.state)

    # A refusal that could not be recorded (the address tables are full)
    # holds it too: the guard fails closed.
    reset()
    code, room, toks, match = _running(2)
    _result(code, toks[1], match, scores=FAKE, who=NAT, now=t0)
    real = S.MAX_BUCKETS
    try:
        S.MAX_BUCKETS = 0
        S.BUCKETS.bad.clear()
        S.BUCKETS.note_reject(NAT, t0 + 30)
    finally:
        S.MAX_BUCKETS = real
    S.sweep(t0 + Q + 1)
    check("[tables full] an unrecorded refusal since the vote: still on",
          room.state == "in_match", room.state)


def Member_fields(m):
    return [k for k in m.__slots__]


# ------------------------------------------------------------- ARENA MODE
#
# run mp1, lane A4 (VSSYNC1 plan section 8): the go plan's `netmode`, the
# digest that counts it, the host's pick, the build switch, and the one lobby
# rule an arena match changes (it has no host in it).


class arena(object):
    """Run a block with the arena knobs set, the way a compose edit plus a
    restart would: ARENA_GAME_IDS ("*" or a tuple of builds), the size limit,
    RELAY_V, a new room's pick and ARENA_LUIGI_GAME_IDS (luigi_ids, "*" or a
    tuple; the default () is the file default, Luigi on lockstep). Restores
    all of them after."""

    def __init__(self, ids="*", max_players=4, relay_v=2, default="arena",
                 luigi_ids=()):
        self.ids = ids
        self.max_players = max_players
        self.relay_v = relay_v
        self.default = default
        self.luigi_ids = luigi_ids

    def __enter__(self):
        self.was = (S.ARENA_ANY, S.ARENA_GAME_IDS, S.ARENA_MAX_PLAYERS,
                    S.RELAY_V, S.NETMODE_DEFAULT, S.ARENA_LUIGI_ANY,
                    S.ARENA_LUIGI_GAME_IDS)
        S.ARENA_ANY = self.ids == "*"
        S.ARENA_GAME_IDS = () if self.ids == "*" else tuple(self.ids)
        S.ARENA_MAX_PLAYERS = self.max_players
        S.RELAY_V = self.relay_v
        S.NETMODE_DEFAULT = self.default
        S.ARENA_LUIGI_ANY = self.luigi_ids == "*"
        S.ARENA_LUIGI_GAME_IDS = (() if self.luigi_ids == "*"
                                  else tuple(self.luigi_ids))
        return self

    def __exit__(self, *exc):
        (S.ARENA_ANY, S.ARENA_GAME_IDS, S.ARENA_MAX_PLAYERS, S.RELAY_V,
         S.NETMODE_DEFAULT, S.ARENA_LUIGI_ANY,
         S.ARENA_LUIGI_GAME_IDS) = self.was
        return False


def test_arena_digest():
    print("\n-- arena: the digest counts the netmode")
    vectors = [
        ((0, 2, "time", None, None, 0, 0), "arena",
         "0|2|time|0|0|0|0|arena|2", 1565304087),
        ((1, 4, "stars", 3, None, 0, 3735928559), "arena",
         "1|4|stars|3|0|0|0|arena|2", 1476757693),
        ((3, 3, "stars", 5, None, 0, 7), "arena",
         "3|3|stars|5|0|0|0|arena|2", 1991793976),
        ((0, 2, "time", None, None, 0, 0), "lockstep",
         "0|2|time|0|0|0|0|lockstep|2", 1172137709),
    ]
    for args, nm, text, want in vectors:
        check("canonical %s" % text,
              S.session_canonical(*args, netmode=nm) == text,
              S.session_canonical(*args, netmode=nm))
        check("digest of %s is %d" % (text, want),
              S.session_digest(*args, netmode=nm) == want,
              S.session_digest(*args, netmode=nm))
    check("the default netmode is still lockstep (every old caller unchanged)",
          S.session_canonical(0, 2, "time", None, None, 0, 0)
          == "0|2|time|0|0|0|0|lockstep|2")
    check("an arena match and a lockstep match of the same settings never "
          "share a digest",
          S.session_digest(0, 2, "time", None, None, 0, 0, netmode="arena")
          != S.session_digest(0, 2, "time", None, None, 0, 0))


def test_arena_off_by_default():
    print("\n-- arena: OFF until ARENA_GAME_IDS names the build")
    reset()
    check("the file default: no arena builds, not '*'",
          S.ARENA_GAME_IDS == () and S.ARENA_ANY is False,
          (S.ARENA_GAME_IDS, S.ARENA_ANY))
    check("the file defaults: arena max 4, new rooms pick arena",
          S.ARENA_MAX_PLAYERS == 4 and S.NETMODE_DEFAULT == "arena",
          (S.ARENA_MAX_PLAYERS, S.NETMODE_DEFAULT))
    code, room, toks, match = _running(2)
    p0, p1 = S.member_plan(room, 1), S.member_plan(room, 2)
    check("the room's pick is arena, but the build is not on the list",
          room.netmode == "arena" and S.arena_ok(room) is False)
    check("so the match plays lockstep", room.match_netmode == "lockstep",
          room.match_netmode)
    check("both plans say lockstep",
          p0["netmode"] == p1["netmode"] == "lockstep",
          (p0.get("netmode"), p1.get("netmode")))
    check("and the digest is the lockstep one, unchanged from before arena",
          p0["digest"] == S.session_digest(0, 2, "time", None, None,
                                           room.match_luigi, room.match_seed)
          and p0["digest"] == 1172137709, p0["digest"])
    v = room.view(1)
    check("the view: pick arena, arena_ok false, arena_now lockstep, "
          "match_netmode lockstep",
          v["netmode"] == "arena" and v["arena_ok"] is False
          and v["arena_now"] == "lockstep" and v["match_netmode"] == "lockstep"
          and v["arena_max"] == 4, v)


def test_arena_plan():
    print("\n-- arena: a listed build plays arena; the plan and the digest")
    for ids in ("*", (GAME,)):
        reset()
        with arena(ids=ids):
            st, h = create(nick="h", pre_ok=True)
            code = h["room"]
            st, g = join(code, "g", who="10.0.0.2", pre_ok=True)
            room = S.ROOMS[code]
            v = room.view(1)
            check("[%s] lobby view: arena_ok, arena_now arena, no match yet"
                  % (ids,), v["arena_ok"] is True and v["arena_now"] == "arena"
                  and v["match_netmode"] is None and v["netmode"] == "arena", v)
            st, p = start(code, h["token"])
            check("[%s] start resolves arena" % (ids,),
                  st == 200 and room.match_netmode == "arena",
                  (st, room.match_netmode))
            arm = [e for e in room.events if e["kind"] == "arming"][-1]
            check("[%s] the arming event carries netmode arena" % (ids,),
                  arm.get("netmode") == "arena", arm)
            p0, p1 = S.member_plan(room, 1), S.member_plan(room, 2)
            check("[%s] both plans say arena" % (ids,),
                  p0["netmode"] == p1["netmode"] == "arena")
            check("[%s] the digest counts arena: 0|2|time|0|0|0|0|arena|2"
                  % (ids,), p0["digest"] == p1["digest"] == 1565304087,
                  p0["digest"])
            check("[%s] slot, want_seat and role are unchanged: host parent "
                  "slot 0, guest child slot 1" % (ids,),
                  (p0["role"], p0["slot"], p0["want_seat"]) == ("parent", 0, 0)
                  and (p1["role"], p1["slot"], p1["want_seat"])
                  == ("child", 1, 1), (p0, p1))
            check("[%s] the code is fresh per match as always" % (ids,),
                  p0["code"] == room.comms_code and len(p0["code"]) == 8)
            check("[%s] the view says match_netmode arena" % (ids,),
                  room.view(2)["match_netmode"] == "arena")
            st, w = join(code, "w", who="10.0.0.3", pre_ok=True)
            check("[%s] a waiting seat's plan is just playing:false" % (ids,),
                  S.member_plan(room, w["member"]) == {"playing": False},
                  S.member_plan(room, w["member"]))
            for i, tok in enumerate([h["token"], g["token"]]):
                ready(code, tok, room.match, who="10.0.0.%d" % (i + 1))
            S.sweep(1000.0 + S.GO_GRACE_S + 0.1)
            _result(code, h["token"], room.match)
            check("[%s] after the result match_netmode is null again" % (ids,),
                  room.state == "lobby" and room.match_netmode is None
                  and room.view(1)["match_netmode"] is None)
    reset()
    with arena(ids=("1.0.0-otherbld",)):
        code, room, toks, match = _running(2)
        check("a build not on ARENA_GAME_IDS plays lockstep",
              room.match_netmode == "lockstep"
              and S.member_plan(room, 1)["netmode"] == "lockstep")
    reset()
    with arena():
        st, a = create(nick="a", session_type="adventure")
        room = S.ROOMS[a["room"]]
        v = room.view(1)
        check("an adventure room's view has no netmode keys",
              not any(k in v for k in ("netmode", "arena_ok", "arena_max",
                                       "arena_now", "match_netmode")), v)
        check("and its plan has no netmode",
              "netmode" not in S.member_plan(room, 1), S.member_plan(room, 1))


def test_arena_when():
    print("\n-- arena: which matches play arena (and the rest lockstep)")

    def resolved(n=2, win="time", star=None, king=None, luigi=0,
                 relay_v=2, max_players=4, pick=None):
        reset()
        with game_max(16, seats=16), arena(relay_v=relay_v,
                                           max_players=max_players):
            st, h = S.do_create({"v": 5, "game": GAME, "nick": "h",
                                 "pre_ok": True}, "10.0.0.1", 1000.0)
            code = h["room"]
            for k in range(2, n + 1):
                S.do_join({"v": 5, "game": GAME, "room": code,
                           "nick": "g%d" % k, "pre_ok": True},
                          "10.0.1.%d" % k, 1000.0)
            extra = {}
            if star is not None:
                extra["star_target"] = star
            if king is not None:
                extra["king_target"] = king
            if pick is not None:
                extra["netmode"] = pick
            st, out = _params(code, h["token"], win_mode=win,
                              match_players=n, luigi=luigi, **extra)
            assert st == 200, (st, out)
            room = S.ROOMS[code]
            now = room.view(1)["arena_now"]
            st, p = start(code, h["token"])
            assert st == 200, (st, p)
            return room.match_netmode, now, S.member_plan(room, 1)

    for n in (2, 3, 4):
        nm, now, plan = resolved(n)
        check("%d players, time: arena" % n, nm == "arena" and now == "arena"
              and plan["netmode"] == "arena", (nm, now))
    nm, now, plan = resolved(3, win="stars", star=5)
    check("stars mode: arena", nm == "arena" and plan["star_target"] == 5)
    nm, now, plan = resolved(5)
    check("5 players with the phase-A limit 4: lockstep (and arena_now said "
          "so before Start)", nm == "lockstep" and now == "lockstep",
          (nm, now))
    nm, now, plan = resolved(6, max_players=16)
    check("6 players once the limit is 16: arena", nm == "arena")
    nm, now, plan = resolved(2, win="king", king=30)
    check("King of the Star: lockstep", nm == "lockstep")
    nm, now, plan = resolved(2, luigi=1)
    check("Luigi Infection: lockstep", nm == "lockstep" and plan["luigi"] == 1)
    nm, now, plan = resolved(2, relay_v=1)
    check("RELAY_V 1: lockstep", nm == "lockstep")
    nm, now, plan = resolved(2, pick="lockstep")
    check("the host picked lockstep (the fallback switch): lockstep, and the "
          "lockstep digest",
          nm == "lockstep" and now == "lockstep"
          and plan["digest"] == S.session_digest(
              0, 2, "time", None, None, 0, plan["seed"]), (nm, now))
    nm, now, plan = resolved(2, pick="arena")
    check("and picking arena again: arena", nm == "arena")


OLD_GAME = "t-0.9"   # an older build: on ARENA_GAME_IDS, not on the Luigi list


def test_arena_luigi():
    """LOBBYLUIGI1: Luigi Infection on the arena referee for the builds on
    ARENA_LUIGI_GAME_IDS only; every other build and every other mode exactly
    as before."""
    print("\n-- arena Luigi: ARENA_LUIGI_GAME_IDS puts a new build's Luigi "
          "room on the referee")
    check("the file default: no arena Luigi builds, not '*' (every Luigi "
          "match lockstep, as before)",
          S.ARENA_LUIGI_GAME_IDS == () and S.ARENA_LUIGI_ANY is False,
          (S.ARENA_LUIGI_GAME_IDS, S.ARENA_LUIGI_ANY))

    def room_of(game, n=2, luigi=1, win="time", star=None, king=None,
                pick=None):
        st, h = S.do_create({"v": 5, "game": game, "nick": "h",
                             "pre_ok": True}, "10.0.0.1", 1000.0)
        assert st == 200, (st, h)
        code = h["room"]
        toks = [h["token"]]
        for k in range(2, n + 1):
            st, out = S.do_join({"v": 5, "game": game, "room": code,
                                 "nick": "g%d" % k, "pre_ok": True},
                                "10.0.1.%d" % k, 1000.0)
            assert st == 200, (st, out)
            toks.append(out["token"])
        extra = {}
        if star is not None:
            extra["star_target"] = star
        if king is not None:
            extra["king_target"] = king
        if pick is not None:
            extra["netmode"] = pick
        st, out = _params(code, toks[0], win_mode=win, match_players=n,
                          luigi=luigi, **extra)
        assert st == 200, (st, out)
        return code, S.ROOMS[code], toks

    def resolve(game, ids, luigi_ids, n=2, luigi=1, relay_v=2,
                max_players=4, **kw):
        reset()
        with game_max(16, seats=16), arena(ids=ids, luigi_ids=luigi_ids,
                                           relay_v=relay_v,
                                           max_players=max_players):
            code, room, toks = room_of(game, n=n, luigi=luigi, **kw)
            v = room.view(1)
            st, p = start(code, toks[0])
            assert st == 200, (st, p)
            plans = [S.member_plan(room, m) for m in sorted(room.members)]
            return room.match_netmode, v, plans

    # 1. a new build (on both lists) creating a Luigi room: arena
    for ids, lids in (((GAME, OLD_GAME), (GAME,)), ("*", "*"),
                      ("*", (GAME,)), ((GAME,), "*")):
        nm, v, plans = resolve(GAME, ids, lids)
        check("[%s / %s] a new build's Luigi room: the view says arena_luigi "
              "and arena_now arena" % (ids, lids),
              v["arena_luigi"] is True and v["arena_now"] == "arena"
              and v["luigi"] == 1, v)
        check("[%s / %s] start resolves arena; both plans arena, luigi 1, "
              "one seed, one digest" % (ids, lids),
              nm == "arena" and all(p["netmode"] == "arena"
                                    and p["luigi"] == 1 for p in plans)
              and plans[0]["seed"] == plans[1]["seed"]
              and plans[0]["digest"] == plans[1]["digest"], (nm, plans))
        p0 = plans[0]
        check("[%s / %s] the digest is the arena digest with the Luigi seed "
              "(map|players|time|0|0|1|seed|arena|2)" % (ids, lids),
              p0["digest"] == S.session_digest(
                  0, 2, "time", None, None, 1, p0["seed"], netmode="arena"),
              p0)
    # 2. an older build creating a Luigi room: lockstep exactly as today
    for ids, lids in (((GAME, OLD_GAME), (GAME,)), ((OLD_GAME,), ()),
                      ((OLD_GAME, GAME), ())):
        nm, v, plans = resolve(OLD_GAME, ids, lids)
        check("[%s / %s] an older build's Luigi room: arena_luigi false, "
              "arena_now lockstep" % (ids, lids),
              v["arena_luigi"] is False and v["arena_now"] == "lockstep", v)
        check("[%s / %s] start resolves lockstep with the lockstep digest"
              % (ids, lids),
              nm == "lockstep" and all(p["netmode"] == "lockstep"
                                       and p["luigi"] == 1 for p in plans)
              and plans[0]["digest"] == S.session_digest(
                  0, 2, "time", None, None, 1, plans[0]["seed"]), plans)
    # 3. the Luigi list alone is not enough, and the other arena rules hold
    nm, v, plans = resolve(GAME, (OLD_GAME,), (GAME,))
    check("a build on ARENA_LUIGI_GAME_IDS but not on ARENA_GAME_IDS: "
          "lockstep (it must be on both)",
          nm == "lockstep" and v["arena_luigi"] is False
          and v["arena_ok"] is False, (nm, v))
    nm, v, plans = resolve(GAME, "*", "*", relay_v=1)
    check("RELAY_V 1: a Luigi room plays lockstep", nm == "lockstep"
          and v["arena_luigi"] is False, (nm, v))
    nm, v, plans = resolve(GAME, "*", "*", n=5)
    check("5 Luigi players with the arena limit 4: lockstep", nm == "lockstep")
    nm, v, plans = resolve(GAME, "*", "*", n=6, max_players=16)
    check("6 Luigi players once the limit is 16: arena", nm == "arena")
    nm, v, plans = resolve(GAME, "*", "*", pick="lockstep")
    check("the host picked lockstep: a Luigi room plays lockstep",
          nm == "lockstep" and v["arena_now"] == "lockstep")
    nm, v, plans = resolve(GAME, "*", "*", luigi=1, win="king", king=30)
    check("Luigi with a King win mode (the launcher can send it): lockstep "
          "(the referee has no King rules)", nm == "lockstep")

    # 4. every other mode: the Luigi list changes nothing
    def strip(ps):
        return [dict((k, x) for k, x in p.items()
                     if k not in ("code", "seed", "digest", "match"))
                for p in ps]

    same = True
    rows = []
    for n in (2, 3, 4, 5):
        for win, star, king in (("time", None, None), ("stars", 3, None),
                                ("king", None, 30)):
            for pick in (None, "lockstep"):
                for game in (GAME, OLD_GAME):
                    a = resolve(game, (GAME, OLD_GAME), (), n=n, luigi=0,
                                win=win, star=star, king=king, pick=pick)
                    b = resolve(game, (GAME, OLD_GAME), (GAME,), n=n,
                                luigi=0, win=win, star=star, king=king,
                                pick=pick)
                    ok = (a[0] == b[0]
                          and a[1]["arena_now"] == b[1]["arena_now"]
                          and strip(a[2]) == strip(b[2]))
                    rows.append((n, win, pick, game, a[0]))
                    same = same and ok
    check("Clock, Stars and King at 2..5 players, both picks, both builds "
          "(%d rooms): the same netmode, arena_now and plans with and without "
          "ARENA_LUIGI_GAME_IDS" % len(rows), same, rows)
    check("and that matrix holds arena and lockstep rooms both",
          {r[4] for r in rows} == {"arena", "lockstep"}, rows)

    # 5. the door: an older build cannot join a new build's arena Luigi room
    reset()
    with arena(ids=(GAME, OLD_GAME), luigi_ids=(GAME,)):
        code, room, toks = room_of(GAME)
        before = sorted(room.members)
        st, out = S.do_join({"v": 5, "game": OLD_GAME, "room": code,
                             "nick": "old", "pre_ok": True},
                            "10.0.2.9", 1000.0)
        check("an older build joining a new build's arena Luigi room: 409 "
              "game_mismatch at the door",
              st == 409 and out == {"error": "game_mismatch"}, (st, out))
        check("and it took no seat (it never sees a plan or a countdown)",
              sorted(room.members) == before, sorted(room.members))
        st, out = S.do_join({"v": 6, "game": OLD_GAME, "room": code,
                             "nick": "old6", "pre_ok": True},
                            "10.0.2.10", 1000.0)
        check("a v6 older build too", st == 409
              and out == {"error": "game_mismatch"}, (st, out))
        st, p = start(code, toks[0])
        check("the new build's match still starts on arena",
              st == 200 and room.match_netmode == "arena", (st, p))
    # and the other way round: a new build cannot join an older build's
    # Luigi room (the one-build rule, unchanged); that room plays lockstep
    reset()
    with arena(ids=(GAME, OLD_GAME), luigi_ids=(GAME,)):
        code, room, toks = room_of(OLD_GAME, n=2)
        st, out = S.do_join({"v": 5, "game": GAME, "room": code,
                             "nick": "new", "pre_ok": True},
                            "10.0.2.11", 1000.0)
        check("a new build joining an older build's Luigi room: 409 "
              "game_mismatch, as before",
              st == 409 and out == {"error": "game_mismatch"}, (st, out))
        st, p = start(code, toks[0])
        check("the older build's Luigi match plays lockstep",
              st == 200 and room.match_netmode == "lockstep"
              and all(S.member_plan(room, m)["netmode"] == "lockstep"
                      for m in room.members), room.match_netmode)

    # 6. the host toggles Luigi in a new build's room: arena_now follows
    reset()
    with arena(ids=(GAME,), luigi_ids=(GAME,)):
        code, room, toks = room_of(GAME, luigi=0)
        a = room.view(1)["arena_now"]
        _params(code, toks[0], win_mode="time", match_players=2, luigi=1)
        b = room.view(1)["arena_now"]
        _params(code, toks[0], win_mode="stars", star_target=3,
                match_players=2, luigi=0)
        c = room.view(1)["arena_now"]
        check("Clock -> Luigi -> Stars: arena_now arena all the way and the "
              "view's arena_luigi stays true",
              (a, b, c) == ("arena", "arena", "arena")
              and room.view(1)["arena_luigi"] is True, (a, b, c))
    reset()
    with arena(ids=(GAME,), luigi_ids=()):
        code, room, toks = room_of(GAME, luigi=0)
        a = room.view(1)["arena_now"]
        _params(code, toks[0], win_mode="time", match_players=2, luigi=1)
        b = room.view(1)["arena_now"]
        check("with the Luigi list empty: Clock arena, Luigi lockstep, "
              "arena_luigi false", (a, b) == ("arena", "lockstep")
              and room.view(1)["arena_luigi"] is False, (a, b))
    # 7. an adventure room's view still has no netmode keys
    reset()
    with arena(luigi_ids="*"):
        st, a = create(nick="a", session_type="adventure")
        v = S.ROOMS[a["room"]].view(1)
        check("an adventure room's view has no arena_luigi key",
              "arena_luigi" not in v, v)
    # 8. the knob's grammar is ARENA_GAME_IDS's
    ids, dropped = S.parse_game_ids("1.0.0-abcdef12, bad id!,t-1.0")
    check("ARENA_LUIGI_GAME_IDS parses like GAME_IDS (a bad entry dropped)",
          tuple(ids) == ("1.0.0-abcdef12", "t-1.0")
          and list(dropped) == ["bad id!"], (ids, dropped))


def test_arena_params():
    print("\n-- arena: the host's netmode pick on params")
    reset()
    with arena():
        st, h = create(nick="h", pre_ok=True)
        code = h["room"]
        room = S.ROOMS[code]
        st, g = join(code, "g", who="10.0.0.2", pre_ok=True)
        check("a new room starts at NETMODE_DEFAULT (arena)",
              room.netmode == "arena")
        st, out = _params(code, h["token"], netmode="lockstep")
        ev = room.events[-1]
        check("netmode lockstep is accepted and rides the params event",
              st == 200 and room.netmode == "lockstep"
              and ev["kind"] == "params" and ev["netmode"] == "lockstep",
              (st, out, ev))
        st, out = _params(code, h["token"])
        check("a params without netmode keeps the room's pick",
              st == 200 and room.netmode == "lockstep", room.netmode)
        for bad in ("ARENA", "rollback", "", 1, True, None, ["arena"],
                    "arena ", "a" * 300):
            st, out = _params(code, h["token"], netmode=bad)
            check("netmode %r is 400 bad_netmode and changes nothing"
                  % (bad if not isinstance(bad, str) else bad[:12],),
                  st == 400 and out["error"] == "bad_netmode"
                  and room.netmode == "lockstep", (st, out))
        st, out = _params(code, g["token"], who="10.0.0.2", netmode="arena")
        check("a guest cannot set it (403 not_host)",
              st == 403 and out["error"] == "not_host"
              and room.netmode == "lockstep", (st, out))
        st, out = _params(code, h["token"], netmode="arena")
        st, p = start(code, h["token"])
        st, out = _params(code, h["token"], netmode="lockstep")
        check("not while a match is arming (409 not_in_lobby)",
              st == 409 and out["error"] == "not_in_lobby"
              and room.netmode == "arena" and room.match_netmode == "arena",
              (st, out))
        st, a = create(nick="a", who="10.0.0.7", session_type="adventure")
        st, out = _params(a["room"], a["token"], who="10.0.0.7",
                          netmode="arena")
        check("on an adventure room it is 400 bad_field",
              st == 400 and out["error"] == "bad_field", (st, out))
    reset()
    with arena(default="lockstep"):
        st, h = create(nick="h", pre_ok=True)
        check("NETMODE_DEFAULT lockstep: a new room picks lockstep",
              S.ROOMS[h["room"]].netmode == "lockstep")


def test_arena_has_no_host_in_the_match():
    print("\n-- arena: the host leaving or failing does not end the match")
    # (1) the host leaves an arena match: left, host; the match runs on.
    for state in ("go", "in_match"):
        reset()
        with game_max(4, seats=4), arena():
            code, room, toks, match = _running(3, state=state)
            st, j = join(code, "late", who="10.0.0.9", now=1010.0, pre_ok=True)
            n_ev = len(room.events)
            S.do_leave({"v": 5, "room": code, "token": toks[0]},
                       "10.0.0.1", 1011.0)
            kinds = [e["kind"] for e in room.events[n_ev:]]
            check("[%s] events: left, host (no failed)" % state,
                  kinds == ["left", "host"]
                  and room.events[-1]["seat"] == 2, room.events[n_ev:])
            check("[%s] the match runs on, same id and code" % state,
                  room.state == state and room.match == match
                  and room.comms_code is not None, (room.state, room.match))
            check("[%s] the new host is seat 2, which played it" % state,
                  room.host_seat == 2 and S.in_frozen_match(room, 2))
            check("[%s] the waiting joiner is still waiting" % state,
                  room.members[j["member"]].playing is False)
            check("[%s] a re-sent plan keeps the roles: seat 2 is still the "
                  "child in slot 1" % state,
                  S.member_plan(room, 2)["role"] == "child"
                  and S.member_plan(room, 2)["slot"] == 1,
                  S.member_plan(room, 2))
            st, r = _result(code, toks[2], match, who="10.0.0.3")
            check("[%s] a guest's result is a vote (1 of 2 live)" % state,
                  st == 200 and room.state == state, (st, r, room.state))
            st, r = _result(code, toks[1], match, who="10.0.0.2")
            check("[%s] the new host's result ends it" % state,
                  st == 200 and room.state == "lobby"
                  and room.events[-1]["kind"] == "result", room.events[-1])
            check("[%s] then the joiner plays the next match" % state,
                  room.members[j["member"]].playing is True)
    # (2) the new host had already voted: the hand-over settles it.
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(3)
        _result(code, toks[1], match, who="10.0.0.2")
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1", 1011.0)
        check("a host leaving after seat 2 voted: seat 2 is host, its "
              "result ends the match", room.state == "lobby"
              and [e["kind"] for e in room.events[-3:]]
              == ["left", "host", "result"], room.events[-3:])
    # (3) arming still ends it (nothing has spawned; the plan is remade).
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(3, state="arming")
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1", 1011.0)
        check("during arming the host leaving ends it as before (failed "
              "host_left)", room.state == "lobby"
              and any(e["kind"] == "failed" and e["reason"] == "host_left"
                      for e in room.events[-3:]), room.events[-3:])
    # (4) fewer than 2 left still ends an arena match.
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(2)
        S.do_failed({"v": 5, "room": code, "token": toks[1], "match": match,
                     "reason": "user_cancelled"}, "10.0.0.2", 1005.0)
        check("a 2P arena match whose guest quit ends (fewer than 2 left)",
              room.state == "lobby", room.state)
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(2)
        S.do_leave({"v": 5, "room": code, "token": toks[1]}, "10.0.0.2",
                   1005.0)
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1",
                   1006.0)
        check("a 2P arena match whose guest left, then the host left: the "
              "room closes (last member out)", room.state == "closed",
              room.state)
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(2)
        st, j = join(code, "late", who="10.0.0.9", now=1004.0, pre_ok=True)
        S.do_leave({"v": 5, "room": code, "token": toks[1]}, "10.0.0.2",
                   1005.0)
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1",
                   1006.0)
        check("both players gone with a waiting joiner left: the match ends "
              "(failed host_left) and the joiner is host",
              room.state == "lobby" and room.host_seat == j["member"]
              and any(e["kind"] == "failed" and e["reason"] == "host_left"
                      for e in room.events[-3:]), (room.state,
                                                   room.events[-3:]))
    # (5) the host's failed drops only the host.
    reset()
    with game_max(4, seats=4), arena():
        code, room, toks, match = _running(3)
        st, f = S.do_failed({"v": 5, "room": code, "token": toks[0],
                             "match": match, "reason": "user_cancelled"},
                            "10.0.0.1", 1005.0)
        ev = room.events[-1]
        check("an arena host's failed drops only the host",
              st == 200 and room.state == "in_match"
              and (ev["kind"], ev["seat"], ev["reason"])
              == ("dropped", 1, "user_cancelled")
              and room.members[1].playing is False, (room.state, ev))
        check("and the host keeps the room's host role", room.host_seat == 1)
        st, r = _result(code, toks[0], match)
        check("the dropped host cannot post a result (403 not_playing)",
              st == 403 and r["error"] == "not_playing", (st, r))
        _result(code, toks[1], match, who="10.0.0.2")
        st, r = _result(code, toks[2], match, who="10.0.0.3")
        check("the two left playing end it by a majority",
              room.state == "lobby" and room.events[-1]["kind"] == "result",
              room.events[-1])
        check("and the host is playing again for the next match",
              room.members[1].playing is True)
    # (6) the SAME events in a lockstep match are unchanged.
    reset()
    with game_max(4, seats=4), arena(ids=()):
        code, room, toks, match = _running(3)
        S.do_leave({"v": 5, "room": code, "token": toks[0]}, "10.0.0.1", 1011.0)
        check("lockstep: the host leaving still ends the match (failed "
              "host_left)", room.state == "lobby"
              and [e["kind"] for e in room.events[-3:]]
              == ["left", "failed", "host"], room.events[-3:])
    reset()
    with game_max(4, seats=4), arena(ids=()):
        code, room, toks, match = _running(3)
        S.do_failed({"v": 5, "room": code, "token": toks[0], "match": match,
                     "reason": "user_cancelled"}, "10.0.0.1", 1005.0)
        check("lockstep: the host's failed still ends the match",
              room.state == "lobby" and room.events[-1]["kind"] == "failed",
              room.events[-1])


# ------------------------------------------------ v6: party and coop rooms


def create6(nick="tango", who="10.0.0.1", now=1000.0, **extra):
    body = {"v": 6, "nick": nick, "game": GAME}
    body.update(extra)
    return S.do_create(body, who, now)


def join6(room, nick, who="10.0.0.2", now=1000.0, **extra):
    body = {"v": 6, "room": room, "nick": nick, "game": GAME}
    body.update(extra)
    return S.do_join(body, who, now)


def start6(code, token, who="10.0.0.1", now=1000.0, **extra):
    body = {"v": 6, "room": code, "token": token}
    body.update(extra)
    return S.do_start(body, who, now)


def poll6(code, token, cursor, who="10.0.0.1", now=1000.0):
    return S.do_poll({"v": 6, "room": code, "token": token, "cursor": cursor,
                      "wait": 0}, who, now)


def leave6(code, token, who="10.0.0.1", now=1000.0):
    return S.do_leave({"v": 6, "room": code, "token": token}, who, now)


def _no_path_anywhere(obj):
    """No key anywhere in an answer looks like a file path or a save path."""
    text = json.dumps(obj)
    return not any(w in text.lower() for w in
                   ("save_path", "coopsav", "\\\\", "c:/", "/home/"))


def test_party_and_coop_rooms():
    """Contract v6 (run mp1 FIELD5, lane ROOMS1): party and coop rooms.
    Hub rooms like an adventure room (one hub code for the room's life, a plan
    at create/join and on every poll) with a host Start on top."""
    print("\n-- v6 party and coop rooms: types, caps, plan, host Start")
    check("the lobby speaks v5..v7", (S.CONTRACT_MIN, S.CONTRACT_V) == (5, 7))
    check("party and coop are session types",
          "party" in S.SESSION_TYPES and "coop" in S.SESSION_TYPES)
    check("both hold four seats by default",
          S.PARTY_SEATS == 4 and S.COOP_SEATS == 4)

    for kind in ("party", "coop"):
        reset()
        extra = {"save_name": "Tango file"} if kind == "coop" else {}
        st, host = create6(nick="host", session_type=kind, **extra)
        check("%s: a v6 create makes a %s room" % (kind, kind),
              st == 200 and S.ROOMS[host["room"]].session_type == kind,
              (st, host))
        code, token = host["room"], host["token"]
        room = S.ROOMS[code]
        plan = host.get("plan", {})
        check("%s: the create answer carries the hub plan at once" % kind,
              plan.get("playing") is True and plan.get("session_type") == kind
              and plan.get("code") == room.comms_code
              and plan.get("relay") == S.RELAY_ADDR
              and plan.get("relay_v") == 2 and plan.get("want_seat") == 255,
              plan)
        check("%s: the plan is not started, launch 0, and this seat is the host"
              % kind, plan.get("started") is False and plan.get("launch") == 0
              and plan.get("host") is True and plan.get("room_seats") == 4,
              plan)
        check("%s: the hub code is 8 room-alphabet characters, not the room "
              "code" % kind, len(plan["code"]) == 8 and plan["code"] != code
              and all(ch in S.ROOM_ALPHABET for ch in plan["code"]))
        check("%s: the plan names no VS field" % kind,
              not any(k in plan for k in ("map", "players", "names", "slot",
                                           "luigi", "seed", "digest", "role")),
              plan)
        check("%s: the view says kind, not started, 4 seats" % kind,
              host["view"]["session_type"] == kind
              and host["view"]["started"] is False
              and host["view"]["launch"] == 0
              and host["view"]["room_seats"] == 4, host["view"])
        check("%s: the view has no VS arena fields" % kind,
              "netmode" not in host["view"] and "arena_ok" not in host["view"])
        check("%s: no path-like key anywhere in the answer" % kind,
              _no_path_anywhere(host), host)
        if kind == "coop":
            check("coop: the save's display name rides plan and view",
                  plan["save_name"] == "Tango file"
                  and host["view"]["save_name"] == "Tango file")
        else:
            check("party: no save name anywhere",
                  "save_name" not in plan and "save_name" not in host["view"])

        # ---- fill the room: four seats, the fifth is room_full ----------
        guests = []
        for k in range(3):
            st, g = join6(code, "g%d" % k, who="10.0.1.%d" % k)
            guests.append(g)
            check("%s: guest %d joins (seat %d)" % (kind, k, g.get("member", 0)),
                  st == 200 and g["member"] == k + 2, (st, g))
        check("%s: every joiner gets the SAME hub code" % kind,
              all(g["plan"]["code"] == plan["code"] for g in guests))
        check("%s: and none of them is the host" % kind,
              all(g["plan"]["host"] is False for g in guests))
        check("%s: everybody is playing (no dial in these rooms)" % kind,
              all(m.playing for m in room.members.values()))
        st, out = join6(code, "fifth", who="10.0.1.9")
        check("%s: the fifth seat is 409 room_full" % kind,
              st == 409 and out["error"] == "room_full", (st, out))
        check("%s: and seated nobody" % kind, len(room.members) == 4)

        # ---- the host's Play --------------------------------------------
        st, out = start6(code, guests[0]["token"], who="10.0.1.0")
        check("%s: a guest's start is 403 not_host" % kind,
              st == 403 and out["error"] == "not_host", (st, out))
        check("%s: and nothing started" % kind,
              room.started is False and room.launch_no == 0)
        cur = room.seq
        st, out = start6(code, token)
        check("%s: the host's start answers launch 1" % kind,
              st == 200 and out["launch"] == 1, (st, out))
        check("%s: the room is started and still in lobby state" % kind,
              room.started is True and room.state == "lobby" and room.launch_no == 1)
        check("%s: a started event reaches the ring" % kind,
              room.events[-1]["kind"] == "started"
              and room.events[-1]["launch"] == 1, room.events[-1])
        for k, g in enumerate(guests):
            st, out = poll6(code, g["token"], cur, who="10.0.1.%d" % k)
            check("%s: guest %d's poll plan says started, launch 1" % (kind, k),
                  out["plan"]["started"] is True and out["plan"]["launch"] == 1
                  and out["plan"]["host"] is False, out.get("plan"))
            check("%s: and the poll carries the started event" % kind,
                  any(e["kind"] == "started" for e in out["events"]))

        # ---- a later joiner is handed a started plan --------------------
        S.do_leave({"v": 6, "room": code, "token": guests[2]["token"]},
                   "10.0.1.2", 1001.0)
        st, late = join6(code, "late", who="10.0.7.7", now=1002.0)
        check("%s: a joiner after the Play is handed a STARTED plan (it "
              "launches on joining)" % kind,
              st == 200 and late["plan"]["started"] is True
              and late["plan"]["launch"] == 1, (st, late))
        # A second Play relaunches everybody.
        st, out = start6(code, token, now=1003.0)
        check("%s: a second start is launch 2" % kind,
              st == 200 and out["launch"] == 2 and room.launch_no == 2, (st, out))

        # ---- no VS machinery answers ------------------------------------
        st, out = _params(code, token, now=1004.0)
        check("%s: params is 409 not_vs (no match settings)" % kind,
              st == 409 and out["error"] == "not_vs", (st, out))
        st, out = S.do_ready({"v": 6, "room": code, "token": token,
                              "match": "0" * 16}, "10.0.0.1", 1004.0)
        check("%s: a forged ready is stale_match and changes nothing" % kind,
              st == 409 and out["error"] == "stale_match"
              and room.state == "lobby", (st, out))
        st, out = S.do_result({"v": 6, "room": code, "token": token,
                               "match": "0" * 16, "win": "draw",
                               "scores": [0, 0, 0, 0]}, "10.0.0.1", 1004.0)
        check("%s: a forged result is stale_match and posts nothing" % kind,
              st == 409 and out["error"] == "stale_match"
              and room.events[-1]["kind"] != "result", (st, out))
        st, out = S.do_failed({"v": 6, "room": code, "token": token,
                               "match": "0" * 16, "reason": "user_cancelled"},
                              "10.0.0.1", 1004.0)
        check("%s: a forged failed is stale_match and ends nothing" % kind,
              st == 409 and out["error"] == "stale_match"
              and room.state == "lobby", (st, out))
        st, out = start6(code, token, now=1005.0, match="x")
        check("%s: start with an unknown field is bad_field" % kind,
              st == 400 and out["error"] == "bad_field", (st, out))

        # ---- chat works like an adventure room's ------------------------
        st, out = S.do_chat({"v": 6, "room": code, "token": token, "text": "hi"},
                            "10.0.0.1", 1006.0)
        check("%s: chat works" % kind, st == 200
              and room.events[-1]["kind"] == "chat"
              and room.events[-1]["text"] == "hi", (st, out))
        check("%s: nothing in any plan or view is a path" % kind,
              _no_path_anywhere([room.view(1), S.member_plan(room, 1),
                                 room.events]))

        # ---- the host leaving -------------------------------------------
        if kind == "party":
            seats = sorted(room.members)
            st, out = leave6(code, token, now=1007.0)
            check("party: the host leaves and the room stays open",
                  st == 200 and room.state == "lobby")
            check("party: the host role passes to the lowest seat in",
                  room.host_seat == min(room.members), room.host_seat)
            check("party: the new host's plan says host, the started room "
                  "keeps its launch number",
                  S.member_plan(room, room.host_seat)["host"] is True
                  and room.launch_no == 2 and room.started)
            new_host_tok = room.members[room.host_seat].token
            st, out = start6(code, new_host_tok, who="10.0.9.9", now=1008.0)
            check("party: the new host can press Play (launch 3)",
                  st == 200 and out["launch"] == 3, (st, out))
        else:
            cur = room.seq
            st, out = leave6(code, token, now=1007.0)
            check("coop: the host leaves", st == 200)
            check("coop: the room is CLOSED, why host_left",
                  room.state == "closed" and room.why == "host_left",
                  (room.state, room.why))
            st, out = poll6(code, guests[0]["token"], cur, who="10.0.1.0",
                            now=1008.0)
            check("coop: a guest's next poll says closed with the reason",
                  st == 200 and out["view"]["state"] == "closed"
                  and any(e["kind"] == "closed" and e["why"] == "host_left"
                          for e in out["events"]), (st, out))
            st, out = join6(code, "toolate", who="10.0.8.8", now=1009.0)
            check("coop: and nobody can join it any more",
                  st == 404 and out["error"] == "no_such_room", (st, out))

    # ---- a party room: empty, then closed by the reaper -----------------
    reset()
    st, host = create6(nick="host", session_type="party")
    room = S.ROOMS[host["room"]]
    leave6(host["room"], host["token"], now=1001.0)
    S.sweep(1002.0)
    S.sweep(1002.0 + S.ADV_EMPTY_S + 1)
    check("party: an empty room closes after ADV_EMPTY_S like an adventure room",
          room.state == "closed" and room.why == "idle", (room.state, room.why))

    # ---- a party room is exempt from the idle timer while polled --------
    reset()
    st, host = create6(nick="host", session_type="party")
    room = S.ROOMS[host["room"]]
    t = 1000.0
    while t < 1000.0 + S.ROOM_IDLE_S + 120:
        t += 20.0
        poll6(host["room"], host["token"], room.seq, now=t)
        S.sweep(t)
    check("party: a room whose member keeps polling outlives ROOM_IDLE_S",
          room.state == "lobby", room.state)
    # A member who goes silent is dropped like anywhere else; a coop host that
    # goes silent ends the room.
    reset()
    st, host = create6(nick="host", session_type="coop", save_name="x")
    st, g = join6(host["room"], "g")
    room = S.ROOMS[host["room"]]
    poll6(host["room"], g["token"], room.seq, who="10.0.0.2", now=1030.0)
    S.sweep(1000.0 + S.MEMBER_TIMEOUT_S + 5)
    check("coop: a host whose launcher goes silent ends the room (host_left)",
          room.state == "closed" and room.why == "host_left",
          (room.state, room.why))

    # ---- the other kinds are exactly as they were ----------------------
    reset()
    st, vs = create6(nick="v")
    check("a v6 VS create is a VS room, with no v6 view fields and no plan",
          st == 200 and "started" not in vs["view"] and "plan" not in vs
          and "room_seats" not in vs["view"], vs)
    st, adv = create6(nick="a", session_type="adventure")
    check("a v6 adventure create has the v5 plan and no v6 plan fields",
          set(adv["plan"]) == {"playing", "session_type", "code", "relay",
                               "relay_v", "want_seat"}, adv["plan"])
    check("and no v6 view fields", "started" not in adv["view"])
    st, out = start6(adv["room"], adv["token"])
    check("start in an adventure room is still 409 not_vs",
          st == 409 and out["error"] == "not_vs", (st, out))


def test_v5_clients_and_party_coop():
    """A v5 launcher keeps VS and world, and cannot create or join the new
    kinds: it is refused with bad_session_type, which it words as "Check for
    an update"."""
    print("\n-- v5 clients: VS and world as before, party and coop refused")
    for kind in ("party", "coop"):
        reset()
        extra = {"save_name": "f"} if kind == "coop" else {}
        st, out = create6(nick="h", session_type=kind, **extra)
        code = out["room"]
        st, out = join(code, "old")          # a v5 join
        check("%s: a v5 join is bad_session_type" % kind,
              st == 400 and out["error"] == "bad_session_type", (st, out))
        check("%s: and seated nobody" % kind, len(S.ROOMS[code].members) == 1)
        st, out = create(nick="old", session_type=kind)
        check("%s: a v5 create is bad_session_type" % kind,
              st == 400 and out["error"] == "bad_session_type", (st, out))
        st, out = create(nick="old", session_type=kind, save_name="f")
        check("%s: a v5 create with save_name is bad_field (v6 field)" % kind,
              st == 400 and out["error"] == "bad_field", (st, out))
        # A v5 poll / chat / leave by a seat that is already in is fine: the
        # caps and plan fields are additive.
    reset()
    st, vs = create(nick="host")
    st, g = join(vs["room"], "friend")
    check("a v5 VS room is untouched: create, join", st == 200 and g["member"] == 2)
    # A v5 client may still create and join an adventure room.
    reset()
    st, adv = create(nick="a", session_type="adventure")
    st, g = join(adv["room"], "b")
    check("a v5 adventure room still creates and joins",
          st == 200 and g["plan"]["session_type"] == "adventure", (st, g))


# ------------------------------------------------ v6: room settings (ROOMSET1)


DEFAULT_PARTY = {"collision": 1, "interact": 1, "damage": 0, "starshare": 1,
                 "soloexit": 1}
DEFAULT_COOP = {"collision": 1, "interact": 1, "damage": 0, "tp": 1,
                "soloexit": 1}


def settings6(code, token, settings, who="10.0.0.1", now=1000.0):
    return S.do_settings({"v": 6, "room": code, "token": token,
                          "settings": settings}, who, now)


def test_room_settings():
    """Contract v6 (ROOMSET1): the host's toggles on a party or coop room.

    Defaults: collision on, interactions on, friendly damage off, TP to friends
    on (coop), star sharing on (party). The toggles ride the create answer, the
    view and every plan, so each launcher can turn them into SM64DS_ROOM_*.
    """
    print("\n-- v6 room settings: defaults, toggles, refusals, host update")
    check("the toggle names per kind are the contract's",
          S.ROOM_SETTINGS == {
              "party": ("collision", "interact", "damage", "starshare",
                        "soloexit"),
              "coop": ("collision", "interact", "damage", "tp", "soloexit")},
          S.ROOM_SETTINGS)

    # ---- defaults: no settings field means the defaults, for each kind -----
    for kind, want in (("party", DEFAULT_PARTY), ("coop", DEFAULT_COOP)):
        reset()
        extra = {"save_name": "f"} if kind == "coop" else {}
        st, host = create6(nick="h", session_type=kind, **extra)
        check("%s: a create with no settings is 200" % kind, st == 200,
              (st, host))
        code, token = host["room"], host["token"]
        check("%s: the defaults are %r" % (kind, want),
              host["plan"]["settings"] == want
              and S.ROOMS[code].settings == want, host["plan"])
        check("%s: the view carries the same settings" % kind,
              host["view"]["settings"] == want, host["view"])
        st, g = join6(code, "g", who="10.0.0.2")
        check("%s: a joiner's plan carries the room's settings" % kind,
              st == 200 and g["plan"]["settings"] == want, (st, g))
        st, p = poll6(code, token, 0)
        check("%s: every poll's plan carries them too" % kind,
              st == 200 and p["plan"]["settings"] == want, (st, p))
        check("%s: and no path-like key anywhere in the answers" % kind,
              _no_path_anywhere(host) and _no_path_anywhere(g))

    # ---- an empty object is the defaults too -------------------------------
    reset()
    st, host = create6(nick="h", session_type="party", settings={})
    check("party: settings {} is the defaults", st == 200
          and host["plan"]["settings"] == DEFAULT_PARTY, host)

    # ---- each toggle, set alone to 0 (and back to 1), reaches the plan ------
    for kind, names in (("party", DEFAULT_PARTY), ("coop", DEFAULT_COOP)):
        for name in names:
            for val in (0, 1):
                reset()
                extra = {"save_name": "f"} if kind == "coop" else {}
                want = dict(names)
                want[name] = val
                st, host = create6(nick="h", session_type=kind,
                                   settings={name: val}, **extra)
                check("%s: %s=%d is accepted" % (kind, name, val), st == 200,
                      (st, host))
                check("%s: %s=%d reaches the create plan, the view and the "
                      "room" % (kind, name, val),
                      st == 200 and host["plan"]["settings"] == want
                      and host["view"]["settings"] == want
                      and S.ROOMS[host["room"]].settings == want, host)
            # The whole set, all at once.
            reset()
            extra = {"save_name": "f"} if kind == "coop" else {}
            allzero = {k: 0 for k in names}
            st, host = create6(nick="h", session_type=kind, settings=allzero,
                               **extra)
            check("%s: every toggle at 0 is accepted" % kind,
                  st == 200 and host["plan"]["settings"] == allzero, host)

    # ---- a joiner's plan reflects the host's toggles -----------------------
    reset()
    st, host = create6(nick="h", session_type="party",
                       settings={"damage": 1, "collision": 0})
    st, g = join6(host["room"], "g", who="10.0.0.2")
    check("party: a joiner is handed the host's damage=1 collision=0",
          st == 200 and g["plan"]["settings"] == {
              "collision": 0, "interact": 1, "damage": 1, "starshare": 1,
              "soloexit": 1},
          (st, g))

    # ---- refusals: the wrong kind of room, the wrong names, the wrong types -
    reset()
    st, out = create6(nick="h", session_type="party", settings={"tp": 1})
    check("party: tp is not a party toggle (bad_setting_key)",
          st == 400 and out["error"] == "bad_setting_key", (st, out))
    st, out = create6(nick="h", session_type="coop", save_name="f",
                      settings={"starshare": 1})
    check("coop: starshare is not a coop toggle (bad_setting_key)",
          st == 400 and out["error"] == "bad_setting_key", (st, out))
    for bad in ("bogus", "COLLISION", "", "SM64DS_ROOM_COLLISION", "collision "):
        st, out = create6(nick="h", session_type="party",
                          settings={bad: 1})
        check("party: unknown toggle %r is bad_setting_key" % bad,
              st == 400 and out["error"] == "bad_setting_key", (st, out))
    for bad in ("1", "0", 2, -1, True, False, 1.0, 0.0, None, [1], {"x": 1}):
        st, out = create6(nick="h", session_type="party",
                          settings={"collision": bad})
        check("party: collision=%r is bad_setting_value" % (bad,),
              st == 400 and out["error"] == "bad_setting_value", (st, out))
    for bad in ([], [("collision", 1)], "collision=1", 1, None, True):
        st, out = create6(nick="h", session_type="party", settings=bad)
        check("party: settings %r (not an object) is bad_settings" % (bad,),
              st == 400 and out["error"] == "bad_settings", (st, out))
    check("every refused create made no room", not S.ROOMS, list(S.ROOMS))

    # ---- VS, adventure: settings are a field those rooms do not have -------
    for kind in ("vs", "adventure"):
        reset()
        st, out = create6(nick="h", session_type=kind, settings={})
        check("%s: a create with settings is bad_field" % kind,
              st == 400 and out["error"] == "bad_field", (st, out))
        check("%s: and made no room" % kind, not S.ROOMS)
    reset()
    st, out = create6(nick="h", settings={})
    check("a create with no session_type (vs) and settings is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))

    # ---- v5 cannot send settings (the field is v6) -------------------------
    reset()
    st, out = create(nick="old", settings={})
    check("a v5 create with settings is bad_field (a v6 field)",
          st == 400 and out["error"] == "bad_field", (st, out))

    # ---- the host's update: partial, host-only, before Start ---------------
    reset()
    st, host = create6(nick="h", session_type="party")
    code, token = host["room"], host["token"]
    st, g = join6(code, "g", who="10.0.0.2")
    gtoken = g["token"]
    st, out = settings6(code, gtoken, {"damage": 1}, who="10.0.0.2")
    check("a guest cannot change the settings (403 not_host)",
          st == 403 and out["error"] == "not_host", (st, out))
    check("and the guest's refusal changed nothing",
          S.ROOMS[code].settings == DEFAULT_PARTY)
    before = S.ROOMS[code].seq
    st, out = settings6(code, token, {"damage": 1})
    check("the host's partial update is 200", st == 200
          and out["settings"] == dict(DEFAULT_PARTY, damage=1), (st, out))
    check("a partial update keeps the other toggles",
          S.ROOMS[code].settings == dict(DEFAULT_PARTY, damage=1),
          S.ROOMS[code].settings)
    st, p = poll6(code, gtoken, 0, who="10.0.0.2")
    check("the guest's next plan carries the host's update",
          st == 200 and p["plan"]["settings"] == dict(DEFAULT_PARTY, damage=1),
          (st, p))
    st, p = poll6(code, token, before)
    check("the update is a settings event every member can poll for",
          any(e.get("kind") == "settings"
              and e.get("settings") == dict(DEFAULT_PARTY, damage=1)
              for e in p.get("events", [])), p)
    st, out = settings6(code, token, {"damage": 2})
    check("an update with a bad value is refused and changes nothing",
          st == 400 and out["error"] == "bad_setting_value"
          and S.ROOMS[code].settings == dict(DEFAULT_PARTY, damage=1),
          (st, out))
    st, out = settings6(code, token, {"tp": 1})
    check("an update with a name of the other kind is refused",
          st == 400 and out["error"] == "bad_setting_key", (st, out))
    st, out = settings6(code, token, {})
    check("an empty update is 200 and changes nothing",
          st == 200 and S.ROOMS[code].settings == dict(DEFAULT_PARTY, damage=1),
          (st, out))

    # ---- after Start the room's toggles are fixed --------------------------
    st, out = start6(code, token)
    check("the host's Start is 200", st == 200, (st, out))
    st, out = settings6(code, token, {"damage": 0})
    check("after Start an update is 409 already_started",
          st == 409 and out["error"] == "already_started", (st, out))
    check("and the started room's settings did not move",
          S.ROOMS[code].settings == dict(DEFAULT_PARTY, damage=1))

    # ---- a VS room has no room settings to change --------------------------
    reset()
    st, vs = create(nick="h")
    st, out = settings6(vs["room"], vs["token"], {"damage": 1})
    check("a VS room's settings update is 409 no_room_settings",
          st == 409 and out["error"] == "no_room_settings", (st, out))

    # ---- a coop room's update reaches a joiner's plan the same way --------
    reset()
    st, host = create6(nick="h", session_type="coop", save_name="f")
    st, out = settings6(host["room"], host["token"], {"tp": 0})
    check("coop: the host sets tp=0", st == 200
          and out["settings"] == dict(DEFAULT_COOP, tp=0), (st, out))
    st, g = join6(host["room"], "g", who="10.0.0.2")
    check("coop: a late joiner's plan says tp=0",
          st == 200 and g["plan"]["settings"] == dict(DEFAULT_COOP, tp=0),
          (st, g))

    # ---- unknown verb fields are still refused on the new verb -------------
    reset()
    st, host = create6(nick="h", session_type="party")
    st, out = S.do_settings({"v": 6, "room": host["room"],
                             "token": host["token"], "settings": {},
                             "save_path": "C:\\x\\y.coopsav"}, "10.0.0.1",
                            1000.0)
    check("settings with a path-like key is bad_field",
          st == 400 and out["error"] == "bad_field", (st, out))
    st, out = S.do_settings({"v": 5, "room": host["room"],
                             "token": host["token"], "settings": {}},
                            "10.0.0.1", 1000.0)
    check("a v5 settings verb is bad_field (the verb is v6, like color at v1)",
          st == 400 and out["error"] == "bad_field", (st, out))


def test_room_soloexit():
    """STAREXIT1: soloexit, only the collector leaves the level (party and coop).

    On by default, on for a client that never sends it (an older launcher),
    0 is kept, the host can flip it before Start, and a room whose stored
    settings predate the name still answers soloexit 1.
    """
    print("\n-- v6 room settings: soloexit (only the collector leaves)")
    for kind in ("party", "coop"):
        extra = {"save_name": "f"} if kind == "coop" else {}
        reset()
        old = {"collision": 1, "interact": 1, "damage": 0}
        old.update({"starshare": 1} if kind == "party" else {"tp": 1})
        st, host = create6(nick="h", session_type=kind, settings=old, **extra)
        check("%s: an older launcher's create (no soloexit) gets soloexit 1"
              % kind, st == 200 and host["plan"]["settings"]["soloexit"] == 1
              and S.ROOMS[host["room"]].settings["soloexit"] == 1, host)
        st, g = join6(host["room"], "g", who="10.0.0.2")
        check("%s: its joiner's plan says soloexit 1" % kind,
              st == 200 and g["plan"]["settings"]["soloexit"] == 1, (st, g))
        st, out = settings6(host["room"], host["token"], {"soloexit": 0})
        check("%s: the host turns soloexit off before Start" % kind,
              st == 200 and out["settings"]["soloexit"] == 0
              and S.ROOMS[host["room"]].settings["soloexit"] == 0, (st, out))
        st, p = poll6(host["room"], host["token"], 0)
        check("%s: the poll's plan carries soloexit 0" % kind,
              st == 200 and p["plan"]["settings"]["soloexit"] == 0, (st, p))
        reset()
        st, host = create6(nick="h", session_type=kind,
                           settings={"soloexit": 0}, **extra)
        check("%s: soloexit 0 at create is kept" % kind,
              st == 200 and host["plan"]["settings"]["soloexit"] == 0, host)
        for bad in (2, "0", True, None):
            st, out = create6(nick="h", session_type=kind,
                              settings={"soloexit": bad}, **extra)
            check("%s: soloexit=%r is bad_setting_value" % (kind, bad),
                  st == 400 and out["error"] == "bad_setting_value", (st, out))
    # A room stored before the name existed: an update fills it in.
    out, err = S.settings_for("coop", {"damage": 1},
                              base={"collision": 1, "interact": 1,
                                    "damage": 0, "tp": 1})
    check("an update of settings stored without soloexit answers soloexit 1",
          err is None and out == dict(DEFAULT_COOP, damage=1), (out, err))


def test_party_coop_malformed_input():
    """Security law: every new field bounds-checked, names opaque and capped,
    no path ever stored, a forged message grants nothing."""
    print("\n-- party and coop: malformed and hostile input")
    bad_names = [
        ("", "bad_save_name"), ("   ", "bad_save_name"),
        (" lead", "bad_save_name"), ("trail ", "bad_save_name"),
        ("a" * 25, "too_long"), ("a" * 4000, "too_long"),
        ("C:\\Users\\bob\\x.coopsav", "bad_save_name"),
        ("save\\file", "bad_save_name"), ("a/b", "bad_save_name"),
        ("../../etc", "bad_save_name"), ("what?", "bad_save_name"),
        ("star*", "bad_save_name"), ('quo"te', "bad_save_name"),
        ("a<b", "bad_save_name"), ("a>b", "bad_save_name"),
        ("a|b", "bad_save_name"), ("tab\there", "bad_save_name"),
        ("nl\nhere", "bad_save_name"), ("nul\x00x", "bad_save_name"),
        ("del\x7fx", "bad_save_name"), ("caf\u00e9", "bad_save_name"),
        ("\ud800", "bad_save_name"), (5, "bad_save_name"),
        (None, "bad_save_name"), (["a"], "bad_save_name"),
        ({"a": 1}, "bad_save_name"), (True, "bad_save_name"),
    ]
    for val, want in bad_names:
        reset()
        st, out = create6(nick="h", session_type="coop", save_name=val)
        check("coop save_name %r is %s" % (str(val)[:20], want),
              st == 400 and out["error"] == want and not S.ROOMS, (st, out))
    for val in ("a", "My file", "a" * 24, "Tango's file", "co-op #2", "a,b"):
        reset()
        st, out = create6(nick="h", session_type="coop", save_name=val)
        check("coop save_name %r is a legal display name" % val,
              st == 200 and out["view"]["save_name"] == val, (st, out))
    reset()
    st, out = create6(nick="h", session_type="coop")
    check("a coop create with no save_name is bad_save_name",
          st == 400 and out["error"] == "bad_save_name", (st, out))
    for kind in ("party", "vs", "adventure"):
        reset()
        st, out = create6(nick="h", session_type=kind, save_name="f")
        check("%s with a save_name is bad_field" % kind,
              st == 400 and out["error"] == "bad_field", (st, out))
    # Fields the server must never take: a path under any plausible key.
    for key in ("save_path", "path", "coop_save", "save", "file", "host",
                "started", "launch", "room_kind", "party", "coop"):
        reset()
        st, out = create6(nick="h", session_type="coop", save_name="f",
                          **{key: "C:\\x\\y.coopsav"})
        check("coop create with an unknown key %r is bad_field" % key,
              st == 400 and out["error"] == "bad_field" and not S.ROOMS,
              (st, out))
    # A join never takes a save name or a started flag.
    reset()
    st, h = create6(nick="h", session_type="coop", save_name="f")
    for key in ("save_name", "save_path", "started", "host"):
        st, out = join6(h["room"], "j", **{key: "x"})
        check("a join carrying %r is bad_field" % key,
              st == 400 and out["error"] == "bad_field", (st, out))
    # start takes nothing but v/room/token; a guest cannot start by forging.
    st, g = join6(h["room"], "j", who="10.0.0.9")
    for extra in ({"launch": 5}, {"started": True}, {"host": True},
                  {"seat": 1}):
        st, out = start6(h["room"], g["token"], who="10.0.0.9", **extra)
        check("a guest's start with %r is bad_field (shape before auth)"
              % list(extra), st == 400 and out["error"] == "bad_field",
              (st, out))
    st, out = start6(h["room"], "0" * 32, who="10.0.0.9")
    check("a start with an unseated token is refused (403)",
          st == 403, (st, out))
    st, out = start6("ABCDEF", h["token"], who="10.0.0.9")
    check("a start in a room that does not exist is 404",
          st == 404 and out["error"] == "no_such_room", (st, out))
    check("the room never started through any of that",
          S.ROOMS[h["room"]].started is False)
    # Oversize / odd names reach party and coop rooms through the same nick rule.
    st, out = join6(h["room"], "n" * 17, who="10.0.0.10")
    check("a 17 byte nick into a coop room is too_long",
          st == 400 and out["error"] == "too_long", (st, out))
    st, out = join6(h["room"], "a,b", who="10.0.0.10")
    check("a comma nick into a coop room is bad_nick",
          st == 400 and out["error"] == "bad_nick", (st, out))
    # A host with a stale token and kicked guests: the kick is the host's.
    st, out = S.do_kick({"v": 6, "room": h["room"], "token": g["token"],
                         "seat": 1}, "10.0.0.9", 1001.0)
    check("a guest cannot kick the host of a coop room (403 not_host)",
          st == 403 and out["error"] == "not_host", (st, out))
    # The seat caps are knobs, clamped.
    was = (S.PARTY_SEATS, S.COOP_SEATS)
    try:
        S.PARTY_SEATS = 2
        reset()
        st, h2 = create6(nick="h", session_type="party")
        st, g2 = join6(h2["room"], "g")
        st, out = join6(h2["room"], "x", who="10.0.0.5")
        check("PARTY_SEATS=2 makes a two-seat party room (room_full on the "
              "third)", st == 409 and out["error"] == "room_full"
              and h2["view"]["room_seats"] == 2, (st, out))
    finally:
        S.PARTY_SEATS, S.COOP_SEATS = was
        reset()
    check("the env knobs are clamped to 2..16",
          S.env_int("PARTY_SEATS_TEST_NOPE", 4, 2, S.DIAL_HARD_MAX) == 4)


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
    test_the_v5_floor()
    test_wide_rooms_need_no_version_gate()
    test_a_dropout_after_go_does_not_end_the_match_for_the_rest()
    test_result_accepts_a_wide_scoreline()
    test_name_and_colour_shapes_move_together()
    test_every_versioned_field_is_gated()
    test_dial_moves_the_fewest_people()
    test_dial_and_seat_reuse()
    test_dial_reaches_the_plan()
    test_seat_stability()
    test_start()
    test_ready_and_go()
    test_session_type()
    test_adventure_room()
    test_game_allow_list()
    test_world_address_cap()
    test_world_pool()
    test_go_plan()
    test_session_digest()
    test_vs_one_settings()
    test_king_of_the_star()
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
    test_a_dropped_player_plays_the_next_match()
    test_no_promotion_into_a_running_match()
    test_the_plan_guard()
    test_join_while_a_match_runs_is_a_waiting_seat()
    test_waiting_join_floods_hit_the_caps()
    test_the_host_leaving_passes_the_host_on()
    test_a_quitters_result_does_not_end_the_match()
    test_the_new_host_played_the_match()
    test_every_guest_agrees_and_the_host_is_silent()
    test_a_throttled_host_is_not_silent()
    test_host_silent_floor()
    test_server_line_width()
    test_arena_digest()
    test_arena_off_by_default()
    test_arena_plan()
    test_arena_when()
    test_arena_luigi()
    test_arena_params()
    test_arena_has_no_host_in_the_match()
    test_party_and_coop_rooms()
    test_v5_clients_and_party_coop()
    test_room_settings()
    test_room_soloexit()
    test_party_coop_malformed_input()
    test_ban()

    print("\n%d passed, %d failed" % (len(PASSED), len(FAILED)))
    for name in FAILED:
        print("  FAILED: %s" % name)
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
