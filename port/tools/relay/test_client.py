#!/usr/bin/env python3
"""Test client and local proof suite for the SM64DS port UDP relay.

Everything here speaks the frozen wire contract in relay.py and nothing else,
so it doubles as a reference for the game side and as the acceptance suite.

  python test_client.py selftest
      Spawn local relay instances on ports derived from this process id and
      run the whole suite: pairing and throughput, routing, mid session join,
      wrong code isolation, the 700 byte drop, the rate cap, idle expiry and
      recovery, malformed HELLO handling and the no amplification property,
      re-HELLO idempotence, and the session and endpoint caps.

  python test_client.py soak [--minutes 10] [--pps 30]
      Two paired endpoints trading packets for the given time. Prints loss
      and the relay's own memory at start and end.

  python test_client.py probe --host HOST [--port 41234] [--code CODE]
      One HELLO to a running relay, prints the ACK and the round trip. Use
      this against the deployed relay after a deploy.

  python test_client.py remote-check --host HOST [--port 41234]
      Pairs a parent and a child from this machine through a remote relay and
      measures 100 round trips. The post deploy end to end check.

Python 3 standard library only.
"""

import argparse
import os
import select
import socket
import struct
import subprocess
import sys
import tempfile
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
RELAY_PY = os.path.join(HERE, "relay.py")

MAGIC_HELLO = b"SMRC"
MAGIC_ACK = b"SMRA"
PROTO_VERSION = 1
ROLE_PARENT = 0
ROLE_CHILD = 1
STATUS_OK = 0
STATUS_FULL = 1
STATUS_BAD = 2
STATUS_NAME = {0: "ok", 1: "full", 2: "bad"}

MAX_PAYLOAD = 700


# ------------------------------------------------------------ wire helpers


def build_hello(code, role, version=PROTO_VERSION, magic=MAGIC_HELLO,
                reserved=0):
    if isinstance(code, str):
        code = code.encode("ascii")
    return (magic + bytes((version & 0xFF, role & 0xFF))
            + struct.pack("<H", reserved) + code.ljust(8, b"\x00")[:8])


def parse_ack(data):
    """Returns (version, status, code) or None if this is not an ACK."""
    if len(data) != 16 or data[:4] != MAGIC_ACK:
        return None
    return (data[4], data[5], data[8:16].rstrip(b"\x00").decode(
        "ascii", "replace"))


class Node(object):
    """One client endpoint: its own UDP socket, its own byte counters."""

    def __init__(self, relay_addr, name):
        self.name = name
        self.relay = relay_addr
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("127.0.0.1", 0))
        self.sock.setblocking(False)
        try:
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1 << 20)
        except OSError:
            pass
        self.bytes_out = 0
        self.bytes_in = 0
        self.pkts_out = 0
        self.pkts_in = 0
        self.pending = []

    def send(self, data):
        self.sock.sendto(data, self.relay)
        self.bytes_out += len(data)
        self.pkts_out += 1

    def hello(self, code, role, **kw):
        self.send(build_hello(code, role, **kw))

    def _recv_raw(self, timeout):
        ready, _, _ = select.select([self.sock], [], [], timeout)
        if not ready:
            return None
        try:
            data, _addr = self.sock.recvfrom(65535)
        except OSError:
            # Windows reports an earlier unreachable send here. Not our news.
            return None
        self.bytes_in += len(data)
        self.pkts_in += 1
        return data

    # Every reader below always polls at least once, even with a zero
    # timeout, so a caller can sweep the socket without waiting.

    def recv_ack(self, timeout=1.0):
        deadline = time.perf_counter() + timeout
        while True:
            left = deadline - time.perf_counter()
            data = self._recv_raw(left if left > 0 else 0.0)
            if data is not None:
                ack = parse_ack(data)
                if ack is not None:
                    return ack
                self.pending.append(data)
                continue
            if left <= 0:
                return None

    def recv_data(self, timeout=0.5):
        if self.pending:
            return self.pending.pop(0)
        deadline = time.perf_counter() + timeout
        while True:
            left = deadline - time.perf_counter()
            data = self._recv_raw(left if left > 0 else 0.0)
            if data is not None:
                if parse_ack(data) is None:
                    return data
                continue
            if left <= 0:
                return None

    def drain(self, window=0.25):
        """Everything that shows up inside the window, ACKs excluded."""
        out = []
        while self.pending:
            out.append(self.pending.pop(0))
        deadline = time.perf_counter() + window
        while True:
            left = deadline - time.perf_counter()
            data = self._recv_raw(left if left > 0 else 0.0)
            if data is not None:
                if parse_ack(data) is None:
                    out.append(data)
                continue
            if left <= 0:
                return out

    def join(self, code, role, tries=6, timeout=1.0):
        """HELLO until ACK, the way the game does. Returns status or None."""
        for _ in range(tries):
            self.hello(code, role)
            ack = self.recv_ack(timeout)
            if ack is not None:
                return ack[1]
        return None

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


