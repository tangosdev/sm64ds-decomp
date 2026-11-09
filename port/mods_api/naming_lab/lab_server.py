"""Naming lab server: the dashboard half of the naming lab.

    python port/mods_api/naming_lab/lab_server.py [--exe build/port-trace/walk_window.exe] [--http 47620]
    then open the URL it prints (it carries this run's access token).

It talks to the naming-lab plugin inside the running port (UDP 127.0.0.1:47621)
and turns what the plugin reports -- raw addresses -- into the repository's own
names:

  * functions: walk_window.symtab (next to the exe) gives every MSVC and Itanium
    name the build defines; MSVC names are undecorated with dbghelp, Itanium ones
    with tools/demangle.py, and each is tied to its ROM address through
    config/**/symbols.txt and to its source file through tools/srcpath.py;
  * classes: an actor's vtable pointer is a ??_7Class@@6B@ symbol (or, for a
    ROM-data vtable, a _ZTV row in symbols.txt);
  * fields: include/<Class>.h, its "/* 0xNNN */" offset comments, followed down
    the base-class chain;
  * assets: build/assets/files.tsv (tools/asset_catalog.py) and the extracted ROM
    tree, BMD textures decoded by bmd.py.

SECURITY. The server binds 127.0.0.1 only, and every /api/ and /events request
must carry this run's random token (the X-Lab-Token header, or ?token= for the
EventSource stream) and a Host of 127.0.0.1/localhost, so another web page in
the same browser cannot drive it (no CSRF, no DNS rebinding). It does not
compile or load code: hot patches are a separate command-line step
(patch.py), run by hand.

Everything it learns from the port is a CANDIDATE. The port runs the decomp's
own source, so it can only suggest; a name is confirmed by the real cartridge
(the melonDS recorder in the name-scrub workflow). Proposals and the verify
queue record that on every entry. Notes go to ~/sm64ds-lab/naming-lab/, never
into the repository.
"""
import argparse
import base64
import bisect
import collections
import ctypes
import hmac
import json
import os
import queue
import re
import secrets
import socket
import struct
import sys
import threading
import time
from http.server import ThreadingHTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'tools'))
sys.path.insert(0, HERE)
import demangle as ITA  # noqa: E402
import bmd  # noqa: E402
import narc  # noqa: E402
try:
    import srcpath as SRCPATH  # noqa: E402
except Exception:  # srcpath needs config/; the lab still runs without source links
    SRCPATH = None

NOTES_DIR = os.path.join(os.path.expanduser('~'), 'sm64ds-lab', 'naming-lab')
PLUGIN = ('127.0.0.1', 47621)
# commands the dashboard may forward to the plugin; detour/undetour are not here
ALLOWED_CMDS = {'status', 'actors', 'hello', 'peek', 'poke', 'watch', 'unwatch', 'trace', 'focus', 'hot',
                'cap', 'uncap', 'tp', 'pause', 'resume', 'step', 'assets'}


# ------------------------------------------------------------------ symbols
class Undecorator:
    """MSVC names through dbghelp's own undecorator (the one the debugger uses)."""
    def __init__(self):
        self.fn = None
        try:
            dll = ctypes.WinDLL('dbghelp')
            fn = dll.UnDecorateSymbolName
            fn.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_uint32, ctypes.c_uint32]
            fn.restype = ctypes.c_uint32
            self.fn = fn
        except Exception:
            pass
        self.buf = ctypes.create_string_buffer(2048)

    def __call__(self, name, flags):
        if not self.fn:
            return name
        n = self.fn(name.encode('latin-1'), self.buf, 2048, flags)
        return self.buf.value[:n].decode('latin-1') if n else name


UND = Undecorator()
UNDNAME_NAME_ONLY = 0x1000
UNDNAME_COMPLETE = 0


class Sym:
    __slots__ = ('addr', 'names', 'qual', 'cls', 'method', 'sig', 'thiscall', 'rom', 'obj', 'kind')


def msvc_is_thiscall(name):
    # ?Method@Class@@<access><cv>E... : member functions with a this pointer
    # carry a cv letter and then the calling convention; E is __thiscall.
    m = re.search(r'@@([A-X])([A-D])([A-Z])', name)
    return bool(m and m.group(1) not in 'CDKLST' and m.group(3) == 'E')


def describe(name):
    """-> (qualified, class, method, signature, thiscall) for one symtab name."""
    if name.startswith('?'):
        full = UND(name, UNDNAME_COMPLETE)
        qual = UND(name, UNDNAME_NAME_ONLY)
        if '::' in qual:
            cls, meth = qual.rsplit('::', 1)
        else:
            cls, meth = '', qual
        return qual, cls, meth, full, msvc_is_thiscall(name)
    base = re.sub(r'__hg_\w+$', '', name)
    if base.startswith('__Z'):
        d = ITA.demangle(base[1:])
        if d:
            q = d.get('qualified') or base[1:]
            args = ', '.join(d.get('args') or [])
            return q, d.get('class') or '', d.get('method') or q, f'{q}({args})', False
    if base.startswith('_'):
        base = base[1:]
    return base, '', base, base, False


