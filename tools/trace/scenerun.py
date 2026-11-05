"""scenerun: drive one scene through a running `ask.py serve` daemon.

A run loads a savestate, aligns on a hooked PC + condition, then steps hooked
frames. At every hooked frame the pad and touch records are forced, neutral when
the script holds nothing, so neither the host keyboard nor leftover input can
reach the game, and the script's ops for that frame are applied.

    align  {"pc": "0x0202c2ac", "when": "[0x020a0db0] == {file:0x020a0db0} + 300"}
           (the default). `{file:ADDR}` is the u32 at ADDR in the savestate
           file's RAM, so every run aligns on the same game frame however long
           the load took. Frame 0 is the aligning hit; frame k is the k-th
           hooked call after it. Snapshots at frame k are taken at that hook,
           before frame k's input is written.

    ops    a list of objects, each applied on its frames:
           {"at": 0, "frames": 8, "buttons": ["B"]}     held for `frames` hooked
                                                        frames (at least 2: the
                                                        game polls at 30 Hz);
                                                        "pressed" on the first
           {"at": 0, "frames": 30, "touch": [192, 96]}  same, the touch screen
           {"at": 0, "bind": "coin", "nearest_actor": 288}
                                                        env name = the actor with
                                                        that id nearest the player
           {"at": 0, "write": "coin+0x5c", "u32": ["[player+0x5c]",
                    "[player+0x60]+0x8000", "[player+0x64]"]}
                                                        memory write: u8/u16/u32
                                                        value expression(s), or
                                                        "hex": "..."
           Expressions are emusession's: [e], u8[e], u16[e], + - * & |, ==, and
           the names `player` (u32[0x0209f394]), `frame`, and bound names.

Injection (as the first RAM-diff pass did, verified there against named
effects): at the hook, CheckInput entry, the raw pad record data_020a0e58[0]
{u16 held, u16 pressed} (NDS KEYINPUT bit order) and the touch record gTouchHeld
{u8 held, u8 edge, u8 x, u8 y} at 0x020a0de8 are overwritten; CheckInput builds
the controller record from them.
"""
import json
import os

import emusession as es

CHECKINPUT = 0x0202c2ac
FRAME_TIMER = 0x020a0db0
PAD = 0x020a0e58
TOUCH = 0x020a0de8
PLAYER_PTR = 0x0209f394
ACTOR_LIST = 0x0209b468
DEFAULT_ALIGN = {'pc': '0x0202c2ac', 'when': '[0x020a0db0] == {file:0x020a0db0} + 300', 'timeout': 60}

KEY = {'A': 1, 'B': 2, 'SELECT': 4, 'START': 8, 'RIGHT': 0x10, 'LEFT': 0x20, 'UP': 0x40, 'DOWN': 0x80,
       'R': 0x100, 'L': 0x200, 'X': 0x400, 'Y': 0x800}
MIN_HOLD = 2


def load_json(path):
    with open(path, encoding='utf-8') as f:
        return json.load(f)


class Script:
    """A validated list of ops (see the module docstring)."""

    def __init__(self, ops):
        self.ops = []
        for op in ops or []:
            op = dict(op)
            op['at'] = int(op.get('at', 0))
            if 'buttons' in op or 'touch' in op:
                op['frames'] = int(op.get('frames', MIN_HOLD))
                if op['frames'] < MIN_HOLD:
                    raise ValueError('op %r: hold at least %d frames (the game polls the pad at 30 Hz)'
                                     % (op, MIN_HOLD))
                if 'buttons' in op:
                    bad = [b for b in op['buttons'] if b.upper() not in KEY]
                    if bad:
                        raise ValueError('op %r: unknown buttons %s (%s)' % (op, bad, ' '.join(KEY)))
            elif 'write' in op:
                if not any(k in op for k in ('u8', 'u16', 'u32', 'hex')):
                    raise ValueError('op %r: write needs u8, u16, u32 or hex' % op)
            elif 'bind' not in op:
                raise ValueError('op %r: expected buttons, touch, write or bind' % op)
            self.ops.append(op)
        self.last = max([o['at'] + o.get('frames', 1) for o in self.ops] or [0])

    def held(self, f):
        """(held mask, touch (x, y) or None) for hooked frame f."""
        m, t = 0, None
        for o in self.ops:
            if o['at'] <= f < o['at'] + o.get('frames', 1):
                if 'buttons' in o:
                    for b in o['buttons']:
                        m |= KEY[b.upper()]
                if 'touch' in o:
                    t = (int(o['touch'][0]), int(o['touch'][1]))
        return m, t

    def actions(self, f):
        return [o for o in self.ops if o['at'] == f and ('write' in o or 'bind' in o)]


