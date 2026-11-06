"""ctlsession: emusession's Session and Daemon over the control socket of the
melonDS AI-controller fork, instead of the stock melonDS GDB stub.

The daemon's RPC ops, the spec format and ask.py's blinding are the same as with
the stub backend; what changes underneath:

  savestates   load_state / save_state by path (no File menu, no slot copy, no
               melonDS.toml swap); the RAM-marker verdict is kept
  shots        the emulator's framebuffer (exact, no window, no settle), upscaled
               by --scale with nearest neighbour so pairs keep their old size
  input        exact-frame: press/touch/input hold for N emulated frames
               (SM64DS polls the pad every other frame: hold at least 2)
  breakpoints  in-process: the overlay canary is a native condition, so a hit
               on another overlay's code never halts; frame counting runs the
               emulator with synchronous unthrottled `step` calls
  RAM          op_ram_snapshot reads every range at one emulated instant

See README.md ("Control-socket backend").
"""
import base64
import ctypes
import os
import re
import shutil
import subprocess
import time

import ctlclient as cc
import emusession as es

FPS = 60                # NDS frames per second (the game's logic runs at 30)
CHUNK = 30              # frames per synchronous step while nothing needs polling
CHUNK_WATCHING = 4      # ... while suspended probes wait for their overlay


class _Cli:
    """What the stub backend's callers do with Session.cli: detach and close."""

    def __init__(self, ctl):
        self.ctl = ctl

    def detach(self):
        """Leave the game running with nothing installed, as a GDB detach does."""
        for cmd in ('breakpoint_clear', 'continue'):
            try:
                self.ctl.call(cmd, timeout=10)
            except (OSError, ValueError, cc.CtlError):
                pass

    def close(self):
        self.ctl.close()