class RomSymbols:
    """config/**/symbols.txt: the ROM's addresses for the repository's names."""
    def __init__(self):
        self.by_name = {}
        self.by_qual = collections.defaultdict(list)
        self.ztv = []
        cfg = os.path.join(REPO, 'config')
        rx = re.compile(r'^(\S+) kind:(\w+)\(([^)]*)\) addr:0x([0-9a-f]+)')
        for dp, _, fs in os.walk(cfg):
            if 'symbols.txt' not in fs:
                continue
            mod = os.path.basename(dp) if os.path.basename(dp).startswith('ov') else 'arm9'
            for line in open(os.path.join(dp, 'symbols.txt'), encoding='latin-1'):
                m = rx.match(line)
                if not m:
                    continue
                name, kind, info, addr = m.group(1), m.group(2), m.group(3), int(m.group(4), 16)
                size = re.search(r'size=0x([0-9a-f]+)', info)
                rec = (addr, mod, int(size.group(1), 16) if size else 0)
                self.by_name[name] = rec
                if kind == 'function' and name.startswith('_Z'):
                    try:
                        d = ITA.demangle(name)
                    except Exception:
                        d = None
                    if d and d.get('qualified'):
                        self.by_qual[d['qualified']].append((name, rec))
                if name.startswith('_ZTV'):
                    self.ztv.append((addr, rec[2], name))
        self.ztv.sort()

    def lookup(self, names, qual):
        for n in names:
            base = re.sub(r'__hg_\w+$', '', n)
            for cand in (base[1:] if base.startswith('_') else base, base):
                if cand in self.by_name:
                    a, mod, size = self.by_name[cand]
                    return {'addr': a, 'module': mod, 'size': size, 'name': cand}
            m = re.search(r'func_(?:(ov\d+)_)?([0-9a-f]{8})', base)
            if m:
                return {'addr': int(m.group(2), 16), 'module': m.group(1) or 'arm9', 'size': 0, 'name': m.group(0)}
        hits = self.by_qual.get(qual)
        if hits and len(hits) == 1:
            name, (a, mod, size) = hits[0]
            return {'addr': a, 'module': mod, 'size': size, 'name': name}
        return None

    def vtable_class(self, rom_vptr):
        i = bisect.bisect_right(self.ztv, (rom_vptr, 1 << 40, '')) - 1
        if i >= 0:
            a, size, name = self.ztv[i]
            if a <= rom_vptr < a + max(size, 0x100):
                d = re.match(r'_ZTV(\d+)(\w+)', name)
                if d:
                    return d.group(2)[:int(d.group(1))]
        return None


class SymDB:
    def __init__(self, exe):
        self.exe = exe
        root = os.path.dirname(exe)
        self.text_lo, self.text_hi = self._text_range(exe)
        by_addr = collections.defaultdict(list)
        for line in open(os.path.join(root, 'walk_window.symtab'), encoding='latin-1'):
            p = line.rstrip('\n').split('\t')
            if len(p) == 2:
                try:
                    by_addr[int(p[1], 16)].append(p[0])
                except ValueError:
                    pass
        self.objs = self._map_objects(os.path.join(root, 'walk_window.map'))
        self.rom = RomSymbols()
        self.fn = {}            # entry address -> Sym (functions in .text)
        self.data = {}          # address -> [names] (everything else)
        for a, names in by_addr.items():
            if self.text_lo <= a < self.text_hi:
                self.fn[a] = self._make(a, names)
            else:
                self.data[a] = names
        # the port often defines a method twice: an extern "C" __ZN... shim that
        # carries the ROM name and the MSVC thiscall body it jumps to. Both are
        # the same ROM function, so the body inherits the shim's ROM address.
        rom_by_qual = {}
        for s in self.fn.values():
            if s.rom and s.cls:
                rom_by_qual.setdefault(s.qual, s.rom)
        for s in self.fn.values():
            if not s.rom and s.qual in rom_by_qual:
                s.rom = dict(rom_by_qual[s.qual], via='same name')
        self.fn_addrs = sorted(self.fn)
        self.data_addrs = sorted(self.data)
        self.by_class = collections.defaultdict(list)
        for s in self.fn.values():
            if s.cls:
                self.by_class[s.cls].append(s)
        self.vt_class = {}
        for a, names in self.data.items():
            for n in names:
                m = re.match(r'\?\?_7(.+?)@@6B@$', n)
                if m:
                    self.vt_class[a] = '::'.join(reversed(m.group(1).split('@')))

    @staticmethod
    def _text_range(exe):
        d = open(exe, 'rb').read(4096)
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        nsec = struct.unpack_from('<H', d, pe + 6)[0]
        opt = struct.unpack_from('<H', d, pe + 20)[0]
        base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
        for i in range(nsec):
            o = pe + 24 + opt + 40 * i
            if d[o:o + 5] == b'.text':
                vs, va = struct.unpack_from('<II', d, o + 8)
                return base + va, base + va + vs
        return base, base + 0x1000000

    @staticmethod
    def _map_objects(path):
        objs = {}
        rx = re.compile(r'^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)\s*$')
        try:
            for line in open(path, encoding='latin-1'):
                m = rx.match(line)
                if m:
                    objs.setdefault(int(m.group(2), 16), m.group(3))
        except OSError:
            pass
        return objs

    def _make(self, a, names):
        s = Sym()
        s.addr, s.names = a, names
        best = None
        thiscall = False
        for n in names:
            d = describe(n)
            thiscall |= d[4]
            score = (2 if n.startswith('?') else 1 if n.startswith('__Z') else 0, bool(d[1]))
            if best is None or score > best[0]:
                best = (score, d)
        q, c, m, sig, _ = best[1]
        s.qual, s.cls, s.method, s.sig, s.thiscall = q, c, m, sig, thiscall
        s.rom = self.rom.lookup(names, q)
        s.obj = self.objs.get(a, '')
        s.kind = 'thiscall' if thiscall else 'cdecl'
        return s

    def entry(self, callee_plus5):
        """the trace records entry + 5 (the return address of `call _penter`)"""
        return self.fn.get(callee_plus5 - 5) or self.at(callee_plus5)

    def at(self, addr):
        i = bisect.bisect_right(self.fn_addrs, addr) - 1
        return self.fn[self.fn_addrs[i]] if i >= 0 else None

    def data_at(self, addr):
        i = bisect.bisect_right(self.data_addrs, addr) - 1
        if i < 0:
            return None, 0
        a = self.data_addrs[i]
        return self.data[a], addr - a

    def data_addr(self, name):
        if not hasattr(self, '_dname'):
            self._dname = {}
            for a, names in self.data.items():
                for n in names:
                    self._dname[n[1:] if n.startswith('_') else n] = a
        return self._dname.get(name)

    def class_of_vtable(self, vt):
        if vt in self.vt_class:
            return self.vt_class[vt], 'vtable symbol'
        names, off = self.data_at(vt)
        for n in names or []:
            m = re.match(r'_?data_(?:(ov\d+)_)?([0-9a-f]{8})$', n)
            if m:
                c = self.rom.vtable_class(int(m.group(2), 16) + off)
                if c:
                    return c, 'ROM _ZTV'
        return None, None


