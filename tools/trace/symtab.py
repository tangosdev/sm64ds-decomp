"""symtab: the decomp's own tables (config/arm9/**) for the runtime tools.

Module names follow emusession: 'arm9' (dsd's main), 'itcm', 'dtcm', 'ovNNN'.
Everything here is read from config/ and, for code bytes, from the gitignored
extracted/ dump via emusession.Images; nothing is written.

  Table()                         all modules, lazily loaded
  .sections(mod)                  {'.text': (start, end), '.data': ..., '.bss': ...}
  .functions(mod)                 [Sym] with kind 'function' (size from the row)
  .data_symbols(mod)              [Sym] .data/.bss rows, size = row size or up to
                                  the next row (bounded by the section end)
  .sym_at(mod, addr)              the data/bss/function Sym containing addr
  .vtable(vptr, mods)             the _ZTV row a stored vptr points into
  .xref(mod, lo, hi)              functions whose literal pools load an address in
                                  [lo, hi), each with how the loaded pointer is
                                  used: R (load through it), W (store through it),
                                  A (passed to a call or stored, use unknown)
  owner_class(name)               the class of a mangled member (None otherwise)
  vtable_class(name)              the class of a _ZTV symbol
  named(name)                     False for func_/data_/sub_ placeholder names
"""
import bisect
import os
import re
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))

SYM_RE = re.compile(r'^(\S+) kind:(\w+)(?:\(([^)]*)\))? addr:(0x[0-9a-fA-F]+)')
SEC_RE = re.compile(r'^\s+(\.\w+)\s+start:(0x[0-9a-fA-F]+) end:(0x[0-9a-fA-F]+) kind:(\w+)')
REL_RE = re.compile(r'^from:(0x[0-9a-fA-F]+) kind:(\w+) to:(0x[0-9a-fA-F]+) module:(\S+)')
PLACEHOLDER = re.compile(r'^(func|data|sub|unk|FUN|DAT)_')


def named(name):
    return not PLACEHOLDER.match(name)


def _demangle(name):
    import sys
    tools = os.path.join(REPO, 'tools')
    if tools not in sys.path:
        sys.path.insert(0, tools)
    import demangle
    try:
        return demangle.demangle(name)
    except Exception:
        return None


def owner_class(name):
    """'_ZN9dCamera_c6RenderEv' -> 'dCamera_c'; None for unmangled or free names."""
    if not name.startswith('_Z') or name.startswith('_ZT'):
        return None
    d = _demangle(name)
    return d.get('class') if d else None


def vtable_class(name):
    """'_ZTV8dActor_c' -> 'dActor_c', '_ZTVN3abi20__si_class_type_infoE' -> 'abi::__si_class_type_info'."""
    if not name.startswith('_ZTV'):
        return None
    t = name[4:]
    inner = t[1:-1] if t.startswith('N') and t.endswith('E') else t
    d = _demangle('_ZN' + inner + '1xEv')
    return d.get('class') if d else None


class Sym:
    __slots__ = ('mod', 'addr', 'name', 'kind', 'size', 'sized', 'thumb')

    def __init__(self, mod, addr, name, kind, size, sized, thumb=False):
        self.mod, self.addr, self.name, self.kind = mod, addr, name, kind
        self.size, self.sized, self.thumb = size, sized, thumb

    def __repr__(self):
        return '%s:%s@%08x+%x' % (self.mod, self.name, self.addr, self.size)