# ----------------------------------------------------------- relay harness


def pid_base_port():
    """A port block private to this process, so parallel runs do not collide."""
    return 20000 + (os.getpid() % 20000)


_next_offset = [0]


def free_port():
    base = pid_base_port()
    for _ in range(400):
        port = base + _next_offset[0]
        _next_offset[0] += 1
        if port > 65000:
            _next_offset[0] = 0
            continue
        probe = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            probe.bind(("127.0.0.1", port))
        except OSError:
            probe.close()
            continue
        probe.close()
        return port
    raise RuntimeError("no free UDP port in this process' block")


class Relay(object):
    """A relay subprocess bound to loopback, with its stdout on disk."""

    def __init__(self, idle_s=None, stats_s=None, tag="relay"):
        self.port = free_port()
        self.addr = ("127.0.0.1", self.port)
        self.tag = tag
        self.env = dict(os.environ)
        self.env["SM64DS_RELAY_PORT"] = str(self.port)
        self.env["SM64DS_RELAY_BIND"] = "127.0.0.1"
        self.env["PYTHONUNBUFFERED"] = "1"
        if idle_s is not None:
            self.env["SM64DS_RELAY_IDLE_S"] = str(idle_s)
        if stats_s is not None:
            self.env["SM64DS_RELAY_STATS_S"] = str(stats_s)
        fd, self.log_path = tempfile.mkstemp(
            prefix="relay_%s_%d_" % (tag, self.port), suffix=".log")
        os.close(fd)
        self.log_fh = open(self.log_path, "wb")
        self.proc = None

    def __enter__(self):
        self.proc = subprocess.Popen(
            [sys.executable, "-u", RELAY_PY],
            stdout=self.log_fh, stderr=subprocess.STDOUT, env=self.env)
        deadline = time.perf_counter() + 15.0
        while time.perf_counter() < deadline:
            if "relay listening" in self.log_text():
                return self
            if self.proc.poll() is not None:
                raise RuntimeError("relay died at startup:\n" + self.log_text())
            time.sleep(0.05)
        raise RuntimeError("relay did not come up:\n" + self.log_text())

    def __exit__(self, *exc):
        if self.proc is not None and self.proc.poll() is None:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=5)
        try:
            self.log_fh.flush()
            self.log_fh.close()
        except Exception:
            pass
        return False

    def log_text(self):
        try:
            self.log_fh.flush()
        except Exception:
            pass
        try:
            with open(self.log_path, "rb") as fh:
                return fh.read().decode("utf-8", "replace")
        except OSError:
            return ""

    def rss_samples(self):
        out = []
        for line in self.log_text().splitlines():
            marker = "rss_kb="
            if "stats " in line and marker in line:
                value = line.split(marker, 1)[1].split()[0]
                if value.isdigit():
                    out.append(int(value))
        return out


# ------------------------------------------------------------- result sink


class Result(object):
    def __init__(self, name):
        self.name = name
        self.ok = True
        self.lines = []

    def check(self, cond, msg):
        self.ok = self.ok and bool(cond)
        self.lines.append("    [%s] %s" % ("PASS" if cond else "FAIL", msg))
        return bool(cond)

    def note(self, msg):
        self.lines.append("    ---- %s" % msg)

    def report(self):
        print("  %s: %s" % (self.name, "PASS" if self.ok else "FAIL"))
        for line in self.lines:
            print(line)
        sys.stdout.flush()


def payload(seq, size=32, tag=b"D"):
    body = tag + struct.pack("<I", seq)
    return body.ljust(size, b"\xAB")[:size]


def pace(start, index, pps):
    """Sleep until packet `index` is due at `pps` packets per second."""
    due = start + (index / float(pps))
    left = due - time.perf_counter()
    if left > 0:
        time.sleep(left)


def percentile(values, frac):
    if not values:
        return float("nan")
    ordered = sorted(values)
    idx = int(round(frac * (len(ordered) - 1)))
    return ordered[idx]


# ------------------------------------------------------------------ tests