# ------------------------------------------------------------------ headers
TYPE_SIZES = {'s8': 1, 'u8': 1, 'bool': 1, 'char': 1, 'signed char': 1, 'unsigned char': 1,
              's16': 2, 'u16': 2, 'short': 2, 'unsigned short': 2,
              's32': 4, 'u32': 4, 'int': 4, 'unsigned int': 4, 'unsigned': 4, 'long': 4, 'float': 4,
              'Fix12i': 4, 'Fix12<int>': 4, 'Fix12<s32>': 4, 'Vector3': 12, 'Vector3_16': 6, 'VecFx32': 12}
FIELD_RX = re.compile(r'^\s*((?:const\s+|unsigned\s+|signed\s+|struct\s+)*[\w:<>]+(?:\s*\*+)?)\s*(\**)\s*(\w+)\s*'
                      r'((?:\[[^\]]*\])*)\s*;\s*/\*\s*(0x[0-9a-fA-F]+)\b\s*(?:--\s*)?(.*?)\s*(?:\*/)?\s*$')


class GitInclude:
    """include/ as it is at a git revision (no checkout needed)"""
    def __init__(self, rev):
        import subprocess
        self.rev, self.sp = rev, subprocess
        r = subprocess.run(['git', '-C', REPO, 'ls-tree', '--name-only', rev, 'include/'],
                           capture_output=True, text=True)
        self.names = {n.split('/', 1)[1] for n in r.stdout.split()} if r.returncode == 0 else set()
        self.texts = {}

    def exists(self, f):
        return f in self.names

    def list(self):
        return sorted(self.names)

    def read(self, f):
        if f not in self.texts:
            r = self.sp.run(['git', '-C', REPO, 'show', f'{self.rev}:include/{f}'], capture_output=True)
            self.texts[f] = r.stdout.decode('utf-8', 'replace') if r.returncode == 0 else ''
        return self.texts[f]


class Headers:
    """Class layouts from include/. With `names_from` (another Headers, e.g. read
    from origin/main), a field this tree still calls unk_ takes the other tree's
    name for the same class and offset as `named`, so the lab never asks you to
    name what the repo has already named since this branch was cut."""
    def __init__(self, git_rev=None, names_from=None):
        self.cache = {}
        self.inc = os.path.join(REPO, 'include')
        self.git = GitInclude(git_rev) if git_rev else None
        self.names_from = names_from
        self.index = None

    def _exists(self, f):
        return self.git.exists(f) if self.git else os.path.exists(os.path.join(self.inc, f))

    def _read(self, f):
        if self.git:
            return self.git.read(f)
        try:
            return open(os.path.join(self.inc, f), encoding='utf-8', errors='replace').read()
        except OSError:
            return ''

    def _find(self, cls):
        if self._exists(cls + '.h'):
            return cls + '.h'
        if self.index is None:
            self.index = {}
            rx = re.compile(r'^\s*(?:struct|class)\s+(\w+)\s*(?::[^{;]*)?\{', re.M)
            for f in (self.git.list() if self.git else os.listdir(self.inc)):
                if f.endswith('.h'):
                    for m in rx.finditer(self._read(f)):
                        self.index.setdefault(m.group(1), f)
        return self.index.get(cls)

    def layout(self, cls, depth=0):
        """-> {'fields': [...], 'chain': [cls, base, ...], 'size': n or None, 'header': path}"""
        if cls in self.cache:
            return self.cache[cls]
        out = {'fields': [], 'chain': [cls], 'size': None, 'header': None}
        path = self._find(cls) if cls else None
        if not path or depth > 8:
            self.cache[cls] = out
            return out
        text = self._read(path)
        m = re.search(r'^\s*(?:struct|class)\s+%s\s*(?::\s*(?:public\s+)?([\w:]+))?\s*\{' % re.escape(cls), text, re.M)
        out['header'] = 'include/' + path
        sz = re.search(r'allocates?\s+(0x[0-9a-fA-F]+)\s+bytes', text)
        if sz:
            out['size'] = int(sz.group(1), 16)
        if not m:
            self.cache[cls] = out
            return out
        base = m.group(1)
        fields = []
        if base:
            b = self.layout(base, depth + 1)
            fields = [dict(f) for f in b['fields']]
            out['chain'] += b['chain']
        i, nest = m.end(), 1
        start = i
        while i < len(text) and nest:
            c = text[i]
            if c == '{':
                nest += 1
            elif c == '}':
                nest -= 1
            i += 1
        body = text[start:i - 1]
        level = 0
        doc = []
        for line in body.split('\n'):
            s = line.strip()
            if level == 0:
                fm = FIELD_RX.match(line)
                if fm and not s.startswith(('//', '/*', '*', 'virtual', 'static', 'typedef', 'return')):
                    fields = [f for f in fields if f['offset'] != int(fm.group(5), 16)]
                    fields.append({'offset': int(fm.group(5), 16), 'type': (fm.group(1) + fm.group(2)).strip(),
                                   'name': fm.group(3), 'array': fm.group(4), 'note': fm.group(6),
                                   'doc': ' '.join(doc)[-600:], 'owner': cls})
                    doc = []
                elif s.startswith(('/*', '*', '//')):
                    doc.append(re.sub(r'^(/\*+|\*+/?|//)\s*', '', s).replace('*/', '').strip())
                elif s:
                    doc = []
            level = max(0, level + line.count('{') - line.count('}'))
        fields.sort(key=lambda f: f['offset'])
        if not any(f['offset'] == 0 for f in fields):
            fields.insert(0, {'offset': 0, 'type': 'void **', 'name': '__vtable', 'array': '',
                              'note': 'vtable pointer', 'doc': '', 'owner': out['chain'][-1]})
        for k, f in enumerate(fields):
            nxt = fields[k + 1]['offset'] if k + 1 < len(fields) else None
            f['size'] = self._size(f, nxt)
        out['fields'] = fields
        if self.names_from and depth == 0:
            self._overlay(cls, fields)
        if out['size'] is None and fields:
            last = fields[-1]
            out['size_guess'] = last['offset'] + max(last['size'], 4)
        self.cache[cls] = out
        return out

    def _overlay(self, cls, fields):
        try:
            other = {f['offset']: f for f in self.names_from.layout(cls)['fields']}
        except Exception:  # a header the regexes cannot read is just no overlay
            return
        for f in fields:
            o = other.get(f['offset'])
            if f['name'].startswith('unk') and o and not o['name'].startswith(('unk', '__')):
                f['named'] = {'name': o['name'], 'type': o['type'] + o['array'], 'owner': o['owner'],
                              'doc': o['doc'], 'rev': self.names_from.git.rev if self.names_from.git else ''}

    @staticmethod
    def _size(f, nxt):
        t = f['type'].replace('const ', '').strip()
        one = 4 if '*' in t else TYPE_SIZES.get(t)
        n = 1
        for dim in re.findall(r'\[([^\]]*)\]', f['array']):
            try:
                n *= int(dim, 0)
            except ValueError:
                n = 0
        gap = (nxt - f['offset']) if nxt is not None else None
        if one and n:
            return one * n if gap is None else min(one * n, gap)
        return min(gap, 64) if gap else 4


