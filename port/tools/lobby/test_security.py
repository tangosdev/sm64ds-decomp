#!/usr/bin/env python3
"""Security regressions for sm64ds-lobby, over raw sockets. Stdlib only.

    python3 test_security.py

Two holes an independent review found at service tip 7a5c8ffb7, each re-run here
as a guard so neither can come back:

  BLOCKER 1  a slow body could pin a worker thread forever, and the rate
             limiter could not even deliver its own 429 because the refusal
             drained the body it was refusing first.
  BLOCKER 2  X-Forwarded-For was read from its FIRST entry, the one entry a
             client controls, so every per-address limit was forgeable.

Plus the honest-cap regression: after the fix, the per-address limit still
engages for a real client.

RAW SOCKETS THROUGHOUT. The whole point of these cases is a request that a
well-behaved client library would never send -- a body announced and then not
sent, a forged proxy header -- so nothing here goes through a convenience
wrapper that would fix it on the way out.

XFF IS HOW ONE MACHINE PRETENDS TO BE MANY. The server trusts X-Forwarded-For
(TRUST_XFF=1, the production default behind Caddy), so a single entry in that
header is read as the client address. That lets this test stand in for several
distinct clients, and it lets it stand in for Caddy by appending a constant
"real peer" entry after a forged one -- which is exactly the shape the fix
turns on.
"""

import json
import socket
import sys
import time

from test_client import Spawned, Client, pick_port

PASSED = []
FAILED = []


def check(name, ok, detail=""):
    (PASSED if ok else FAILED).append(name)
    print("%-5s %s%s" % ("ok" if ok else "FAIL", name,
                         "" if ok else "   <- " + str(detail)))
    return ok