def t_pair_and_throughput(relay):
    """Pairing, then 1000 datagrams each way with RTT and loss."""
    res = Result("pair + 1000 datagrams each way")
    code = "THRU0001"
    parent = Node(relay.addr, "parent")
    child = Node(relay.addr, "child")
    try:
        res.check(parent.join(code, ROLE_PARENT) == STATUS_OK, "parent ACK ok")
        res.check(child.join(code, ROLE_CHILD) == STATUS_OK, "child ACK ok")

        count = 1000
        pps = 100
        stop = threading.Event()
        echoed = [0]
        rtts = []
        seen = set()

        def echo_worker():
            while not stop.is_set():
                data = parent.recv_data(0.05)
                if data is None:
                    continue
                echoed[0] += 1
                parent.send(data)

        def collect_worker():
            # Its own thread, so a round trip time is the path's and not
            # this script's polling cadence.
            while not stop.is_set():
                back = child.recv_data(0.05)
                if back is None:
                    continue
                now = time.perf_counter()
                rseq, sent_at = struct.unpack("<Id", back[:12])
                if rseq not in seen:
                    seen.add(rseq)
                    rtts.append((now - sent_at) * 1000.0)

        workers = [threading.Thread(target=echo_worker),
                   threading.Thread(target=collect_worker)]
        for worker in workers:
            worker.start()
        start = time.perf_counter()
        try:
            for seq in range(count):
                pace(start, seq, pps)
                pkt = struct.pack("<Id", seq, time.perf_counter())
                child.send(pkt.ljust(32, b"\x00"))
            deadline = time.perf_counter() + 2.0
            while time.perf_counter() < deadline and len(seen) < count:
                time.sleep(0.05)
        finally:
            stop.set()
            for worker in workers:
                worker.join(timeout=5)

        elapsed = time.perf_counter() - start
        lost = count - len(seen)
        res.note("sent %d child->parent at %d/s over %.1fs"
                 % (count, pps, elapsed))
        res.note("parent received %d, echoed %d back (parent->child)"
                 % (echoed[0], echoed[0]))
        res.note("child received %d echoes, lost %d (%.2f%%)"
                 % (len(seen), lost, 100.0 * lost / count))
        if rtts:
            res.note("rtt ms: min %.3f  p50 %.3f  avg %.3f  p95 %.3f  max %.3f"
                     % (min(rtts), percentile(rtts, 0.50),
                        sum(rtts) / len(rtts), percentile(rtts, 0.95),
                        max(rtts)))
        res.check(echoed[0] == count,
                  "parent got all %d child datagrams (got %d)"
                  % (count, echoed[0]))
        res.check(lost == 0, "child got all %d echoes back (lost %d)"
                  % (count, lost))
    finally:
        parent.close()
        child.close()
    return res


def t_routing(relay):
    """Parent to all children, child to parent only, never child to child."""
    res = Result("routing: star topology")
    code = "ROUTE001"
    parent = Node(relay.addr, "parent")
    c1 = Node(relay.addr, "c1")
    c2 = Node(relay.addr, "c2")
    try:
        res.check(parent.join(code, ROLE_PARENT) == STATUS_OK, "parent joined")
        res.check(c1.join(code, ROLE_CHILD) == STATUS_OK, "child 1 joined")
        res.check(c2.join(code, ROLE_CHILD) == STATUS_OK, "child 2 joined")

        c1.send(payload(1, tag=b"1"))
        got_parent = parent.drain(0.4)
        got_c2 = c2.drain(0.1)
        res.check(len(got_parent) == 1 and got_parent[0][:1] == b"1",
                  "child 1 -> parent delivered (%d)" % len(got_parent))
        res.check(len(got_c2) == 0,
                  "child 1 -> child 2 NOT delivered (%d)" % len(got_c2))

        c2.send(payload(2, tag=b"2"))
        got_parent = parent.drain(0.4)
        got_c1 = c1.drain(0.1)
        res.check(len(got_parent) == 1 and got_parent[0][:1] == b"2",
                  "child 2 -> parent delivered (%d)" % len(got_parent))
        res.check(len(got_c1) == 0,
                  "child 2 -> child 1 NOT delivered (%d)" % len(got_c1))

        parent.send(payload(3, tag=b"P"))
        got_c1 = c1.drain(0.4)
        got_c2 = c2.drain(0.4)
        res.check(len(got_c1) == 1 and got_c1[0][:1] == b"P",
                  "parent -> child 1 delivered (%d)" % len(got_c1))
        res.check(len(got_c2) == 1 and got_c2[0][:1] == b"P",
                  "parent -> child 2 delivered (%d)" % len(got_c2))
        res.check(len(parent.drain(0.1)) == 0, "parent got no echo of its own")
    finally:
        parent.close()
        c1.close()
        c2.close()
    return res