# ------------------------------------------------------------------ assets
class Assets:
    def __init__(self):
        self.files = {}
        p = os.path.join(REPO, 'build', 'assets', 'files.tsv')
        if os.path.exists(p):
            for line in open(p, encoding='utf-8'):
                c = line.rstrip('\n').split('\t')
                if c and c[0].isdigit() and len(c) >= 5:
                    self.files[int(c[0])] = {'id': int(c[0]), 'path': c[2], 'kind': c[3], 'size': int(c[4])}
        self.root = os.path.normpath(os.path.join(REPO, 'extracted', 'dsd', 'files'))
        # ov0's handle table: handle -> path (tools/asset_catalog.py)
        self.handles = {}
        p = os.path.join(REPO, 'build', 'assets', 'handles.tsv')
        if os.path.exists(p):
            for line in open(p, encoding='utf-8'):
                c = line.rstrip('\n').split('\t')
                if c and c[0].isdigit() and len(c) >= 7:
                    self.handles[int(c[0])] = {'id': int(c[2]), 'path': c[4], 'kind': c[5], 'size': int(c[6])}
        self.game_ids = None   # game file id -> handle, read from the running game
        self.archives = []     # [(first, end, path)], read from the running game
        self._narc = {}        # path -> [(member path, bytes)]

    def load_game_ids(self, lab):
        """The game asks for a file by an id that is either a FAT id (< 0x8000)
        or a member of one of its NARC archives (>= 0x8000). Overlay 0 builds a
        handle -> id table at boot (data_0209d3b8, data_0209d3b0 entries;
        port/hal/s4ov0_ov0table.cpp); read it from the game and invert it, so an
        archive member gets the path of the handle that names it."""
        tab = lab.syms.data_addr('data_0209d3b8')
        cnt = lab.syms.data_addr('data_0209d3b0')
        if not tab or not cnt:
            return
        def peek(a, n):
            r = lab.ask(f'peek {a:x} {n}', 'mem', timeout=1.0)
            for d in r:
                q = d.split()
                if len(q) >= 3 and int(q[1], 16) == a and q[2] != '!fault':
                    return bytes.fromhex(q[2])
            return None
        ptr, n = peek(tab, 4), peek(cnt, 4)
        if not ptr or not n:
            return
        ptr, n = struct.unpack('<I', ptr)[0], struct.unpack('<i', n)[0]
        if not ptr or not 0 < n < 0x4000:
            return
        raw = b''
        while len(raw) < n * 2:
            chunk = peek(ptr + len(raw), min(4096, n * 2 - len(raw)))
            if not chunk:
                return
            raw += chunk
        ids = {}
        for h in range(n):
            ids.setdefault(struct.unpack_from('<H', raw, h * 2)[0], h)
        self.game_ids = ids
        # the archive table (data_0208ecf4): 13 entries of 0x14 bytes, u16 first
        # and end id at +8/+0xa, path string pointer at +0x10
        arc = lab.syms.data_addr('data_0208ecf4')
        raw = peek(arc, 13 * 0x14) if arc else None
        out = []
        for i in range(13 if raw else 0):
            first, end = struct.unpack_from('<HH', raw, i * 0x14 + 8)
            sp = peek(struct.unpack_from('<I', raw, i * 0x14 + 0x10)[0], 32)
            if sp and 0x8000 <= first < end:
                out.append((first, end, sp.split(b'\0')[0].decode('latin-1').lstrip('/')))
        self.archives = out

    def narc(self, path):
        if path not in self._narc:
            p = os.path.normpath(os.path.join(self.root, path))
            ok = p.startswith(self.root + os.sep) and os.path.isfile(p)
            self._narc[path] = narc.members(open(p, 'rb').read()) if ok else []
        return self._narc[path]

    def member(self, gid):
        """-> (archive path, index, member path, bytes) for an archive id, else None"""
        for first, end, path in self.archives:
            if first <= gid < end:
                m = self.narc(path)
                i = gid - first
                if i < len(m):
                    return path, i, m[i][0], m[i][1]
        return None

    def resolve(self, gid):
        """game file id -> {path, kind, size, how}"""
        if gid >= 0x8000:
            m = self.member(gid)
            if m:
                name = m[2] or f'{m[0]}#{m[1]}'
                return {'path': name, 'kind': os.path.splitext(name)[1].lstrip('.') or '?',
                        'size': len(m[3]), 'how': f'{m[0]} member {m[1]}'}
        if self.game_ids and gid in self.game_ids and self.game_ids[gid] in self.handles:
            h = self.handles[self.game_ids[gid]]
            return dict(h, how=f'handle 0x{self.game_ids[gid]:04x}' + (' (archive member)' if gid >= 0x8000 else ''))
        if gid in self.files:
            return dict(self.files[gid], how='FAT id')
        return {'path': f'? id 0x{gid:04x}', 'kind': '?', 'size': 0, 'how': 'unresolved'}

    def textures(self, fid):
        f = self.resolve(fid)
        if not f or not f['path'].lower().endswith('.bmd'):
            return []
        m = self.member(fid) if fid >= 0x8000 else None
        if m:
            data = m[3]
        else:
            p = os.path.normpath(os.path.join(self.root, f['path']))
            if not p.startswith(self.root + os.sep) or not os.path.isfile(p):
                return []
            data = open(p, 'rb').read()
        out = []
        for t in bmd.textures(data):
            t = dict(t)
            t['png'] = base64.b64encode(t['png']).decode() if t['png'] else None
            out.append(t)
        return out