class Table:
    def __init__(self, repo=None):
        self.repo = repo or REPO
        self.cfg = os.path.join(self.repo, 'config', 'arm9')
        self.mods = {'arm9': self.cfg, 'itcm': os.path.join(self.cfg, 'itcm'),
                     'dtcm': os.path.join(self.cfg, 'dtcm')}
        od = os.path.join(self.cfg, 'overlays')
        for d in sorted(os.listdir(od)) if os.path.isdir(od) else ():
            if re.match(r'ov\d{3}$', d):
                self.mods[d] = os.path.join(od, d)
        self._sec, self._rows, self._fn, self._data, self._starts = {}, {}, {}, {}, {}
        self._vt = {}
        self._fstarts = {}
        self._xidx = None
        self._images = None

    # ---------------------------------------------------------------- tables
    def modules(self):
        return list(self.mods)

    def sections(self, mod):
        if mod not in self._sec:
            out = {}
            p = os.path.join(self.mods[mod], 'delinks.txt')
            with open(p, encoding='utf-8') as f:
                for line in f:
                    m = SEC_RE.match(line)
                    if m:
                        out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
                    elif line.strip() and not line.startswith(' '):
                        break           # the section header block ends at the first file entry
            self._sec[mod] = out
        return self._sec[mod]

    def data_range(self, mod):
        """[start, end) of the module's .data + .bss, or None."""
        s = self.sections(mod)
        parts = [s[k] for k in ('.data', '.bss') if k in s and s[k][1] > s[k][0]]
        if not parts:
            return None
        return min(p[0] for p in parts), max(p[1] for p in parts)

    def _load(self, mod):
        if mod in self._rows:
            return
        rows = []
        with open(os.path.join(self.mods[mod], 'symbols.txt'), encoding='utf-8') as f:
            for line in f:
                m = SYM_RE.match(line)
                if not m:
                    continue
                name, kind, extra, addr = m.group(1), m.group(2), m.group(3) or '', int(m.group(4), 16)
                ms = re.search(r'size=(0x[0-9a-fA-F]+)', extra)
                size = int(ms.group(1), 16) if ms else None
                rows.append((addr, name, kind, size, extra.startswith('thumb')))
        rows.sort()
        self._rows[mod] = rows
        secs = self.sections(mod)
        fns, data = [], []
        for i, (addr, name, kind, size, thumb) in enumerate(rows):
            if kind == 'function':
                fns.append(Sym(mod, addr, name, kind, size or 4, size is not None, thumb))
            elif kind in ('data', 'bss'):
                end = None
                for s0, e0 in secs.values():
                    if s0 <= addr < e0:
                        end = e0
                        break
                nxt = end if end is not None else addr + 4
                for j in range(i + 1, len(rows)):
                    if rows[j][0] > addr and rows[j][2] in ('data', 'bss', 'function'):
                        nxt = min(nxt, rows[j][0])
                        break
                data.append(Sym(mod, addr, name, kind, size or max(1, nxt - addr), size is not None))
        self._fn[mod] = fns
        self._data[mod] = data
        allsyms = sorted(fns + data, key=lambda s: (s.addr, s.kind != 'function'))
        self._starts[mod] = ([s.addr for s in allsyms], allsyms)

    def functions(self, mod):
        self._load(mod)
        return self._fn[mod]

    def data_symbols(self, mod):
        self._load(mod)
        return self._data[mod]

    def sym_at(self, mod, addr):
        self._load(mod)
        starts, syms = self._starts[mod]
        i = bisect.bisect_right(starts, addr) - 1
        while i >= 0:
            s = syms[i]
            if s.addr <= addr < s.addr + s.size:
                return s
            if addr - s.addr > 0x10000:
                break
            i -= 1
        return None

    def function_at(self, mod, addr):
        """The function whose body (its literal pool included) holds addr."""
        fns = self.functions(mod)
        if mod not in self._fstarts:
            self._fstarts[mod] = [f.addr for f in fns]
        i = bisect.bisect_right(self._fstarts[mod], addr) - 1
        if i >= 0 and addr < fns[i].addr + fns[i].size:
            return fns[i]
        return None

    # ---------------------------------------------------------------- vtables
    def vtables(self, mod):
        if mod not in self._vt:
            self._load(mod)
            vt = [s for s in self._data[mod] if s.name.startswith('_ZTV')]
            self._vt[mod] = ([s.addr for s in vt], vt)
        return self._vt[mod]

    def vtable(self, vptr, mods):
        """(Sym, offset) of the _ZTV row in one of mods that vptr points into.
        symbols.txt places each _ZTV row at the address an object stores (its first
        virtual slot), so a primary vptr is offset 0. The type_info pointer is the
        word before it and offset-to-top the word before that (checked at run time
        on _ZTV9dCamera_c, _ZTV10dScStage_c and _ZTV8dActor_c)."""
        for mod in mods:
            if mod not in self.mods:
                continue
            starts, vt = self.vtables(mod)
            i = bisect.bisect_right(starts, vptr) - 1
            if i >= 0 and vptr < vt[i].addr + vt[i].size:
                return vt[i], vptr - vt[i].addr
        return None, None

    # ---------------------------------------------------------------- code
    def images(self):
        if self._images is None:
            import emusession as es
            try:
                self._images = es.Images()
            except (OSError, FileNotFoundError):
                self._images = False
        return self._images or None

    def code(self, mod, addr, n):
        im = self.images()
        if im is None or mod not in im.mods:
            return None
        try:
            return im.bytes_at(mod, addr, n)
        except ValueError:
            return None

    # ---------------------------------------------------------------- xrefs
    @staticmethod
    def _targets(spec):
        if spec == 'main':
            return ['arm9']
        if spec in ('itcm', 'dtcm'):
            return [spec]
        m = re.match(r'overlays?\(([0-9,]+)\)$', spec)
        if m:
            return ['ov%03d' % int(x) for x in m.group(1).split(',')]
        return []

    def _xref_index(self):
        if self._xidx is None:
            idx = defaultdict(list)
            for mod, d in self.mods.items():
                p = os.path.join(d, 'relocs.txt')
                if not os.path.exists(p):
                    continue
                with open(p, encoding='utf-8') as f:
                    for line in f:
                        m = REL_RE.match(line)
                        if not m or m.group(2) != 'load':
                            continue
                        for tm in self._targets(m.group(4)):
                            idx[tm].append((int(m.group(3), 16), mod, int(m.group(1), 16)))
            self._xidx = {tm: (sorted(v), [x[0] for x in sorted(v)]) for tm, v in idx.items()}
        return self._xidx

    def xref(self, mod, lo, hi):
        """[(from_mod, function name, access set)] for literal-pool loads of an
        address in [lo, hi) of mod, grouped by function."""
        rows, keys = self._xref_index().get(mod, ([], []))
        i = bisect.bisect_left(keys, lo)
        out = {}
        while i < len(rows) and rows[i][0] < hi:
            _, fm, pool = rows[i]
            fn = self.function_at(fm, pool)
            key = (fm, fn.name if fn else '?%08x' % pool)
            acc = out.setdefault(key, set())
            if fn is not None:
                acc.update(self._uses(fn, pool))
            i += 1
        return [(k[0], k[1], ''.join(sorted(v)) or '?') for k, v in sorted(out.items())]

    def _uses(self, fn, pool):
        code = self.code(fn.mod, fn.addr, fn.size)
        if code is None:
            return set()
        if fn.thumb:
            return _thumb_uses(code, fn.addr, pool)
        return _arm_uses(code, fn.addr, pool)