class CtlSession(es.Session):
    backend = 'ctl'

    def __init__(self, images, port=None, token=None, log_path=None):
        super().__init__(images, port=port or cc.default_port(), log_path=log_path)
        self.token = token or cc.default_token()
        self.ctl = None
        self.seq = 0               # the last breakpoint-hit sequence this session processed
        self._regs = None          # registers of the hit the target is halted at
        self.ping_info = None

    # ------------------------------------------------------------ transport
    def connect(self, timeout=60.0):
        t0 = time.time()
        last = None
        while time.time() - t0 < timeout:
            try:
                self.ctl = cc.CtlClient(self.port, self.token).connect()
                self.ping_info = self.ctl.call('ping', timeout=5)
                break
            except (OSError, ValueError, cc.CtlError) as e:
                last = e
                if self.ctl:
                    self.ctl.close()
                self.ctl = None
                time.sleep(1.0)
        if not self.ctl:
            raise es.StubDropped('no control server on port %d: %r' % (self.port, last))
        self.cli = _Cli(self.ctl)
        self.dead = None
        st = self._call('status')
        while st.get('state') == 'inactive' and time.time() - t0 < timeout:
            time.sleep(0.25)            # the server answers before the ROM has booted
            st = self._call('status')
        if st.get('state') == 'inactive':
            raise RuntimeError('the fork answers, but no ROM is running')
        if not st.get('managed_breakpoints_available'):
            raise RuntimeError('the fork reports no managed breakpoints (JIT on?): %s' % st)
        # like a stub attach: nothing installed, target stopped
        self._call('breakpoint_clear')
        self._call('pause')
        st = self._call('status')
        self.seq = int((st.get('last_breakpoint_hit') or {}).get('sequence') or 0)
        self.running = False
        self._regs = None
        self.event({'ev': 'attach', 'backend': 'ctl', 'port': self.port, 'state': st.get('state'),
                    'server': self.ping_info.get('server'), 'nds_frame': st.get('nds_frame')})
        try:
            self.refresh_overlays()
        except (RuntimeError, ValueError):
            self.loaded = None
        return 'ctl:%s' % st.get('state')

    def _call(self, cmd, timeout=None, **args):
        if self.dead:
            raise es.StubDropped(self.dead)
        try:
            r = self.ctl.call(cmd, timeout=timeout, **args)
        except (OSError, ValueError) as e:      # ConnectionError is an OSError
            self._drop(e)
        self.last_io = time.time()
        return r

    def _pkt(self, payload):
        """The stub backend's breakpoint packets (ret-on-lr and teardown use them)."""
        parts = payload.split(',')
        if len(parts) == 3 and parts[0] in ('Z0', 'z0'):
            a = int(parts[1], 16)
            if parts[0] == 'Z0':
                self._call('breakpoint_add', address=a)
            else:
                self._call('breakpoint_remove', address=a)
            return 'OK'
        raise RuntimeError('ctl backend: no GDB packet %r' % payload)

    def _take_hit(self, h):
        """Record a hit (the fork's last_breakpoint_hit): halted, its registers cached."""
        self.seq = int(h['sequence'])
        self.running = False
        regs = {k: int(v, 16) for k, v in h['registers'].items()}
        regs['pc'] = int(h['address'], 16)
        regs.update(sp=regs['r13'], lr=regs['r14'])
        self._regs = regs
        return 'ctl hit %d' % self.seq

    def poll_stop(self, timeout):
        if self.pending:
            p, self.pending = self.pending, None
            return p
        if not self.running:
            return None
        ms = int(max(0.0, timeout) * 1000)
        r = self._call('wait_for_break', timeout=ms / 1000.0 + 10, after_sequence=self.seq, timeout_ms=ms)
        if not r.get('hit'):
            return None
        return self._take_hit(r['last_breakpoint_hit'])

    # ------------------------------------------------------------ primitives
    def read(self, addr, n):
        r = self._call('read_memory', address=int(addr), size=int(n), encoding='base64')
        return base64.b64decode(r['data'])

    def write(self, addr, data):
        self._call('write_memory', address=int(addr), base64=base64.b64encode(bytes(data)).decode())

    def read_ranges(self, ranges):
        """[(addr, size)] read at one emulated instant -> (meta, [bytes])."""
        r = self._call('read_ranges', timeout=120, ranges=[[int(a), int(n)] for a, n in ranges])
        blob = base64.b64decode(r.pop('data'))
        return r, [blob[x['offset']:x['offset'] + x['size']] for x in r['ranges']]

    def regs(self):
        if self.running:
            raise RuntimeError('target is running')
        if self._regs is not None:
            return dict(self._regs)
        r = self._call('registers')
        d = {k: int(v, 16) if isinstance(v, str) else int(v) for k, v in r['registers'].items()}
        d.update(sp=d['r13'], lr=d['r14'], pc=int(r['pc'], 16) if isinstance(r['pc'], str) else int(r['pc']))
        return d

    def cont(self):
        if self.running:
            return
        self._regs = None
        self._call('continue')
        self.running = True

    # ------------------------------------------------------------ breakpoints
    @staticmethod
    def _canary_cond(bp):
        """A native condition: the code at the breakpoint is the module's (another
        overlay at the same address never halts the emulator)."""
        c = bp.canary
        if not c:
            return None
        w = 2 if bp.thumb else 4
        ty = 'u16' if bp.thumb else 'u32'
        return ' && '.join('%s[0x%x] == 0x%x' % (ty, bp.addr + i, int.from_bytes(c[i:i + w], 'little'))
                           for i in range(0, len(c), w))

    def _z(self, bp, on):
        if on:
            args = {'address': bp.addr}
            cond = self._canary_cond(bp)
            if cond:
                args['condition'] = cond
            self._call('breakpoint_add', **args)
        else:
            self._call('breakpoint_remove', address=bp.addr)
        bp.armed = on

    def maybe_refresh(self, force=False):
        """Reads never cross a stop here, so the table can be read while running."""
        if self._in_refresh or not self._watching():
            return
        if not force and time.time() - self.ovl_read_at < self.ovl_poll:
            return
        self._in_refresh = True
        try:
            self.refresh_overlays()
        finally:
            self._in_refresh = False

    # ------------------------------------------------------------ running
    def _step_chunk(self, n):
        """Step n frames, or to the next hit. Returns (frames run, stop or None)."""
        self._regs = None
        try:
            r = self._call('step', timeout=300, frames=int(n), expect_sequence=self.seq)
        except cc.CtlError as e:
            if 'unprocessed breakpoint hit' not in str(e):
                raise
            # a hit landed while free running, after the last poll: take it first
            r = self._call('breakpoint_list', include_log=False)
            return 0, self._take_hit(r['last_breakpoint_hit'])
        self.running = False
        if r.get('interrupted_by_breakpoint'):
            return int(r.get('frames_run', 0)), self._take_hit(r['last_breakpoint_hit'])
        return int(r.get('frames_run', n)), None

    def run(self, until, timeout):
        """As Session.run, but timed in emulated frames (timeout * 60) and driven by
        synchronous unthrottled steps; a wall-clock cap guards a stuck emulator.
        On timeout the target is left free running, as with the stub."""
        if self.held is not None:
            info, self.held = self.held, None
            if until(info):
                return info
        if self.running:
            pkt = self.poll_stop(0.0)
            if pkt is not None:
                act, info = self._handle_stop(pkt)
                if act == 'hit' and until(info):
                    return info
        budget = max(1, int(round(float(timeout) * FPS)))
        t_end = time.time() + float(timeout) * 4 + 30
        while budget > 0 and time.time() < t_end:
            ran, pkt = self._step_chunk(min(budget, CHUNK_WATCHING if self._watching() else CHUNK))
            budget -= ran
            if pkt is None:
                self.maybe_refresh()
                continue
            act, info = self._handle_stop(pkt)
            if act == 'hit' and until(info):
                return info
        self.cont()
        return None

    def idle_service(self):
        if not self.running or self.dead:
            return
        if not self._any_armed():
            self.maybe_refresh()
            return
        super().idle_service()