# ------------------------------------------------------------------ the live link
class Lab:
    def __init__(self, syms, headers, assets):
        self.syms, self.headers, self.assets = syms, headers, assets
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 8 << 20)
        self.sock.bind(('127.0.0.1', 0))
        self.sock.settimeout(0.2)
        self.clients = []
        self.lock = threading.Lock()
        self.status = {}
        self.hello = {}
        self.actors = []
        self.player = 0
        self.focus = None
        self.fn_ids = {}
        self.fn_meta = []
        self.waiters = collections.defaultdict(list)
        self.last_seen = 0
        self.hot = []
        self.pending = {}

    def send(self, cmd):
        self.sock.sendto(cmd.encode(), PLUGIN)

    def ask(self, cmd, prefix, timeout=1.5, settle=None):
        """send and wait for the reply whose first word is `prefix`; with settle,
        keep collecting datagrams of a multi-part reply for that long"""
        ev = {'event': threading.Event(), 'data': []}
        with self.lock:
            self.waiters[prefix].append(ev)
        self.send(cmd)
        ev['event'].wait(timeout)
        if settle and ev['data']:
            time.sleep(settle)
        with self.lock:
            if ev in self.waiters[prefix]:
                self.waiters[prefix].remove(ev)
        return ev['data']

    def push(self, obj):
        msg = 'data: ' + json.dumps(obj, separators=(',', ':')) + '\n\n'
        with self.lock:
            for q in list(self.clients):
                try:
                    q.put_nowait(msg)
                except queue.Full:
                    pass

    def run(self):
        last_hello = 0
        while True:
            if time.time() - last_hello > 2:
                self.send('hello')
                last_hello = time.time()
            try:
                data = self.sock.recv(70000).decode('latin-1')
            except socket.timeout:
                continue
            except OSError:  # ICMP port unreachable while the game is down
                time.sleep(0.2)
                continue
            self.last_seen = time.time()
            try:
                self.handle(data)
            except Exception as e:  # one bad datagram must not end the link
                print('lab: bad message:', repr(data[:80]), e, flush=True)

    def handle(self, data):
        head, _, rest = data.partition('\n')
        kind = head.split(' ', 1)[0]
        with self.lock:
            for w in self.waiters.get(kind, ()):
                w['data'].append(data)
                w['event'].set()
        if kind == 'hi':
            self.hello = dict(kv.split('=', 1) for kv in head.split()[1:] if '=' in kv)
        elif kind == 'status':
            self.status = dict(kv.split('=', 1) for kv in head.split()[1:] if '=' in kv)
            self.push({'t': 'status', 's': self.status, 'link': True})
        elif kind == 'actors':
            self.on_actors(head, rest)
        elif kind == 'hits':
            self.on_hits(head, rest)
        elif kind == 'snap':
            _, tick, ptr, hexs = data.split(' ', 3)
            if self.focus and int(ptr, 16) == self.focus['ptr']:
                self.push({'t': 'snap', 'tick': int(tick), 'hex': hexs.strip()})
        elif kind == 'hot':
            self.on_hot(head, rest)
        elif kind == 'level':
            self.focus = None
            self.push({'t': 'level', 'level': head.split()[1]})

    def on_actors(self, head, rest):
        _, tick, n, player = head.split()
        rows = []
        for line in rest.split('\n'):
            p = line.split()
            if len(p) != 13 or p[0] != 'a':
                continue
            ptr, vt = int(p[1], 16), int(p[2], 16)
            cls, how = self.syms.class_of_vtable(vt)
            rows.append({'ptr': ptr, 'vt': vt, 'uid': int(p[3]), 'aid': int(p[4]), 'alive': int(p[5]),
                         'x': int(p[6]), 'y': int(p[7]), 'z': int(p[8]), 'ang': int(p[9]),
                         'flags': int(p[10], 16), 'area': int(p[11]), 'calls': int(p[12]),
                         'cls': cls or f'actor#{int(p[4])}', 'clsHow': how})
        # one actors message can arrive as several datagrams of the same tick
        if self.pending.get('actors_tick') != tick:
            self.pending['actors_tick'] = tick
            self.pending['actors'] = []
        self.pending['actors'] += rows
        # rows that fault on read are skipped by the plugin and there is no end
        # marker, so publish what has arrived of this tick on every datagram
        if True:
            self.actors = self.pending['actors']
            self.player = int(player, 16) if not player.isdigit() else int(player)
            self.push({'t': 'actors', 'tick': int(tick), 'player': self.player, 'list': self.actors})

    def fn_id(self, s, callee):
        key = s.qual if s else hex(callee)
        if key not in self.fn_ids:
            self.fn_ids[key] = len(self.fn_meta)
            self.fn_meta.append({'id': len(self.fn_meta), 'key': key, 'cls': s.cls if s else '',
                                 'method': s.method if s else key, 'sig': s.sig if s else '', 'addrs': [],
                                 'rom': s.rom if s else None, 'names': s.names[:6] if s else [],
                                 'obj': s.obj if s else ''})
        meta = self.fn_meta[self.fn_ids[key]]
        a = s.addr if s else callee - 5
        if a not in meta['addrs']:
            meta['addrs'].append(a)
            self.push({'t': 'fn', 'fn': meta})
        return self.fn_ids[key]

    def on_hits(self, head, rest):
        p = head.split()
        if not self.focus:
            return
        counts = collections.Counter()
        for line in rest.split('\n'):
            q = line.split()
            if len(q) != 4:
                continue
            ptr, callee, kind, n = int(q[0], 16), int(q[1], 16), int(q[2]), int(q[3])
            if ptr != self.focus['ptr']:
                continue
            s = self.syms.entry(callee)
            # a thiscall member gets its object in ecx; anything else takes it as
            # the first stack argument. Either register can hold the pointer by
            # accident, so each callee is only counted on its own convention.
            if s and s.thiscall and not kind & 1:
                continue
            if s and not s.thiscall and not kind & 2:
                continue
            counts[self.fn_id(s, callee)] += n
        self.push({'t': 'hits', 'tick': int(p[1]), 'calls': int(p[2]), 'fns': sorted(counts.items())})

    def on_hot(self, head, rest):
        frames = int(head.split()[2])
        agg = collections.Counter()
        meta = {}
        for line in rest.split('\n'):
            q = line.split()
            if len(q) != 2:
                continue
            s = self.syms.entry(int(q[0], 16))
            key = s.qual if s else q[0]
            agg[key] += int(q[1])
            if s:
                meta[key] = {'cls': s.cls, 'rom': s.rom, 'obj': s.obj}
        self.hot = [{'key': k, 'n': n, 'perFrame': round(n / max(frames, 1), 1), **meta.get(k, {})}
                    for k, n in agg.most_common(400)]
        self.push({'t': 'hot', 'frames': frames, 'list': self.hot})

    def set_focus(self, ptr):
        a = next((r for r in self.actors if r['ptr'] == ptr), None)
        if not a:
            return None
        lay = self.headers.layout(a['cls'])
        size = lay['size'] or lay.get('size_guess') or 0x200
        size = max(0xd0, min(0x2000, (size + 0x3f) & ~0x3f))
        self.focus = {'ptr': ptr, 'cls': a['cls'], 'uid': a['uid'], 'size': size}
        self.send('unwatch')
        self.send(f'watch {ptr:x} {size}')
        self.send(f'focus {ptr:x}')
        return {'focus': self.focus, 'layout': lay, 'class': self.class_info(a['cls'])}

    def class_info(self, cls):
        out, seen = [], set()
        for s in sorted(self.syms.by_class.get(cls, []), key=lambda s: s.rom['addr'] if s.rom else 1 << 40):
            if s.qual in seen:
                continue
            seen.add(s.qual)
            out.append({'key': s.qual, 'method': s.method, 'sig': s.sig, 'rom': s.rom, 'obj': s.obj})
        return {'methods': out}