# ------------------------------------------------------------------ ARM / Thumb
# Follow the register a literal-pool word is loaded into, in straight-line code
# after the load, and record how it is used: R = a load through it, W = a store
# through it, A = passed in r0-r3 to a call or stored to memory. Aliases made by
# mov/add/sub are followed; anything that overwrites the register ends it.

def _arm_uses(code, base, pool):
    n = len(code) // 4
    words = [int.from_bytes(code[4 * k:4 * k + 4], 'little') for k in range(n)]
    acc = set()
    for k, ins in enumerate(words):
        if (ins & 0x0F7F0000) == 0x051F0000:
            off = ins & 0xFFF
            tgt = base + 4 * k + 8 + (off if ins & (1 << 23) else -off)
            if tgt == pool:
                acc |= _arm_trace(words, k + 1, {(ins >> 12) & 15})
    return acc


def _arm_trace(words, k, regs, limit=48):
    acc = set()
    end = min(len(words), k + limit)
    while k < end and regs:
        ins = words[k]
        k += 1
        cond = ins >> 28
        if cond == 0xF:
            continue
        if (ins & 0x0FFFFFF0) == 0x012FFF10:              # bx
            if cond == 0xE:
                break
            continue
        if (ins & 0x0FFFFFF0) == 0x012FFF30:              # blx reg
            if regs & {0, 1, 2, 3}:
                acc.add('A')
            regs -= {0, 1, 2, 3, 12, 14}
            continue
        if (ins & 0x0E000000) == 0x0A000000:              # b / bl
            if ins & (1 << 24):
                if regs & {0, 1, 2, 3}:
                    acc.add('A')
                regs -= {0, 1, 2, 3, 12, 14}
            elif cond == 0xE:
                break
            continue
        if (ins & 0x0E000000) == 0x08000000:              # ldm / stm
            rn, load, lst = (ins >> 16) & 15, ins & (1 << 20), ins & 0xFFFF
            if rn in regs and rn != 13:
                acc.add('R' if load else 'W')
            if load:
                regs -= {r for r in range(16) if lst & (1 << r)}
                if lst & 0x8000 and cond == 0xE:
                    break
            elif rn != 13 and any(lst & (1 << r) for r in regs):
                acc.add('A')
            continue
        if (ins & 0x0C000000) == 0x04000000:              # ldr / str (word, byte)
            rn, rt, load = (ins >> 16) & 15, (ins >> 12) & 15, ins & (1 << 20)
            if rn in regs:
                acc.add('R' if load else 'W')
            elif not load and rt in regs and rn != 13:
                acc.add('A')
            if load:
                regs.discard(rt)
                if rt == 15 and cond == 0xE:
                    break
            continue
        if (ins & 0x0E000090) == 0x00000090 and (ins & 0x60):   # ldrh / strh / ldrsb / ldrsh
            rn, rt, load = (ins >> 16) & 15, (ins >> 12) & 15, ins & (1 << 20)
            if rn in regs:
                acc.add('R' if load else 'W')
            if load:
                regs.discard(rt)
            continue
        if (ins & 0x0E0000F0) == 0x00000090:              # multiply / swap
            regs.discard((ins >> 16) & 15)
            regs.discard((ins >> 12) & 15)
            continue
        if (ins & 0x0C000000) == 0x00000000:              # data processing
            op, rn, rd = (ins >> 21) & 15, (ins >> 16) & 15, (ins >> 12) & 15
            if 8 <= op <= 11:                             # tst teq cmp cmn
                continue
            imm = ins & (1 << 25)
            rm = ins & 15
            src = None
            if op in (2, 4) and rn in regs:               # sub / add from the pointer
                src = rn
            elif op == 4 and not imm and rm in regs:
                src = rm
            elif op == 13 and not imm and (ins & 0xFF0) == 0 and rm in regs:   # mov rd, rm
                src = rm
            if src is not None:
                regs.add(rd)
            else:
                regs.discard(rd)
            if rd == 15 and cond == 0xE:
                break
            continue
    return acc