# ------------------------------------------------------------------- daemon
class _PidProc:
    """poll()/wait() for a process this daemon did not spawn itself (Windows)."""

    def __init__(self, pid):
        self.pid = int(pid)
        self.returncode = None
        self._k = ctypes.windll.kernel32
        self._h = self._k.OpenProcess(0x1000 | 0x00100000, False, self.pid)   # QUERY_LIMITED | SYNCHRONIZE

    def poll(self):
        if self.returncode is None and self._h:
            code = ctypes.c_ulong()
            if self._k.GetExitCodeProcess(self._h, ctypes.byref(code)) and code.value != 259:   # STILL_ACTIVE
                self.returncode = code.value
        return self.returncode

    def wait(self, timeout=None):
        if self._h:
            self._k.WaitForSingleObject(self._h, 0xFFFFFFFF if timeout is None else int(timeout * 1000))
        return self.poll()


def _upscale(frame, k):
    import emuwin
    if k <= 1:
        return frame
    out = bytearray()
    for y in range(frame.h):
        row = frame.row(y)
        wide = b''.join(row[i:i + 3] * k for i in range(0, len(row), 3))
        out += wide * k
    return emuwin.Frame(frame.w * k, frame.h * k, out)


class CtlDaemon(es.Daemon):
    def __init__(self, session, rpc_port=es.DEFAULT_RPC_PORT, proc=None, workdir=None, scale=2):
        super().__init__(session, rpc_port, proc=proc, hwnd=None, workdir=workdir)
        self.scale = int(scale)
        self.ctl_quit = False      # stop --kill: quit the fork through its control socket
        self._load_path = None

    # ---- what the stub backend does with the window
    def window(self):
        raise RuntimeError('ctl backend: no emulator window is used (shots come from the framebuffer)')

    def _grab(self, which, settle_ms, stable_tries):
        import emuwin
        w, h, rgb = self.s.ctl.screen_rgb(which)
        return _upscale(emuwin.Frame(w, h, rgb), self.scale), True, 0

    def _input(self, buttons=None, touch=None, frames=2):
        frames = max(1, es.parse_int(frames))
        args = {'buttons': list(buttons or []), 'frames': frames}
        args['touch'] = [int(touch[0]), int(touch[1])] if touch else False
        self.s._call('set_input', **args)
        return {'exact': True, 'frames': frames}

    def op_press(self, buttons, ms=100):
        """Hold buttons for ms of emulated time (at least 2 frames: the game polls at 30 Hz)."""
        bs = buttons if isinstance(buttons, list) else [buttons]
        return self._input(bs, None, max(2, int(round(float(ms) * FPS / 1000.0))))

    def op_touch(self, x, y, ms=120):
        return self._input(None, (x, y), max(2, int(round(float(ms) * FPS / 1000.0))))

    def op_input(self, buttons=None, touch=None, frames=2):
        """Hold buttons and/or touch [x, y] for exactly `frames` emulated frames,
        starting with the next frame the emulator runs."""
        return self._input(buttons, touch, frames)

    # ---- savestates by path
    def slot_path(self, slot):
        if not self.workdir:
            raise RuntimeError('the daemon has no work dir for slot files; use load_state_file')
        return os.path.join(self.workdir, 'game.ml%d' % int(slot))

    def op_load_state(self, slot=1, settle=1.0):
        p = self.slot_path(slot)
        if not os.path.exists(p) or os.path.getsize(p) == 0:
            raise RuntimeError('slot %d holds no savestate (%s); use load_state_file' % (int(slot), p))
        return self.op_load_state_file(p, slot=slot, settle=settle)

    def _released(self, cmd, **args):
        """A command the fork refuses while halted, sent once the halt is released."""
        for _ in range(40):
            try:
                return self.s._call(cmd, **args)
            except cc.CtlError as e:
                if 'breakpoint' not in str(e) and 'halted' not in str(e):
                    raise
                time.sleep(0.05)
        return self.s._call(cmd, **args)

    def _load_slot(self, slot, settle):
        p = self._load_path or self.slot_path(slot)
        st = self._released('load_state', timeout=60, path=p, paused=False)
        self.s.running = True
        self.s._regs = None
        time.sleep(float(settle))       # as with the stub: the game runs a moment after the load
        self.s.event({'ev': 'load_state', 'path': p, 'nds_frame': st.get('nds_frame')})
        return 'ctl load_state %s' % p

    def op_load_state_file(self, path, slot=8, settle=1.0, verify=True, tries=2, marker=True, dry_run=False):
        """Load any savestate file by path (no slot copy, no menu). Verified as with
        the stub backend: the RAM marker gives the verdict (see Daemon.op_load_state_file)."""
        path = os.path.abspath(path)
        if not os.path.isfile(path):
            raise RuntimeError('no savestate %s' % path)
        file_ram = es.savestate_ram(path, self.s.images) if verify else None
        res = {'path': path, 'backend': 'ctl'}
        if dry_run:
            res['dry_run'] = True
        self._load_path = path
        try:
            self._menu_unarmed(lambda: self._load_verified(res, file_ram, slot, settle, verify, tries, marker,
                                                           dry_run))
        finally:
            self._load_path = None
        self.s.event(dict(res, ev='load_state_file'))
        return res

    def _verify_windows(self):
        return self.s.read_ranges(es.VERIFY_WINDOWS)[1]

    def op_save_state(self, slot=1):
        return self.op_save_state_file(self.slot_path(slot))

    def op_save_state_file(self, path, slot=8, timeout=30):
        """Write a savestate straight to path."""
        path = os.path.abspath(path)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        self._menu_unarmed(lambda: self._released('save_state', timeout=float(timeout) + 30, path=path))
        self.s.event({'ev': 'save_state_file', 'path': path})
        return {'path': path, 'bytes': os.path.getsize(path), 'backend': 'ctl'}

    # ---- RAM, status, raw access
    def _read_ranges(self, ranges):
        meta, blobs = self.s.read_ranges(ranges)
        self._rr_meta = {'nds_frame': meta.get('nds_frame'), 'completed_frames': meta.get('completed_frames'),
                         'at_breakpoint': meta.get('stopped_at_breakpoint'), 'atomic': True}
        return blobs

    def _ranges_meta(self):
        return getattr(self, '_rr_meta', {})

    def op_backend(self):
        p = self.s.ping_info or {}
        return {'backend': 'ctl', 'port': self.s.port, 'server': p.get('server'),
                'melonds_version': p.get('melonds_version'), 'capabilities': p.get('capabilities')}

    def op_status(self):
        out = super().op_status()
        out['backend'] = 'ctl'
        try:
            st = self.s._call('status')
            lst = self.s._call('breakpoint_list', include_log=False)
        except RuntimeError:
            return out
        out.update(state=st.get('state'), nds_frame=st.get('nds_frame'), completed_frames=st.get('completed_frames'))
        native = {}
        for b in lst.get('breakpoints', []):
            a = b.get('address')
            native[int(a, 16) if isinstance(a, str) else int(a)] = b
        for d in out['bps']:
            nb = native.get(int(d['addr'], 16))
            if nb is not None and self.s.bps[int(d['addr'], 16)].canary is not None:
                # the canary is the native condition: false evaluations are its skips
                d['canary_skips'] = int(nb.get('evaluations', 0)) - int(nb.get('hits', 0))
        return out

    def op_fork(self, cmd, args=None):
        """One raw control-socket command (coverage_*, overlays, research_snapshot ...).
        Commands that move the emulator (continue, step, load_state ...) bypass the
        session's bookkeeping, so the session re-reads the run state afterwards."""
        if cmd in ('quit', 'breakpoint_clear', 'breakpoint_add', 'breakpoint_remove'):
            raise RuntimeError('use stop --kill / the daemon breakpoint ops for %r' % cmd)
        r = self.s._call(cmd, **(args or {}))
        st = self.s._call('status')
        halted = st.get('state') == 'breakpoint'
        hit = st.get('last_breakpoint_hit') or {}
        if int(hit.get('sequence') or 0) != self.s.seq:
            self.s.seq = int(hit.get('sequence') or 0)
            self.s._regs = None
        self.s.running = not halted
        return r

    # ---- `this` sweep: log-action breakpoints (they never halt) on many entries
    def _this_cond(self, w):
        parts = []
        if w['canary']:
            ty, step = ('u16', 2) if w['thumb'] else ('u32', 4)
            c = w['canary']
            parts += ['%s[0x%x] == 0x%x' % (ty, w['addr'] + i, int.from_bytes(c[i:i + step], 'little'))
                      for i in range(0, len(c), step)]
        parts += ['u32[r0] != 0x%x' % k for k in sorted(k for k in w['keys'] if isinstance(k, int))]
        return ' && '.join(parts)

    def _this_arm(self, w):
        """(Re-)add w's log breakpoint: a fresh log, hits and evaluations reset."""
        smp = getattr(self, '_this_sample', None)
        if smp:
            # one breakpoint for the whole run: the fork keeps a uniform sample of every
            # hit (reservoir), with the argument registers and bytes at each of r0-r3
            args = {'address': w['addr'], 'action': 'log', 'sample': 'reservoir', 'registers': 'args',
                    'log_limit': smp['log_limit'],
                    'dump': [{'address': 'r%d' % i, 'size': max(4, smp['arg_bytes'])} for i in range(4)]}
            cond = self._this_cond(dict(w, keys={}))
            if cond:
                args['condition'] = cond
            self.s._call('breakpoint_add', **args)
            w['eval_base'] = 0
            return
        args = {'address': w['addr'], 'action': 'log', 'dump': [{'address': 'r0', 'size': 0x10}]}
        cond = self._this_cond(w)
        if cond:
            args['condition'] = cond
        self.s._call('breakpoint_add', **args)
        w['eval_base'] = 0

    def op_this_watch(self, functions, max_keys=32, sample=None, log_limit=64, arg_bytes=16):
        """functions: [{addr, module, thumb, name}] (module 'arm9', 'itcm' or 'ovNNN').

        sample=None (distinct): each entry gets a log breakpoint that records r0 and
        16 bytes at r0, but only for a vptr (u32[r0]) not yet seen there. Poll
        this_collect every few frames (a breakpoint logs at most 4096 hits between
        polls); it re-arms with the seen vptrs filtered out.

        sample='reservoir': each entry gets one log breakpoint for the whole run that
        keeps a uniform sample of log_limit hits out of every hit (the fork's reservoir
        log), each with r0-r3, r12, sp, lr and arg_bytes bytes at each of r0-r3. No
        hit halts and none is re-armed, so the samples spread over the run instead of
        bunching at its start. this_collect reads the current sample (call it at the end).

        Either way an overlay function's condition also checks its code bytes, so
        another overlay's code at the same address never logs."""
        import symtab
        if any(b.armed for b in self.s.bps.values() if b.role != 'frame'):
            raise RuntimeError('this_watch: clear the daemon probes/triggers first')
        if sample not in (None, 'distinct', 'reservoir'):
            raise RuntimeError('this_watch: sample must be distinct or reservoir')
        self._this_unwatch()
        self._watch = {}
        self._this_max = int(max_keys)
        self._this_sample = ({'log_limit': int(log_limit), 'arg_bytes': int(arg_bytes)}
                             if sample == 'reservoir' else None)
        for f in functions:
            a = es.parse_int(f['addr'])
            mod = f.get('module') or 'arm9'
            thumb = bool(f.get('thumb') or (a & 1))
            a &= ~1
            if a in self.s.bps:
                raise RuntimeError('this_watch: %08x is a daemon breakpoint' % a)
            canary = self.s.images.bytes_at(mod, a, 8) if mod.startswith('ov') else None
            w = {'addr': a, 'module': mod, 'thumb': thumb, 'name': f.get('name'), 'canary': canary,
                 'keys': {}, 'hits': 0, 'evaluations': 0, 'logged': 0, 'truncated': 0, 'many': False}
            self._watch[a] = w
            self._this_arm(w)
        self._this_table = symtab.table()
        self._this_names = symtab.actor_names()
        return {'watching': len(self._watch)}

    def op_this_collect(self):
        """Fold the logs into the per-function vptr sets, then re-arm with the new
        vptrs filtered out. Returns how many new (function, vptr) pairs it found."""
        import symtab
        watch = getattr(self, '_watch', None) or {}
        if not watch:
            return {'new': 0}
        lst = self.s._call('breakpoint_list', include_log=True)
        self.s.refresh_overlays()
        mods = ['arm9'] + ['ov%03d' % i for i in sorted(self.s.loaded or ())]
        cache = self.__dict__.setdefault('_vptr_cache', {})
        if getattr(self, '_this_sample', None):
            return self._this_collect_sample(lst, mods, cache)
        new = 0
        for b in lst.get('breakpoints', []):
            a = b.get('address')
            a = int(a, 16) if isinstance(a, str) else int(a)
            w = watch.get(a)
            if w is None or w['many']:
                continue
            ev = int(b.get('evaluations', 0))
            w['evaluations'] += ev - w['eval_base']
            w['eval_base'] = ev
            log = b.get('log') or []
            if not log:
                continue
            hits = int(b.get('hits', 0))
            w['hits'] += hits
            w['logged'] += len(log)
            w['truncated'] += max(0, hits - len(log))
            for h in log:
                d = (h.get('dumps') or [{}])[0].get('hex')
                r0 = int(h['registers']['r0'], 16)
                raw = bytes.fromhex(d) if d else b''
                key = int.from_bytes(raw[:4], 'little') if len(raw) >= 4 else 'unreadable'
                k = w['keys'].get(key)
                if k is None:
                    k = {'n': 0, 'obj': '%08x' % r0, 'lr': h['registers'].get('r14'), 'actor_ids': []}
                    if isinstance(key, int):
                        k.update(symtab.resolve_vptr(self.s.read, key, mods, self._this_table, cache))
                    w['keys'][key] = k
                    new += 1
                k['n'] += 1
                if k.get('is_actor') and len(raw) >= 0x0e:
                    aid = int.from_bytes(raw[0x0c:0x0e], 'little')
                    if aid not in k['actor_ids'] and len(k['actor_ids']) < 8:
                        k['actor_ids'].append(aid)
            if len(w['keys']) > self._this_max:
                w['many'] = True        # r0 is no object pointer here (or too many kinds): stop
                self.s._call('breakpoint_remove', address=a)
            else:
                self._this_arm(w)
        return {'new': new}

    def _this_collect_sample(self, lst, mods, cache):
        """Reservoir mode: the log is the sample so far; rebuild each function's keys
        and samples from it (nothing is re-armed, so nothing resets)."""
        import symtab
        watch = self._watch
        for b in lst.get('breakpoints', []):
            a = b.get('address')
            a = int(a, 16) if isinstance(a, str) else int(a)
            w = watch.get(a)
            if w is None:
                continue
            w['evaluations'] = int(b.get('evaluations', 0))
            w['hits'] = int(b.get('hits', 0))
            w['seen'] = int(b.get('log_seen', 0))
            log = b.get('log') or []
            w['logged'] = len(log)
            w['truncated'] = max(0, w['hits'] - len(log))
            keys, samples = {}, []
            for h in log:
                regs = h['registers']
                dumps = h.get('dumps') or []
                raw0 = bytes.fromhex(dumps[0]['hex']) if dumps and dumps[0].get('hex') else b''
                key = int.from_bytes(raw0[:4], 'little') if len(raw0) >= 4 else 'unreadable'
                k = keys.get(key)
                if k is None:
                    k = {'n': 0, 'obj': regs['r0'][2:], 'lr': regs.get('r14'), 'actor_ids': [], 'objs': []}
                    if isinstance(key, int):
                        k.update(symtab.resolve_vptr(self.s.read, key, mods, self._this_table, cache))
                    keys[key] = k
                k['n'] += 1
                r0 = int(regs['r0'], 16)
                if r0 not in k['objs'] and len(k['objs']) < 64:
                    k['objs'].append(r0)
                if k.get('is_actor') and len(raw0) >= 0x0e:
                    aid = int.from_bytes(raw0[0x0c:0x0e], 'little')
                    if aid not in k['actor_ids'] and len(k['actor_ids']) < 8:
                        k['actor_ids'].append(aid)
                s = {'frame': h.get('frame'), 'hit': h.get('hit_count'),
                     'key': key if isinstance(key, str) else '%08x' % key}
                for i in range(4):
                    s['r%d' % i] = regs.get('r%d' % i)
                    if i < len(dumps):
                        s['m%d' % i] = dumps[i].get('hex') or 'ERR'
                s['lr'], s['sp'] = regs.get('r14'), regs.get('r13')
                samples.append(s)
            samples.sort(key=lambda s: (s['frame'] or 0, s['hit'] or 0))
            w['keys'] = keys
            w['samples'] = samples
            w['many'] = len(keys) > self._this_max
        return {'new': sum(len(w['keys']) for w in watch.values())}

    def op_this_report(self):
        """Per watched function: the vptrs seen in r0 (each resolved to its class,
        RTTI chain, actor ids), logged hits and breakpoint evaluations."""
        out = []
        names = getattr(self, '_this_names', {})
        for w in (getattr(self, '_watch', None) or {}).values():
            keys = []
            for k, v in w['keys'].items():
                v = dict(v)
                v['actor_names'] = [names.get(i, '?') for i in v.get('actor_ids', [])]
                v['key'] = k if isinstance(k, str) else '%08x' % k
                keys.append(v)
            rec = {'addr': '%08x' % w['addr'], 'module': w['module'], 'name': w['name'],
                   'thumb': w['thumb'], 'evaluations': w['evaluations'], 'logged': w['logged'],
                   'truncated': w['truncated'], 'many': w['many'], 'keys': keys}
            if 'samples' in w:
                rec.update(sample='reservoir', hits=w['hits'], seen=w.get('seen', 0), samples=w['samples'])
            out.append(rec)
        return out

    def _this_unwatch(self):
        for a in list(getattr(self, '_watch', None) or {}):
            try:
                self.s._call('breakpoint_remove', address=a)
            except cc.CtlError:
                pass

    def op_this_unwatch(self):
        """Remove the sweep's breakpoints; returns the final this_report."""
        rep = self.op_this_report()
        self._this_unwatch()
        self._watch = {}
        return rep

    def op_reconnect(self):
        self.s.ctl.close()
        stop = self.s.connect(timeout=20)
        for b in self.s.bps.values():
            if b.armed:
                b.armed = False
                self.s._z(b, True)
        return stop

    def op_quit(self, kill_emu=False, clean=False):
        self.quit = True
        self.ctl_quit = bool(kill_emu)
        self.kill_emu = False           # never emuwin.kill: the fork is quit through its socket
        self.clean = bool(clean) and self.ctl_quit
        return True