# ------------------------------------------------------------------ notes
WW_LOCK = threading.Lock()


def who_writes(addr, length, secs):
    """Arm a data breakpoint on addr for secs of game time and name every
    function that wrote there (the writing function, its caller, and the one
    above). The plugin's breakpoints stay armed only for this call."""
    secs = max(0.1, min(secs, 5.0))
    length = length if length in (1, 2, 4) else 4
    with WW_LOCK:
        LAB.ask('wwatch clear', 'wwatch')
        r = LAB.ask(f'wwatch {addr:x} {length}', 'wwatch')
        ok = bool(r) and r[0].split()[-1] == 'ok'
        if ok:
            time.sleep(secs)
        rep = LAB.ask('writers', 'writers', settle=0.15) if ok else []
        LAB.ask('wwatch clear', 'wwatch')
    if not ok:
        return {'error': 'could not arm the breakpoint: ' + (r[0] if r else 'no reply from the game')}

    def fn(a):
        s = LAB.syms.at(a) if a else None
        if not s or not (LAB.syms.text_lo <= a < LAB.syms.text_hi):
            return None
        return {'qual': s.qual, 'cls': s.cls, 'sig': s.sig, 'rom': s.rom, 'off': a - s.addr}
    head, hits = None, []
    for d in rep:
        for line in d.split('\n'):
            p = line.split()
            if p and p[0] == 'writers':
                head = {'frames': int(p[2]), 'lost': int(p[3])}
            elif p and p[0] == 'h' and len(p) == 6:
                eip, r1, r2, n = int(p[2], 16), int(p[3], 16), int(p[4], 16), int(p[5])
                hits.append({'eip': eip, 'n': n, 'writer': fn(eip), 'caller': fn(r1), 'caller2': fn(r2)})
    hits.sort(key=lambda h: -h['n'])
    return {'addr': addr, 'len': length, 'secs': secs, **(head or {}), 'hits': hits}