def t_midsession_join(relay):
    """A second child joins a session that is already carrying traffic."""
    res = Result("second child joins mid session")
    code = "MIDJOIN1"
    parent = Node(relay.addr, "parent")
    c1 = Node(relay.addr, "c1")
    c2 = Node(relay.addr, "c2")
    try:
        parent.join(code, ROLE_PARENT)
        c1.join(code, ROLE_CHILD)
        parent.send(payload(10, tag=b"A"))
        res.check(len(c1.drain(0.4)) == 1, "traffic flows before the join")

        res.check(c2.join(code, ROLE_CHILD) == STATUS_OK, "child 2 ACK ok")
        parent.send(payload(11, tag=b"B"))
        got1 = c1.drain(0.4)
        got2 = c2.drain(0.4)
        res.check(len(got1) == 1 and got1[0][:1] == b"B",
                  "child 1 still served after the join")
        res.check(len(got2) == 1 and got2[0][:1] == b"B",
                  "child 2 served from the join on")
        c2.send(payload(12, tag=b"C"))
        gotp = parent.drain(0.4)
        res.check(len(gotp) == 1 and gotp[0][:1] == b"C",
                  "child 2 -> parent works")
        res.check(len(c1.drain(0.1)) == 0, "child 1 saw nothing of child 2")
    finally:
        parent.close()
        c1.close()
        c2.close()
    return res


def t_code_isolation(relay):
    """Two live sessions on one relay never see each other's packets."""
    res = Result("wrong code isolation")
    p1 = Node(relay.addr, "p1")
    c1 = Node(relay.addr, "c1")
    p2 = Node(relay.addr, "p2")
    c2 = Node(relay.addr, "c2")
    try:
        res.check(p1.join("SESSAAA1", ROLE_PARENT) == STATUS_OK, "A parent")
        res.check(c1.join("SESSAAA1", ROLE_CHILD) == STATUS_OK, "A child")
        res.check(p2.join("SESSBBB2", ROLE_PARENT) == STATUS_OK, "B parent")
        res.check(c2.join("SESSBBB2", ROLE_CHILD) == STATUS_OK, "B child")

        for _ in range(20):
            c1.send(payload(1, tag=b"A"))
            p2.send(payload(2, tag=b"B"))
        time.sleep(0.3)
        a_parent = p1.drain(0.3)
        b_child = c2.drain(0.3)
        a_leak = c1.drain(0.1) + p1.drain(0.0)
        b_leak = p2.drain(0.1)
        res.check(len(a_parent) == 20 and all(d[:1] == b"A" for d in a_parent),
                  "session A parent got its 20 (%d)" % len(a_parent))
        res.check(len(b_child) == 20 and all(d[:1] == b"B" for d in b_child),
                  "session B child got its 20 (%d)" % len(b_child))
        res.check(len(a_leak) == 0, "no B traffic reached session A (%d)"
                  % len(a_leak))
        res.check(len(b_leak) == 0, "no A traffic reached session B (%d)"
                  % len(b_leak))
    finally:
        for node in (p1, c1, p2, c2):
            node.close()
    return res


def t_payload_cap(relay):
    """700 bytes go through, 701 is dropped, and the link survives it."""
    res = Result("700 byte payload cap")
    code = "SIZE0001"
    parent = Node(relay.addr, "parent")
    child = Node(relay.addr, "child")
    try:
        parent.join(code, ROLE_PARENT)
        child.join(code, ROLE_CHILD)

        child.send(b"S" + b"\x11" * (MAX_PAYLOAD - 1))
        got = parent.drain(0.4)
        res.check(len(got) == 1 and len(got[0]) == MAX_PAYLOAD,
                  "700 bytes forwarded intact (%d datagrams, %d bytes)"
                  % (len(got), len(got[0]) if got else 0))

        child.send(b"X" + b"\x22" * MAX_PAYLOAD)
        got = parent.drain(0.4)
        res.check(len(got) == 0, "701 bytes dropped in silence (%d)" % len(got))
        res.check(len(child.drain(0.1)) == 0, "no complaint sent back")

        child.send(b"S" + b"\x33" * (MAX_PAYLOAD - 1))
        got = parent.drain(0.4)
        res.check(len(got) == 1, "link still works after the oversize drop")

        parent.send(b"Y" + b"\x44" * MAX_PAYLOAD)
        res.check(len(child.drain(0.3)) == 0,
                  "oversize from the parent is dropped too")
    finally:
        parent.close()
        child.close()
    return res