def raw_head(port, verb, announced, xff=None, ctype="application/json"):
    """Open a socket and send the request line and headers and the blank line,
    announcing `announced` body bytes but sending NONE of them yet. Returns the
    connected socket for the caller to dribble, or read, or abandon."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(10)
    s.connect(("127.0.0.1", port))
    lines = ["POST /port/lobby/%s HTTP/1.1" % verb,
             "Host: 127.0.0.1",
             "Content-Type: %s" % ctype,
             "Content-Length: %d" % announced]
    if xff is not None:
        lines.append("X-Forwarded-For: %s" % xff)
    head = ("\r\n".join(lines) + "\r\n\r\n").encode("ascii")
    s.sendall(head)
    return s


def read_reply(s, timeout):
    """Read whatever the server sends, until EOF or `timeout`. Returns the text,
    or "" if nothing arrived. A reset after a partial read still returns what
    was read."""
    s.settimeout(timeout)
    got = b""
    try:
        while True:
            chunk = s.recv(4096)
            if not chunk:
                break
            got += chunk
            if b"\r\n\r\n" in got:
                # headers are in; one more short read for the small body, then stop
                s.settimeout(0.4)
                try:
                    got += s.recv(4096)
                except OSError:
                    pass
                break
    except OSError:
        pass
    return got.decode("latin-1")


def status_of(reply):
    if not reply.startswith("HTTP/"):
        return 0
    try:
        return int(reply.split(" ", 2)[1])
    except (IndexError, ValueError):
        return 0


def quick_post(c, verb, obj, xff):
    """One POST with a chosen X-Forwarded-For, returning (status, parsed)."""
    status, text = c.raw("POST", "/port/lobby/" + verb, json.dumps(obj).encode(),
                         extra=(("X-Forwarded-For", xff),))
    try:
        return status, json.loads(text)
    except ValueError:
        return status, {"_raw": text}


# ---------------------------------------------------------- BLOCKER 1


def blocker1():
    print("\n== BLOCKER 1: a slow body must not pin a thread, and a refusal "
          "must not block on the body it is refusing ==")
    # A short read timeout so the test is quick; tiny budgets and a tiny
    # bad-sender threshold so one address can be pushed to `ignored` in a few
    # requests. Everything else is the shipped default.
    port = pick_port()
    sp = Spawned(port, {
        "HANDLER_TIMEOUT_S": "3",
        "RATE_REQ_PER_S": "1", "RATE_BURST": "2",
        "BAD_LIMIT": "3", "BAD_WINDOW_S": "10", "BAD_IGNORE_S": "10",
    })
    c = sp.wait_ready()
    try:
        # (a) THE REFUSAL PATH. Push one address past its budget, confirm it is
        #     now getting `ignored`, then open ONE slow-body connection from it
        #     and time how long its 429 takes to arrive. Before the fix this was
        #     "nothing after 6 s", because the 429 drained the body first.
        burned = "10.9.9.1"
        last = None
        for _ in range(12):
            last = quick_post(c, "create", {"v": 1, "nick": "x"}, burned)
        check("an address can be pushed to 429 ignored",
              last is not None and last[0] == 429
              and last[1].get("error") == "ignored", last)

        s = raw_head(port, "create", 4000, xff=burned)
        s.sendall(b'{"v":1,')          # a few body bytes, then nothing more
        t0 = time.time()
        reply = read_reply(s, timeout=6)
        dt = time.time() - t0
        s.close()
        check("the over-budget slow sender gets its 429 fast, WITHOUT the "
              "server draining the body first",
              status_of(reply) == 429 and dt < 2.0,
              "status=%s after %.2fs" % (status_of(reply), dt))

        # (b) THE THREAD. A valid, in-budget request that then dribbles its body
        #     must be DROPPED at the read timeout, not held forever.
        s = raw_head(port, "create", 4000, xff="10.9.9.2")
        s.sendall(b'{"v":1,"nick":"ab')   # partial, never completed
        t0 = time.time()
        reply = read_reply(s, timeout=8)   # server should close ~3 s in
        dt = time.time() - t0
        s.close()
        check("a valid request that dribbles its body is dropped at the read "
              "timeout, not held",
              dt < 7.0 and dt > 1.0,
              "connection closed after %.2fs (read_timeout=3s)" % dt)

        # (c) MANY of them at once, and the server still serves and then fully
        #     recovers. Locally there is no pid cap to hit -- that is the
        #     container's, and the reviewer hit it -- so what is proved here is
        #     the mechanism that stops it: held connections do not wedge the
        #     server, and they are all gone after the timeout.
        held = []
        for i in range(25):
            hs = raw_head(port, "create", 4000, xff="10.8.%d.%d" % (i, i))
            hs.sendall(b'{')
            held.append(hs)
        gt0 = time.time()
        hstatus, _ = c.get("health")
        gdt = time.time() - gt0
        check("health still answers promptly while 25 slow bodies are held",
              hstatus == 200 and gdt < 2.0, "status=%s in %.2fs" % (hstatus, gdt))
        # Let them time out and be reaped.
        time.sleep(4)
        for hs in held:
            try:
                hs.close()
            except OSError:
                pass
        st, body = c.post("create", {"v": 1, "nick": "after"})
        check("after the held connections time out the server serves normally",
              st == 200, (st, body))
    finally:
        sp.stop()


# ---------------------------------------------------------- BLOCKER 2


def blocker2():
    print("\n== BLOCKER 2: X-Forwarded-For must be read from the entry Caddy "
          "controls, not the entry the client controls ==")
    # A tiny join budget so the cap is quick to reach.
    port = pick_port()
    sp = Spawned(port, {"RATE_JOIN_PER_MIN": "5", "RATE_REQ_PER_S": "1000",
                        "RATE_BURST": "2000"})
    c = sp.wait_ready()
    try:
        REAL = "203.0.113.9"      # the peer Caddy would append, last in the trail

        # (a) THE FIX. 30 joins, each with a DIFFERENT forged first entry but the
        #     same real peer appended last, the way Caddy produces the header.
        #     The forged first entries must NOT buy 30 separate budgets.
        codes = {}
        for i in range(30):
            forged = "%d.%d.%d.%d" % (i + 1, i + 2, i + 3, i + 4)
            st, _ = quick_post(c, "join", {"v": 1, "room": "ZZZZZZ", "nick": "n"},
                               "%s, %s" % (forged, REAL))
            codes[st] = codes.get(st, 0) + 1
        check("forged FIRST entries do not bypass the join cap when the real "
              "peer is the LAST entry (behind Caddy)",
              codes.get(429, 0) > 0,
              "status counts across 30 forged joins: %s" % codes)
        check("and only about the budget got through before the cap engaged",
              codes.get(404, 0) <= 8,
              "%d joins reached the room lookup (budget is 5/min)"
              % codes.get(404, 0))

        # (b) THE REGRESSION. A single honest address is still capped -- the fix
        #     did not turn legitimate rate limiting off.
        time.sleep(1)
        honest = {}
        for _ in range(30):
            st, _ = quick_post(c, "join", {"v": 1, "room": "ZZZZZZ", "nick": "n"},
                               "198.51.100.7")
            honest[st] = honest.get(st, 0) + 1
        check("an honest single address is still rate-limited",
              honest.get(429, 0) > 0, honest)

        # (c) THE CAVEAT, measured rather than asserted. With NO proxy appending
        #     a real entry -- i.e. direct exposure, which the compose warns
        #     against -- a single forged entry IS the last entry, so it is still
        #     forgeable. This is why the compose ships TRUST_XFF=1 only because
        #     it is behind Caddy, and the README says set TRUST_XFF=0 for any
        #     deployment reachable directly. Recorded, not gated.
        time.sleep(1)
        direct = {}
        for i in range(30):
            forged = "%d.%d.%d.%d" % (100 + i, i, i, i)
            st, _ = quick_post(c, "join", {"v": 1, "room": "ZZZZZZ", "nick": "n"},
                               forged)   # single entry, no appended peer
            direct[st] = direct.get(st, 0) + 1
        print("info  direct-exposure control (no proxy appending a real peer): "
              "%s -- forgeable by design, which is why TRUST_XFF must be 0 off "
              "Caddy" % direct)
    finally:
        sp.stop()


def main():
    blocker1()
    blocker2()
    print("\n%d passed, %d failed" % (len(PASSED), len(FAILED)))
    for name in FAILED:
        print("  FAILED: %s" % name)
    return 1 if FAILED else 0


if __name__ == "__main__":
    sys.exit(main())