def _thumb_uses(code, base, pool):
    n = len(code) // 2
    hw = [int.from_bytes(code[2 * k:2 * k + 2], 'little') for k in range(n)]
    acc = set()
    for k, h in enumerate(hw):
        if (h & 0xF800) == 0x4800:
            tgt = ((base + 2 * k + 4) & ~3) + (h & 0xFF) * 4
            if tgt == pool:
                acc |= _thumb_trace(hw, k + 1, {(h >> 8) & 7})
    return acc


def _thumb_trace(hw, k, regs, limit=64):
    acc = set()
    end = min(len(hw), k + limit)
    while k < end and regs:
        h = hw[k]
        k += 1
        top5 = h >> 11
        if (h & 0xF800) == 0xF000 and k < len(hw) and (hw[k] & 0xE800) == 0xE800:   # bl / blx pair
            k += 1
            if regs & {0, 1, 2, 3}:
                acc.add('A')
            regs -= {0, 1, 2, 3}
            continue
        if (h & 0xFF80) == 0x4700:                       # bx
            break
        if (h & 0xFF80) == 0x4780:                       # blx reg
            if regs & {0, 1, 2, 3}:
                acc.add('A')
            regs -= {0, 1, 2, 3}
            continue
        if (h & 0xF800) == 0xE000:                       # b
            break
        if (h & 0xFF00) == 0xBD00:                       # pop {.., pc}
            break
        if (h & 0xFE00) == 0xBC00:                       # pop
            regs -= {r for r in range(8) if h & (1 << r)}
            continue
        if 0x0C <= top5 <= 0x11:                         # ldr/str/ldrb/strb/ldrh/strh imm
            rn, rt, load = (h >> 3) & 7, h & 7, h & (1 << 11)
            if rn in regs:
                acc.add('R' if load else 'W')
            elif not load and rt in regs:
                acc.add('A')
            if load:
                regs.discard(rt)
            continue
        if (h & 0xF000) == 0x5000:                       # register offset
            opc, rn, rt = (h >> 9) & 7, (h >> 3) & 7, h & 7
            load = opc >= 3
            if rn in regs:
                acc.add('R' if load else 'W')
            if load:
                regs.discard(rt)
            continue
        if (h & 0xF000) == 0xC000:                       # ldmia / stmia
            rn, load, lst = (h >> 8) & 7, h & (1 << 11), h & 0xFF
            if rn in regs:
                acc.add('R' if load else 'W')
            if load:
                regs -= {r for r in range(8) if lst & (1 << r)}
            continue
        if (h & 0xF800) == 0x4800:                       # ldr literal
            regs.discard((h >> 8) & 7)
            continue
        if (h & 0xF000) == 0x9000:                       # sp-relative
            if h & (1 << 11):
                regs.discard((h >> 8) & 7)
            continue
        if (h & 0xF000) == 0xA000:                       # add rd, pc/sp
            regs.discard((h >> 8) & 7)
            continue
        if top5 <= 0x02:                                 # lsl/lsr/asr imm
            rd, rm, imm = h & 7, (h >> 3) & 7, (h >> 6) & 31
            if top5 == 0 and imm == 0 and rm in regs:
                regs.add(rd)
            else:
                regs.discard(rd)
            continue
        if top5 == 0x03:                                 # add/sub reg/imm3
            rd, rn = h & 7, (h >> 3) & 7
            if rn in regs:
                regs.add(rd)
            else:
                regs.discard(rd)
            continue
        if 0x04 <= top5 <= 0x07:                         # mov/cmp/add/sub imm8
            op, rd = (h >> 11) & 3, (h >> 8) & 7
            if op == 0:
                regs.discard(rd)
            continue
        if (h & 0xFC00) == 0x4000:                       # ALU
            op, rd = (h >> 6) & 15, h & 7
            if op not in (8, 10, 11):
                regs.discard(rd)
            continue
        if (h & 0xFC00) == 0x4400:                       # hi-register add/cmp/mov
            op = (h >> 8) & 3
            rd = (h & 7) | ((h >> 4) & 8)
            rm = (h >> 3) & 15
            if op == 1:
                continue
            if rm in regs:
                regs.add(rd)
            elif op == 2:
                regs.discard(rd)
            if rd == 15:
                break
            continue
    return acc


