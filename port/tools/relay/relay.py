#!/usr/bin/env python3
"""SM64DS port UDP relay.

Two players behind ordinary home routers cannot reach each other directly.
Both of them dial OUT to this relay instead, which needs no port forwarding
on either side. The relay pairs them by session code and forwards their
packets. It never looks inside a forwarded packet.

Wire contract (frozen, shared with the game side):

  HELLO      16 bytes  "SMRC" | version u8 = 1 | role u8 (0 parent, 1 child)
                       | reserved u16 | code 8B ASCII, right padded with NUL
  HELLO-ACK  16 bytes  "SMRA" | version u8 = 1 | status u8 (0 ok, 1 full,
                       2 bad request) | reserved u16 | code echoed 8B

Clients repeat HELLO about once a second until they get an ACK. That doubles
as the NAT keepalive. A repeat HELLO from an endpoint we already know is
idempotent: it is re-ACKed and the pairing is left alone.

A session is one code with one parent and up to three children. Once paired,
every datagram that is not a HELLO is forwarded verbatim: parent to all
children, child to the parent only. Payloads are opaque here, always.

Configuration (environment):

  SM64DS_RELAY_PORT     UDP port to bind          (default 41234)
  SM64DS_RELAY_BIND     address to bind           (default 0.0.0.0)
  SM64DS_RELAY_IDLE_S   per endpoint idle expiry  (default 90)
  SM64DS_RELAY_STATS_S  stats line interval       (default 60)

Python 3 standard library only. No dependencies, no state on disk.
"""

import asyncio
import os
import socket
import sys
import time

# ---------------------------------------------------------------- protocol

MAGIC_HELLO = b"SMRC"
MAGIC_ACK = b"SMRA"
PROTO_VERSION = 1
HELLO_LEN = 16

ROLE_PARENT = 0
ROLE_CHILD = 1

STATUS_OK = 0
STATUS_FULL = 1
STATUS_BAD = 2

# ------------------------------------------------------------------ limits

MAX_PAYLOAD = 700           # bytes; larger datagrams are dropped in silence
RATE_PPS = 120              # sustained packets per second, per endpoint
RATE_BURST = 120            # token bucket depth, same units
MAX_SESSIONS = 64
MAX_CHILDREN = 3
MAX_ENDPOINTS_PER_SESSION = 4
BAD_ACK_INTERVAL_S = 1.0    # at most one status 2 per second per endpoint
BAD_IGNORE_S = 10.0         # then that endpoint is ignored for this long
BUCKET_IDLE_S = 30.0        # forget a rate bucket after this much quiet
MAX_BUCKETS = 8192          # hard ceiling on tracked source addresses
SWEEP_INTERVAL_S = 1.0

DEFAULT_PORT = 41234
DEFAULT_BIND = "0.0.0.0"
DEFAULT_IDLE_S = 90.0
DEFAULT_STATS_S = 60.0

# ------------------------------------------------------------------- utils


def log(msg):
    """One line to stdout. systemd picks these up; journalctl shows them."""
    ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    print("%s %s" % (ts, msg), flush=True)


def short_addr(addr):
    """A logging form of a client address with the host part blunted.

    We keep enough to tell two clients apart in a log and to spot a region,
    and drop the rest. Full client addresses never reach the log.
    """
    try:
        host = addr[0]
        port = addr[1]
    except Exception:
        return "?"
    if ":" in host:
        groups = host.split(":")
        head = ":".join(g for g in groups[:2] if g)
        host = (head + "::x") if head else "::x"
    else:
        octets = host.split(".")
        if len(octets) == 4:
            host = "%s.%s.x.x" % (octets[0], octets[1])
        else:
            host = "x"
    return "%s#%d" % (host, port)


def parse_code(raw):
    """Validate the 8 byte code field. Returns str, or None if unusable.

    Right padded with NUL, printable ASCII, no spaces, no interior NUL.
    """
    if len(raw) != 8:
        return None
    stripped = raw.rstrip(b"\x00")
    if not stripped:
        return None
    for b in stripped:
        if b < 0x21 or b > 0x7E:
            return None
    return stripped.decode("ascii")