def notes_path(name):
    os.makedirs(NOTES_DIR, exist_ok=True)
    return os.path.join(NOTES_DIR, name)


def read_jsonl(name):
    p = notes_path(name)
    if not os.path.exists(p):
        return []
    out = []
    for line in open(p, encoding='utf-8'):
        try:
            out.append(json.loads(line))
        except ValueError:
            pass
    return out


def append_jsonl(name, rec):
    with open(notes_path(name), 'a', encoding='utf-8') as f:
        f.write(json.dumps(rec) + '\n')


# ------------------------------------------------------------------ source
def source_for(rom_name):
    if not SRCPATH or not rom_name:
        return None
    try:
        p = SRCPATH.path_for(rom_name)
    except Exception:
        return None
    if not p:
        return None
    p = os.path.normpath(os.path.join(REPO, str(p)))
    if not p.startswith(os.path.join(REPO, 'src') + os.sep) or not os.path.isfile(p):
        return None
    text = open(p, encoding='utf-8', errors='replace').read()
    line = 1
    d = None
    if rom_name.startswith('_Z'):
        try:
            d = ITA.demangle(rom_name)
        except Exception:
            d = None
    pat = (r'^[^\n;]*\b%s::%s\s*\(' % (re.escape(d.get('class') or ''), re.escape(d['method']))
           if d and d.get('method') and d.get('class') else r'^[^\n;]*\b%s\s*\(' % re.escape(rom_name))
    m = re.search(pat, text, re.M)
    if m:
        line = text.count('\n', 0, m.start()) + 1
    return {'path': os.path.relpath(p, REPO).replace('\\', '/'), 'line': line, 'text': text}


# ------------------------------------------------------------------ http
LAB = None
TOKEN = secrets.token_urlsafe(24)
PORT = 47620