# ------------------------------------------------------------------ objects
RAM_LO, RAM_HI = 0x02000000, 0x02400000
ACTOR_ID_OFF = 0x0c          # dBase_c::actorID (include/dBase_c.h)
_TI_KIND = {'_ZTVN3abi17__class_type_infoE': 'class', '_ZTVN3abi20__si_class_type_infoE': 'si',
            '_ZTVN3abi21__vmi_class_type_infoE': 'vmi'}


def _u32(read, a):
    return int.from_bytes(read(a, 4), 'little')


def _cstr(read, a, n=96):
    b = read(a, n)
    k = b.find(b'\0')
    return (b if k < 0 else b[:k]).decode('latin-1')


def type_name(zts):
    """A type_info name string ('8dActor_c', 'N3abi20__si_class_type_infoE') -> class."""
    return vtable_class('_ZTV' + zts)


def rtti_bases(read, ti, table, depth=12):
    """[class, base, base's base, ...] walking type_info objects in RAM (MWCC lays
    them out as the Itanium ABI does: vptr, name, then the base(s))."""
    kinds = table.__dict__.get('_ti_kinds')
    if kinds is None:
        # a type_info's own vptr is the abi::__*class_type_info _ZTV row's address
        kinds = {s.addr: _TI_KIND[s.name] for s in table.data_symbols('arm9') if s.name in _TI_KIND}
        table._ti_kinds = kinds
    out, todo, seen = [], [ti], set()
    while todo and len(out) < depth:
        t = todo.pop(0)
        if t in seen or not (RAM_LO <= t < RAM_HI):
            continue
        seen.add(t)
        kind = kinds.get(_u32(read, t))
        if kind is None:
            break
        name = _u32(read, t + 4)
        out.append(type_name(_cstr(read, name)) if RAM_LO <= name < RAM_HI else '?')
        if kind == 'si':
            todo.append(_u32(read, t + 8))
        elif kind == 'vmi':
            n = _u32(read, t + 12)
            for i in range(min(n, 8)):
                todo.append(_u32(read, t + 16 + 8 * i))
    return out