def t_rate_cap(relay):
    """The cap engages under a flood and releases once the sender calms down."""
    res = Result("rate cap engages and releases")
    code = "RATE0001"
    parent = Node(relay.addr, "parent")
    child = Node(relay.addr, "child")
    try:
        parent.join(code, ROLE_PARENT)
        child.join(code, ROLE_CHILD)
        time.sleep(1.2)  # let the bucket refill after the HELLOs

        burst = 600
        t0 = time.perf_counter()
        for seq in range(burst):
            child.send(payload(seq, size=64))
        burst_s = time.perf_counter() - t0
        delivered = len(parent.drain(1.0))
        res.note("sent %d in %.3fs (%.0f/s), parent received %d"
                 % (burst, burst_s, burst / max(burst_s, 1e-6), delivered))
        res.check(delivered < burst // 2,
                  "cap engaged: %d of %d got through" % (delivered, burst))
        res.check(delivered > 0, "cap is not a blackhole (%d through)"
                  % delivered)
        res.check(len(child.drain(0.1)) == 0, "dropped packets are silent")

        time.sleep(2.0)  # bucket refills
        count = 60
        start = time.perf_counter()
        for seq in range(count):
            pace(start, seq, 30)
            child.send(payload(seq, size=64))
        calm = len(parent.drain(1.0))
        res.note("after 2s idle, sent %d at 30/s, parent received %d"
                 % (count, calm))
        res.check(calm == count, "cap released: %d of %d delivered"
                  % (calm, count))
    finally:
        parent.close()
        child.close()
    return res


def t_idle_expiry():
    """Idle expiry drops the seat, and a fresh HELLO gets it back."""
    res = Result("idle expiry and re-HELLO recovery")
    with Relay(idle_s=3, stats_s=5, tag="idle") as relay:
        code = "IDLE0001"
        parent = Node(relay.addr, "parent")
        child = Node(relay.addr, "child")
        try:
            res.check(parent.join(code, ROLE_PARENT) == STATUS_OK,
                      "parent joined")
            res.check(child.join(code, ROLE_CHILD) == STATUS_OK, "child joined")
            child.send(payload(1))
            res.check(len(parent.drain(0.4)) == 1, "forwarding works at first")

            time.sleep(5.0)  # longer than the 3s idle window, no traffic
            child.send(payload(2))
            res.check(len(parent.drain(0.5)) == 0,
                      "after expiry the packet is not forwarded")

            res.check(parent.join(code, ROLE_PARENT) == STATUS_OK,
                      "parent re-HELLO ACK ok")
            res.check(child.join(code, ROLE_CHILD) == STATUS_OK,
                      "child re-HELLO ACK ok")
            child.send(payload(3))
            got = parent.drain(0.5)
            res.check(len(got) == 1, "forwarding recovered after re-HELLO")
            parent.send(payload(4))
            res.check(len(child.drain(0.5)) == 1, "and in the other direction")

            log = relay.log_text()
            res.check("expire" in log, "relay logged the expiry")
            res.note("relay log has %d session lines"
                     % sum(1 for l in log.splitlines() if "session " in l))
        finally:
            parent.close()
            child.close()
    return res


def t_malformed_and_amplification(relay):
    """Bad HELLOs get at most one small answer, then silence. Never more
    bytes out than in."""
    res = Result("malformed HELLO + no amplification")
    total_out = 0
    total_in = 0
    nodes = []

    def case(name, data, expect_status, expect_reply):
        node = Node(relay.addr, name)
        nodes.append(node)
        node.send(data)
        ack = node.recv_ack(0.6)
        node.drain(0.05)
        if expect_reply:
            ok = ack is not None and ack[1] == expect_status
            res.check(ok, "%s -> status %s (%s)"
                      % (name, STATUS_NAME.get(expect_status, expect_status),
                         "none" if ack is None
                         else STATUS_NAME.get(ack[1], ack[1])))
        else:
            res.check(ack is None and node.bytes_in == 0,
                      "%s -> no reply at all (%d bytes back)"
                      % (name, node.bytes_in))
        res.check(node.bytes_in <= node.bytes_out,
                  "%s: %d bytes in, %d bytes out, no amplification"
                  % (name, node.bytes_out, node.bytes_in))
        return node

    try:
        case("bad magic", build_hello("CODE0001", ROLE_PARENT,
                                      magic=b"SMRX"), STATUS_BAD, True)
        case("bad version", build_hello("CODE0002", ROLE_PARENT, version=2),
             STATUS_BAD, True)
        case("bad role", build_hello("CODE0003", 7), STATUS_BAD, True)
        case("empty code", build_hello("", ROLE_PARENT), STATUS_BAD, True)
        case("junk code", MAGIC_HELLO + b"\x01\x00\x00\x00" + b"\x01\x02\x03"
             + b"\x00\x00\x00\x00\x00", STATUS_BAD, True)
        case("short 4 bytes", MAGIC_HELLO, None, False)
        case("long 300 bytes", MAGIC_HELLO + b"\x99" * 296, None, False)
        case("stray data, unpaired", b"\x01\x02\x03\x04\x05", None, False)

        # One endpoint, malformed then valid: it must be ignored for 10s and
        # then be able to join normally.
        ign = Node(relay.addr, "ignored")
        nodes.append(ign)
        ign.send(build_hello("CODE0004", ROLE_PARENT, version=9))
        first = ign.recv_ack(0.6)
        res.check(first is not None and first[1] == STATUS_BAD,
                  "ignore case: first bad HELLO answered with status 2")
        for _ in range(3):
            ign.hello("CODE0004", ROLE_PARENT)
            time.sleep(0.2)
        res.check(ign.recv_ack(0.5) is None,
                  "valid HELLOs during the ignore window get nothing")
        res.note("waiting out the 10s ignore window")
        time.sleep(10.4)
        res.check(ign.join("CODE0004", ROLE_PARENT, tries=3) == STATUS_OK,
                  "endpoint recovers once the ignore window ends")

        for node in nodes:
            total_out += node.bytes_out
            total_in += node.bytes_in
        res.note("all malformed traffic: %d bytes sent to relay, %d bytes "
                 "back (ratio %.3f)"
                 % (total_out, total_in, total_in / float(max(total_out, 1))))
        res.check(total_in <= total_out,
                  "aggregate: never more bytes out than in")
    finally:
        for node in nodes:
            node.close()
    return res