class Runner:
    def __init__(self, client=None, port=es.DEFAULT_RPC_PORT):
        self.c = client or es.Client(port)
        self.backend = self.c.backend().get('backend')
        self.env = {}
        self.log = []

    # ---- memory helpers through the daemon
    def read(self, a, n):
        return bytes.fromhex(self.c.read(addr=a, n=n))

    def u32(self, a):
        return int.from_bytes(self.read(a, 4), 'little')

    def ev(self, expr):
        return es.evaluate(expr, self.env, lambda a, n: int.from_bytes(self.read(a, n), 'little'))

    # ---- one run
    def load(self, state):
        self.c.clear_bps()
        r = self.c.load_state_file(path=os.path.abspath(state))
        if r.get('verdict') != 'loaded':
            raise RuntimeError('savestate did not load: %s' % r)
        if self.backend == 'ctl':
            self.c.fork(cmd='release_input')      # the keypad too: nothing held, API-owned
        return r

    def align(self, state, align=None):
        al = dict(DEFAULT_ALIGN, **(align or {}))
        when = al['when']
        if '{file:' in when:
            ram = es.savestate_ram(state)
            import re
            when = re.sub(r'\{file:([^}]+)\}',
                          lambda m: '0x%x' % int.from_bytes(ram(es.parse_int(m.group(1)), 4), 'little'), when)
        pc = es.parse_int(al['pc'])
        self.c.add_bp(addr=pc, role='trigger', name='align', module='arm9', when=when, max_hits=1)
        info = self.c.wait_trigger(names=['align'], timeout=float(al.get('timeout', 60)))
        if info is None:
            raise RuntimeError('align: %s at %08x not reached' % (when, pc))
        self.c.remove_bp(addr=pc)
        self.c.set_frame(addr=pc, module='arm9', name='hook')
        self.align_when = when
        return info

    def status(self):
        if self.backend != 'ctl':
            return {}
        st = self.c.fork(cmd='status')
        return {k: st.get(k) for k in ('host_input_ignored', 'input_override', 'jit_enabled', 'nds_frame')}

    def frames(self, script, n, on_frame=None):
        """Step hooked frames 0..n. on_frame(f) runs at each hook before injection
        (frame n: on_frame only). Leaves the target halted at frame n's hook."""
        self.env = {'frame': 0}
        prev_held = 0
        touching = False
        t0 = self.read(TOUCH, 4)
        tx, ty = t0[2], t0[3]
        for f in range(n + 1):
            if f > 0:
                self.c.step(n=1)
            self.env['frame'] = f
            self.env['player'] = self.u32(PLAYER_PTR)
            if on_frame is not None:
                on_frame(f)
            if f == n:
                break
            held, touch = script.held(f)
            pressed = held & ~prev_held
            prev_held = held
            if touch is not None:
                tx, ty = touch
                rec = bytes([1, 0 if touching else 1, tx, ty])
                touching = True
            else:
                rec = bytes([0, 1 if touching else 0, tx, ty])
                touching = False
            writes = [['0x%08x' % PAD, (held.to_bytes(2, 'little') + pressed.to_bytes(2, 'little')).hex()],
                      ['0x%08x' % TOUCH, rec.hex()]]
            for op in script.actions(f):
                if 'bind' in op:
                    self.env[op['bind']] = self.bind(op)
                    continue
                a = self.ev(op['write']) & 0xffffffff
                if 'hex' in op:
                    data = bytes.fromhex(op['hex'])
                else:
                    w = next(k for k in ('u8', 'u16', 'u32') if k in op)
                    vals = op[w] if isinstance(op[w], list) else [op[w]]
                    size = {'u8': 1, 'u16': 2, 'u32': 4}[w]
                    data = b''.join((self.ev(v) & ((1 << (8 * size)) - 1)).to_bytes(size, 'little') for v in vals)
                writes.append(['0x%08x' % a, data.hex()])
                self.log.append({'frame': f, 'write': '%08x' % a, 'hex': data.hex()})
            self.c.write_many(writes=writes)
        return n

    def bind(self, op):
        if 'nearest_actor' in op:
            ident = int(op['nearest_actor'])
            pl = self.env['player']
            pp = self._pos(pl)
            best = None
            for a, aid in self.actors():
                if aid != ident:
                    continue
                p = self._pos(a)
                d = sum((p[i] - pp[i]) ** 2 for i in range(3))
                if best is None or d < best[0]:
                    best = (d, a, p)
            if best is None:
                raise RuntimeError('bind %s: no actor id %d' % (op['bind'], ident))
            self.log.append({'frame': self.env['frame'], 'bind': op['bind'], 'actor': '%08x' % best[1],
                             'pos': best[2], 'player_pos': pp})
            return best[1]
        return self.ev(op['expr'])

    def _pos(self, a):
        b = self.read(a + 0x5c, 12)
        return tuple(int.from_bytes(b[4 * i:4 * i + 4], 'little', signed=True) for i in range(3))

    def actors(self):
        """[(actor, actor id)] from the global actor list (list node at actor + 0x50)."""
        out, n, k = [], self.u32(ACTOR_LIST), 0
        while 0x02000000 <= n < 0x02400000 and k < 4000:
            a = self.u32(n + 8)
            if 0x02000000 <= a < 0x02400000:
                out.append((a, int.from_bytes(self.read(a + 0x0c, 2), 'little')))
            n, k = self.u32(n + 4), k + 1
        return out

    def resident(self):
        """Overlay ids in the game's loaded-overlay table."""
        return sorted(o['id'] for o in self.c.overlays()['loaded'])

    def finish(self):
        self.c.clear_bps()
        self.c.go()
