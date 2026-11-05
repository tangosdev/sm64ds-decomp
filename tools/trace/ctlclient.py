"""ctlclient: a client for the control socket of the melonDS AI-controller fork.

The fork (a local, unpublished melonDS 1.1 build: see README "Control-socket
backend") serves JSON lines on 127.0.0.1, by default port 45987:

    {"id": 1, "token": T, "cmd": "step", "args": {"frames": 2}}
    {"id": 1, "ok": true, "result": {...}}

Every emulator-state command runs on the emulator thread between frames, or
while the ARM9 is halted at a breakpoint, so results are frame-exact. This is
the subset of the fork's control_client.py that emusession needs; the fork's
CONTROL_API.md documents every command.
"""
import base64
import json
import os
import socket

DEFAULT_PORT = 45987
DEFAULT_TOKEN = 'sm64-research'


class CtlError(RuntimeError):
    pass


def default_port():
    return int(os.environ.get('MELONDS_CONTROL_PORT', DEFAULT_PORT))


def default_token():
    return os.environ.get('MELONDS_CONTROL_TOKEN') or DEFAULT_TOKEN


class CtlClient:
    def __init__(self, port=None, token=None, host='127.0.0.1', timeout=120.0):
        self.host = host
        self.port = port or default_port()
        self.token = token or default_token()
        self.timeout = timeout
        self.sock = None
        self.buf = b''
        self.n = 0

    def connect(self, timeout=3.0):
        self.sock = socket.create_connection((self.host, self.port), timeout=timeout)
        self.sock.settimeout(self.timeout)
        return self

    def close(self):
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
        self.sock = None

    def call(self, cmd, timeout=None, **args):
        if self.sock is None:
            self.connect()
        self.n += 1
        req = {'id': self.n, 'token': self.token, 'cmd': cmd, 'args': args}
        self.sock.settimeout(timeout if timeout is not None else self.timeout)
        self.sock.sendall((json.dumps(req) + '\n').encode())
        while True:
            while b'\n' not in self.buf:
                d = self.sock.recv(1 << 20)
                if not d:
                    self.close()
                    raise ConnectionError('control server closed the connection')
                self.buf += d
            line, self.buf = self.buf.split(b'\n', 1)
            r = json.loads(line)
            if r.get('id') not in (None, self.n):
                continue            # a stale reply to an earlier request that timed out
            if not r.get('ok'):
                raise CtlError('%s: %s' % (cmd, r.get('error')))
            return r.get('result')

    # ---- the commands emusession uses, with their decoding
    def read(self, addr, n):
        r = self.call('read_memory', address=int(addr), size=int(n), encoding='base64')
        return base64.b64decode(r['data'])

    def write(self, addr, data):
        return self.call('write_memory', address=int(addr), base64=base64.b64encode(bytes(data)).decode())

    def read_ranges(self, ranges, hash_only=False):
        """[(addr, size), ...] read at one emulated instant -> (result, [bytes, ...])."""
        r = self.call('read_ranges', ranges=[[int(a), int(n)] for a, n in ranges], hash_only=bool(hash_only))
        if hash_only:
            return r, None
        blob = base64.b64decode(r['data'])
        return r, [blob[x['offset']:x['offset'] + x['size']] for x in r['ranges']]

    def screen_rgb(self, screen='top'):
        """(w, h, RGB24 bytes) of the last completed frame."""
        r = self.call('screenshot', screen=screen, encoding='rgb')
        return r['width'], r['height'], base64.b64decode(r['data'])


def ping(port=None, token=None, timeout=1.0):
    """The fork's ping result, or None when nothing (or something else) answers."""
    try:
        c = CtlClient(port, token, timeout=timeout).connect(timeout=timeout)
    except OSError:
        return None
    try:
        r = c.call('ping', timeout=timeout)
        return r if isinstance(r, dict) and r.get('protocol') == 2 else None
    except (OSError, ValueError, CtlError):
        return None
    finally:
        c.close()