def resolve_vptr(read, vptr, mods, table, cache=None):
    """What a stored vptr says about its object: the _ZTV symbol (and offset into
    it: 8 for a primary vtable), the class, and the RTTI base chain."""
    key = (vptr, tuple(mods))
    if cache is not None and key in cache:
        return cache[key]
    out = {'vptr': '%08x' % vptr}
    sym, off = table.vtable(vptr, mods)
    if sym is not None:
        out.update(vtable=sym.name, vtable_module=sym.mod, vtable_offset=off, cls=vtable_class(sym.name))
    if RAM_LO <= vptr - 4 < RAM_HI:
        try:
            bases = rtti_bases(read, _u32(read, vptr - 4), table)
        except Exception:
            bases = []
        if bases:
            out['rtti'] = bases
            out.setdefault('cls', bases[0])
            out['is_actor'] = 'dActor_c' in bases
    if cache is not None:
        cache[key] = out
    return out


def resolve_object(read, obj, mods, table, cache=None, names=None):
    """`this` for a function-entry hit: the object's vptr resolved to its class and,
    for a dActor_c, its actor id (dBase_c::actorID) and the id's debug name."""
    out = {'obj': '%08x' % obj}
    if not (RAM_LO <= obj < RAM_HI) or obj & 3:
        out['object'] = False
        return out
    vptr = _u32(read, obj)
    out.update(resolve_vptr(read, vptr, mods, table, cache))
    out['object'] = 'cls' in out
    if out.get('is_actor'):
        aid = int.from_bytes(read(obj + ACTOR_ID_OFF, 2), 'little')
        out['actor_id'] = aid
        if names is None:
            names = actor_names(table.repo)
        if aid in names:
            out['actor_name'] = names[aid]
    return out


# ------------------------------------------------------------------ actors
def actor_names(repo=None):
    """{actor id: debug name} from symbols/actor_debug_names.tsv (empty if absent)."""
    p = os.path.join(repo or REPO, 'symbols', 'actor_debug_names.tsv')
    out = {}
    if os.path.exists(p):
        with open(p, encoding='utf-8') as f:
            for line in f:
                parts = line.rstrip('\n').split('\t')
                if len(parts) >= 2 and parts[0].isdigit():
                    out[int(parts[0])] = parts[1]
    return out


_TABLE = None


def table():
    global _TABLE
    if _TABLE is None:
        _TABLE = Table()
    return _TABLE