# ------------------------------------------------------------------- launching
def find_fork(path=None):
    """The fork's checkout: --fork, MELONDS_AI_ROOT, or C:\\tmp\\melonds-ai-controller."""
    for p in (path, os.environ.get('MELONDS_AI_ROOT'), r'C:\tmp\melonds-ai-controller'):
        if p and os.path.isfile(os.path.join(p, 'tools', 'start_control.ps1')):
            return os.path.abspath(p)
    return None


def launch_fork(workdir, rom, port=None, token=None, sav=None, window=False, fork=None, wait_other=False):
    """Start the fork through its start_control.ps1 with a private config dir in
    workdir and a copy of the ROM. Returns the pid (None if one already answered)."""
    root = find_fork(fork)
    if not root:
        raise RuntimeError('melonDS AI-controller fork not found (--fork or MELONDS_AI_ROOT)')
    port = port or cc.default_port()
    token = token or cc.default_token()
    os.makedirs(workdir, exist_ok=True)
    game = os.path.join(workdir, 'game.nds')
    if not os.path.exists(game) or os.path.getsize(game) != os.path.getsize(rom):
        shutil.copyfile(rom, game)
    cfg = os.path.join(workdir, 'ctl-config')
    os.makedirs(os.path.join(cfg, 'saves'), exist_ok=True)
    if sav:
        shutil.copy2(sav, os.path.join(cfg, 'saves', 'game.sav'))
    args = ['powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File',
            os.path.join(root, 'tools', 'start_control.ps1'), '-Rom', game, '-Port', str(port),
            '-Token', token, '-ConfigDir', cfg, '-NoCopy']
    if window:
        args.append('-Window')
    if wait_other:
        args.append('-WaitForOther')
    log = os.path.join(workdir, 'start_control.log')
    # a file, never a pipe: melonDS inherits the handle and a pipe would never reach EOF
    with open(log, 'w') as f:
        rc = subprocess.call(args, stdout=f, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                             timeout=25 * 60 if wait_other else 180)
    with open(log, encoding='utf-8', errors='replace') as f:
        txt = f.read()
    if rc == 3:
        raise RuntimeError('another melonDS is running; not starting a second one (serve --wait-other waits)')
    if rc != 0:
        raise RuntimeError('start_control.ps1 failed (%d): %s' % (rc, txt.strip()[-400:]))
    m = re.search(r'ready: pid (\d+)', txt)
    return int(m.group(1)) if m else None


def quit_fork(port=None, token=None, proc=None, timeout=20):
    """Close the fork cleanly (its config stays in its own config dir)."""
    try:
        cc.CtlClient(port, token, timeout=10).connect().call('quit')
    except (OSError, ValueError, cc.CtlError):
        pass
    if proc is not None:
        if proc.wait(timeout) is None:
            import emuwin
            emuwin.kill(proc.pid)
            proc.wait(10)
        return proc.returncode
    return None
