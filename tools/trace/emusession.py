"""emusession: one long-lived melonDS GDB-stub session, served to short-lived clients.

The melonDS 1.1 stub accepts ONE client per launch, and a client that dies
without sending `D` wedges it (see README "GDB-stub quirks"). So the connection
lives in a daemon (`ask.py serve`) and every script talks to the daemon over a
local JSON-lines socket instead (`Client`). A client can crash or be killed
without touching the stub.

What the session adds over rsp.py:
- keepalive: a 4-byte read every few seconds while idle, plus TCP keepalive;
  a closed or silent stub raises StubDropped (the daemon reports
  `stub_dropped` to every later call instead of hanging).
- pause without 0x03: a "frame" breakpoint on a once-per-frame function. A pause
  is "run to the next frame-bp hit"; step(n) is "run n frame-bp hits".
- residency canaries: every breakpoint carries the 8 bytes its module image
  has at that address. A hit whose live bytes differ (another overlay is
  loaded at that address) is counted as a skip and continued, never reported.
- probes: log-and-continue breakpoints that dump registers and memory
  expressions, optionally catching the return value (a temporary breakpoint
  on lr, matched by sp).
- address expressions: `r0+0x1b`, `[0x020ad634]+4`, `u8[[obj+4]+0x113]`, ...
"""
import json
import os
import re
import select
import socket
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from rsp import RspClient, RspError  # noqa: E402

DEFAULT_RPC_PORT = 3390
MAX_READ = 0x80          # longer `m` reads time out on the melonDS stub


class StubDropped(RuntimeError):
    pass


# --------------------------------------------------------------------- images
def repo_root():
    return os.path.dirname(os.path.dirname(HERE))


def main_checkout():
    """The main checkout (holds the gitignored ROM inputs) even from a worktree."""
    root = repo_root()
    try:
        common = subprocess.run(['git', '-C', root, 'rev-parse', '--git-common-dir'],
                                capture_output=True, text=True, timeout=10).stdout.strip()
        if common:
            if not os.path.isabs(common):
                common = os.path.join(root, common)
            return os.path.dirname(os.path.abspath(common))
    except Exception:
        pass
    return root


def find_input(rel):
    for base in (repo_root(), main_checkout()):
        p = os.path.join(base, rel)
        if os.path.exists(p):
            return p
    raise FileNotFoundError(rel + ' (not in this checkout or the main checkout)')


class Images:
    """Module images from extracted/dsd, for residency canaries."""

    def __init__(self, extracted=None):
        self.dir = extracted or find_input(os.path.join('extracted', 'dsd'))
        self.mods = {}   # name -> (base, path)
        ay = open(os.path.join(self.dir, 'arm9', 'arm9.yaml')).read()
        self.mods['arm9'] = (int(re.search(r'^base_address:\s*(\d+)', ay, re.M).group(1)),
                             os.path.join(self.dir, 'arm9', 'arm9.bin'))
        oy = open(os.path.join(self.dir, 'arm9_overlays', 'overlays.yaml')).read()
        for m in re.finditer(r'-\s*id:\s*(\d+)\s*\n\s*base_address:\s*(\d+)', oy):
            n = int(m.group(1))
            self.mods['ov%03d' % n] = (int(m.group(2)),
                                       os.path.join(self.dir, 'arm9_overlays', 'ov%03d.bin' % n))
        self._data = {}

    def data(self, mod):
        if mod not in self._data:
            self._data[mod] = open(self.mods[mod][1], 'rb').read()
        return self._data[mod]

    def bytes_at(self, mod, addr, n=8):
        base = self.mods[mod][0]
        d = self.data(mod)
        off = addr - base
        if off < 0 or off + n > len(d):
            raise ValueError('%08x is outside %s' % (addr, mod))
        return d[off:off + n]

    def default_module(self, addr):
        base = self.mods['arm9'][0]
        if base <= addr < base + len(self.data('arm9')):
            return 'arm9'
        return None


# ---------------------------------------------------------------- expressions
_TOK = re.compile(r'\s*(0x[0-9a-fA-F]+|\d+|u8\[|u16\[|u32\[|s8\[|s16\[|s32\[|[A-Za-z_]\w*|==|!=|[\[\]()+\-&|*])')


def parse_int(v):
    if isinstance(v, int):
        return v
    return int(str(v), 0)