def rss_kb():
    """Resident set size of this process in KB, or None if we cannot tell."""
    try:
        if sys.platform.startswith("linux"):
            with open("/proc/self/statm", "r") as fh:
                pages = int(fh.read().split()[1])
            return pages * (os.sysconf("SC_PAGE_SIZE") // 1024)
        if os.name == "nt":
            import ctypes

            class _PMC(ctypes.Structure):
                _fields_ = [
                    ("cb", ctypes.c_ulong),
                    ("PageFaultCount", ctypes.c_ulong),
                    ("PeakWorkingSetSize", ctypes.c_size_t),
                    ("WorkingSetSize", ctypes.c_size_t),
                    ("QuotaPeakPagedPoolUsage", ctypes.c_size_t),
                    ("QuotaPagedPoolUsage", ctypes.c_size_t),
                    ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t),
                    ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                    ("PagefileUsage", ctypes.c_size_t),
                    ("PeakPagefileUsage", ctypes.c_size_t),
                ]

            kernel32 = ctypes.WinDLL("kernel32")
            psapi = ctypes.WinDLL("psapi")
            # Without these the 64 bit process handle is passed as a 32 bit
            # int and the call quietly fails.
            kernel32.GetCurrentProcess.restype = ctypes.c_void_p
            psapi.GetProcessMemoryInfo.argtypes = [
                ctypes.c_void_p, ctypes.POINTER(_PMC), ctypes.c_ulong]
            psapi.GetProcessMemoryInfo.restype = ctypes.c_int
            counters = _PMC()
            counters.cb = ctypes.sizeof(_PMC)
            ok = psapi.GetProcessMemoryInfo(
                kernel32.GetCurrentProcess(), ctypes.byref(counters),
                counters.cb)
            if not ok:
                return None
            return int(counters.WorkingSetSize) // 1024
        import resource
        maxrss = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        if sys.platform == "darwin":
            return int(maxrss) // 1024
        return int(maxrss)
    except Exception:
        return None


# ------------------------------------------------------------------ tables


class Endpoint(object):
    __slots__ = ("addr", "code", "role", "last_seen")

    def __init__(self, addr, code, role, now):
        self.addr = addr
        self.code = code
        self.role = role
        self.last_seen = now


class Session(object):
    __slots__ = ("code", "parent", "children", "created")

    def __init__(self, code, now):
        self.code = code
        self.parent = None
        self.children = []
        self.created = now

    def size(self):
        return (1 if self.parent is not None else 0) + len(self.children)

    def members(self):
        out = list(self.children)
        if self.parent is not None:
            out.append(self.parent)
        return out


class Bucket(object):
    """Token bucket, one per source address."""
    __slots__ = ("tokens", "stamp")

    def __init__(self, now):
        self.tokens = float(RATE_BURST)
        self.stamp = now


# --------------------------------------------------------------- the relay