class H(BaseHTTPRequestHandler):
    def _json(self, obj, code=200):
        b = json.dumps(obj).encode()
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(b)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(b)

    def log_message(self, *a):
        pass

    def _host_ok(self):
        host = (self.headers.get('Host') or '').lower()
        if host not in (f'127.0.0.1:{PORT}', f'localhost:{PORT}'):
            return False
        origin = self.headers.get('Origin')
        return origin is None or origin.lower() in (f'http://127.0.0.1:{PORT}', f'http://localhost:{PORT}')

    def _authorized(self, q):
        if not self._host_ok():
            return False
        tok = self.headers.get('X-Lab-Token') or q.get('token') or ''
        return hmac.compare_digest(tok, TOKEN)

    def do_GET(self):
        u = urlparse(self.path)
        q = {k: v[0] for k, v in parse_qs(u.query).items()}
        if u.path in ('/', '/index.html'):
            if not self._host_ok():
                self.send_error(403)
                return
            b = open(os.path.join(HERE, 'lab.html'), 'rb').read()
            self.send_response(200)
            self.send_header('Content-Type', 'text/html; charset=utf-8')
            self.send_header('Content-Length', str(len(b)))
            self.send_header('Cache-Control', 'no-store')
            self.send_header('X-Frame-Options', 'DENY')
            self.end_headers()
            self.wfile.write(b)
            return
        if not self._authorized(q):
            self.send_error(403, 'missing or wrong lab token')
            return
        if u.path == '/events':
            self.events()
        elif u.path == '/api/class':
            cls = q.get('name', '')
            self._json({'layout': LAB.headers.layout(cls), 'class': LAB.class_info(cls)})
        elif u.path == '/api/source':
            self._json(source_for(q.get('rom')) or {'error': 'no source file for that symbol'})
        elif u.path == '/api/assets':
            self._json(self.assets())
        elif u.path == '/api/textures':
            self._json(LAB.assets.textures(int(q.get('file', '-1'))))
        elif u.path == '/api/notes':
            self._json({'proposals': read_jsonl('proposals.jsonl'), 'verify': read_jsonl('verify-queue.jsonl'),
                        'dir': NOTES_DIR})
        elif u.path == '/api/sym':
            s = LAB.syms.at(int(q.get('addr', '0'), 16))
            self._json({'qual': s.qual, 'sig': s.sig, 'rom': s.rom, 'names': s.names, 'obj': s.obj} if s else {})
        else:
            self.send_error(404)

    def events(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/event-stream')
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        qq = queue.Queue(maxsize=2000)
        with LAB.lock:
            LAB.clients.append(qq)
        try:
            init = {'t': 'hello', 'hello': LAB.hello, 'status': LAB.status, 'fns': LAB.fn_meta,
                    'focus': LAB.focus, 'link': time.time() - LAB.last_seen < 3}
            self.wfile.write(('data: ' + json.dumps(init) + '\n\n').encode())
            if LAB.actors:
                self.wfile.write(('data: ' + json.dumps({'t': 'actors', 'tick': 0, 'player': LAB.player,
                                                         'list': LAB.actors}) + '\n\n').encode())
            if LAB.hot:
                self.wfile.write(('data: ' + json.dumps({'t': 'hot', 'frames': 0, 'list': LAB.hot}) + '\n\n').encode())
            self.wfile.flush()
            while True:
                try:
                    msg = qq.get(timeout=3)
                except queue.Empty:
                    msg = ': keepalive\n\n'
                    if time.time() - LAB.last_seen > 3:
                        msg = 'data: {"t":"status","link":false}\n\n'
                self.wfile.write(msg.encode())
                self.wfile.flush()
        except (ConnectionError, OSError):
            pass
        finally:
            with LAB.lock:
                if qq in LAB.clients:
                    LAB.clients.remove(qq)

    def assets(self):
        A = LAB.assets
        if A.game_ids is None:
            A.load_game_ids(LAB)
        data = LAB.ask('assets', 'assets', timeout=1.0, settle=0.15)
        recs = []
        for d in data:
            for line in d.split('\n')[1:]:
                p = line.split()
                if len(p) != 6:
                    continue
                try:
                    recs.append((int(p[0]), int(p[1], 16), int(p[2], 16), int(p[3], 16), int(p[4], 16), int(p[5])))
                except ValueError:
                    pass
        loaders = {r[1] for r in recs}
        # one load runs through several loaders (Model::LoadFile ->
        # SharedFilePtr::LoadFile -> Load); keep the call made from game code
        # outside the loaders, which names who wanted the file
        groups = collections.OrderedDict()
        for r in recs:
            groups.setdefault((r[0], r[3]), []).append(r)
        rows, seen = [], {}
        for (tick, sfp), rs in groups.items():
            outer = None
            for r in rs:
                cs = LAB.syms.at(r[2])
                if not cs or cs.addr + 5 not in loaders:
                    outer = r
                    break
            outer = outer or rs[0]
            _, loader, caller, _, actor, fid = outer
            cs, ls = LAB.syms.at(caller), LAB.syms.entry(loader)
            acls = None
            if actor:
                a = next((x for x in LAB.actors if x['ptr'] == actor), None)
                acls = a['cls'] if a else None
            key = (fid, cs.qual if cs else caller)
            if key in seen:
                seen[key]['count'] += 1
                continue
            f = A.resolve(fid)
            row = {'tick': tick, 'file': fid, 'path': f['path'], 'kind': f['kind'], 'size': f['size'], 'how': f['how'],
                   'loader': ls.qual if ls else hex(loader), 'caller': cs.qual if cs else hex(caller),
                   'callerRom': cs.rom if cs else None, 'cls': (cs.cls if cs else '') or '', 'actorCls': acls or '',
                   'actor': actor, 'count': 1}
            seen[key] = row
            rows.append(row)
        return rows

    def do_POST(self):
        u = urlparse(self.path)
        if not self._authorized({}) or self.headers.get('Content-Type', '').split(';')[0] != 'application/json':
            self.send_error(403)
            return
        n = min(int(self.headers.get('Content-Length', 0)), 1 << 20)
        try:
            b = json.loads(self.rfile.read(n) or b'{}')
        except ValueError:
            self.send_error(400)
            return
        if u.path == '/api/cmd':
            cmd = str(b.get('cmd', ''))
            if cmd.split(' ', 1)[0] not in ALLOWED_CMDS or '\n' in cmd:
                self._json({'error': 'command not allowed from the dashboard'}, 403)
                return
            want = b.get('reply')
            if want:
                self._json({'reply': LAB.ask(cmd, str(want))})
            else:
                LAB.send(cmd)
                self._json({'ok': True})
        elif u.path == '/api/focus':
            r = LAB.set_focus(int(b['ptr']))
            self._json(r or {'error': 'no such actor now'})
        elif u.path == '/api/unfocus':
            LAB.focus = None
            LAB.send('unwatch')
            LAB.send('focus')
            self._json({'ok': True})
        elif u.path == '/api/whowrites':
            self._json(who_writes(int(b['addr']), int(b.get('len', 4)), float(b.get('secs', 1.0))))
        elif u.path in ('/api/propose', '/api/verify'):
            rec = {k: b[k] for k in list(b)[:32]}
            rec['time'] = time.strftime('%Y-%m-%d %H:%M:%S')
            rec['evidence_source'] = 'port (naming lab): a candidate until the ROM agrees'
            rec['game_release'] = LAB.hello.get('release')
            if u.path == '/api/verify':
                rec['why'] = ('Port observation. Confirm on the cartridge before renaming: record this class in '
                              'melonDS with the name-scrub recorder (logging breakpoints on these ROM addresses, '
                              'condition r0 == this).')
            name = 'proposals.jsonl' if u.path == '/api/propose' else 'verify-queue.jsonl'
            append_jsonl(name, rec)
            self._json({'ok': True, 'file': notes_path(name)})
        else:
            self.send_error(404)


def main():
    global LAB, PORT
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--exe', default=os.path.join(REPO, 'build', 'port-trace', 'walk_window.exe'))
    ap.add_argument('--http', type=int, default=47620)
    ap.add_argument('--names-from', default='origin/main',
                    help='a git revision whose include/ names fields this tree still calls unk_ ("" = off)')
    a = ap.parse_args()
    PORT = a.http
    t0 = time.time()
    syms = SymDB(a.exe)
    print(f'lab: {len(syms.fn)} functions, {len(syms.vt_class)} vtables, {len(syms.rom.by_name)} ROM names '
          f'({time.time() - t0:.1f}s)', flush=True)
    names_from = Headers(git_rev=a.names_from) if a.names_from else None
    if names_from and not names_from.git.names:
        print(f'lab: no include/ at {a.names_from}; field names come from this tree only')
        names_from = None
    LAB = Lab(syms, Headers(names_from=names_from), Assets())
    threading.Thread(target=LAB.run, daemon=True).start()
    srv = ThreadingHTTPServer(('127.0.0.1', PORT), H)
    srv.daemon_threads = True
    url = f'http://127.0.0.1:{PORT}/#token={TOKEN}'
    with open(notes_path('dashboard-url.txt'), 'w') as f:
        f.write(url + '\n')
    print(f'lab: dashboard {url}', flush=True)
    srv.serve_forever()


if __name__ == '__main__':
    main()