def evaluate(expr, env, read):
    """Evaluate an address/value expression.

    env: name -> int (r0..r15, sp, lr, pc, obj, node, ...). read(addr, size) -> int.
    Grammar: cmp := or (('=='|'!=') or)?  (yields 1/0)
             or := and ('|' and)*; and := add ('&' add)*; add := mul (('+'|'-') mul)*;
             mul := atom ('*' atom)*; atom := number | name | [e] (u32) | u8[e] |
             u16[e] | s8[e] | s16[e] | s32[e] | (e)
    """
    if isinstance(expr, int):
        return expr
    toks = []
    s = str(expr)
    pos = 0
    while pos < len(s):
        m = _TOK.match(s, pos)
        if not m:
            raise ValueError('bad expression %r at %d' % (s, pos))
        toks.append(m.group(1))
        pos = m.end()
        while pos < len(s) and s[pos].isspace():
            pos += 1
    i = [0]

    def peek():
        return toks[i[0]] if i[0] < len(toks) else None

    def take(t=None):
        tok = peek()
        if t is not None and tok != t:
            raise ValueError('expected %r in %r' % (t, s))
        i[0] += 1
        return tok

    def atom():
        t = take()
        if t is None:
            raise ValueError('truncated expression %r' % s)
        if t == '(':
            v = e_or()
            take(')')
            return v
        if t == '-':
            return -atom()
        if t in ('[', 'u32[', 'u8[', 'u16[', 's8[', 's16[', 's32['):
            a = e_or()
            take(']')
            size = {'[': 4, 'u32[': 4, 's32[': 4, 'u8[': 1, 's8[': 1, 'u16[': 2, 's16[': 2}[t]
            v = read(a & 0xffffffff, size)
            if t.startswith('s') and v & (1 << (8 * size - 1)):
                v -= 1 << (8 * size)
            return v
        if t[0].isdigit():
            return int(t, 0)
        if t in env:
            return env[t]
        raise ValueError('unknown name %r in %r' % (t, s))

    def mul():
        v = atom()
        while peek() == '*':
            take()
            v = v * atom()
        return v

    def add():
        v = mul()
        while peek() in ('+', '-'):
            op = take()
            w = mul()
            v = v + w if op == '+' else v - w
        return v

    def e_and():
        v = add()
        while peek() == '&':
            take()
            v &= add()
        return v

    def e_or():
        v = e_and()
        while peek() == '|':
            take()
            v |= e_and()
        return v

    v = e_or()
    if peek() in ('==', '!='):
        op = take()
        w = e_or()
        v = int((v == w) if op == '==' else (v != w))
    if peek() is not None:
        raise ValueError('trailing tokens in %r' % s)
    return v


# --------------------------------------------------------------------- session
class Bp:
    def __init__(self, addr, role, name='', module=None, dump=(), ret=False, max_hits=0, when=None,
                 obj=None, thumb=False):
        self.addr = addr & ~1
        self.thumb = bool(thumb or (addr & 1))
        self.role = role            # frame | trigger | probe
        self.name = name or '%08x' % self.addr
        self.module = module
        self.canary = None
        self.dump = list(dump)      # [[label, expr, size], ...]
        self.ret = ret
        self.max_hits = max_hits
        self.when = when            # expression; trigger/probe only fires when nonzero
        self.obj = obj              # expression for `obj` at the hit
        self.armed = False
        self.hits = 0
        self.skips = 0

    def desc(self):
        return {'addr': '%08x' % self.addr, 'role': self.role, 'name': self.name, 'module': self.module,
                'armed': self.armed, 'hits': self.hits, 'canary_skips': self.skips}