class RelayProtocol(asyncio.DatagramProtocol):

    def __init__(self, idle_s):
        self.idle_s = idle_s
        self.transport = None
        self.sessions = {}      # code -> Session
        self.endpoints = {}     # addr -> Endpoint
        self.buckets = {}       # addr -> Bucket
        self.bad_ack_at = {}    # addr -> monotonic of last status 2 sent
        self.ignored = {}       # addr -> monotonic when the ignore ends
        # counters, all cumulative
        self.n_rx = 0
        self.n_fwd = 0
        self.n_ack = 0
        self.n_drop_rate = 0
        self.n_drop_size = 0
        self.n_drop_unpaired = 0
        self.n_drop_ignored = 0
        self.n_bad = 0
        self.n_full = 0
        self.n_expired = 0

    # ---- asyncio hooks

    def connection_made(self, transport):
        self.transport = transport

    def error_received(self, exc):
        # Windows raises WSAECONNRESET on the socket when a previous sendto
        # hit a closed port. It says nothing about the socket's health, and
        # there is nothing useful to do with it.
        return

    def datagram_received(self, data, addr):
        now = time.monotonic()
        self.n_rx += 1

        until = self.ignored.get(addr)
        if until is not None:
            if until > now:
                self.n_drop_ignored += 1
                return
            del self.ignored[addr]

        if not self._rate_ok(addr, now):
            self.n_drop_rate += 1
            return

        endpoint = self.endpoints.get(addr)
        if endpoint is None:
            # A stranger. The only thing a stranger has any business sending
            # is a HELLO, so a 16 byte datagram is read as one and anything
            # else is dropped without a word. That also means a reply can
            # never be larger than what arrived.
            if len(data) == HELLO_LEN:
                self._on_hello(data, addr, now)
            else:
                self.n_drop_unpaired += 1
            return

        if len(data) == HELLO_LEN and data[:4] == MAGIC_HELLO:
            self._on_hello(data, addr, now)
            return

        self._forward(data, addr, now, endpoint)

    # ---- rate limiting

    def _rate_ok(self, addr, now):
        bucket = self.buckets.get(addr)
        if bucket is None:
            if len(self.buckets) >= MAX_BUCKETS:
                # Tracking table is full. Fail closed rather than grow.
                return False
            bucket = Bucket(now)
            self.buckets[addr] = bucket
        else:
            elapsed = now - bucket.stamp
            if elapsed > 0:
                bucket.tokens = min(
                    float(RATE_BURST), bucket.tokens + elapsed * RATE_PPS)
                bucket.stamp = now
        if bucket.tokens < 1.0:
            return False
        bucket.tokens -= 1.0
        return True

    # ---- sending

    def _send_ack(self, addr, status, code_raw, request_len):
        """Reply to a HELLO.

        Never sends more bytes than we just received from this endpoint.
        An unpaired stranger can therefore never use us as an amplifier.
        """
        if len(code_raw) != 8:
            code_raw = (code_raw + b"\x00" * 8)[:8]
        reply = (MAGIC_ACK + bytes((PROTO_VERSION, status)) + b"\x00\x00"
                 + code_raw)
        if len(reply) > request_len:
            return
        try:
            self.transport.sendto(reply, addr)
        except Exception:
            return
        self.n_ack += 1

    # ---- HELLO

    def _reject(self, data, addr, now, code_raw=None):
        """Malformed HELLO: answer at most once, then go quiet on them."""
        last = self.bad_ack_at.get(addr)
        if last is None or (now - last) >= BAD_ACK_INTERVAL_S:
            self.bad_ack_at[addr] = now
            self.n_bad += 1
            if code_raw is None:
                code_raw = data[8:16] if len(data) >= 16 else b""
            self._send_ack(addr, STATUS_BAD, code_raw, len(data))
            log("bad request from %s len=%d" % (short_addr(addr), len(data)))
        self.ignored[addr] = now + BAD_IGNORE_S

    def _on_hello(self, data, addr, now):
        version = data[4]
        role = data[5]
        code_raw = data[8:16]
        if data[:4] != MAGIC_HELLO:
            self._reject(data, addr, now, code_raw=b"\x00" * 8)
            return
        if version != PROTO_VERSION or role not in (ROLE_PARENT, ROLE_CHILD):
            self._reject(data, addr, now)
            return
        code = parse_code(code_raw)
        if code is None:
            self._reject(data, addr, now, code_raw=b"\x00" * 8)
            return
        # The reserved field is not policed: we ignore what arrives in it and
        # always send zeros back.

        endpoint = self.endpoints.get(addr)
        if endpoint is not None and (endpoint.code != code
                                     or endpoint.role != role):
            # Same endpoint asking for something else. Drop the old seat.
            self._detach(endpoint, "rejoin")
            endpoint = None

        if endpoint is not None:
            # Known endpoint, same seat. Idempotent: refresh and re-ACK.
            endpoint.last_seen = now
            self._send_ack(addr, STATUS_OK, code_raw, len(data))
            return

        session = self.sessions.get(code)
        if session is None:
            if len(self.sessions) >= MAX_SESSIONS:
                self.n_full += 1
                log("session %s refused: relay at %d sessions"
                    % (code, MAX_SESSIONS))
                self._send_ack(addr, STATUS_FULL, code_raw, len(data))
                return
            session = Session(code, now)
            self.sessions[code] = session
            log("session %s create by %s" % (code, short_addr(addr)))

        if role == ROLE_PARENT:
            if session.parent is not None and session.parent != addr:
                self.n_full += 1
                log("session %s refused parent %s: parent seat taken"
                    % (code, short_addr(addr)))
                self._send_ack(addr, STATUS_FULL, code_raw, len(data))
                self._drop_if_empty(session)
                return
            session.parent = addr
        else:
            if (len(session.children) >= MAX_CHILDREN
                    or session.size() >= MAX_ENDPOINTS_PER_SESSION):
                self.n_full += 1
                log("session %s refused child %s: %d children already"
                    % (code, short_addr(addr), len(session.children)))
                self._send_ack(addr, STATUS_FULL, code_raw, len(data))
                self._drop_if_empty(session)
                return
            session.children.append(addr)

        self.endpoints[addr] = Endpoint(addr, code, role, now)
        log("session %s join %s as %s (%d/%d)"
            % (code, short_addr(addr),
               "parent" if role == ROLE_PARENT else "child",
               session.size(), MAX_ENDPOINTS_PER_SESSION))
        self._send_ack(addr, STATUS_OK, code_raw, len(data))

    # ---- forwarding

    def _forward(self, data, addr, now, endpoint):
        if len(data) > MAX_PAYLOAD:
            self.n_drop_size += 1
            return
        endpoint.last_seen = now
        session = self.sessions.get(endpoint.code)
        if session is None:
            self.n_drop_unpaired += 1
            return
        if endpoint.role == ROLE_PARENT:
            targets = session.children
        else:
            targets = (session.parent,) if session.parent is not None else ()
        for target in targets:
            if target == addr:
                continue
            try:
                self.transport.sendto(data, target)
            except Exception:
                continue
            self.n_fwd += 1

    # ---- bookkeeping

    def _detach(self, endpoint, why):
        self.endpoints.pop(endpoint.addr, None)
        session = self.sessions.get(endpoint.code)
        if session is None:
            return
        if session.parent == endpoint.addr:
            session.parent = None
        if endpoint.addr in session.children:
            session.children.remove(endpoint.addr)
        log("session %s leave %s (%s)"
            % (endpoint.code, short_addr(endpoint.addr), why))
        self._drop_if_empty(session)

    def _drop_if_empty(self, session):
        if session.size() == 0 and self.sessions.get(session.code) is session:
            del self.sessions[session.code]
            log("session %s expire" % session.code)

    def sweep(self, now):
        for addr, endpoint in list(self.endpoints.items()):
            if (now - endpoint.last_seen) > self.idle_s:
                self.n_expired += 1
                self._detach(endpoint, "idle")
        for addr, bucket in list(self.buckets.items()):
            if (now - bucket.stamp) > BUCKET_IDLE_S and addr not in self.endpoints:
                del self.buckets[addr]
        for addr, until in list(self.ignored.items()):
            if until <= now:
                del self.ignored[addr]
        for addr, stamp in list(self.bad_ack_at.items()):
            if (now - stamp) > BAD_IGNORE_S * 2:
                del self.bad_ack_at[addr]

    def stats_line(self):
        rss = rss_kb()
        return ("stats sessions=%d endpoints=%d rx=%d fwd=%d ack=%d "
                "drop_rate=%d drop_size=%d drop_unpaired=%d drop_ignored=%d "
                "bad=%d full=%d expired=%d rss_kb=%s"
                % (len(self.sessions), len(self.endpoints), self.n_rx,
                   self.n_fwd, self.n_ack, self.n_drop_rate, self.n_drop_size,
                   self.n_drop_unpaired, self.n_drop_ignored, self.n_bad,
                   self.n_full, self.n_expired,
                   "?" if rss is None else str(rss)))


