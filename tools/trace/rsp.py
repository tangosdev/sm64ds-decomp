"""Minimal GDB Remote Serial Protocol (RSP) client over a TCP socket.

No gdb / gdb-multiarch dependency -- we speak RSP directly to the melonDS (or
DeSmuME) GDB stub. This is the Phase-0 transport for the emu-trace pipeline
(see notes/emu-trace-build.md).

Only the handful of packets the tracer needs are implemented:
  ?           stop reason / are-we-halted
  <0x03>      interrupt (halt a running target)
  g           read all registers
  m addr,len  read memory
  Z0/z0       set/clear software breakpoint
  c           continue
  D           detach

Register-layout note: the first 16 words of the `g` reply are always r0..r15
in every ARM GDB layout (core, legacy-FPA, ...). sp=r13, lr=r14, pc=r15 are
therefore reliable regardless of stub quirks. CPSR position DOES vary between
stubs -- read_registers() detects it heuristically and reports what it found;
that ambiguity is exactly what the Phase-0 spike exists to pin down.
"""
import socket
import time


class RspError(Exception):
    pass


class RspClient:
    def __init__(self, host="127.0.0.1", port=3333, timeout=10.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.sock = None
        self._buf = b""

    # ------------------------------------------------------------------ conn
    def connect(self, retries=20, retry_delay=0.25):
        # melonDS rebuilds its listen socket on each client disconnect, leaving a
        # brief window where connect() is refused. Retry across it.
        last = None
        for _ in range(retries):
            try:
                self.sock = socket.create_connection((self.host, self.port), self.timeout)
                break
            except ConnectionRefusedError as e:
                last = e
                time.sleep(retry_delay)
        else:
            raise last
        self.sock.settimeout(self.timeout)
        # melonDS handshake: on accept the stub calls WaitAckBlocking expecting
        # the CLIENT to send '+' first (within ~1s), then it SendAck's '+' back.
        # Without this leading '+' it closes the connection on the first packet.
        self.sock.sendall(b"+")
        # swallow its single handshake '+' byte (best-effort; _read_reply also
        # skips stray acks, so a miss here is harmless).
        old = self.sock.gettimeout()
        self.sock.settimeout(1.0)
        try:
            self._read_byte()
        except (socket.timeout, OSError):
            pass
        finally:
            self.sock.settimeout(old)
        return self

    def close(self):
        if self.sock:
            try:
                self.sock.close()
            finally:
                self.sock = None

    def __enter__(self):
        return self.connect()

    def __exit__(self, *exc):
        self.close()

    # -------------------------------------------------------------- framing
    @staticmethod
    def _checksum(data: bytes) -> int:
        return sum(data) & 0xFF

    def _read_byte(self) -> bytes:
        if not self._buf:
            chunk = self.sock.recv(4096)
            if not chunk:
                raise RspError("connection closed by stub")
            self._buf = chunk
        b, self._buf = self._buf[:1], self._buf[1:]
        return b

    def send_packet(self, payload: str, expect_reply=True, retries=3) -> str:
        """Send `$payload#cc`, consume the +/- ack, optionally return the reply."""
        data = payload.encode("ascii")
        pkt = b"$" + data + b"#" + f"{self._checksum(data):02x}".encode("ascii")
        for _ in range(retries):
            self.sock.sendall(pkt)
            ack = self._read_byte()
            if ack == b"+":
                break
            if ack == b"-":
                continue  # stub asked for retransmit
            # some stubs skip the ack; treat the byte as start of the reply
            self._buf = ack + self._buf
            break
        else:
            raise RspError("stub kept NAKing packet: %r" % payload)
        if not expect_reply:
            return ""
        return self._read_reply()

    def _read_reply(self) -> str:
        # skip stray acks, then read $...#cc, verify checksum, ACK it.
        while True:
            b = self._read_byte()
            if b == b"$":
                break
            # '+'/'-' or notification bytes: ignore
        body = b""
        while True:
            b = self._read_byte()
            if b == b"#":
                break
            body += b
        cs = self._read_byte() + self._read_byte()
        got = int(cs, 16)
        if got != self._checksum(body):
            self.sock.sendall(b"-")
            raise RspError("bad checksum in reply %r" % body)
        self.sock.sendall(b"+")
        return body.decode("ascii", "replace")

    def interrupt(self):
        """Send a raw Ctrl-C (0x03) to halt a running target."""
        self.sock.sendall(b"\x03")

    def drain(self, timeout=0.5):
        """Best-effort read of one pending reply (e.g. the stop packet an
        interrupt produces). Returns the packet, or None if nothing arrived.
        Used to keep the interrupt->command handshake from desyncing."""
        old = self.sock.gettimeout()
        self.sock.settimeout(timeout)
        try:
            return self._read_reply()
        except (socket.timeout, OSError):
            return None
        finally:
            self.sock.settimeout(old)

    def halt_and_sync(self):
        """Halt a running target and swallow the resulting stop packet so the
        socket is left clean for subsequent commands."""
        self.interrupt()
        return self.drain(timeout=1.0)

    # -------------------------------------------------------------- commands
    def stop_reason(self) -> str:
        return self.send_packet("?")

    def cont(self, expect_reply=False) -> str:
        # `c` does not reply until the target next stops; callers that want the
        # stop packet should use wait_for_stop() instead.
        return self.send_packet("c", expect_reply=expect_reply)

    def wait_for_stop(self) -> str:
        """Block until the target reports a stop (S/T packet) and return it."""
        return self._read_reply()

    def set_breakpoint(self, addr: int, kind=4) -> bool:
        r = self.send_packet(f"Z0,{addr:x},{kind}")
        return r == "OK"

    def clear_breakpoint(self, addr: int, kind=4) -> bool:
        r = self.send_packet(f"z0,{addr:x},{kind}")
        return r == "OK"

    def read_mem(self, addr: int, length: int) -> bytes:
        r = self.send_packet(f"m{addr:x},{length:x}")
        if r.startswith("E"):
            raise RspError(f"m{addr:x},{length:x} -> {r}")
        return bytes.fromhex(r)

    def read_registers(self) -> dict:
        """Return {'raw', 'words', 'r0'..'r15', 'sp','lr','pc', 'cpsr', 'cpsr_src'}.

        First 16 words are r0..r15 in every layout. CPSR is located heuristically:
        a valid CPSR has a legal mode field in bits[4:0]. We test the two common
        positions (word[16] for core layout, word[-1] for legacy-FPA) and report
        which one looked valid.
        """
        raw = self.send_packet("g")
        if raw.startswith("E") or len(raw) < 16 * 8:
            raise RspError(f"g -> {raw!r}")
        words = [
            int.from_bytes(bytes.fromhex(raw[i:i + 8]), "little")
            for i in range(0, len(raw) - len(raw) % 8, 8)
        ]
        out = {"raw": raw, "words": words}
        for i in range(16):
            out[f"r{i}"] = words[i]
        out["sp"], out["lr"], out["pc"] = words[13], words[14], words[15]
        out["cpsr"], out["cpsr_src"] = self._pick_cpsr(words)
        return out

    @staticmethod
    def _pick_cpsr(words):
        valid_modes = {0x10, 0x11, 0x12, 0x13, 0x16, 0x17, 0x1B, 0x1F}
        candidates = []
        if len(words) >= 17:
            candidates.append(("word16", words[16]))
        if len(words) >= 18:
            candidates.append(("last", words[-1]))
        for src, val in candidates:
            if (val & 0x1F) in valid_modes:
                return val, src
        return (candidates[0][1] if candidates else None,
                candidates[0][0] if candidates else "none")

    def detach(self):
        # `D` tells the stub to remove its breakpoints and resume the target,
        # releasing the single session so the next client can attach. Never
        # precede this with a 0x03 break -- on the melonDS Windows stub that
        # resets the connection.
        try:
            self.send_packet("D")
        except (RspError, OSError):
            pass