class Session:
    def __init__(self, images, port=3333, log_path=None):
        self.images = images
        self.port = port
        self.cli = None
        self.running = False
        self.pending = None
        self.bps = {}              # addr -> Bp
        self.frame = None          # the frame Bp
        self.frames = 0            # frame-bp hits seen
        self.rets = {}             # lr addr -> {'kind': 2|4, 'waits': [call records]}
        self.held = None           # a trigger hit that arrived while no client was waiting
        self.events = []
        self.nevents = 0
        self.dead = None
        self.last_io = time.time()
        self.log = open(log_path, 'a', encoding='utf-8') if log_path else None

    # ------------------------------------------------------------ transport
    def connect(self, timeout=60.0):
        t0 = time.time()
        last = None
        while time.time() - t0 < timeout:
            try:
                self.cli = RspClient(port=self.port, timeout=5.0).connect(retries=4)
                self.cli.sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
                self.cli.read_mem(0x02000000, 4)
                break
            except (OSError, RspError) as e:
                last = e
                try:
                    if self.cli:
                        self.cli.close()
                except Exception:
                    pass
                self.cli = None
                time.sleep(1.0)
        if not self.cli:
            raise StubDropped('could not attach to the stub on port %d: %r' % (self.port, last))
        self.dead = None
        stop = self.cli.stop_reason()      # melonDS halts the target on attach
        self.running = False
        self.event({'ev': 'attach', 'stop': stop})
        return stop

    def _drop(self, e):
        self.dead = repr(e)
        self.running = False
        try:
            self.cli.close()
        except Exception:
            pass
        self.event({'ev': 'stub_dropped', 'err': self.dead})
        raise StubDropped(self.dead)

    def _any_armed(self):
        return bool(self.rets) or any(b.armed for b in self.bps.values())

    def _quiesce(self, timeout=0.5):
        """melonDS 1.1 closes the connection when a request crosses an async stop
        packet (it wants '+' for its stop and reads our '$' instead). So while the
        target runs with breakpoints armed, wait for the next stop and send while
        halted. Returns 'resume' (cont afterwards), 'held' (a trigger: stay
        halted for its waiter) or None (no stop came; send anyway)."""
        pkt = self.poll_stop(timeout)
        if pkt is None:
            return None
        act, info = self._handle_stop(pkt)
        if act == 'hit' and info.get('role') == 'trigger':
            self.held = info
            return 'held'
        return 'resume'

    def _pkt(self, payload):
        if self.dead:
            raise StubDropped(self.dead)
        resume = self.running and self._any_armed() and self._quiesce() == 'resume'
        try:
            r = self.cli.send_packet(payload)
            while r and r[0] in 'ST' and not payload.startswith('?'):
                self.pending = r           # an async stop crossed our request
                r = self.cli._read_reply()
            self.last_io = time.time()
        except (OSError, RspError) as e:
            self._drop(e)
        if resume:
            self.cont()
        return r

    def poll_stop(self, timeout):
        """A stop packet if one arrives within timeout, else None."""
        if self.pending:
            p, self.pending = self.pending, None
            return p
        if not self.running:
            return None
        if not self.cli._buf:
            try:
                r, _, _ = select.select([self.cli.sock], [], [], max(0.0, timeout))
            except (OSError, ValueError) as e:
                self._drop(e)
            if not r:
                return None
        try:
            self.cli.sock.settimeout(5.0)
            p = self.cli._read_reply()
            self.last_io = time.time()
            return p
        except (OSError, RspError) as e:
            self._drop(e)

    # ------------------------------------------------------------ primitives
    def read(self, addr, n):
        out = b''
        while n > 0:
            k = min(n, MAX_READ)
            r = self._pkt('m%x,%x' % (addr, k))
            if not r or r.startswith('E'):
                raise RuntimeError('read %08x,%x -> %r' % (addr, k, r))
            out += bytes.fromhex(r)
            addr += k
            n -= k
        return out

    def read_int(self, addr, size=4):
        return int.from_bytes(self.read(addr, size), 'little')

    def write(self, addr, data):
        r = self._pkt('M%x,%x:%s' % (addr, len(data), data.hex()))
        if r != 'OK':
            raise RuntimeError('write %08x -> %r' % (addr, r))

    def regs(self):
        if self.running:
            raise RuntimeError('target is running')
        raw = self._pkt('g')
        w = [int.from_bytes(bytes.fromhex(raw[i:i + 8]), 'little') for i in range(0, len(raw) - len(raw) % 8, 8)]
        d = {'r%d' % i: w[i] for i in range(16)}
        d.update(sp=w[13], lr=w[14], pc=w[15], cpsr=w[16] if len(w) > 16 else 0)
        return d

    def cont(self):
        if self.running:
            return
        if self.dead:
            raise StubDropped(self.dead)
        try:
            self.cli.send_packet('c', expect_reply=False)
        except (OSError, RspError) as e:
            self._drop(e)
        self.running = True

    def ping(self):
        t = time.time()
        self.read(0x02000000, 4)
        return round((time.time() - t) * 1000, 1)

    def event(self, rec):
        rec.setdefault('t', round(time.time(), 3))
        rec.setdefault('frames', self.frames)
        self.nevents += 1
        rec['n'] = self.nevents
        self.events.append(rec)
        if len(self.events) > 5000:
            del self.events[:1000]
        if self.log:
            self.log.write(json.dumps(rec) + '\n')
            self.log.flush()

    # ------------------------------------------------------------ breakpoints
    def add_bp(self, bp, arm=True):
        if bp.module is None:
            bp.module = self.images.default_module(bp.addr)
        if bp.module:
            bp.canary = self.images.bytes_at(bp.module, bp.addr, 4 if bp.thumb else 8)
        old = self.bps.get(bp.addr)
        if old and old.armed and not arm:
            self._z(old, False)
        self.bps[bp.addr] = bp
        if old and old.armed:
            bp.armed = True
        if arm and not bp.armed:
            self._z(bp, True)
        if bp.role == 'frame':
            self.frame = bp
        return bp

    def _z(self, bp, on):
        kind = 2 if bp.thumb else 4
        r = self._pkt(('Z0,%x,%d' if on else 'z0,%x,%d') % (bp.addr, kind))
        if r != 'OK':
            raise RuntimeError('%s breakpoint %08x -> %r' % ('set' if on else 'clear', bp.addr, r))
        bp.armed = on

    def arm(self, addr, on=True):
        bp = self.bps[addr & ~1]
        if bp.armed != on:
            self._z(bp, on)

    def remove_bp(self, addr):
        bp = self.bps.pop(addr & ~1, None)
        if bp and bp.armed:
            self._z(bp, False)
        if bp is self.frame:
            self.frame = None

    def resident(self, module, addr=None, n=16):
        """Does live RAM at addr (default: the module's first word) match the image?"""
        base = self.images.mods[module][0]
        a = addr if addr is not None else base
        return self.read(a, n) == self.images.bytes_at(module, a, n)

    # ------------------------------------------------------------ hits
    def env(self, regs, extra=None):
        e = dict(regs)
        if extra:
            e.update(extra)
        return e

    def ev(self, expr, env):
        return evaluate(expr, env, lambda a, n: self.read_int(a, n))

    def _dump(self, bp, env):
        out = {}
        for label, expr, size in bp.dump:
            try:
                a = self.ev(expr, env)
                out[label] = self.read(a & 0xffffffff, int(size)).hex()
            except Exception as e:
                out[label] = 'ERR %s' % e
        return out

    def _handle_stop(self, pkt):
        """Decode a stop. Returns ('go', None) to continue or ('hit', info) to report."""
        self.running = False
        regs = self.regs()
        pc = regs['pc']
        info = {'pc': '%08x' % pc, 'regs': {k: '%08x' % v for k, v in regs.items()}, 'stop': pkt}
        go = False
        if pc in self.rets:
            slot = self.rets[pc]
            waits = slot['waits']
            for k in range(len(waits) - 1, -1, -1):
                if waits[k]['sp'] == regs['sp']:
                    rec = waits.pop(k)
                    out = {'ev': 'ret', 'name': rec['name'], 'call': rec['call'], 'r0': '%08x' % regs['r0'],
                           'ret_at': '%08x' % pc}
                    src = self.bps.get(rec['addr'])
                    if src is not None and src.dump:
                        env = self.env(regs, {'obj': rec['obj']} if rec['obj'] is not None else None)
                        out.update(('ret_' + k2, v) for k2, v in self._dump(src, env).items())
                    self.event(out)
                    break
            if not waits:
                del self.rets[pc]
                if pc not in self.bps:
                    self._pkt('z0,%x,%d' % (pc, slot['kind']))
            go = True
        bp = self.bps.get(pc)
        if bp is None:
            if go:
                return 'go', None
            self.event({'ev': 'unknown_stop', 'pc': '%08x' % pc, 'stop': pkt})
            info['role'] = None
            return 'hit', info
        if bp.canary is not None and self.read(pc, len(bp.canary)) != bp.canary:
            bp.skips += 1
            return 'go', None
        env = self.env(regs)
        if bp.obj:
            try:
                env['obj'] = self.ev(bp.obj, env) & 0xffffffff
                info['obj'] = '%08x' % env['obj']
            except Exception as e:
                info['obj_error'] = str(e)
        if bp.when:
            try:
                if not self.ev(bp.when, env):
                    return 'go', None
            except Exception as e:
                self.event({'ev': 'when_error', 'name': bp.name, 'err': str(e)})
                return 'go', None
        bp.hits += 1
        info.update(role=bp.role, name=bp.name, hits=bp.hits)
        if bp.role == 'frame':
            self.frames += 1
            info['frames'] = self.frames
            return 'hit', info
        if bp.role == 'probe':
            rec = {'ev': 'probe', 'name': bp.name, 'pc': '%08x' % pc, 'lr': '%08x' % regs['lr'],
                   'r0': '%08x' % regs['r0'], 'r1': '%08x' % regs['r1'], 'r2': '%08x' % regs['r2'],
                   'r3': '%08x' % regs['r3'], 'sp': '%08x' % regs['sp']}
            if 'obj' in info:
                rec['obj'] = info['obj']
            rec.update(self._dump(bp, env))
            self.event(rec)
            if bp.ret:
                lr = regs['lr'] & ~1
                if lr not in self.rets:
                    self.rets[lr] = {'kind': 2 if regs['lr'] & 1 else 4, 'waits': []}
                    if lr not in self.bps:
                        self._pkt('Z0,%x,%d' % (lr, self.rets[lr]['kind']))
                self.rets[lr]['waits'].append({'sp': regs['sp'], 'name': bp.name, 'call': rec['n'],
                                               'addr': bp.addr, 'obj': env.get('obj')})
            if bp.max_hits and bp.hits >= bp.max_hits:
                self._z(bp, False)
            return 'go', None
        # trigger
        info['dump'] = self._dump(bp, env)
        self.event({'ev': 'trigger', 'name': bp.name, 'pc': info['pc'], 'obj': info.get('obj'),
                    'lr': '%08x' % regs['lr'], 'dump': info['dump']})
        if bp.max_hits and bp.hits >= bp.max_hits:
            self._z(bp, False)
        return 'hit', info

    def run(self, until, timeout):
        """Continue; service hits until until(info) is true (target left halted,
        info returned) or timeout (target left running, None returned)."""
        if self.held is not None:
            info, self.held = self.held, None
            if until(info):
                return info
        t_end = time.time() + timeout
        self.cont()
        while True:
            rem = t_end - time.time()
            if rem <= 0:
                return None
            pkt = self.poll_stop(min(rem, 0.5))
            if pkt is None:
                continue
            act, info = self._handle_stop(pkt)
            if act == 'hit' and until(info):
                return info
            if act == 'hit' and info.get('role') not in ('frame', None):
                pass   # an armed trigger nobody is waiting for: logged by _handle_stop
            self.cont()

    def idle_service(self):
        """Called by the daemon between requests: handle async stops."""
        if not self.running or self.dead:
            return
        pkt = self.poll_stop(0.0)
        if pkt is None:
            return
        act, info = self._handle_stop(pkt)
        if act == 'hit' and info.get('role') == 'frame':
            self.cont()            # frame bp left armed while idle: ignore it
        elif act == 'hit':
            self.held = info       # a trigger: keep the game paused for the next waiter
        else:
            self.cont()

    # ------------------------------------------------------------ high level
    def step(self, n=1, timeout=60.0):
        if self.frame is None:
            raise RuntimeError('no frame breakpoint configured (op set_frame)')
        if not self.frame.armed:
            self._z(self.frame, True)
        left = [int(n)]

        def until(info):
            if info.get('role') == 'frame':
                left[0] -= 1
                return left[0] <= 0
            return False
        held, self.held = self.held, None
        if held is not None:
            self.event({'ev': 'held_dropped', 'name': held.get('name')})
        info = self.run(until, timeout)
        if info is None:
            raise TimeoutError('frame breakpoint not hit %d times in %.0fs (module not resident?)' % (n, timeout))
        return info

    def go(self):
        if self.frame is not None and self.frame.armed:
            self._z(self.frame, False)
        self.cont()

    def wait_trigger(self, names=None, timeout=60.0):
        names = set(names or [])

        def until(info):
            return info.get('role') == 'trigger' and (not names or info.get('name') in names)
        if self.frame is not None and self.frame.armed:
            self._z(self.frame, False)
        return self.run(until, timeout)