# ------------------------------------------------------------------- entry


async def run():
    port = int(os.environ.get("SM64DS_RELAY_PORT", DEFAULT_PORT))
    bind = os.environ.get("SM64DS_RELAY_BIND", DEFAULT_BIND)
    idle_s = float(os.environ.get("SM64DS_RELAY_IDLE_S", DEFAULT_IDLE_S))
    stats_s = float(os.environ.get("SM64DS_RELAY_STATS_S", DEFAULT_STATS_S))

    family = socket.AF_INET6 if ":" in bind else socket.AF_INET
    loop = asyncio.get_running_loop()
    transport, protocol = await loop.create_datagram_endpoint(
        lambda: RelayProtocol(idle_s), local_addr=(bind, port), family=family)

    sock = transport.get_extra_info("socket")
    if sock is not None:
        try:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1 << 20)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 1 << 20)
        except OSError:
            pass

    log("relay listening on %s:%d idle=%.0fs max_sessions=%d "
        "max_payload=%d rate_pps=%d"
        % (bind, port, idle_s, MAX_SESSIONS, MAX_PAYLOAD, RATE_PPS))
    log(protocol.stats_line())

    stop = asyncio.Event()
    try:
        import signal
        for signame in ("SIGTERM", "SIGINT"):
            signum = getattr(signal, signame, None)
            if signum is not None:
                loop.add_signal_handler(signum, stop.set)
    except (NotImplementedError, RuntimeError, ImportError):
        pass  # Windows: fall back to KeyboardInterrupt

    last_stats = time.monotonic()
    try:
        while not stop.is_set():
            try:
                await asyncio.wait_for(stop.wait(), timeout=SWEEP_INTERVAL_S)
            except asyncio.TimeoutError:
                pass
            now = time.monotonic()
            protocol.sweep(now)
            if (now - last_stats) >= stats_s:
                last_stats = now
                log(protocol.stats_line())
    finally:
        log(protocol.stats_line())
        log("relay stopping")
        transport.close()


def main():
    try:
        asyncio.run(run())
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