def t_rehello_idempotent(relay):
    """A repeated HELLO re-ACKs and leaves the pairing exactly as it was."""
    res = Result("re-HELLO idempotence")
    code = "IDEMP001"
    parent = Node(relay.addr, "parent")
    child = Node(relay.addr, "child")
    try:
        res.check(parent.join(code, ROLE_PARENT) == STATUS_OK, "parent joined")
        res.check(child.join(code, ROLE_CHILD) == STATUS_OK, "child joined")
        child.send(payload(1, tag=b"1"))
        res.check(len(parent.drain(0.4)) == 1, "forwarding works")

        for i in range(5):
            child.hello(code, ROLE_CHILD)
            ack = child.recv_ack(1.0)
            res.check(ack is not None and ack[1] == STATUS_OK
                      and ack[2] == code, "re-HELLO %d re-ACKed ok" % (i + 1))
        child.send(payload(2, tag=b"2"))
        got = parent.drain(0.4)
        res.check(len(got) == 1 and got[0][:1] == b"2",
                  "pairing survived the re-HELLOs")
        parent.send(payload(3, tag=b"P"))
        res.check(len(child.drain(0.4)) == 1, "and the reverse path too")

        joins = [l for l in relay.log_text().splitlines()
                 if ("session %s join" % code) in l]
        res.check(len(joins) == 2,
                  "relay logged exactly 2 joins for this code (%d)"
                  % len(joins))
    finally:
        parent.close()
        child.close()
    return res


def t_caps():
    """Four endpoints per session, sixty four sessions, then refusals."""
    res = Result("session and endpoint caps")
    with Relay(tag="caps") as relay:
        nodes = []
        try:
            code = "CAP00001"
            seats = []
            for i in range(4):
                node = Node(relay.addr, "seat%d" % i)
                nodes.append(node)
                role = ROLE_PARENT if i == 0 else ROLE_CHILD
                seats.append(node.join(code, role))
            res.check(all(s == STATUS_OK for s in seats),
                      "parent + 3 children all ACK ok (%s)" % seats)

            extra = Node(relay.addr, "seat5")
            nodes.append(extra)
            res.check(extra.join(code, ROLE_CHILD, tries=2) == STATUS_FULL,
                      "4th child refused with status 1")
            second_parent = Node(relay.addr, "parent2")
            nodes.append(second_parent)
            res.check(
                second_parent.join(code, ROLE_PARENT, tries=2) == STATUS_FULL,
                "second parent refused with status 1")

            ok_sessions = 1
            for i in range(63):
                node = Node(relay.addr, "s%d" % i)
                nodes.append(node)
                if node.join("S%07d" % i, ROLE_PARENT, tries=2) == STATUS_OK:
                    ok_sessions += 1
            res.check(ok_sessions == 64, "64 sessions accepted (%d)"
                      % ok_sessions)
            over = Node(relay.addr, "over")
            nodes.append(over)
            res.check(over.join("OVERFLOW", ROLE_PARENT, tries=2)
                      == STATUS_FULL, "65th session refused with status 1")
            res.note("relay logged %d refusals"
                     % sum(1 for l in relay.log_text().splitlines()
                           if "refused" in l))
        finally:
            for node in nodes:
                node.close()
    return res


# ------------------------------------------------------------------- soak