# ------------------------------------------------------------------- daemon
class Daemon:
    """Owns the Session (and optionally the melonDS process); serves JSON-lines RPC."""

    def __init__(self, session, rpc_port=DEFAULT_RPC_PORT, proc=None, hwnd=None, pid=None):
        self.s = session
        self.rpc_port = rpc_port
        self.proc = proc
        self.pid = pid if pid is not None else (proc.pid if proc else None)
        self.hwnd = hwnd
        self.quit = False
        self.kill_emu = False
        self.clean = False

    # ---- ops: everything a client can ask for. Values are JSON-friendly.
    def op_status(self):
        s = self.s
        return {'running': s.running, 'dead': s.dead, 'frames': s.frames, 'held': s.held is not None,
                'pid': self.pid, 'emu_alive': self.emu_alive(), 'nevents': s.nevents,
                'bps': [b.desc() for b in s.bps.values()]}

    def op_ping(self):
        return self.s.ping()

    def op_read(self, addr, n=4):
        return self.s.read(parse_int(addr), parse_int(n)).hex()

    def op_write(self, addr, hex):
        a = parse_int(addr)
        before = self.s.read(a, len(bytes.fromhex(hex))).hex()
        self.s.write(a, bytes.fromhex(hex))
        after = self.s.read(a, len(bytes.fromhex(hex))).hex()
        self.s.event({'ev': 'write', 'addr': '%08x' % a, 'before': before, 'after': after})
        return {'before': before, 'after': after}

    def op_eval(self, expr, env=None):
        regs = {} if self.s.running else self.s.regs()
        e = self.s.env(regs, {k: parse_int(v) for k, v in (env or {}).items()})
        return self.s.ev(expr, e)

    def op_regs(self):
        return {k: '%08x' % v for k, v in self.s.regs().items()}

    def op_set_frame(self, addr, module=None, name='frame'):
        if self.s.frame is not None:
            self.s.remove_bp(self.s.frame.addr)
        bp = self.s.add_bp(Bp(parse_int(addr), 'frame', name=name, module=module), arm=False)
        return bp.desc()

    def op_add_bp(self, addr, role='trigger', name='', module=None, dump=(), ret=False, max_hits=0,
                  when=None, obj=None, arm=True):
        bp = Bp(parse_int(addr), role, name=name, module=module, dump=dump, ret=ret,
                max_hits=parse_int(max_hits), when=when, obj=obj)
        return self.s.add_bp(bp, arm=arm).desc()

    def op_arm(self, addr, on=True):
        self.s.arm(parse_int(addr), on)
        return True

    def op_remove_bp(self, addr):
        self.s.remove_bp(parse_int(addr))
        return True

    def op_clear_bps(self, role=None):
        for a, b in list(self.s.bps.items()):
            if role is None or b.role == role:
                self.s.remove_bp(a)
        return True

    def op_resident(self, module, addr=None):
        return self.s.resident(module, parse_int(addr) if addr is not None else None)

    def op_step(self, n=1, timeout=60):
        return self.s.step(parse_int(n), float(timeout))

    def op_pause(self, timeout=60):
        return self.s.step(1, float(timeout))

    def op_go(self):
        self.s.go()
        return True

    def op_wait_trigger(self, names=None, timeout=60):
        return self.s.wait_trigger(names, float(timeout))

    def op_run_for(self, seconds):
        """Let the game run (probes keep logging) for a while; ends running."""
        self.s.go()
        t_end = time.time() + float(seconds)
        while time.time() < t_end:
            self.s.idle_service()
            if self.s.held is not None:
                self.s.held = None
                self.s.cont()
            time.sleep(0.005)
        return self.s.frames

    def op_events(self, since=0, max=500):
        ev = [e for e in self.s.events if e['n'] > int(since)]
        return ev[:int(max)]

    def _env(self, env):
        regs = {} if self.s.running else self.s.regs()
        return self.s.env(regs, {k: parse_int(v) for k, v in (env or {}).items()})

    def op_snapshot(self, fields, env=None):
        """fields: [{name, expr, size=1, signed=false}] -> {name: int | hex string | error}."""
        e = self._env(env)
        out = {}
        for f in fields:
            try:
                a = self.s.ev(f['expr'], e) & 0xffffffff
                size = parse_int(f.get('size', 1))
                b = self.s.read(a, size)
                if size <= 4:
                    v = int.from_bytes(b, 'little', signed=bool(f.get('signed')))
                    out[f['name']] = v
                else:
                    out[f['name']] = b.hex()
            except Exception as ex:
                out[f['name']] = 'ERR %s' % ex
        return out

    def op_locate(self, locate, env=None):
        """{'expr': e} or {'walk': {'heads': [e...], 'next': e(node), 'max': n}, 'where': [e(node)...]}
        -> first matching address (int) or None."""
        e = self._env(env)
        if 'expr' in locate:
            return self.s.ev(locate['expr'], e) & 0xffffffff
        w = locate['walk']
        seen = 0
        for h in w['heads']:
            node = self.s.ev(h, e) & 0xffffffff
            while 0x02000000 <= node < 0x02400000 and seen < parse_int(w.get('max', 256)):
                seen += 1
                e['node'] = node
                if all(self.s.ev(c, e) for c in locate.get('where', [])):
                    return node
                node = self.s.ev(w['next'], e) & 0xffffffff
        return None

    def op_poke(self, expr, op='set', mask=0xff, value=0, size=1, env=None):
        """Read-modify-write: op set|clear|flip (under mask), write (value under mask), same."""
        e = self._env(env)
        a = self.s.ev(expr, e) & 0xffffffff
        size = parse_int(size)
        mask = parse_int(mask)
        value = self.s.ev(value, e) if isinstance(value, str) else int(value)   # may read RAM
        old = int.from_bytes(self.s.read(a, size), 'little')
        full = (1 << (8 * size)) - 1
        new = {'set': old | mask, 'clear': old & ~mask, 'flip': old ^ mask, 'same': old,
               'write': (old & ~mask) | (value & mask)}[op] & full
        self.s.write(a, new.to_bytes(size, 'little'))
        back = int.from_bytes(self.s.read(a, size), 'little')
        self.s.event({'ev': 'poke', 'addr': '%08x' % a, 'op': op, 'mask': mask, 'old': old, 'new': back})
        return {'addr': '%08x' % a, 'old': old, 'new': back, 'size': size}

    def op_shot(self, path=None, which='top', settle_ms=250, stable_tries=6):
        """Capture the DS screen(s). Waits settle_ms, then re-captures until two
        consecutive grabs agree (the halted frame has been repainted)."""
        import emuwin
        time.sleep(float(settle_ms) / 1000.0)
        prev = emuwin.capture_screens(self.window(), which)
        stable = False
        for _ in range(int(stable_tries)):
            time.sleep(0.08)
            cur = emuwin.capture_screens(self.window(), which)
            if cur.diff_fraction(prev, tol=4) == 0.0:
                stable = True
                break
            prev = cur
        if path:
            prev.save_png(path)
        return {'w': prev.w, 'h': prev.h, 'path': path, 'stable': stable}

    def op_press(self, buttons, ms=100):
        import emuwin
        if not self.s.running:
            raise RuntimeError('target is halted: the game cannot see input (op go first)')
        emuwin.press(self.window(), buttons if isinstance(buttons, list) else [buttons], int(ms))
        return True

    def op_touch(self, x, y, ms=120):
        import emuwin
        emuwin.touch(self.window(), int(x), int(y), int(ms))
        return True

    def op_load_state(self, slot=1, settle=1.0):
        """Load a savestate slot through the File menu. The target must run while
        melonDS loads (a stub-halted emu thread would deadlock its UI)."""
        import emuwin
        self.s.held = None
        self.s.go()
        out = emuwin.load_state_slot(self.pid, int(slot))
        time.sleep(float(settle))
        self.s.event({'ev': 'load_state', 'slot': int(slot)})
        return out

    def op_save_state(self, slot=1):
        import emuwin
        self.s.go()
        return emuwin.save_state_slot(self.pid, int(slot))

    def op_reconnect(self):
        """Re-attach after a dropped connection (works when melonDS rebuilt its listener)."""
        try:
            self.s.cli.close()
        except Exception:
            pass
        stop = self.s.connect(timeout=20)
        for b in self.s.bps.values():
            if b.armed:
                b.armed = False
                self.s._z(b, True)
        return stop

    def op_quit(self, kill_emu=False, clean=False):
        """Detach cleanly; optionally close melonDS and delete its work directory."""
        self.quit = True
        self.kill_emu = bool(kill_emu)
        self.clean = bool(clean) and self.kill_emu
        return True

    # ---- plumbing
    def window(self):
        import emuwin
        if not self.hwnd or not emuwin.user32.IsWindow(self.hwnd):
            self.hwnd = emuwin.find_window(self.pid)
        if not self.hwnd:
            raise RuntimeError('no melonDS window')
        return self.hwnd

    def emu_alive(self):
        if self.proc is not None:
            return self.proc.poll() is None
        if self.pid:
            r = subprocess.run(['tasklist', '/FI', 'PID eq %d' % self.pid, '/NH'], capture_output=True, text=True)
            return str(self.pid) in r.stdout
        return None

    def handle(self, req):
        op = req.get('op')
        fn = getattr(self, 'op_' + str(op), None)
        if fn is None:
            return {'id': req.get('id'), 'ok': False, 'error': 'unknown op %r' % op}
        try:
            res = fn(**req.get('args', {}))
            return {'id': req.get('id'), 'ok': True, 'result': res}
        except StubDropped as e:
            return {'id': req.get('id'), 'ok': False, 'error': 'stub_dropped', 'detail': str(e),
                    'emu_alive': self.emu_alive()}
        except Exception as e:
            return {'id': req.get('id'), 'ok': False, 'error': '%s: %s' % (type(e).__name__, e)}

    def serve(self, keepalive=2.0):
        ls = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ls.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        ls.bind(('127.0.0.1', self.rpc_port))
        ls.listen(4)
        clients = {}
        last_ka = time.time()
        while not self.quit:
            r, _, _ = select.select([ls] + list(clients), [], [], 0.02)
            for sock in r:
                if sock is ls:
                    c, _ = ls.accept()
                    clients[c] = b''
                    continue
                try:
                    data = sock.recv(65536)
                except OSError:
                    data = b''
                if not data:
                    clients.pop(sock, None)
                    sock.close()
                    continue
                clients[sock] += data
                while b'\n' in clients[sock]:
                    line, clients[sock] = clients[sock].split(b'\n', 1)
                    if not line.strip():
                        continue
                    try:
                        req = json.loads(line)
                    except ValueError:
                        req = {'op': None}
                    resp = self.handle(req)
                    try:
                        sock.sendall((json.dumps(resp) + '\n').encode())
                    except OSError:
                        pass
            if self.s.dead:
                continue
            try:
                self.s.idle_service()
                if time.time() - last_ka > keepalive and time.time() - self.s.last_io > keepalive:
                    self.s.ping()
                    last_ka = time.time()
            except StubDropped:
                pass
            if self.proc is not None and self.proc.poll() is not None and not self.s.dead:
                self.s.dead = 'melonDS exited (%s)' % self.proc.returncode
                self.s.event({'ev': 'emu_exited'})
        # teardown: clear breakpoints, detach (D) so the stub stays usable
        if not self.s.dead:
            try:
                for a in list(self.s.bps):
                    self.s.remove_bp(a)
                for lr, slot in list(self.s.rets.items()):
                    self.s._pkt('z0,%x,%d' % (lr, slot['kind']))
                self.s.cli.detach()
                self.s.cli.close()
            except Exception:
                pass
        for c in clients:
            c.close()
        ls.close()
        if self.kill_emu and self.pid:
            import emuwin
            emuwin.kill(self.pid)


class RemoteError(RuntimeError):
    pass


class Client:
    """Talks to a running daemon. Every method maps to a Daemon op_<name>."""

    def __init__(self, port=DEFAULT_RPC_PORT, timeout=900):
        self.sock = socket.create_connection(('127.0.0.1', port), timeout=10)
        self.sock.settimeout(timeout)
        self.buf = b''
        self.n = 0

    def call(self, _op, **args):
        self.n += 1
        self.sock.sendall((json.dumps({'id': self.n, 'op': _op, 'args': args}) + '\n').encode())
        while b'\n' not in self.buf:
            d = self.sock.recv(65536)
            if not d:
                raise RemoteError('daemon closed the connection')
            self.buf += d
        line, self.buf = self.buf.split(b'\n', 1)
        r = json.loads(line)
        if not r.get('ok'):
            raise RemoteError('%s: %s' % (r.get('error'), r.get('detail', '')))
        return r.get('result')

    def __getattr__(self, op):
        if op.startswith('_'):
            raise AttributeError(op)
        return lambda **kw: self.call(op, **kw)

    def close(self):
        self.sock.close()


def daemon_up(port=DEFAULT_RPC_PORT):
    try:
        socket.create_connection(('127.0.0.1', port), timeout=0.5).close()
        return True
    except OSError:
        return False