def soak(minutes=10.0, pps=30):
    print("soak: %g minutes, two paired endpoints, %d packets/sec each way"
          % (minutes, pps))
    sys.stdout.flush()
    with Relay(stats_s=60, tag="soak") as relay:
        code = "SOAK0001"
        parent = Node(relay.addr, "parent")
        child = Node(relay.addr, "child")
        try:
            if parent.join(code, ROLE_PARENT) != STATUS_OK:
                raise RuntimeError("parent could not join")
            if child.join(code, ROLE_CHILD) != STATUS_OK:
                raise RuntimeError("child could not join")

            duration = minutes * 60.0
            total = int(duration * pps)
            stop = threading.Event()
            stats = {}

            def worker(node, name):
                sent = 0
                got = 0
                seen = set()
                start = time.perf_counter()
                for seq in range(total):
                    pace(start, seq, pps)
                    node.send(struct.pack("<I", seq).ljust(48, b"\x5A"))
                    sent += 1
                    while True:
                        data = node.recv_data(0.0)
                        if data is None:
                            break
                        got += 1
                        seen.add(struct.unpack("<I", data[:4])[0])
                    if stop.is_set():
                        break
                deadline = time.perf_counter() + 3.0
                while time.perf_counter() < deadline:
                    data = node.recv_data(0.2)
                    if data is None:
                        continue
                    got += 1
                    seen.add(struct.unpack("<I", data[:4])[0])
                stats[name] = (sent, got, len(seen),
                               time.perf_counter() - start)

            rss_start = relay.rss_samples()
            threads = [threading.Thread(target=worker, args=(parent, "parent")),
                       threading.Thread(target=worker, args=(child, "child"))]
            t0 = time.perf_counter()
            for th in threads:
                th.start()
            last_print = t0
            while any(th.is_alive() for th in threads):
                time.sleep(1.0)
                now = time.perf_counter()
                if now - last_print >= 60.0:
                    last_print = now
                    print("  soak %4.0fs elapsed of %.0f" % (now - t0,
                                                             duration))
                    sys.stdout.flush()
            for th in threads:
                th.join()
            elapsed = time.perf_counter() - t0
        finally:
            parent.close()
            child.close()
        time.sleep(1.5)
        rss_end = relay.rss_samples()
        log = relay.log_text()

    print("soak finished in %.1fs (target %.0fs)" % (elapsed, minutes * 60.0))
    ok = True
    for name in ("parent", "child"):
        sent, got, uniq, took = stats[name]
        peer_sent = stats["child" if name == "parent" else "parent"][0]
        lost = peer_sent - uniq
        ok = ok and (lost == 0)
        print("  %-6s sent %d in %.1fs (%.1f/s), received %d (%d unique) "
              "from peer, lost %d (%.3f%%)"
              % (name, sent, took, sent / took, got, uniq, lost,
                 100.0 * lost / max(peer_sent, 1)))
    if rss_start and rss_end:
        print("  relay rss_kb: start %d, end %d, max %d, delta %+d KB"
              % (rss_start[0], rss_end[-1], max(rss_end),
                 rss_end[-1] - rss_start[0]))
    else:
        print("  relay rss: not reported on this platform")
    tail = [l for l in log.splitlines() if "stats " in l]
    for line in tail[-1:]:
        print("  final relay counters: %s" % line.split("stats ", 1)[1])
    print("soak: %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


# --------------------------------------------------------- remote helpers


def probe(host, port, code, role, tries=5):
    addr = (host, port)
    node = Node(addr, "probe")
    try:
        for attempt in range(tries):
            t0 = time.perf_counter()
            node.hello(code, role)
            ack = node.recv_ack(1.0)
            rtt = (time.perf_counter() - t0) * 1000.0
            if ack is not None:
                print("ACK from %s:%d version=%d status=%d (%s) code=%r "
                      "rtt=%.1f ms"
                      % (host, port, ack[0], ack[1],
                         STATUS_NAME.get(ack[1], "?"), ack[2], rtt))
                return 0 if ack[1] == STATUS_OK else 1
            print("no ACK (try %d of %d)" % (attempt + 1, tries))
        print("no ACK from %s:%d after %d tries" % (host, port, tries))
        return 1
    finally:
        node.close()


def remote_check(host, port, code, count=100, pps=50):
    """Pair two endpoints through a remote relay and measure round trips."""
    addr = (host, port)
    parent = Node(addr, "parent")
    child = Node(addr, "child")
    try:
        if parent.join(code, ROLE_PARENT) != STATUS_OK:
            print("parent could not join %s" % code)
            return 1
        if child.join(code, ROLE_CHILD) != STATUS_OK:
            print("child could not join %s" % code)
            return 1
        print("paired on code %s through %s:%d" % (code, host, port))
        stop = threading.Event()

        def echo_worker():
            while not stop.is_set():
                data = parent.recv_data(0.05)
                if data is not None:
                    parent.send(data)

        rtts = []
        seen = set()

        def collect_worker():
            while not stop.is_set():
                back = child.recv_data(0.05)
                if back is None:
                    continue
                now = time.perf_counter()
                rseq, sent_at = struct.unpack("<Id", back[:12])
                if rseq not in seen:
                    seen.add(rseq)
                    rtts.append((now - sent_at) * 1000.0)

        workers = [threading.Thread(target=echo_worker),
                   threading.Thread(target=collect_worker)]
        for worker in workers:
            worker.start()
        start = time.perf_counter()
        try:
            for seq in range(count):
                pace(start, seq, pps)
                child.send(struct.pack("<Id", seq,
                                       time.perf_counter()).ljust(32, b"\x00"))
            deadline = time.perf_counter() + 3.0
            while time.perf_counter() < deadline and len(seen) < count:
                time.sleep(0.05)
        finally:
            stop.set()
            for worker in workers:
                worker.join(timeout=5)
        lost = count - len(seen)
        print("round trips: %d sent, %d returned, %d lost (%.1f%%)"
              % (count, len(seen), lost, 100.0 * lost / count))
        if rtts:
            print("rtt ms: min %.1f  p50 %.1f  avg %.1f  p95 %.1f  max %.1f"
                  % (min(rtts), percentile(rtts, 0.50), sum(rtts) / len(rtts),
                     percentile(rtts, 0.95), max(rtts)))
        return 0 if lost == 0 else 1
    finally:
        parent.close()
        child.close()


# ------------------------------------------------------------------ suite


def selftest():
    print("relay selftest, python %s on %s"
          % (sys.version.split()[0], sys.platform))
    print("port block for this process (pid %d): %d and up"
          % (os.getpid(), pid_base_port()))
    sys.stdout.flush()
    results = []
    t0 = time.perf_counter()
    # Frequent stats lines: on Windows the harness kills the relay outright,
    # so the shutdown tally never gets written and the last periodic line is
    # the honest one. Under systemd it gets SIGTERM and exits cleanly.
    with Relay(stats_s=10, tag="main") as relay:
        print("relay up on 127.0.0.1:%d" % relay.port)
        sys.stdout.flush()
        for test in (t_pair_and_throughput, t_routing, t_midsession_join,
                     t_code_isolation, t_payload_cap, t_rate_cap,
                     t_malformed_and_amplification, t_rehello_idempotent):
            res = test(relay)
            res.report()
            results.append(res)
    stats = [l for l in relay.log_text().splitlines() if "stats " in l]
    if stats:
        print("  main relay last stats line: %s"
              % stats[-1].split("stats ", 1)[1])
    for test in (t_idle_expiry, t_caps):
        res = test()
        res.report()
        results.append(res)
    elapsed = time.perf_counter() - t0

    failed = [r.name for r in results if not r.ok]
    print("")
    print("%d tests, %d passed, %d failed, %.1fs"
          % (len(results), len(results) - len(failed), len(failed), elapsed))
    if failed:
        print("FAILED: %s" % ", ".join(failed))
        return 1
    print("SUITE PASS")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="cmd")
    sub.add_parser("selftest", help="run the whole local suite")
    soak_p = sub.add_parser("soak", help="long run with two endpoints")
    soak_p.add_argument("--minutes", type=float, default=10.0)
    soak_p.add_argument("--pps", type=int, default=30)
    probe_p = sub.add_parser("probe", help="one HELLO to a running relay")
    probe_p.add_argument("--host", required=True)
    probe_p.add_argument("--port", type=int, default=41234)
    probe_p.add_argument("--code", default="PROBE001")
    probe_p.add_argument("--role", choices=("parent", "child"),
                         default="parent")
    rc_p = sub.add_parser("remote-check",
                          help="pair two endpoints through a remote relay")
    rc_p.add_argument("--host", required=True)
    rc_p.add_argument("--port", type=int, default=41234)
    rc_p.add_argument("--code", default="RMTCHK01")
    rc_p.add_argument("--count", type=int, default=100)
    args = parser.parse_args(argv)

    if args.cmd in (None, "selftest"):
        return selftest()
    if args.cmd == "soak":
        return soak(args.minutes, args.pps)
    if args.cmd == "probe":
        role = ROLE_PARENT if args.role == "parent" else ROLE_CHILD
        return probe(args.host, args.port, args.code, role)
    if args.cmd == "remote-check":
        return remote_check(args.host, args.port, args.code, args.count)
    parser.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
